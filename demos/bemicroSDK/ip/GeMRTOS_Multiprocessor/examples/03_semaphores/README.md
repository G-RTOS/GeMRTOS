# 03 — `semaphores`

Four independent scenarios exercising all four GeMRTOS semaphore flavours,
the acquisition-timeout path, and the static priority-ceiling protocol
protocol. The first multi-file example in this catalogue: task functions and
semaphore creation live in `sync_demo.c`/`sync_demo.h`, `main.c` only creates
tasks.

## The four scenarios

| Scenario | Tasks | Semaphore flavour | Demonstrates |
|---|---|---|---|
| A | `ping`, `pong` | `gu_SemaphoreCreateBinary` | One-shot signal between two tasks |
| B | `pool_holder 0`, `pool_holder 1`, `pool_impatient` | `gu_SemaphoreCreateCounting` | Bounded resource pool; `gu_SemaphoreGetCount`; the acquisition-timeout path |
| C | `mutex_low`, `mutex_medium`, `mutex_high` | `gu_SemaphoreCreateMutex` | Static priority-ceiling protocol |
| D | `recursive_demo` | `gu_SemaphoreCreateRecursiveMutex` | Nested Take/Give from the same task |

Ready/run priority in GeMRTOS: **larger numeric value = lower priority** (see
`gu_TaskReadyPrioritySet()`/`gu_TaskRunPrioritySet()` in `gemrtos_core.h`).
Scenario C is the only one where relative priority matters; every other task
here runs at `SYNC_PRIORITY_DEFAULT`.

## Scenario A — binary semaphore signal

`ping` gives `g_sem_ping` (created with `gu_SemaphoreCreateBinary(0)`, i.e.
starting empty) every 2 s. `pong` blocks on `gu_SemaphoreTake(g_sem_ping,
G_LATEST_TIME)` and prints once each time it is signalled. This is the
simplest possible use of a semaphore: not mutual exclusion, just a one-shot
wakeup from one task to another.

## Scenario B — counting semaphore pool, with a guaranteed timeout

`g_sem_pool` is created with `gu_SemaphoreCreateCounting(2, 2)`: two slots,
both free initially. `pool_holder 0` and `pool_holder 1` each take a slot
immediately (uncontended — there are exactly as many holders as slots), hold
it for 3 s, release it, wait 1 s, and repeat.

`pool_impatient` starts 500 ms after the holders, by which point both slots
are already held. It requests a slot with a 500 ms timeout — shorter than the
holders' 3 s hold — so **its first attempt is guaranteed to time out on every
run**, printing `slot request timed out` rather than blocking indefinitely.
This is the deliberate exercise of the recoverable-error path: a real
GeMRTOS application must handle `gu_SemaphoreTake()`
returning `G_FALSE`, not just the happy path. `gu_SemaphoreGetCount()` is
printed before every request so the UART log shows exactly how many slots
were free at the moment of contention.

## Scenario C — mutex and the static priority-ceiling protocol

Three tasks: `mutex_low` (priority 30, holds the mutex), `mutex_medium`
(priority 20, **never touches the mutex** — it exists purely as a competing
priority), and `mutex_high` (priority 5, waits for the mutex). `mutex_low`'s
"critical section" is a CPU-bound busy loop, not a `gu_TaskDelay()` — a
time-based delay would elapse in real time regardless of scheduling and could
never show contention.

