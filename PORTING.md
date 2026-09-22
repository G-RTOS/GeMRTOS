# Porting GeMRTOS to a New Processor or Platform

This guide describes what a new port has to supply. It is written from the
implementation as it is today, not from an intended design — where the code is
less tidy than a port author would like, this guide says so.

Paths are relative to `GeMRTOS/ip/` in this repository.

## What exists today

GeMRTOS has three implementations of its platform boundary. There is no fourth;
nothing in this guide has been exercised on any other processor.

| | Nios II / Nios V (FPGA) | Windows simulation |
|---|---|---|
| Register access | Altera `IORD` / `IOWR` on the Avalon bus | C functions over a software model of the controller's register file (`GeMRTOS_Multiprocessor/gemrtos_windows/gemrtos_controller_windows.c`) |
| Context switch | `GeMRTOS_controller/HAL/src/grtos_switch_nios2.S`, `grtos_switch_niosV.S` | Windows fibers (`gemrtos_windows/grtos_fiber.c`) |
| Processor | Nios II or Nios V soft core, one per GeMRTOS processor | one OS thread per GeMRTOS processor |
| Mutual exclusion | the Controller IP's hardware mutex | emulated in the register-write handler |
| Standard output | JTAG UART through newlib | host `printf` |

The Windows simulation is a complete, independently built second implementation
of the boundary. It is the best worked example of what a port supplies, and the
quickest way to find out what a change to the boundary breaks.

## The boundary is a layer, not a module

The portable kernel reaches hardware only through the `gm_*` primitives in
`GeMRTOS_controller/HAL/inc/gemrtos_primitives.h`, the register map in
`GeMRTOS_controller/inc/grtos_regs.h`, the context-switch assembly, and
`GeMRTOS_controller/HAL/src/gemrtos_hal.c`. Most `gm_*` primitives are
`static inline` functions built on two macros, `gm_IORD` and `gm_IOWR`, which
delegate to `IORD` and `IOWR`. **Those two are the seam**: everything the kernel
does to the Controller IP passes through them. A port therefore does not
re-implement `gm_ProcessorId()`, `gm_IsPrcGranted()` or the critical-section
macros — it makes the register reads and writes they perform behave as the
Controller IP's registers do.

This is a convention, not an enforced rule: nothing in the build stops kernel code
from calling `IORD` directly.

## What a port supplies

1. **`IORD` / `IOWR`** with the Controller IP's register semantics
   (`grtos_regs.h` defines every register and bit; the Windows model is the
   executable specification of the behaviour).
2. **A context-switch mechanism**, and a per-processor boot entry.
3. **The C-runtime hooks** in `gemrtos_hal.c`.
4. **The platform values** the kernel expects to find defined.
5. **The matching branch** of the architecture detection in
   `GeMRTOS_controller/HAL/inc/gemrtos.h`.

## Contract

1. Portable code contains no raw `IORD`/`IOWR`, no direct processor-register access
   and no operating-system calls: everything goes through `gm_*`, the
   `GRTOS_CMD_*` mutex commands and the context-switch symbols.
2. Critical-section entry and exit balance. The task's `TCB_MTX_NESTED` count
   tracks re-entrancy and is restored into the controller on every context return.
3. The context-switch assembly reads the layout of kernel structures at run time
   through globals that `gk_Init_AssemblerInterface()` fills in before the first
   switch (`gs_sizeof_G_PCBTbl`, `gs_addressof_G_PCBTbl`, `gs_offsetof_PCB_IDLETCB`,
   `gs_offsetof_PCB_EXECTCB`, `gs_offsetof_TCB_StackPointer`,
   `gs_offsetof_PCB_SUSPENDED_STK_PTR`). A port reads those; it does not hard-code
   offsets.
4. A port provides mechanisms only. Scheduling, memory and synchronisation policy
   stay in the kernel.

## Step by step

### 1. Architecture detection

`GeMRTOS_controller/HAL/inc/gemrtos.h` defines `__niosX_arch__` when the compiler
defines `__nios2_arch__` or `__riscv`, and includes the processor's own headers.
A new embedded target adds its branch here. Code that differs between embedded and
Windows builds is selected with `#ifdef __niosX_arch__`; keep any new
platform-specific code behind the same kind of switch and out of shared algorithms.

### 2. The register seam

