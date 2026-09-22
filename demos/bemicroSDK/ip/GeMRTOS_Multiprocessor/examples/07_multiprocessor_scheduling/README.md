# 07 — `multiprocessor_scheduling`

GeMRTOS's differentiator: multiple **scheduling lists** (`GS_LCB`), each
independently associated with a subset of processors, each with its own
priority-vs-DEFAULT ordering and its own concurrency cap. This example
builds three configurations side by side so the effect of each mechanism is
directly visible on the UART, not merely claimed:

| Configuration | List | Processors associated | Exclusion | Demonstrates |
|---|---|---|---|---|
| Global | `G_TASK_LCB_DEFAULT` (built in) | all, at boot | none (`G_NUMBER_OF_PCB + 1`) | ordinary GeMRTOS scheduling: a task may run on whichever processor is free |
| Affinity | `g_lcb_affinity` | **one** (`TOPOLOGY_AFFINITY_PROCESSOR`) | none | partitioned scheduling / CPU affinity: a task can never run anywhere else |
| Exclusive | `g_lcb_exclusive` | all | `TOPOLOGY_EXCLUSION_LIMIT` = 1 | mutual exclusion between lists: at most one task from this list runs anywhere, system-wide, with no semaphore involved |

`topology.c`/`.h` create and configure the two custom lists; `workload.c`/`.h`
hold the three worker task bodies; `main.c` wires them together.

## Why this example exists now, and not earlier

`gu_SchedulingListCreate()` / `gu_SchedulingListAssociateProcessor()` /
`gu_SchedulingListAssociateTask()` / `gu_SchedulingListExclusionSet()` are
long-standing public API, but no example or application in this repository
had used them before — `03_semaphores`' Scenario C wanted a
dedicated list for its priority-ceiling demonstration and hit a kernel
livelock: assigning any task to a second
scheduling list froze the affected task's foreground list in an endless
2-processor eviction cycle. The root cause (`gk_PCB_GetNextTCB()` treating
an empty ready queue as "nothing here" even when the emptiness was because
the processor's own running task *was* the list's only member) was found
via a live JTAG memory trace and fixed with a one-line change to
`gemrtos_list.c`, hardware-confirmed. **This is the first example
built on top of that fix.**

## Two documentation defects found and corrected here (not in the kernel)

The real semantics of the scheduling-list API were verified before being
displayed or relied on (an earlier `KCB_NUMBER_OF_TCBs` misreading is
exactly the failure mode this guards against). Reading the actual implementations
(`gemrtos_core.c`, `gemrtos_task.c`, `gemrtos_list.c`) rather than trusting
the header comments turned up two real discrepancies:

1. **`cpu_id` is 1-based, not zero-based.** The inline comment on
   `gu_SchedulingListAssociateProcessor()`'s prototype in `gemrtos_core.h`
   says "Zero-based processor index", but the implementation asserts
   `(cpu_id >= 1) && (cpu_id <= G_NUMBER_OF_PCB)` and indexes
   `G_PCBTbl[cpu_id-1]` — matching `PCBID` and `gm_ProcessorId()`, both
   1-based. `topology.c` uses `cpu_id` values `1..G_NUMBER_OF_PCB` and
   `TOPOLOGY_AFFINITY_PROCESSOR = 2U`, verified correct against the
   implementation.
2. **`gu_SchedulingListExclusionSet()`'s exclusion is a running-task count,
   not a processor-ID bitmask.** The same header's inline comment says
   "Bitmask of processor IDs (0-based) that must NOT be assigned tasks from
   this list". The field it actually sets, `GS_LCB::LCBExclusion`, is
   documented at its own declaration as "Maximum number of running task",
   `gu_SchedulingListExclusionSet()`'s own (accurate) Doxygen block in
   `gemrtos_task.c` describes it the same way, and `gk_PCB_GetNextTCB()`
   checks it as `LCBExclusion > LCBCurrentRunning` — a count comparison, not
   a mask test. `topology.c`'s `TOPOLOGY_EXCLUSION_LIMIT = 1` is used
   exactly as the accurate documentation describes.