`SEM_GrantedPriority` (the mutex's internal ceiling) starts at the least
urgent possible value and only ratchets toward more urgent, never resets
(confirmed in `gemrtos_sem.c`'s `gk_SemaphoreGrant`). The very first
task ever granted a fresh mutex sets the ceiling to **its own** priority —
which grants it no boost over itself. Once `mutex_low` releases and
`mutex_high` is granted the mutex for the first time, the ceiling ratchets
down to `mutex_high`'s priority (5) and stays there permanently, so from
`mutex_low`'s **second** acquisition onward it runs its critical section at
the boosted (ceiling) priority. This internal state transition is real and
matches the documented protocol.

**Known limitation — this board cannot make the ceiling's effect
observable.** An earlier version of this README told the reader to "watch
for a gap in `mutex_medium`'s tick cadence" once the ceiling is boosted.
That claim is wrong and has been removed: this board builds with
`NPROCESSORS 2` (`system.h`), and every task here lives in the default
scheduling list, whose `LCBExclusion` imposes no concurrency limit — so its
tasks may run on both processors at once. `mutex_medium` never touches the
mutex, so while `mutex_low` holds it, `mutex_medium` simply runs on the
free processor. No preemption occurs, so there is no priority inversion for
the ceiling to correct, on **any** of `mutex_low`'s cycles — boosted or not.
Hardware-measured: `mutex_medium`'s tick
cadence stays perfectly steady throughout the whole run. This is correct
multiprocessor behaviour, not a kernel defect. Demonstrating the ceiling's
actual protective effect would require forcing all three Scenario C tasks
onto a single processor (a dedicated scheduling list with
`gu_SchedulingListExclusionSet(plcb, 1)`); this is currently blocked by an
unrelated defect in the multi-scheduling-list path — see the `KNOWN
LIMITATION` block in `sync_demo.h` for the full reproduction detail.

`MUTEX_LOW_CRIT_SECTION_ITERATIONS` (`sync_demo.c`) is calibrated to
`02_tasks_and_delays`' own proven `WORKER_BUSY_ITERATIONS` value. A first
estimate at 10x that value was tried and measured on real hardware:
it did not complete within a 30 s UART capture, because on its
first (unprotected) cycle `mutex_low` -- priority 30, the least urgent task
in this whole example -- can be preempted by every other task here, not only
`mutex_medium`. Reverting to the already-proven value fixed it.

## Scenario D — recursive mutex

`recursive_demo` takes `g_recursive_mutex`, prints, then calls a helper
(`recursive_inner()`) that takes the **same** mutex again before giving it
back — a stand-in for a nested critical section (e.g. a helper function
called from inside another helper that already holds the lock). A plain
`gu_SemaphoreCreateMutex()` handle would deadlock a task against itself here;
`gu_SemaphoreCreateRecursiveMutex()` tracks re-entry (`SEM_Recurrence`) so the
outer `Give()` is the one that actually releases it.

## Building and running

```
new_script.bat -std -ex 03_semaphores --board m10_rgmii_project_msgdma ...
```

or, once the board tree exists:

```
gemrtos_build.bat -std -ex 03_semaphores -qpf top -qsys q_sys
```

## Expected output (JTAG UART, instance 0)

Interleaved, roughly (exact ordering/timing between scenarios will vary —
each is independent):

```
[ping] signal sent
[pong] signal received
[pool_holder 0] slot acquired, 1 remaining
[pool_holder 1] slot acquired, 0 remaining
[pool_impatient] requesting slot, 0 currently free
[pool_impatient] slot request timed out
[mutex_low] mutex acquired at t=3000 ms
[mutex_medium] tick 0 at t=3010 ms
[mutex_high] requesting mutex at t=3500 ms
[mutex_medium] tick 5 at t=4510 ms
[mutex_low] mutex released at t=5000 ms
[mutex_high] mutex acquired at t=5000 ms
[recursive_demo] recursive: outer take
[recursive_demo] recursive: inner take (nested)
[recursive_demo] recursive: outer give
...
```

`mutex_medium`'s ticks keep appearing at a steady cadence throughout —
including during `mutex_low`'s boosted (ceiling) cycles — because it runs on
the board's other processor rather than being preempted; see the "Known
limitation" note in Scenario C above.

## Things to try

- Lower `POOL_IMPATIENT_TIMEOUT_MS` below the two holders' combined release
  cadence and watch `pool_impatient` succeed instead of timing out.
- (Do **not** actually try this on hardware.) Take `g_recursive_mutex` twice
  in a row using a plain `gu_SemaphoreCreateMutex()` handle instead — this
  would deadlock the task against itself, which is exactly why
  `gu_SemaphoreCreateRecursiveMutex()` exists.
