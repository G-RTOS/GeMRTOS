# 10_full_demo — everything combined

The complete GeMRTOS demonstration: this is the configuration that has been
verified on real hardware, and it is what a build with **no** `-ex` argument
produces.

**Application directory:** `software/hellogemrtos/` (declared in
`../manifest.txt`, not derived from this directory's name — existing documents,
scripts and tool paths all refer to `hellogemrtos`).

## What it runs

Selected at compile time by the `INCLUDE_*` switches near the top of
`hellogemrtos.c`:

| Switch | What it adds |
|---|---|
| `INCLUDE_PHILOSOPHERS` | 30 dining-philosopher tasks contending for fork semaphores with acquisition timeouts, plus a `TCB_ABORTED` signal handler |
| `INCLUDE_TASK_PERIODIC` | A periodic task whose period is retunable at runtime from the web page |
| `INCLUDE_TASK_INFINITE_LOOP` | A one-shot task looping with a runtime-adjustable delay |
| `INCLUDE_JTAG_UART_SERVER` | Interrupt-driven JTAG UART read/write servers built on the trigger abstraction |
| `INCLUDE_TSE` | LwIP over the Triple-Speed Ethernet MAC: DHCP, link debounce, and an HTTP server with SSI variables, a CGI configuration form and AJAX live updates |

## Hardware prerequisites

JTAG UART, LEDs, and — for the networking half — a TSE/RGMII PHY on a
DHCP-capable network.

## Expected output

On the JTAG UART: philosopher tasks reporting think/eat transitions, the
periodic task ticking at its configured period, and the network stack
reporting link speed, DHCP state and the acquired IP address.

With networking up, browsing to the board's IP serves a page with a left
navigation menu (`Home` / `Status`). The Status page shows a live table
(task-pool size, uptime, IP address, periodic-task period, loop delay) that
refreshes every 2 s without a full page reload, and a form that changes the
period and loop delay at runtime.

**Uptime must visibly increment on its own.** If it freezes, the page is being
served with a stale `Content-Length`. That symptom was traced to a
`%CD%`/`pushd` interaction inside `niosv-shell`.

## Note on this file's history

`hellogemrtos.c` here is byte-identical to
`misc/gemrtos_ips/GeMRTOS_Multiprocessor/hellogemrtos.c`. Both exist during the
transition; CI checks that they stay identical. **Edit this copy**, not
the original.