Provide `IORD(base, register)` and `IOWR(base, register, data)` with the same
signature as the Altera HAL macros. `gm_IORD` / `gm_IOWR` cast the base address to
`uintptr_t` and pass the register index through unchanged. The controller's
registers are 32 bits wide.

### 3. Platform values

The kernel expects these to be defined by the platform description. On the FPGA
targets the BSP's generated `system.h` provides them; the Windows port provides
them in its own header.

- `GEMRTOS_DRIVER_BASE` — base address of the Controller IP's global register block
- `GEMRTOS_DRIVER_GRTOSFREQUENCY` and `GEMRTOS_DRIVER_PRESCALE` — the controller
  clock and the prescaler that yields the system time unit (10 MHz by default);
  `G_TICKS_PER_SECOND` is derived from them

### 4. Task stack frame

Each task's saved context is a frame of `G_STACK_STATUS_LENGTH` words, defined per
architecture in `GeMRTOS_controller/HAL/inc/gemrtos_core.h` (28 for Nios II, 34
for Nios V). `gk_TASK_STK_INIT` builds a new task's first frame; the assembly that
restores a frame must agree with it word for word. A new architecture defines its
own length and its own frame layout, and both sides are changed together.

### 5. Context switch and boot

Both Nios assembly files define the same twelve labels, and the kernel's C code
depends on them. What each is for, as far as the sources establish it:

| Label | Role |
|---|---|
| `GRTOS_Suspend_Task` | called by `gk_KERNEL_TASK_SUSPEND_CURRENT`, in the critical section: leaves the running task |
| `GRTOS_Start_Task` | starts executing a task from its saved frame; the processor does not return from it |
| `GRTOS_Change_SP_to_IDLE` | loads the stack pointer of the processor's idle task and continues in `gk_KERNEL_TASK_COMPLETE` |
| `GRTOS_Return_from_Signal` | return point from executing a signal handler |
| `grtos_irq_entry` | the interrupt entry point |
| `GRTOS_RETURN_CONTEXT`, `GRTOS_RETURN_CONTEXT_END`, `GRTOS_int_ret_code` | a block of code that `gk_INIT_KERNEL` copies, word by word, into the controller's memory so that returning to a task re-enables the mutex and interrupts |
| `GRTOS_jump_to_reset`, `GRTOS_jump_to_reset_end` | a second block copied into the controller the same way; an absolute jump, because it runs from the controller's memory |
| `grtos_start_rest_of_processors` | boot entry for processors 2..N |
| `Switch_to_idle` | switch to the idle context (not further documented in the source) |

The assembly in turn calls C entry points that belong to the kernel, not to the
port: `gk_KERNEL_TASK_COMPLETE`, `gk_ENTRY_SIGNAL_RETURN` and
`gk_ENTRY_BAD_TASK_RETURN`. A new architecture reimplements the assembly and keeps
those.

The two Nios files are the reference; where they differ, the difference is an
architecture decision a new port has to make for itself.

### 6. C-runtime hooks

`GeMRTOS_controller/HAL/src/gemrtos_hal.c` supplies the standard-output path
(`gu_printf`, `gu_fprintf`) and newlib's locking hooks (`__malloc_lock`,
`__malloc_unlock`, `__env_lock`, `__env_unlock`), which serialise on the GeMRTOS
critical section. A port with a different C library provides the equivalents.

## Testing a port

1. Build with warnings enabled at `-O0` and `-O2`.
2. Run `01_hello_task` first: it is the smallest complete application, one task
   printing to the console.
3. Run the rest of the catalogue in `GeMRTOS_Multiprocessor/examples/`. Each example
   has a `verify.expect` file listing the output it must produce, so a port can be
   judged against the same checks the supported targets pass. Examples that need two
   processors, Ethernet or LEDs say so in `manifest.txt`.
4. Compare against the Windows simulation for the same example: the same kernel
   sources should produce the same behaviour.

## Limits

- This guide covers the **software** side. The GeMRTOS Controller is delivered as
  an encrypted Platform Designer component for Altera devices; using it with a
  different processor or bus is a hardware question this document does not answer.
- The boundary is documented and consistent but not statically enforced (see
  "The boundary is a layer, not a module").
- Only the three implementations in the table above have ever been built and run.

For a processor or platform this guide does not cover, contact GeMRTOS through
[gemrtos.com](https://gemrtos.com).
