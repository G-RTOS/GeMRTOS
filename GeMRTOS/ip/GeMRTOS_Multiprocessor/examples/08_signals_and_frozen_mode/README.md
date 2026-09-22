# 08 — `signals_and_frozen_mode`

Fault handling and robustness: two deliberately-induced, deterministic
faults, both of which recover unattended and neither of which halts the
kernel. `fault_handlers.c`/`.h` hold the task bodies and signal callbacks;
`main.c` only creates tasks and registers signals.

## Fault 1: deadline overrun (`G_SCBType_TCB_ABORTED`)

`fault_glitchy_worker` is a `G_TCBType_Periodic` task (period
`FAULT_GLITCH_PERIOD_MS` = 300 ms): it does its work and returns, the same
way `02_tasks_and_delays/task_periodic_beacon` does — the kernel
re-releases it automatically, it never delays itself. Every
`FAULT_GLITCH_OVERRUN_EVERY_N`-th release (every 4th) runs a busy loop
calibrated to take longer than 300 ms; every other release finishes almost
instantly.

**Why this reliably fires the signal, verified against the kernel source,
not assumed:** `G_SCBType_TCB_ABORTED` is checked in exactly one place,
`gk_TASK_RELEASE()` (`gemrtos_list.c`) — "if task is not completed, then
the Abort signal is triggered" — and `gk_TASK_RELEASE()` has exactly one
caller: `gk_TimeCallback()`'s `G_ECBType_PERIODIC` case
(`gemrtos_task.c:715-717`), which only exists for `G_TCBType_Periodic` /
`G_TCBType_Periodic_Skip` tasks (`gemrtos_task.c:975-979`). A
`G_TCBType_OneShot` task that re-arms itself with `gu_TaskDelay()` (like
most "recurring" tasks in this catalogue, e.g. `04_dining_philosophers`'s
philosophers) never goes through this path at all, no matter how long its
body runs — its "next release" is entirely self-scheduled and can never
race with itself. **This is a real, deterministic mechanism**, not a
probabilistic contention artifact.

When the overrun happens, the still-running instance is reset (stack
reinitialised via `gk_TASK_STK_INIT`, re-linked into the ready list) and
the pending `G_SCBType_TCB_ABORTED` signal executes once, automatically,
before the next release resumes — `fault_signal_task_aborted()` only
reports it; the kernel does the actual recovery.

## Fault 2: timed-event service backlog (`G_SCBType_FROZEN_MODE`)

**What "frozen mode" actually measures** (confirmed directly by Ricardo,
not inferred from documentation, which only describes it vaguely as a
state "to ensure the integrity of time and events"): GeMRTOS services one
due timed event at a time — `gemrtos_kernel.c:170`, "Processor handles just
one event and continue to avoid starving other processors from mutex".
Servicing takes real processing time, so if several events are due close
together, the later ones in the batch are serviced measurably after their
own due time. If `(now − the still-unattended event's own due time)`
exceeds the frozen-mode threshold while frozen mode is enabled, the
`FROZEN` event fires; once the backlog drains back within threshold, the
`UNFROZEN` event fires. **It is a timed-event *service-latency* watchdog,
not a CPU/interrupt-liveness one** — this example never touches a critical
section or disables interrupts.

The kernel enables frozen mode at boot with a generous 500 ms threshold
(`gemrtos_kernel.c:563-565`, `gu_ConvertTime(0, 0, 0, 500)`) — comfortably
above anything any other example in this catalogue has ever produced.
`main.c` lowers it to `FAULT_FROZEN_THRESHOLD_MS` (20 ms, 25× more
aggressive) via `gu_FrozenModeThresholdSet()`, then creates
`FAULT_BURST_TASK_COUNT` (40) `G_TCBType_Periodic` tasks
(`fault_burst_member`) that all share the exact same period and start
offset — so all 40 release on the identical tick, every
`FAULT_BURST_PERIOD_S` (4) seconds. Servicing 40 releases back-to-back is
what creates the backlog; nothing else in this example does.

`main.c` also registers `G_SCBType_FROZEN_MODE` and
`G_SCBType_UNFROZEN_MODE` signals at KCB level (`pxcb = NULL` — these
signal types are not task-specific). The kernel's own `gk_FrozenCallback()`
/ `gk_UnFrozenCallback()` (`gemrtos_task.c`) already print `FROZEN Proc:
<n>` / `UNFROZEN Proc: <n>` unconditionally, *before* either registered
signal runs — that pair of lines appears on every run regardless of
whether an application registers anything at all;
`fault_signal_frozen()`/`fault_signal_unfrozen()` add the
example-specific lines that demonstrate `gu_SignalCreate()` itself.

