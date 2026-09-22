# GeMRTOS — Generic Embedded Multiprocessor RTOS

GeMRTOS is a real-time operating system for multiprocessor FPGA-based systems,
targeting Altera FPGAs with Nios V soft processors (Nios II is supported as a legacy
route). It provides
a hardware-assisted kernel with hybrid partition scheduling, a timer-tickless
scheduler, and event-driven task management across multiple processors sharing a
single system bus.

## License

GeMRTOS has two kinds of material, licensed separately.

**The software** — the kernel, drivers, hardware abstraction layer, public API headers, example applications
and documentation — is licensed under the **PolyForm Noncommercial License 1.0.0** (`LICENSE.md`). It
permits use, copying, modification and distribution for any noncommercial purpose as that licence defines
it. Any other use requires a separate written agreement with GeMRTOS.

**The hardware IP** — the GeMRTOS Controller, a Platform Designer component — is a separate proprietary
work. It is free of charge for academic and research use; commercial use requires a separate written
agreement with GeMRTOS. It is delivered encrypted, and Quartus® Prime needs a FlexLM licence file (`.dat`)
to use encrypted IP. That file is a technical requirement of the encryption, not a commercial gate. The
downloadable files work with any network adapter; a file tied to one specific licence server is issued on
request.

Third-party components, including the lwIP networking stack, keep their own licences, which these terms do
not change; they are listed in `THIRD_PARTY_LICENSES.md`. Files in the published IP tree that it does not
list are GeMRTOS's own work, licensed as the component they accompany.

This is a summary for orientation; the licence texts are authoritative. It is not legal advice. For
commercial licensing, contact GeMRTOS through gemrtos.com.

Required Notice: Copyright GeMRTOS Project Contributors (https://gemrtos.com)

## Key Features

- **Multiprocessor scheduling** — N soft-processors share one kernel instance with
  hardware-assisted mutual exclusion via the GeMRTOS Controller IP
- **Hybrid partition scheduling** — tasks and processors are assigned to scheduling
  lists, combining benefits of global and partitioned scheduling
- **Timer-tickless scheduler** — no periodic tick interrupt; events fire at exact
  scheduled times, minimising overhead
- **Event-driven model** — triggers map hardware IRQs and software events to ISR tasks;
  semaphores and message queues for inter-task communication
- **Eleven example applications** — each with a `verify.expect` file of the output it
  must produce, so its behaviour is checked automatically
- **Windows simulation** — run the same kernel sources on a PC, with no FPGA
- **lwIP integration** — an HTTP server example with SSI/CGI pages
  (`09_network_httpd`), on boards with Ethernet

## Supported Platforms

| Processor | Architecture | Status |
|-----------|-------------|--------|
| Nios V | RISC-V (RV32) | Current path |
| Nios II | RISC | Legacy route — still supported |

Quartus® Prime edition follows from the device: **Standard** for MAX 10, **Pro** for
Agilex 3 (each edition supports devices the other does not).

| Board | Device | Level |
|-------|--------|-------|
| MAX 10 development board | MAX 10 | **Run on hardware** — the whole example catalogue, each example's output checked automatically |
| BeMicro SDK | Cyclone IV E | Project available |
| Atum A3 | Agilex 3 | Project available; the networked demo runs on the board over gigabit Ethernet with DHCP (not among the published demos) |

"Project available" means a board project exists and builds; the example catalogue has
not been run there.

Porting to another processor means supplying the register seam (`IORD`/`IOWR`), a
context-switch assembly file and the C-runtime hooks. See [PORTING.md](PORTING.md).

## System Requirements

- Quartus® Prime — Standard edition for MAX 10 devices, Pro edition for Agilex 3
- Nios V: the Nios V tools that install with Quartus® Prime. Nios II (legacy route):
  the Nios II tools, run under WSL through `gemrtos_build.sh`
- The GeMRTOS FlexLM licence file for the encrypted Hardware IP (see License above)
- Windows simulation only: a MinGW GCC toolchain

## Repository Structure

```
GeMRTOS/ip/                  The IP components — GeMRTOS_Multiprocessor,
                             GeMRTOS_controller, GeMRTOS_Bridge, GeMRTOS_Monitor,
                             gemrtos_sdram. The kernel and HAL sources are in
                             GeMRTOS_controller/HAL/, the eleven examples in
                             GeMRTOS_Multiprocessor/examples/
demos/<board>/               Board demo projects (MAX10FPGA, MAX10_RGMII, bemicroSDK),
                             each with its own copy of the ip/ tree
license/                     FlexLM licence files for the encrypted hardware IP
LICENSE.md                   PolyForm Noncommercial License 1.0.0 (official text)
THIRD_PARTY_LICENSES.md      Third-party component disclosures
```

## Quick Start

1. Download the GeMRTOS FlexLM licence file at [gemrtos.com/license](https://gemrtos.com/license) (see License above)
2. Copy the `GeMRTOS/ip/` folder into your Quartus® Prime project directory as `ip/`
   (the components must sit directly inside it; see `GeMRTOS/ip/README.md`)
3. In Platform Designer add the **GeMRTOS Multiprocessor** component; configure the
   processor count, processor type and clock, then generate the system
4. Platform Designer copies `gemrtos_build.bat` (Nios V) or `gemrtos_build.sh` (Nios II)
   into your project directory. Build with
   `gemrtos_build.bat -std -qpf <your_qpf> -qsys <your_qsys>` (`-pro` for the Pro edition;
   `-ex <NN_name>` selects one of the examples, `-ex-list` lists them, `-help` shows the rest)
5. In your application call `gu_TaskCreate()`, `gu_TaskTypeSet()`, `gu_TaskStartWithOffset()`
   in `main()`, then return — GeMRTOS takes over

`GeMRTOS/ip/GeMRTOS_Multiprocessor/examples/01_hello_task/main.c` is the smallest complete
application.

## Documentation

The User and Development Manual — architecture, controller, IP integration, design flow,
programming guide, examples, debugging, the API reference and the Windows simulation — is
published at [gemrtos.com/gemrtos-manuals](https://gemrtos.com/gemrtos-manuals/), whole and
chapter by chapter. Each example has its own `README.md` with the output to expect.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).

## Porting to New Hardware

See [PORTING.md](PORTING.md).
