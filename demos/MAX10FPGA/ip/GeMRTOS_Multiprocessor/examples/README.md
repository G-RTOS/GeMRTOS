# GeMRTOS Example Applications

Each subdirectory is one complete, self-contained example application. The
directory contents are exactly what gets compiled — there is no shared support
library to chase through, and no `#ifdef` selecting between examples.

## Selecting an example

```
new_script.bat -std -ex 10_full_demo --board <board> ...
```

or directly:

```
gemrtos_build.bat -std -ex 10_full_demo -qpf top -qsys q_sys
gemrtos_build.bat -ex-list          # list examples, build nothing
```

**Omitting `-ex` builds the full demo into `software/hellogemrtos/`, exactly as
before this mechanism existed.** Nothing changes for an existing workflow.

## How selection works

The build copies the selected example's files into its own application
directory under `software/`, overwriting same-named files and leaving anything
else in place. **Nothing is deleted** — a build script must not remove files
from a directory you may have put your own work into.

Each example gets its **own** application directory and its own BSP, so two
examples cannot contaminate each other. The mapping from example name to
application directory is declared in `manifest.txt`, not derived from the
directory name: `10_full_demo` deliberately maps to `hellogemrtos` so that
every existing document, script and tool path keeps working.

If you force two examples into one directory with an explicit `-app`, the build
warns you and continues — it will not delete your files to make room. Expect a
duplicate `main()` at link time; that is what the warning is telling you.

## Editing an example

Edit it **here**, in `misc/gemrtos_ips/GeMRTOS_Multiprocessor/examples/`.

`software/<app>/` is a build output: `new_script.bat` copies this whole IP tree
into `<board>/ip/` on every run, and the build then deploys from there. Changes
made directly under `software/` are overwritten without warning.

## Catalogue

| Example | Application dir | What it shows |
|---|---|---|
| `01_hello_task` | `01_hello_task` | Minimum viable GeMRTOS application — one task printing over the JTAG UART. Board smoke test. |
| `02_tasks_and_delays` | `02_tasks_and_delays` | Task model: periodic vs one-shot tasks, ready/run priority, start offsets, external suspend/resume. |
| `03_semaphores` | `03_semaphores` | All four semaphore flavours, acquisition timeouts, and the static priority-ceiling protocol. First multi-file example. |
| `04_dining_philosophers` | `04_dining_philosophers` | Classic contention/deadlock-avoidance stress demo — extracted from `10_full_demo` as a behaviour-preserving move. |
| `05_message_queue_pubsub` | `05_message_queue_pubsub` | Message queues as publish/subscribe — fan-out broadcast, guaranteed partial-delivery/timeout path, late-subscriber semantics. |
| `06_triggers_and_isr` | `06_triggers_and_isr` | Hardware triggers and ISR-as-task using the JTAG UART's real interrupt — happy path and trigger timeout path. Own copy of the JTAG UART trigger driver. |
| `07_multiprocessor_scheduling` | `07_multiprocessor_scheduling` | Multiple scheduling lists — partitioned scheduling (CPU affinity), global scheduling, and mutual exclusion between lists with no semaphore involved. Requires `G_NUMBER_OF_PCB >= 2` (compile-time `#error` otherwise). |
| `08_signals_and_frozen_mode` | `08_signals_and_frozen_mode` | Fault handling and robustness — a deliberate task-deadline overrun (`G_SCBType_TCB_ABORTED`) and a deliberate timed-event service backlog (`G_SCBType_FROZEN_MODE`/`UNFROZEN_MODE`), both self-recovering, neither halting the kernel. |
| `09_network_httpd` | `09_network_httpd` | Network showcase — LwIP + raw HTTP server with SSI status variables, CGI-editable runtime parameters, and client-side AJAX live updates. No `fs/` of its own; references the shared `GeMRTOS_Monitor` pages. |
| `10_full_demo` | `hellogemrtos` | Everything combined — philosophers, periodic and one-shot tasks, JTAG UART server, LwIP with an SSI/CGI web status page. The regression reference: this is the configuration verified on hardware. |
| `11_watchdog_recovery` | `11_watchdog_recovery` | Per-processor watchdog recovery — a task pinned to processor 1 hangs deliberately, the watchdog fires, and processor 1 is reset and brought back through the `avalon_monitor` reset-vector redirection while a task on processor 2 keeps running. Requires `G_NUMBER_OF_PCB >= 2` and the `avalon_monitor`. |


## Adding an example

1. Create `examples/<NN_name>/` with its sources and a `README.md` stating what
   it demonstrates and what output to expect.
2. Add one line to `manifest.txt` (no spaces around the `|` separators).
3. Add a `verify.expect` file — one pattern per line, all of which must appear
   in the UART capture — so the verification matrix can check it automatically.

Examples are deliberately self-contained: if two of them need the same
supporting code, it is duplicated rather than shared. Being able to read one
directory and see everything that runs is worth more here than avoiding
duplication.
