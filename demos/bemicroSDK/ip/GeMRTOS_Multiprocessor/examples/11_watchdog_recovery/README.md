# 11_watchdog_recovery

Per-processor watchdog recovery on **processor 1**.

`faulty_cpu1` (pinned to processor 1) runs a few iterations, then arms
processor 1's watchdog and hangs deliberately. When the watchdog fires,
processor 1 is reset. The `avalon_monitor` redirects processor 1's
reset-vector fetch to the CPU2..N boot stub — which does **not** clear
`.bss` — so `g_kcb` survives, `gk_START_KERNEL`'s CPU1 branch reaches
`gk_Watchdog_Recovery_Cleanup()`, the abandoned task is killed, and the
registered handler runs. `steady_worker` (pinned to processor 2) prints
throughout, proving the rest of the system is undisturbed.

Requires two processors (`dual_proc`) and a JTAG UART.

## Expected output (shape)

```
[steady_worker] alive, count=0, processor=2
[faulty_cpu1] iteration 1 on processor 1
[steady_worker] alive, count=1, processor=2
[faulty_cpu1] iteration 2 on processor 1
[faulty_cpu1] iteration 3 on processor 1
                                             <- no line: arming the monitor + watchdog and hanging is deliberately silent
[steady_worker] alive, count=2, processor=2
...                                          <- ~2 s: watchdog fires, processor 1 resets
[ MESSAGE ] Processor 1 running              <- came back via the CPU2..N boot stub, not crt0
[watchdog] recovered: cpu=1 was_idle=0 task="faulty_cpu1" canary=0xc0ffee01 recoveries=1
[steady_worker] alive, count=N, processor=2  <- never stopped
```

`canary=0xc0ffee01` after the recovery is direct proof `.bss` was not
cleared (a wrongly-run crt0 would have zeroed it); `task="faulty_cpu1"` is
the same proof for `g_kcb`.

The arming window prints nothing on purpose: `gu_printf` takes the global
GeMRTOS mutex, and the controller suspends a processor's watchdog countdown
while that processor owns it, so a print there could postpone the very
timeout being demonstrated. `verify.expect` therefore checks the recovery
evidence (`recovered: cpu=1`, the canary, `steady_worker`), not the arming.

On the Windows simulation the write that arms the monitor's reset-vector
redirection is accepted and discarded — the simulation has no instruction
bus to redirect, and its own watchdog emulation recovers the processor.
Only the recovery path is exercised there, not the redirection itself.

## Status

This version arms the monitor's `RESET_REDIRECT` registers *from the task*,
so the redirect can be proven in isolation. The kernel itself does not arm it:
an application that wants this recovery has to do what `faulty_cpu1` does here.
