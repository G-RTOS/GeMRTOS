# 04 — `dining_philosophers`

The classic dining-philosophers contention and deadlock-avoidance stress
demo: 30 tasks, 30 shared binary semaphores ("forks") arranged in a ring,
bounded acquisition timeouts. This is the second multi-file example in the
catalogue: task functions and semaphore creation live in
`philosophers.c`/`philosophers.h`, `main.c` only creates tasks.

**This example is extracted from `10_full_demo/hellogemrtos.c`** — the same
philosopher demo that has run in every hardware-verified build of this
project to date. The extraction is a **behaviour-preserving move**
not a rewrite: `task_philosopher()`, the fork semaphore
array, and every timing/priority constant are unchanged from the original.
See "Extraction notes" below for the small, deliberate, and fully
documented set of things that *did* change, including one real (not
scaffolding-only) behavioural change: dropping the original's dead
`G_SCBType_TCB_ABORTED` registration.

## The mechanism

Philosopher `i` needs fork `i` (left) and fork `(i+1) % 30` (right) — each
fork is a `gu_SemaphoreCreateBinary(1)` shared with one neighbour. Acquiring
both forks, in an order that alternates by index parity (even philosophers
take left-then-right, odd take right-then-left), is the classic
asymmetric-ordering strategy that prevents the circular-wait condition
required for deadlock. Both `gu_SemaphoreTake()` calls use a 100 ms timeout
(`TIMEOUT_FORK_MS`) rather than blocking forever — a philosopher that can't
get both forks in time gives up cleanly (`UNSUCCESSFULL SEMAPHORE TAKEN`)
and retries a second later, rather than risking an unbounded wait.

Half the philosophers (`task_index > 15`) simply print when eating; the
other half additionally update a shared LED value under
`gu_SemaphoreCreateRecursiveMutex()` (`mutex_leds`), itself guarded by a
10 ms timeout (`TIMEOUT_LED_MUTEX_MS`).

**No `G_SCBType_TCB_ABORTED` signal is registered in this extraction**,
unlike the original combined demo — see "Extraction notes" item 4 below for
why: it could never fire for these `OneShot` philosophers, on any run.

## Extraction notes

Everything below is a scope/scaffolding change made *during* the extraction,
not a change to the synchronization algorithm. Each is called out
explicitly — functional changes are never introduced without being
documented — even though none of them touch fork acquisition, timeouts, priorities,
periods, the start offset, or the abort path.

1. **The `INCLUDE_JTAG_UART_SERVER` branch is gone.** In the original
   combined demo, the "high half" of philosophers routed their eating
   message through the JTAG UART interactive server's message queue instead
   of `gu_printf()`, whenever that server was compiled in
   (`INCLUDE_JTAG_UART_SERVER == 1`, true on real hardware). That server is
   an unrelated subsystem — its own driver copy belongs to
   `06_triggers_and_isr`, which owns its copy, not here. This example always takes
   the plain `gu_printf()` path, which is exactly what the *original file
   itself* already executed whenever that macro was `0` (its own non-niosX
   branch) — not a new or untested code path, just the only one a
   standalone philosophers example can reach without dragging in the whole
   JTAG-server subsystem.
2. **The `#if (INCLUDE_PHILOSOPHERS == 1)` wrapper is gone.** It existed so
   the combined demo could compile the philosophers in or out depending on
   target; a dedicated philosophers example has no "out" state.
3. **Semaphore/task-creation failure messages are slightly reworded.**
   `philosophers_semaphores_create()` returns `G_FALSE` on pool exhaustion
   and `main()` prints a single generic message, rather than the original's
   per-resource inline messages (`"Error when creating fork semaphore %d\n"`,
   `"Error when creating task\n"`) — a necessary consequence of moving
   semaphore creation into its own function. This path has essentially never
   been exercised on real hardware (the semaphore/TCB pools are sized for
   this fixed, compile-time-known demo), so the practical risk is nil, but
   it is a real text difference and is recorded here rather than silently
   carried forward as if nothing changed.
