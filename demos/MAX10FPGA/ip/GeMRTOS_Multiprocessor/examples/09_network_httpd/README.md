# 09 — `network_httpd`

The network showcase: LwIP + a raw HTTP server with SSI status variables,
CGI-editable runtime parameters, and client-side AJAX live updates. The
fourth multi-file example in the catalogue: `net_init.c`/`.h` (device state,
topology wiring, DHCP/link lifecycle callbacks), `tse_servers.c`/`.h` (the
mSGDMA-driven RX/TX task pair), `web_sources.c`/`.h` (the SSI/CGI web
content), `main.c` (two small LED tasks that give the web page's editable
rows something real to show and change, plus wiring).

**This example is extracted from `10_full_demo/hellogemrtos.c`** — the same
network/web code that has run in every hardware-verified build of this
project (DHCP validation, link
debounce, PHY auto-negotiation, the web status page split and style
overhaul, the SSI source registry, CGI configuration, AJAX polling). The
extraction is a **behaviour-preserving move**: every
function body in `tse_servers.c` and `web_sources.c` is unchanged from the
original. See "Extraction notes" below for the small, deliberate, and
fully documented set of things that *did* change.

## The mechanism

| Task | Role |
|---|---|
| `tse_receive_server` | Brings up lwIP, the netif, DHCP, and the HTTP server once; then loops forever servicing received frames via mSGDMA and PHY link-state changes (with P1-P3's DHCP/link-debounce fixes intact). |
| `tse_transmit_server` | Subscribes to the device's outgoing message queue (`gu_MessageQueue*`) and drives mSGDMA to transmit each frame lwIP hands it. |
| `task_periodic` | `G_TCBType_Periodic`, blinks LED 0. Its period is one of the two CGI-editable values. |
| `task_infinite_loop` | `G_TCBType_OneShot`, self-scheduled, blinks LED 1. Its delay is the other CGI-editable value. |

The web status page (served from the shared `GeMRTOS_Monitor` `fs/` tree —
see "Where the pages come from" below) shows a live status table via the
generic SSI source registry (`gemrtos_web_sources[]`, `web_sources.c`) —
task pool size, uptime, IP address, and the two editable values — refreshed
client-side via AJAX polling, no WebSocket/SSE support needed in this
`httpd`. A form posts to `/set.cgi`, handled by `gemrtos_web_cgi_set_handler()`,
which applies the new period/delay immediately (clamped to
`[GEMRTOS_WEB_MIN_PERIOD_MS, GEMRTOS_WEB_MAX_PERIOD_MS]` so a malformed
submission can't set either to 0, which would busy-loop a task or halt the
system via `PRINT_ASSERT`).

## Where the pages come from

This example carries **no `fs/` directory of its own**. `gemrtos_build.bat`'s
`makefsdata.exe` step always regenerates `fsdata.c` from
`misc/gemrtos_ips/GeMRTOS_Monitor/HAL/apps/http/fs/`, regardless of which
example is selected — that path is fixed, not example-relative. Carrying a
second copy here would duplicate a build chain that took considerable
effort to stabilize (SSI extension handling, staleness detection, the
`%CD%`/`niosv-shell` fix) for no benefit, and risk the same class of bug
recurring in two places instead of one.

## Extraction notes

Everything below is a scope/scaffolding change made *during* the
extraction, not a change to the network or web-content logic itself. Each
is called out explicitly, because functional changes are never introduced
without being documented.

1. **Dining philosophers, the JTAG UART interactive server, and the
   unconditional `G_SCBType_FROZEN_MODE` registration are gone.** Each is
   unrelated to networking and already has its own dedicated example
   (`04_dining_philosophers`, `06_triggers_and_isr`,
   `08_signals_and_frozen_mode` — the last confirmed with Ricardo before
   dropping it here, since unlike the philosophers' dead abort signal
   (`04`'s Extraction notes item 4), this one *is* functional, just
   off-topic for a network showcase).
2. **The inline MAC-address/netif-struct wiring block that used to sit
   directly in `main()` is now `net_init_topology()`** (`net_init.c`) — pure
   code motion into a named function, matching this catalogue's
   `<subsystem>_semaphores_create()`/`topology_setup()`-style convention
   (`03`, `07`) of one fallible setup function main() checks before creating
   tasks. The only behavioural difference: on failure, main() now decides
   how to halt (`while (1)`, same as every other creation failure in this
   catalogue) instead of the failure being buried inside the setup code
   itself — the system still halts either way, only *where* that decision
   is made changed.
3. **`StatusCallback`/`LinkCallback` and the SSI/CGI handlers are no longer
   `static`.** They were `static` in the original monolithic file; splitting
   across files requires external linkage for anything called from a
   different `.c` than the one it's defined in (`tse_servers.c` calls
   `StatusCallback`/`LinkCallback`, `gemrtos_web_ssi_handler`, and
   `gemrtos_web_cgi_table`, all defined elsewhere). No change to what any of
   them do.

## Building and running

```
new_script.bat -std -ex 09_network_httpd --board m10_rgmii_project_msgdma ...
```

or, once the board tree exists:

```
gemrtos_build.bat -std -ex 09_network_httpd -qpf top -qsys q_sys
```

## Expected output (JTAG UART)

```
task periodic task running
task infinite-loop task running
tse_receive_server: netif->state = 0x...
IP address= 0.0.0.0
[ethernet] Acquired IP address via DHCP client for interface: en
[ethernet] IP address : 192.168.x.x
[ethernet] IP validation PASSED: configuration is coherent
SPEED= 1Gb, FULL, dhcp->state= 10, netif_ip_is_up= 1, Link_alive= 1, IP address: 192.168.x.x
...
```

`dhcp->state= 10` is `DHCP_STATE_BOUND` (`lwip/prot/dhcp.h`) -- `netif_ip_is_up()`
(`lwip_main.c`) only ever returns 1 once DHCP has reached that exact state, so a
healthy line always pairs `netif_ip_is_up= 1` with `dhcp->state= 10`. A state
other than 10 (5 = `DHCP_STATE_RENEWING` is the one to know, since it is a
normal, brief phase mid-lease) paired with `netif_ip_is_up= 0` is expected
and transient; the same pairing that never clears is the symptom of a defect
that has been fixed in the shipped code -- `sys_check_timeouts()` starved by RX
traffic, leaving DHCP stuck in RENEWING indefinitely.

Then, from a browser on the same network: `http://<the printed IP address>/`
— the status page's task-pool-size, uptime, IP-address, and both editable
rows should update every few seconds via AJAX, and the "Set" form should
apply immediately.

## Things to try

- Change the period/delay via the web form and watch the corresponding
  `task ... running` cadence on the UART change to match, live.
- Unplug and replug the network cable — `LinkCallback`/`StatusCallback`
  (`net_init.c`) release and re-acquire the DHCP lease exactly as they did
  in `10_full_demo`, including the P1-P3 fixes.
- Compare `tse_receive_server`'s trigger-based mSGDMA wait against
  `06_triggers_and_isr`'s JTAG UART trigger driver — same `gu_TriggerWait()`
  mechanism, applied to a completely different peripheral.