Neither of these is a kernel defect — the code is internally consistent and
behaves as its own two accurate doc sources (`gemrtos_core.h`'s
`LCBExclusion` field, `gemrtos_task.c`'s function-level Doxygen) describe.
Only the terse inline comments duplicated onto the `gemrtos_core.h`
prototypes are wrong. Flagged for a documentation-only kernel fix; not
applied here (out of this example's scope).

## `KCB_NUMBER_OF_PCBs`: verified, not displayed as "active" count

`g_kcb.KCB_NUMBER_OF_PCBs` is written exactly once, in `gk_Init_PCBs()`, to
`(G_UINT32) G_NUMBER_OF_PCB` — a static mirror of the same compile-time
constant this example's `#error` guard already checks. It is correct but
carries no additional runtime information (it never changes, and does not
reflect which processors are associated with which list, running, or
excluded), so this example does not use it — printing `G_NUMBER_OF_PCB`
directly says the same thing without implying a dynamic measurement.

## Building and running

```
new_script.bat -std -ex 07_multiprocessor_scheduling --board m10_rgmii_project_msgdma ...
```

or, once the board tree exists:

```
gemrtos_build.bat -std -ex 07_multiprocessor_scheduling -qpf top -qsys q_sys
```

## Expected output (JTAG UART, hardware-verified)

```
[global_worker 0]: iteration 1 on processor 1
[global_worker 1]: iteration 1 on processor 2
[affinity_worker]: iteration 1 on processor 2 (pinned to 2)
[exclusive_worker 0]: ENTER cycle 1 on processor 1
[global_worker 0]: iteration 2 on processor 2
[global_worker 1]: iteration 2 on processor 2
[exclusive_worker 0]: EXIT cycle 1 on processor 1
[exclusive_worker 1]: ENTER cycle 1 on processor 1
[affinity_worker]: iteration 2 on processor 2 (pinned to 2)
[exclusive_worker 1]: EXIT cycle 1 on processor 1
[global_worker 0]: iteration 3 on processor 1
...
```
(from a capture of this example on the MAX 10 board; result PASS)

- `global_worker 0`/`1` report *either* processor and swap between their own
  iterations — that variability is the point (global scheduling). Confirmed:
  both workers were observed on both processors within one capture.
- `affinity_worker` reports processor `TOPOLOGY_AFFINITY_PROCESSOR` (2) on
  every single iteration, with no exception, for the entire capture.
- `exclusive_worker 0` and `exclusive_worker 1` strictly alternate — an
  `ENTER` for one never appears before the matching `EXIT` for the other,
  confirming `TOPOLOGY_EXCLUSION_LIMIT` (1) is enforced. **Hardware-observed
  nuance, not anticipated in the original design note:** in this exact
  topology both exclusive workers were observed running on processor 1
  only, every cycle, across the whole capture — not "either processor" as
  a naive reading of the mechanism might suggest. Reason: `g_lcb_exclusive`
  is processor 1's own *foreground* list (nothing else is associated there
  ahead of it), so `gk_PCB_GetNextTCB()`'s same-list branch (CASE 2) lets
  processor 1 reclaim a freshly-freed exclusive slot unconditionally, with
  no exclusion check at all. Processor 2's foreground list is
  `g_lcb_affinity` instead, so it can only pick up `g_lcb_exclusive` work
  through the cross-list branch (CASE 1), which *is* exclusion-gated — and
  in practice processor 1 wins that race every time in this capture. The
  exclusion mechanism itself (never more than one runner, system-wide) is
  demonstrated correctly; free load-balancing of the exclusive list across
  both processors is not, because this topology gives processor 1 a
  structural head start on it. See "Things to try" below to observe the
  alternative.

## Things to try

- Change `TOPOLOGY_AFFINITY_PROCESSOR` in `topology.h` to the other
  processor and watch `affinity_worker`'s reported processor follow it.
- Raise `TOPOLOGY_EXCLUSION_LIMIT` to `2` (== `G_NUMBER_OF_PCB` on this
  board) in `topology.h` and watch `exclusive_worker 0`/`1`'s `ENTER`/`EXIT`
  pairs start interleaving freely — per `gu_SchedulingListExclusionSet()`'s
  own documentation, a value at or above the number of processors associated
  with the list has no restrictive effect.
- Compare `affinity_worker`'s single-processor association against
  `g_lcb_exclusive`'s every-processor association with a count cap — two
  different mechanisms (`topology.c`) that both restrict concurrency, for
  different reasons.
- The hardware capture above shows exclusive workers landing on processor 1
  every cycle (see the explanation above the trace) because processor 1 has
  no other foreground list competing with `g_lcb_exclusive` there. Associate
  a third, higher-priority list with processor 1 too (mirroring
  `g_lcb_affinity` on processor 2) and the exclusive workers should start
  appearing on processor 2 as well, exercising `gk_PCB_GetNextTCB()`'s
  cross-list (CASE 1) exclusion check on both processors symmetrically
  instead of just one.
