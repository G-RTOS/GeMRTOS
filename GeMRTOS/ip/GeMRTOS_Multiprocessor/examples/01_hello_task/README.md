# 01 — `hello_task`

The minimum viable GeMRTOS application. One task, created in `main()` and
released with `gu_TaskStartWithOffset()`, printing a greeting and a counter
over the JTAG UART once per second. If this example runs, the board boots,
the kernel starts, and the JTAG UART console works — the baseline every
other example builds on.

## What it demonstrates

- `gu_TaskCreate()` — allocate a task and set its entry point.
- `gu_TaskTypeSet()` — mark the task `G_TCBType_OneShot` (see
  `enum tcbtype` in `gemrtos_core.h`): it runs once and keeps itself alive
  with its own `while(1)` loop, rather than being periodically re-released
  by the scheduler.
- `gu_TaskStartWithOffset()` — move the task out of the `UNLINKED` state a
  freshly created task starts in. **This call is required**: without it the
  task is never linked into the ready list and never runs, even though it
  is not one of the four APIs the example is nominally built around.
- `gu_TaskDelay()` / `gu_printf()` — the basic building blocks used by
  almost every task in every other example.

## Building and running

```
new_script.bat -std -ex 01_hello_task --board m10_rgmii_project_msgdma ...
```

or, once the board tree exists:

```
gemrtos_build.bat -std -ex 01_hello_task -qpf top -qsys q_sys
```

Omitting `-ex` builds `10_full_demo` as before; nothing about the default
path changes.

## Expected output (JTAG UART, instance 0)

```
[hello_task] Hello from GeMRTOS! count=0
[hello_task] Hello from GeMRTOS! count=1
[hello_task] Hello from GeMRTOS! count=2
...
```

One line per second, indefinitely. The exact count value when you attach
the terminal depends on how long the board has been running.

## Things to try

- Change `HELLO_TASK_DELAY_MS` and rebuild — the printing rate follows.
- Add a second `gu_TaskCreate()` call in `main()` for a second task; give it
  a different description string and watch both interleave on the same
  UART, printed under the same internal `gu_printf` semaphore.