## Calibration (hardware-verified)

`FAULT_GLITCH_OVERRUN_BUSY_ITERATIONS` (400,000) and
`FAULT_FROZEN_THRESHOLD_MS` (20) were first-pass estimates, calibrated the
same way `03_semaphores/sync_demo.h`'s busy loop was (empirically against
this board/toolchain, not derived from a clock frequency) — **confirmed
correct on the first hardware run**, no re-tuning needed. Every 4th
`glitchy_worker` release overran and aborted exactly as designed (releases
4, 8, 12, 16, ... through 52 in the verification capture); every burst of
40 `burst_member` tasks tripped at least one `FROZEN`/`UNFROZEN` pair (one
burst tripped it twice, as the backlog dipped and reformed while draining
— still recovered both times). If retuning is ever needed on different
hardware: raise `FAULT_GLITCH_OVERRUN_BUSY_ITERATIONS` if the overrun loop
finishes faster than 300 ms; lower `FAULT_FROZEN_THRESHOLD_MS` further or
raise `FAULT_BURST_TASK_COUNT` (bounded well under `G_NUMBER_OF_TCB` = 64
and `G_NUMBER_OF_ECB` = 80, `gemrtos_config.h`) if a burst ever fails to
trip it.

**Windows-simulation tier:** the 400,000-iteration
value above is calibrated against real Nios V hardware and does not
translate to host CPU speed — measured directly on the Windows dev
machine with this tier's own build flags, 400,000 iterations took ~1 ms,
roughly 300x short of the 300 ms needed to overrun the period, so
`G_SCBType_TCB_ABORTED` never fired there. `fault_handlers.h` now guards a
host-specific value (`175,000,000`, measured ~447–482 ms on the same
machine) behind `#ifndef __niosX_arch__`; embedded is unaffected — the
value above still applies on real hardware, unchanged.

## Building and running

```
new_script.bat -std -ex 08_signals_and_frozen_mode --board m10_rgmii_project_msgdma ...
```

or, once the board tree exists:

```
gemrtos_build.bat -std -ex 08_signals_and_frozen_mode -qpf top -qsys q_sys
```

## Expected output (JTAG UART, hardware-verified)

```
[burst_member 0] released
[burst_member 1] released
...
[burst_member 39] released
FROZEN Proc: 1
fault_handlers: FROZEN_MODE signal delivered on processor 1 -- timed-event backlog exceeded threshold
UNFROZEN Proc: 1
fault_handlers: UNFROZEN_MODE signal delivered on processor 1 -- backlog drained, recovered
[glitchy_worker] release 1: normal, finishes well within period
[glitchy_worker] release 2: normal, finishes well within period
[glitchy_worker] release 3: normal, finishes well within period
[glitchy_worker] release 4: deliberately overrunning (busy loop, period is 300 ms)
[glitchy_worker] release 4: overrun finished (too late -- the next release was already due)
[glitchy_worker] ABORTED: previous release was still running when this one came due -- recovered automatically
[glitchy_worker] release 5: normal, finishes well within period
...
```

`FROZEN Proc: <n>` / `UNFROZEN Proc: <n>` (kernel built-in) and the two
`fault_handlers:` lines are expected every `FAULT_BURST_PERIOD_S` seconds —
not errors. `[glitchy_worker] ABORTED: ...` is expected every
`FAULT_GLITCH_OVERRUN_EVERY_N`-th release — also not an error. Both faults
are self-recovering: normal operation resumes immediately after each,
without any special handling beyond what the kernel already does.

## Things to try

- Lower `FAULT_GLITCH_OVERRUN_EVERY_N` in `fault_handlers.h` and watch
  `ABORTED` lines become more frequent.
- Raise `FAULT_FROZEN_THRESHOLD_MS` back toward the kernel's 500 ms default
  and watch the burst stop tripping frozen mode at all — direct evidence
  that the fault comes from the threshold/backlog relationship, not from
  the burst itself.
- Compare `fault_glitchy_worker` (kernel-driven periodic re-release,
  overrun detected and reported automatically) against
  `02_tasks_and_delays/task_worker` (self-scheduled via `gu_TaskDelay()`,
  no overrun detection possible even in principle) — the same
  "does work, returns" task shape, with and without the kernel's own
  deadline-tracking machinery underneath it.
