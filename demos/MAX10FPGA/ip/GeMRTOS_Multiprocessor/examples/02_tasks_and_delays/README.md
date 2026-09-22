# 02 — `tasks_and_delays`

Three tasks demonstrating GeMRTOS's task model: the periodic-vs-one-shot
distinction, ready/run priority ordering, start offsets, and external
suspend/resume control between tasks.

## The three tasks

| Task | Type | Priority | Starts at | Role |
|---|---|---|---|---|
| `supervisor` | `G_TCBType_OneShot` | 0 (highest) | 100 ms | Controls `worker` via suspend/resume |
| `beacon` | `G_TCBType_Periodic` | 5 | 2 s, then every 1 s | Re-released automatically by the kernel |
| `worker` | `G_TCBType_OneShot` | 10 (lowest) | 2 s | Re-schedules itself with its own loop + `gu_TaskDelay()` |

Ready/run priority in GeMRTOS: **larger numeric value = lower priority**
(see `gu_TaskReadyPrioritySet()`/`gu_TaskRunPrioritySet()` in
`gemrtos_core.h`). `supervisor` is the most urgent task in this example;
`worker` is the least.

## Periodic vs one-shot

`beacon` is `G_TCBType_Periodic`: `gu_TaskPeriodSet()` gives it a period,
the kernel re-releases it on every boundary, and `task_periodic_beacon()`
does its work and returns — it never delays itself.

`worker` and `supervisor` are `G_TCBType_OneShot`: released once, and each
keeps itself alive with its own `while(1)` loop ending in `gu_TaskDelay()`.
This is the pattern used for every "recurring" task elsewhere in this
codebase (see `10_full_demo`'s `task_infinite_loop()`) — `OneShot` does not
mean "runs once", it means "the kernel does not re-release it; the task
controls its own re-execution."

## Suspend/resume: what actually happens

`gu_TaskSuspend()` only succeeds when its target is currently `READY` or
`RUNNING` — not while the target is blocked inside its own `gu_TaskDelay()`
(that is a different internal wait state). Calling it on a task that isn't
runnable is safe: it returns `G_FALSE` and changes nothing (see
`gu_TaskSuspend()`'s documentation in `gemrtos_task.c` — this is the fixed
behaviour, confirmed in the implementation, not an assumption).

`worker` spends most of each 1.5 s cycle asleep in `gu_TaskDelay()`, so
`supervisor` cannot suspend it at an arbitrary moment — it retries roughly
every 5 ms until it catches `worker` actually running. `worker`'s busy loop
(`WORKER_BUSY_ITERATIONS`) exists specifically to give `supervisor` a real,
non-instantaneous window to catch: without it, `worker`'s runnable moment
would be a handful of machine instructions, and the retry loop would almost
always exhaust its budget for no good reason.

Once suspended, `worker` is `WAITING` and stays there — no race — so the
matching `gu_TaskResume()` after the hold period always succeeds on the
first call.

**Windows-simulation tier:** `WORKER_BUSY_ITERATIONS`
is calibrated against real Nios V hardware speed and does not translate to
a host CPU — measured directly, 2,000,000 iterations take ~5 ms there,
about the same as `SUPERVISOR_RETRY_DELAY_MS`'s own 5 ms poll granularity,
so whether a given poll catches `worker` RUNNING was close to a coin flip.
`main.c` now guards a host-specific value (`12,000,000`, measured ~27–31 ms
on the same machine) behind `#ifndef __niosX_arch__`; embedded is
unaffected.

**The number of retries printed is expected to vary between runs.** This is
inherent to suspending a task from outside rather than a bug: this example
exercises the actual documented contract (check the return value, retry if
necessary) rather than glossing over it.

## Building and running

```
new_script.bat -std -ex 02_tasks_and_delays --board m10_rgmii_project_msgdma ...
```

or, once the board tree exists:

```
gemrtos_build.bat -std -ex 02_tasks_and_delays -qpf top -qsys q_sys
```

## Expected output (JTAG UART, instance 0)

Interleaved, roughly (exact timing/ordering of `beacon` and `worker` lines
around the same second will vary):

```
[worker] iteration 0 starting
[worker] iteration 0 done
[beacon] tick 0
supervisor: worker suspended (attempt 3)
[beacon] tick 1
[beacon] tick 2
supervisor: worker resumed
[worker] iteration 1 starting
[worker] iteration 1 done
[beacon] tick 3
...
```

Notice `beacon` keeps ticking every second throughout — suspending `worker`
does not affect any other task. `worker`'s own iteration count does not
advance while it is suspended.

## Things to try

- Raise `worker`'s priority above `beacon`'s (smaller number) and observe
  that both still run fine — GeMRTOS's priority scheme governs contention
  order, not exclusion; with only one processor and short-lived work here,
  there is little contention to see. Compare with `07_multiprocessor_scheduling`
  for where priority and scheduling-list placement actually matter.
- Change `SUPERVISOR_HOLD_DELAY_S` and watch how long `worker`'s iteration
  count stalls.
- Remove `WORKER_BUSY_ITERATIONS`' busy loop (set it to a small value) and
  watch `supervisor`'s attempt counts climb — this is the direct evidence
  for why the busy window is there.
