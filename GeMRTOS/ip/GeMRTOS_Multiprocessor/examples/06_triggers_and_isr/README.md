# 06 — `triggers_and_isr`

GeMRTOS's ISR-as-task mechanism, using the JTAG UART's real hardware
interrupt: one task registered directly to a hardware-IRQ trigger acts as
the actual interrupt service routine, dispatching to two independent
software triggers that two ordinary tasks wait on. Carries its own copy of
the JTAG UART trigger driver (`jtag_uart_trigger.c`/`.h`) —
extracted from `10_full_demo/hellogemrtos.c` as a **behaviour-preserving
move**, same discipline as `04_dining_philosophers`.

## The mechanism

Three tasks, three triggers:

| Task | Trigger | IRQ-bound? | Timeout? |
|---|---|---|---|
| `jtag_uart_irq_manager` | `jtag_uart_trigger` | yes — the JTAG UART's real hardware IRQ | no |
| `jtag_uart_read_server` | `trigger_in` | no (software-only) | yes — `JTAG_TRIGGER_READ_TIMEOUT_MS` = 100 ms |
| `jtag_uart_write_server` | `trigger_out` | no (software-only) | no |

`jtag_uart_irq_manager` is registered to a trigger created with the
peripheral's actual IRQ number (`gu_TriggerCreate(DEVICE_IRQ(...), 0)`) —
this is what makes it the literal hardware ISR, not just a task that polls.
On every JTAG UART interrupt it reads the control register once and
releases whichever of `trigger_in`/`trigger_out` the interrupt was actually
for (`gu_TriggerRelease()`), waking the corresponding server task.
`gu_TriggerEnableHook()`/`gu_TriggerDisableHook()` are how the driver plugs
itself into the peripheral's own interrupt-enable bits — the trigger
mechanism itself knows nothing about UART registers.

## The timeout path

`trigger_in` is created **with** a timeout
(`gu_TriggerCreate(-1, JTAG_TRIGGER_READ_TIMEOUT_MS)`); `trigger_out` is
created **without** one (output, once queued, always eventually gets FIFO
space — there is nothing to time out on). `jtag_uart_read_server()` loops
on `gu_TriggerWait()`:

- **Happy path:** the trigger fires because RX data arrived — drain the
  FIFO, assemble a line, publish it to `pqueue_in`.
- **Timeout path:** no RX activity within the 100 ms window — print
  `Read timeout in JTAG UART` and flush whatever partial line has
  accumulated, if any.

With nobody typing over the JTAG UART during an unattended hardware
verification run, the **timeout path is the one that fires
deterministically, once per window, every run** — this example does not
depend on manual interaction to demonstrate it, unlike the happy path
(typing something is a "things to try" item below, not part of the
guaranteed verification pattern).

## Demonstrating the write side without operator interaction

`main.c` adds `task_uart_announcer`, a small periodic task (not part of the
driver copy above) that publishes `announcer: tick <n>` through
`jtag_uart_0_info->pqueue_out` every 3 s, starting 6 s after boot (1 s after
the driver's own tasks come up at their proven 5 s offset, unchanged from
`10_full_demo`). Each line is drained to the JTAG UART by
`jtag_uart_write_server`, waiting on `trigger_out` for FIFO space exactly as
`10_full_demo`'s original write server did — this is the write-side
trigger's happy path, deterministic and observable on every run, with no
need for anyone to type anything.

## Building and running

```
new_script.bat -std -ex 06_triggers_and_isr --board m10_rgmii_project_msgdma ...
```

or, once the board tree exists:

```
gemrtos_build.bat -std -ex 06_triggers_and_isr -qpf top -qsys q_sys
```

## Expected output (JTAG UART, instance 0)

```
Read timeout in JTAG UART
Read timeout in JTAG UART
announcer: tick 1
Read timeout in JTAG UART
...
announcer: tick 2
...
```

`Read timeout in JTAG UART` lines are expected and not errors — see "The
timeout path" above. `announcer: tick <n>` lines appear roughly every 3 s
starting at t=6s.

## Things to try

- Attach a terminal to JTAG UART instance 0 and type a line, then press
  Enter — watch `INPUT FROM JTAG-UART = <your line>` appear, the happy path
  of `jtag_uart_read_server`, and notice the timeout lines pause while you
  are actively typing (RX activity keeps resetting the trigger).
- Lower `JTAG_TRIGGER_READ_TIMEOUT_MS` in `jtag_uart_trigger.h` and watch
  the timeout lines fire more often.
- Compare this hardware-IRQ-bound trigger against `03_semaphores` or
  `05_message_queue_pubsub`'s purely software-driven synchronization — none
  of those primitives are ever created with a real IRQ number; this is the
  only example in the catalogue so far where a trigger genuinely *is* the
  hardware ISR.