4. **The original's `gu_SignalCreate(G_SCBType_TCB_ABORTED, ...)`
   registration is dropped — the one item in this list that is
   a genuine behavioural change, not scaffolding.** Traced from source
   while building `08_signals_and_frozen_mode`: `G_SCBType_TCB_ABORTED` is
   checked in exactly one place, `gk_TASK_RELEASE()` (`gemrtos_list.c`),
   whose only caller is `gk_TimeCallback()`'s `G_ECBType_PERIODIC` case
   (`gemrtos_task.c`), which only exists for `G_TCBType_Periodic`/
   `_Periodic_Skip` tasks. Every philosopher here is `G_TCBType_OneShot`,
   self-rescheduled via its own `gu_TaskDelay()` at the end of
   `task_philosopher()` — it never goes through the periodic re-release
   path, no matter how long its body runs, so the original's registered
   signal (and `signal_philosopher_task_aborted()`, removed along with it)
   could never fire, on any run, regardless of contention. Confirmed by
   Ricardo directly: signal registration for `G_SCBType_TCB_ABORTED` only
   makes sense for genuinely `G_TCBType_Periodic` tasks. This also
   retracts this README's own prior claim (below, in "Expected output")
   that a `TASK <n> ABORTED` line was "possible but not guaranteed" — it
   was never reachable at all. See `08_signals_and_frozen_mode` for a task
   that is genuinely `G_TCBType_Periodic` and can actually reach this
   signal, with the mechanism verified working on hardware.

**Two quirks preserved exactly, on purpose, not fixed:**

- Every philosopher's task-description format string is `"task %d"` (not
  `"philosopher %d"`), and it's formatted with `%d` against an `unsigned
  int` argument — both exactly as in the original. Neither affects
  behaviour; changing either would be a cosmetic rewrite this move
  deliberately avoids.
- `gu_TaskPeriodSet()` is called on every philosopher even though each is
  `G_TCBType_OneShot` (not `G_TCBType_Periodic`) — also exactly as in the
  original. `02_tasks_and_delays`' README documents the normal
  periodic-vs-one-shot distinction; this demo's `OneShot` tasks re-schedule
  themselves via their own `gu_TaskDelay(0, 0, 1, 0)`, so the period value
  set here has no observable effect. Preserved as-is rather than silently
  removed, since its original purpose (if any) was not established during
  this extraction.

## Building and running

```
new_script.bat -std -ex 04_dining_philosophers --board m10_rgmii_project_msgdma ...
```

or, once the board tree exists:

```
gemrtos_build.bat -std -ex 04_dining_philosophers -qpf top -qsys q_sys
```

## Expected output (JTAG UART, instance 0)

Interleaved, roughly (exact ordering between philosophers will vary with
timing and contention, as in the original combined demo):

```
task 0 is eating in processor 1
task 3 is eating in processor 1
task 17 is eating in processor 1
UNSUCCESSFULL SEMAPHORE TAKEN task 5
task 5 is eating in processor 1
...
```

`UNSUCCESSFULL SEMAPHORE TAKEN <task>` lines are expected and not errors —
they show the 100 ms fork-acquisition timeout doing its job under
contention. No `TASK <n> ABORTED` line ever appears — see "Extraction
notes" item 4 above for why the original demo's abort signal could never
fire here.

## Things to try

- Compare directly against `10_full_demo`'s UART output — the philosopher
  behaviour (eating cadence, timeout frequency, LED updates) should be
  indistinguishable; only the JTAG-queue-routed half of the messages differ
  in transport, not content (see "Extraction notes" above).
- Lower `TIMEOUT_FORK_MS` in `philosophers.h` and watch
  `UNSUCCESSFULL SEMAPHORE TAKEN` lines become more frequent.
- Compare this fixed asymmetric-ordering strategy against
  `03_semaphores`' Scenario B (a bounded counting-semaphore pool) — both
  solve resource contention, but this one solves the specific circular-wait
  problem of *needing two resources at once*, which a simple pool count
  does not.
