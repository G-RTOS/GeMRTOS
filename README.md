-   [Introduction](#introduction)
-   [GRTOS system architecture](#grtos-system-architecture)
    -   [GRTOS with Intel NIOS II
        processors](#grtos-with-intel-nios-ii-processors)
    -   [GRTOS Controller](#grtos-controller)
    -   [GRTOS controller registers](#grtos-controller-registers)
    -   [Mutex module 210](#mutex-module-210)
    -   [GRTOS controller time module](#grtos-controller-time-module)
    -   [Interrupt input module and interrupt output
        module](#interrupt-input-module-and-interrupt-output-module)
    -   [Event module](#event-module)
-   [System components and Data
    Structures](#system-components-and-data-structures)
    -   [Task ](#task)
    -   [Events ](#events)
    -   [Time](#time)
    -   [Resources](#resources)
    -   [Signals](#signals)
    -   [Ready and Running Linked Lists (TCBRDYL list and TCBRUNL
        list)](#ready-and-running-linked-lists-tcbrdyl-list-and-tcbrunl-list)
    -   [Processors](#processors)
    -   [RRDS data structure](#rrds-data-structure)
-   [Control block fields](#control-block-fields)
    -   [KCB data structure 1200](#kcb-data-structure-1200)
    -   [TCB data structure](#tcb-data-structure)
    -   [ECB data structure](#ecb-data-structure)
    -   [RCB data structure](#rcb-data-structure)
    -   [SCB data structure](#scb-data-structure)
    -   [LCB data structure](#lcb-data-structure)
    -   [PCB data structure](#pcb-data-structure)
    -   [RRDS data structure](#rrds-data-structure-1)
    -   [GRTOS structure](#grtos-structure)
    -   [Initialization section](#initialization-section)
    -   [Task switch section](#task-switch-section)
    -   [Interrupt handler section](#interrupt-handler-section)
    -   [System call section](#system-call-section)
    -   [Task suspend section](#task-suspend-section)
    -   [Task complete section](#task-complete-section)
-   [Priority Management](#priority-management)
-   [A "Hello World" for GRTOS](#a-hello-world-for-grtos)
-   [Configuration functions](#configuration-functions)
    -   [gu_Set_Default_Task_Type(unsigned int
        type)](#gu_set_default_task_typeunsigned-int-type)

# Introduction

The GRTOS is a multiprocessor system architecture for facilitating
real-time multitasking processing. In GRTOS, each task is assigned to a
list of tasks and one or more processors may serve that list. Processors
may serve one or more lists of tasks, thereby allowing the
predictability of partitioned scheduling with a flexible balance of
processor loads. Scheduling overhead is reduced using a timer-tickless
scheduler. Floating scheduling reduces the overhead for the highest
priority tasks and isolates the design of the system from the number of
processors of the hardware architecture. The generic feature allows
partial configuring global and partitioned scheduling.

Scheduling mechanism, priority disciplines, number of processors and
fault-tolerance strategies can be global or partitioned and partially or
totally implemented in the generic, timer-tickless and event-driven
GRTOS system. The event-driven nature of the GRTOS system lets it
produce low-overhead and high predictability. The frozen and unfrozen
modes allow a responsive mechanism suitable for both fault tolerance
implementations and the management of saturated situations.

# GRTOS system architecture

In the GRTOS system architecture, a set of *N* processors shares a
common memory and *M* input/output devices through a common bus. FIG. 1
shows a layout of the GRTIOS controller interconnection in a
multiprocessor architecture. The GRTOS controller receives device
interrupt requests (DIRQs) (101-103) from input/output peripheral
devices (104-106). The GRTOS controller sends processor interrupt
requests (PIRQs) (107-109) to the system processors (110-112). The
system bus (113) allows the processors (110-112) to share the system
memory (114). Each processor has a unique internal identification
register CPUID (123-125).

FIG. 1: GRTOS multiprocessor system layout.

## GRTOS with Intel NIOS II processors

The GRTOS multiprocessor system is implemented based on the NIOS II
processor of Intel Corporation. The system architecture is designed
using the Platform Designer tool from Intel Corporation. Table 1 shows
the components that includes the GRTOS multiprocessor architecture based
on NIOS II processors.

  Component                                   Quantity (i)       Name
  ------------------------------------------- ------------------ ---------------------
  Clock Source                                1                  clk_0
  Nios II Processor                           1..NProcessors     nios2_qsys\_\$i
  Nios II Avalon monitor module               1                  nios_avalon_monitor
  Nios II Avalon bridge                       1                  nios_avalon_bridge
  gRTOS                                       1                  grtos_0
  JTAG UART Intel FPGA IP                     0..NProcessors+1   jtag_uart\_\$i
  Interval Timer Intel FPGA IP                1                  timer_0
  On-Chip Memory (RAM or ROM) Intel FGPA IP   2                  onchip_memory1\_\$i
  System ID Peripheral Intel FPGA IP          1                  sysid_qsys_0
  JTAG to Avalon Master Bridge                1                  master_0
  IRQ Bridge                                  1                  irq_bridge_0
  Reset Bridge                                1                  reset_bridge_0

Table 1: GRTOS multiprocessor components in Intel Corporation's Platform
Designer.

Table 2 shows the memory requirements of each component of the GRTOS
multiprocessor architecture.

+----------+----------+----------+----------+----------+-------+
| C        | Quantity | I        | Address  |          | Order |
| omponent | (i)      | nterface | span     |          |       |
+==========+==========+==========+==========+==========+=======+
| Nios II  | 1..NPr   | debug_me | 7FFH     | 0x0      | 3     |
| P        | ocessors | m_master |          | 400_2800 |       |
| rocessor |          |          |          |          |       |
|          |          |          |          | 0x0      |       |
|          |          |          |          | 400_3000 |       |
|          |          |          |          |          |       |
|          |          |          |          | 0x0      |       |
|          |          |          |          | 400_3800 |       |
+----------+----------+----------+----------+----------+-------+
| Nios II  | 1        | s_Global | FFH      | 0x0      | 5     |
| Avalon   |          |          |          | 400_4200 |       |
| monitor  |          |          |          |          |       |
| module   |          |          |          | 0x0      |       |
|          |          |          |          | 400_4300 |       |
+----------+----------+----------+----------+----------+-------+
| Nios II  | 1..NPr   | s\$i     | 0x0 --   |          |       |
| Avalon   | ocessors |          | 0xF      |          |       |
| monitor  |          |          | FFF_FFFF |          |       |
| module   |          |          |          |          |       |
+----------+----------+----------+----------+----------+-------+
| Nios II  | 1..NPr   | s\$i     | 0x0 --   |          |       |
| Avalon   | ocessors |          | 0xF      |          |       |
| bridge   |          |          | FFF_FFFF |          |       |
+----------+----------+----------+----------+----------+-------+
| gRTOS    | 1        | s_Global | 1FFH     | 0x0      | 4     |
|          |          |          |          | 400_4000 |       |
+----------+----------+----------+----------+----------+-------+
| gRTOS    | 1..NPr   | s_proc   | 7H       | 0x0      | 7     |
|          | ocessors | essor\$i |          | 400_4420 |       |
|          |          |          |          |          |       |
|          |          |          |          | 0x0      |       |
|          |          |          |          | 400_4428 |       |
|          |          |          |          |          |       |
|          |          |          |          | 0x0      |       |
|          |          |          |          | 400_4430 |       |
+----------+----------+----------+----------+----------+-------+
| gRTOS    | 1        | s_p      | 7H       | 0x0      | 8     |
|          |          | rocessor |          | 400_4438 |       |
|          |          | _monitor |          |          |       |
+----------+----------+----------+----------+----------+-------+
| JTAG     | 0..NProc | a        | 7H       | 0x0      | 9     |
| UART     | essors+1 | valon_jt |          | 400_4440 |       |
| Intel    |          | ag_slave |          |          |       |
| FPGA IP  |          |          |          | 0x0      |       |
|          |          |          |          | 400_4448 |       |
|          |          |          |          |          |       |
|          |          |          |          | 0x0      |       |
|          |          |          |          | 400_4450 |       |
|          |          |          |          |          |       |
|          |          |          |          | 0x0      |       |
|          |          |          |          | 400_4458 |       |
|          |          |          |          |          |       |
|          |          |          |          | 0x0      |       |
|          |          |          |          | 400_4460 |       |
+----------+----------+----------+----------+----------+-------+
| Interval | 1        | s1       | 1FH      | 0x0      | 6     |
| Timer    |          |          |          | 400_4400 |       |
| Intel    |          |          |          |          |       |
| FPGA IP  |          |          |          |          |       |
+----------+----------+----------+----------+----------+-------+
| onchip_m | 1        | s1       | 7FFH     | 0x0      | 2     |
| emory1_0 |          |          |          | 400_2000 |       |
+----------+----------+----------+----------+----------+-------+
| onchip_m | 1        | s1       | 1FFFH    | 0x0      | 1     |
| emory1_1 |          |          |          | 400_0000 |       |
+----------+----------+----------+----------+----------+-------+
| sysi     | 1        | contr    | 7H       | 0x0      | 10    |
| d_qsys_0 |          | ol_slave |          | 400_4468 |       |
+----------+----------+----------+----------+----------+-------+

Table 2: GRTOS multiprocessor component memory requirements.

## GRTOS Controller

The GRTOS controller manages and synchronizes all the hardware events in
the system. As shown in FIG. 2, the GRTOS controller includes the
interrupt input module (200) containing device interrupt request inputs
(DIRQIs) (201-203) that receives the device interrupt requests (DIRQs)
(101-103), the interrupt output module (204) containing processor
interrupt request outputs (PIRQOs) (205-207) sending the processor
interrupt requests (PIRQs) (107-109), the controller registers (208),
the time module (209), the mutex module (210), the event module (211)
and the addressable control logic (212) including an interface element
(213) and a decoder element (214).

FIG. 2: GRTOS controller architecture.

## GRTOS controller registers

The behavior of the GRTOS controller is determined by a set of GRTOS
controller registers (208). Table 1 shows the GRTOS controller
registers. Some of these registers may be configured through the
interface element of the GRTOS controller. They are assigned with a
unique memory map address to be read and written. For instance, the
R_CTRL register can be used to configure some functions of the GRTOS
controller such as enabling or disabling the frozen mode.

  R_CTRL             
  ------------------ --
  R_MTX_PRC_GRN      
  R_MTX_CLR_CNT      
  R_MTX_NXT_PRC      
  R_MTX_RSV_PRC      
  R_NXT_EVN_CNT      
  R_TM_PSC           
  R_TM_CNT           
  R_FRZ_TM_CNT       
  R_TM_PSC_CNT       
  R_NXT_EVN_TM       
  R_FRZ_TM_THR       
  R_NXT_TM_EVN_PRC   
  R_LOW_PRI_PRC      
  R_TRG_PRC_INT      
  R_INT_ENB          
  R_IRQ_ENB          
  R_INT_CLR          
  R_IRQ_PND          
  R_LST_EVN          

Table 1: GRTOS controller registers.

### GRTOS controller: internal signals

GRTOS controller uses internal signals to manage the different events of
the multiprocessor hardware architecture. Table 2 shows the internal
signals of the GRTOS controller.

+--------------------+------------------------------------------------+
| C1_MTX_GRN         | > C1_MTX_GRN signal is 0 when the              |
|                    | > R_MTX_PRC_GRN register is equal to 0;        |
|                    | > otherwise, it is 1.                          |
+====================+================================================+
| C1_NXT_EVN_PRC_ZRO |                                                |
+--------------------+------------------------------------------------+
| R1_IRQ_PND         |                                                |
+--------------------+------------------------------------------------+
| R1_FRZ_MDE_ENB     |                                                |
+--------------------+------------------------------------------------+
| C1_FRZ_MDE         |                                                |
+--------------------+------------------------------------------------+
| C1_EVN_TM_OCC      |                                                |
+--------------------+------------------------------------------------+
| C1_IRQ_PND         |                                                |
+--------------------+------------------------------------------------+

Table 2: GRTOS controller registers.

The states of these signals determines the behavior of the GRTOS
controller.

## Mutex module 210

In a multiprocessor system, a hardware-implemented mutex is required to
preserve coherency in critical sections among the processors. The
R_MTX_PRC_GRN register, it is called the *mutex register*, is the main
register of the mutex module and it is used to implement a critical
section among the processors. A processor may execute a critical section
when the value of the R_MTX_PRC_GRN register is equal to its internal
identification register CPUID. The R_MTX_PRC_GRN register can take three
different kinds of values:

-   0: there is no processor that grants the critical section. A
    processor may request the mutex when the R_MTX_PRC_GRN register
    equals to 0.

-   values from 1 to *N*: the critical section is granted to the
    processor with internal identification register CPUID equal to the
    R_MTX_PRC_GRN register.

-   constant MTX_BLCK: the critical section is blocked when the
    R_MTX_CLR_CNT register is different from 0. The value MTX_BLCK is
    assigned to the R_MTX_PRC_GRN register when the critical section is
    released by the processor that grants it. This mechanism blocks the
    critical section until the count of the R_MTX_CLR_CNT register
    reaches a value equal to 0. The next processor to grant the critical
    section is stored in the R_MTX_NXT_PRC register. The R_MTX_NXT_PRC
    register is used to avoid an active competition among the processors
    that are requesting access to the mutex.

Before a processor starts executing a critical section code, the
processor may request the mutex by repeatedly executing a mutex request
and verifying that the mutex is granted to it by checking that the mutex
register holds its identification CPUID. However, this mechanism may be
inefficient when several processors simultaneously request the mutex. A
more efficient mechanism is implemented using the R_MTX_RSV_PRC
register. Each processor sets the corresponding bit of the R_MTX_RSV_PRC
register according to its internal identification register CPUID
executing the

GRTOS_USER_CRITICAL_SECTION_GET

instruction when the processor requests access to the mutex. The GRTOS
controller halts a processor when its corresponding bit is set to 1 in
the R_MTX_RSV_PRC register and the mutex is not granted to it. The
processor halt mechanism is implemented by asserting a wait_request
signal for each processor in the interface element of the GRTOS
controller. The processor that grants the critical section executes the

GRTOS_USER_CRITICAL_SECTION_RELEASE

instruction to release the mutex in order to leave the critical section
code.

## GRTOS controller time module

Most applications require time support. For an adequate and flexible
time basis, the system clock signal is prescaled through the up counter
R_TM_PSC_CNT register, as shown in FIG. 3.

FIG. 3: GRTOS time prescaler.

The modulus of the up-counter R_TM_PSC_CNT register is equal to the
value configured in the R_TM_PSC register plus 1. The R_TM_PSC register
should be configured to obtain the desired system time unit, prescaling
the CLK signal. The value stored in the R_TM_PSC register is set by
executing a

GRTOS_CMD_SET_TIME_PRESCALE(scale)

instruction.

### GRTOS controller time modes

The GRTOS controller may stay in one of two time modes:

-   **unfrozen mode**: the GRTOS controller starts and remains in
    unfrozen mode while the condition (1) holds:

+-----------------------------------------------+-----+
| (R_FRZ_TM_THR + R_NXT_EVN_CNT \< R_TM_CNT and | (1) |
|                                               |     |
| C1_FRZ_MDE_ENB == 1)                          |     |
|                                               |     |
| or                                            |     |
|                                               |     |
| C1_FRZ_MDE_ENB == 0                           |     |
+-----------------------------------------------+-----+

The up-counter R_TM_CNT, it is called the time counting register, is
incremented at the system time unit rate when the GRTOS controller is in
unfrozen mode.

-   **frozen mode**: the GRTOS controller switches to frozen mode when
    the C1_FRZ_MDE_ENB is equal to one (frozen mode enabled) and the
    time elapsed since the next time event, stored in the R_NXT_EVN_CNT,
    is greater than the threshold time configured in the R_FRZ_TM_THR.
    In frozen mode, the up-counter R_FRZ_TM_CNT, it is called the
    *frozen time counting register*, is incremented at the system time
    unit rate.

During runtime, the normal mode should be the unfrozen mode. In this
mode, the system time is held in the up counter R_TM_CNT register. The
GRTOS controller holds the next time event in the R_NXT_EVN_CNT
register. Each time that the up counter R_TM_CNT register reaches the
value stored in the R_NXT_EVN_CNT register, a time event is considered
to have occurred. If the code associated with the time event is being
executed by a system processor (the C1_NXT_EVN_PRC_ZRO signal is
asserted), then the processor executing such code is interrupted;
otherwise, the system processor with the lowest priority is interrupted.
The interrupted system processor should execute the code to process the
time event and then the R_NXT_EVN_CNT register should be updated with
the next system event. In this mode, the events are processed according
to the time they happen. However, if the event processing takes longer
than a certain threshold, it becomes convenient in some applications to
freeze this dynamic in order to avoid a domino effect over the system
events. When the frozen mode is enabled (C1_FRZ_MDE_ENB signal is equal
to one) and the next event remains unprocessed, with an interval equal
to or greater than the time defined in the R_FRZ_TM_THR register, then
the GRTOS controller apparatus switches to frozen mode. In frozen mode,
the up-counter R_TM_CNT register is frozen and the up-counter
R_FRZ_TM_CNT register is incremented at the system time unit rate.
Consequently, the up-counter R_TM_CNT register keeps the time (in system
time units) that the GRTOS controller remains in unfrozen mode, while
the up counter R_FRZ_TM_CNT register keeps the time (in system time
units) that the GRTOS controller remains in frozen mode. When the GRTOS
controller switches to frozen mode, the GRTOS controller produces an
event (C1_FRZ_EVN). When all the time events are processed (R_TM_CNT \>
R_NXT_EVN_CNT), the GRTOS controller switches back to unfrozen mode.

### Unfrozen/frozen related instructions

  GRTOS_CMD_FRZ_ENB_SET            Enables the frozen mode event
  -------------------------------- ------------------------------------------------------------------------------------------------
  GRTOS_CMD_FRZ_DSB_SET            Disables the frozen mode event
  GRTOS_CMD_GET_FRZ_ENB            Returns the status of the frozen mode (G_TRUE if enabled, G_FALSE if disabled)
  GRTOS_CMD_GET_FRZ_ACT            Returns the status of the frozen mode event (G_TRUE if active, G_FALSE if inactive)
  GRTOS_CMD_FRZ_EVN_CLR            Clears the C1_FRZ_EVN flags. It should be executed when the frozen mode strategy is completed.
  GRTOS_CMD_FRZ_TM_THR_SET(time)   Sets the Frozen Time Threshold register (R_FRZ_TM_THR) of the GRTOS controller.
  GRTOS_CMD_FRZ_TM_THR_GET         Returns the value of the Frozen Time Threshold (R_FRZ_TM_THR) register of the GRTOS controller

## Interrupt input module and interrupt output module

For most cases, an application is implemented as a set of tasks that has
to be executed by system processors. A task may be defined as a program
code that is executed by a processor in order to produce a certain
function of an application. Because there may be more tasks than system
processors a priority is assigned to each task in order to determine the
order that tasks are scheduled on the system . Higher priority tasks are
executed first. Lower priority tasks are executed when highest priority
tasks do not require system processors to be executed.

The pattern of the tasks requiring execution changes during runtime
according to the events that occur. When one event takes place, a system
processor must be interrupted in order to execute the code that
processes that event. At a GRTOS controller level, events that produce
processor interrupt requests (PIRQs) , may be classified as:

-   **time events**: these events are configured to happen at a certain
    system time. Time events may be sorted according to the occurrence
    time in a total order relationship. The next time the event is the
    earliest time event. Several embodiments may be utilized to break
    ties when two or more time events are configured with the same
    occurrence time. A time event takes place when the following
    condition holds:

R_NXT_EVN_CNT ≥ R_TM_CNT (1)

where the R_NXT_EVN_CNT register holds the earliest occurrence time
event and the up-counter R_TM_CNT register keeps the system time. The
C1_EVN_TM_OCC signal is asserted when Equation (1) is true; otherwise,
it is de-asserted. In the R_NXT_TM_EVN_PRC register, the bit
corresponding to the processor that executes the task associated with
the next timed event is set to 1. Otherwise, if the next time event is
not associated with an executing task, all the bits of the
R_NXT_TM_EVN_PRC register should be set to 0.

-   **device interrupt requests (DIRQs) events**: these events are
    produced by the input/output peripheral devices connected to the of
    input/output peripheral device ports. Most of these events require
    the execution of the associated interrupt service routine (ISR) in
    order to satisfy the demand of the input/output peripheral device.
    Interrupt requests from device interrupt requests (DIRQs) may be
    enabled or disabled by setting or clearing, respectively, the
    corresponding bit in the R_IRQ_ENB register. Therefore, a device
    interrupt request (DIRQ) will produce a processor interrupt request
    (PIRQ) when there exists a request from that device and the
    corresponding bit in the R_IRQ_ENB register is enabled.

-   **internal interrupt trigger events**: a processor may interrupt
    another processor using the R_TRG_PRC_INT register. Setting the
    corresponding bit of the R_TRG_PRC_INT register triggers the
    interrupt (bit 0: processor 1, bit 1: processor 2, etc.).

The processor interrupt requests (PIRQs) may be enabled or disabled by
setting the corresponding bit of the R_INT_ENB register. When there
exists at least a pending interrupt, then the C1_IRQ_PND signal is set
to 1, otherwise, it is set to 0.

A processor should be interrupted (the corresponding processor interrupt
request (PIRQ) of the GRTOS controller should be asserted) when:

-   Critical section is not granted to any processor (the C1_MTX_GRN
    signal is not set), the processor interrupt is enabled (the
    corresponding bit of the R_INT_ENB register is set) and:

    a.  the processor is executing the task associated with the next
        time event (the corresponding bit of the R_NXT_TM_EVN_PRC
        register is set) and a time event occurred (the C1_EVN_TM_OCC
        signal is set), or

    b.  the processor is executing the lowest priority task (the
        corresponding bit of the R_LOW_PRI_PRC register is set) and
        there is an IRQ request pending (the C1_IRQ_PND signal is set),
        or

    c.  the processor is executing the lowest priority task (the
        corresponding bit of the R_LOW_PRI_PRC register is set), a time
        event occurred (the C1_EVN_TM_OCC signal is set) and no
        processor is executing the next time event (the C1_EVN_TM_OCC
        signal is not set).

-   Trigger processor interrupt is set (the corresponding bit of the
    R_TRG_PRC_INT register is set) and the processor interrupt is
    enabled (the corresponding bit of the R_INT_ENB register is set).

The processor interrupt request outputs (PIRQOs) are registered to avoid
spurious triggers. The R_IRQ_PND register preserves the status of the
pending processor interrupts that should be reset by the proper
processor when it executes the interrupt handler routine (ISR). The
R_INT_CLR register allows clearing the bit in the R_IRQ_PND register of
the corresponding processor interrupt in the GRTOS controller. The
R1_IRQ_PND signal is asserted when a pending interrupt is set.

When a processor is interrupted, it should execute the interrupt service
routine (ISR). When the ISR is executed, the processor interrupt is
disabled in the GRTOS controller (R_INT_ENB register). To avoid being
interrupted in the ISR, the mutex is requested for access to the
critical section and then the processor waits until the mutex access is
granted.

When the system requires a processor to execute a higher priority task,
the GRTOS controller interrupts the processor that is executing the
lowest priority task. In the R_LOW_PRI_PRC register, the bit
corresponding to the processor that is executing the lowest priority
task is set to 1; otherwise, it is set to 0.

### Input/output interrupts related instructions

  GRTOS_CMD_IRQ_ENB_CLR(*irq*)              Disables the device interrupt request event *irq* (DIRQ).
  ----------------------------------------- -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  GRTOS_CMD_IRQ_ENB_SET(*irq*)              Enables the device interrupt request event *irq* (DIRQ).
  GRTOS_CMD_NXT_TM_PRC_SET(*processor*)     Sets the Lowest Priority Processor in the GRTOS Controller.
  GRTOS_CMD_NXT_OCC_TM_EVN_SET(*timeset*)   Sets the Next Occurence Time register of the GRTOS controller to produce a timed event when system time reaches the value of *timeset*.
  GRTOS_CMD_LOW_PRC_SET(*processor*)        Sets the processor with CPUID equal to *processor* as the Lowest Priority Processor in the GRTOS Controller.
  GRTOS_CMD_CRITICAL_SECTION_GET            Defines the entry to a critical section to handle system variables and ISR routines. It asks for critical section and puts the processor in halt mode. GRTOS controller enables the processor when section is granted to it.
  GRTOS_CMD_CRITICAL_SECTION_RELEASE        Releases the critical section from the current processor. The final released is delayed by the controller to let the processor finishes executing the return from the critical section. It should be executed from the interrupt routine.
  GRTOS_CMD_MTX_RQS_GET                     Returns the current value of the Mutex.
  GRTOS_CMD_PRC_INT_ENB                     Enables the processor interrupt request of the current processor in the GRTOS controller
  GRTOS_CMD_PRC_INT_DSB                     Disables the interrupt of the current processor in the GRTOS controller.
  GRTOS_CMD_IRQ_ENB_GET(*irq*)              Reads the enabled status of the device interrupt request (DIRQ) *irq.*
  GRTOS_CMD_TRG_PRC_INT_SET(*proc*)         Interrupts the processor *proc*. If proc interrupt is enabled in the GRTOS controller, then the processor is interrupted.
  GRTOS_CMD_INT_PRC_PND_CLR                 Clears the pending interrupt of the current processor.

## Event module

The GRTOS controller handles a set of events and determines the way they
are managed to preserve the consistency of the system. When a processor
is interrupted because an event happened, it must execute the event
handler routine associated with that event. Once the critical section is
granted in the ISR, the processor must get the event that produced the
interrupt by reading the event code in the R_LST_EVN register.

When a processor executes the GRTOS_CMD_EVN_OCC instruction, the
R_LST_EVN register is transferred through the interface element to the
processor. Several events may be asserted when the processor reads the
R_LST_EVN register. In order to avoid inconsistency, the event module
sorts the events and assigns to the R_LST_EVN register the code of the
asserted event with the lowest sorted order. The following table shows
the sorted list of events of GRTOS controller:

  Sorted Order   Event Code                            Description
  -------------- ------------------------------------- ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  1              FROZEN event code (EVN_CODE_FROZEN)   This event takes place when the GRTOS controller changes from unfrozen mode to frozen mode. The C1_FRZ_MDE_ENB and C1_FRZ_EVN signals should be clear to allow accessing the rest of events when the GRTOS controller is in frozen mode.
  2              TIMED event code (EVN_CODE_TIMED)     This event takes place when a time event happens (C1_EVN_TM_OCC signal is asserted).
  3 to \...      DIRQ*i*\_CODE (DIRQ event codes)      The DIRQ*i*\_CODE is asserted when there is a pending request from the device interrupt requests (DIRQs) with the index *i*

### Event module related instructions

  ------------------- ------------------------------------------------------
  GRTOS_CMD_EVN_OCC   Returns the event happened from the GRTOS controller
  ------------------- ------------------------------------------------------

# System components and Data Structures

## Task 

A task is defined as a unit of execution. FIG. 3 shows the main
components of a task implementation that consists of:

-   **TCB data structure**: this is a control block (TCB) that stores
    the information required for task management.

-   **task code**: this is the executable code, loaded into the system
    memory, that implements a task function when it is executed by a
    processor.

-   **task stack**: this is data storage to preserve the status of a
    suspended task in order to resume it properly and store subroutine
    data.

FIG. 3: Task components and structure.

Tasks are executed by the processors to produce their designed
functions. A task may stay in different states. FIG. 3 shows an
embodiment of these states and the different events that produce their
changes:

-   **waiting state**: the task is waiting for an event to release. When
    a task is in the waiting state, it is called a "*waiting task*".

-   **ready state**: the task is ready to be executed and it is waiting
    to grant a processor for execution. A task in the ready state is
    called a "*ready task*".

-   **running state**: the task is executing on a processor. A task is
    executed until it finishes the execution or is preempted by a higher
    priority task. A task in the running state is called an "*executing
    task*".

FIG. 3: Task states and events.

There are different causes of changes in the state of a task such as an
event in a device interrupt requests (DIRQs), an internal time event, an
event produced by another task or, the availability of a requested
resource as well as other events specified in the system. In FIG. 3, the
state changes are grouped in the following set of events:

-   **create events**: create a task and set its state as the waiting
    state. All tasks should be created through a create event. When a
    task is created, its stack is initialized to start execution from
    the start address of the task.

-   **release events**: change the state of the task from the waiting
    state to the ready state. The state of a task changes to the ready
    state when that task is ready to be executed by a processor.

-   **suspend events**: change the state of a task from the ready state
    to the waiting state. The status of the task is saved into the task
    stack in order to restore it when it is resumed.

-   **blocking events**: change the state of a task from the running
    state to the waiting state.

-   **execution events**: change the state of a task from the ready
    state to the running state. A processor restores its status from the
    task stack.

-   **deallocate events**: change the state of a task from the running
    state to the ready state. When a task is deallocated, its status is
    saved in the task stack in order to restore it when an execution
    event returns it to the running state. Consequently, the task
    executes as if it had not been deallocated.

-   **finish events**: destroy a task. A finish event destroys a
    task-independent of its state.

The tasks may be classified into three main kinds:

-   **User Task**: this task is released for execution according to the
    configuration that the user performed. It can be executed
    periodically or when an internal event occurs.

-   **Event Handler Routine Task**: this task is released when an event
    occurs in a device interrupt request (DIRQ). An Event Handler
    Routine Task is associated with that device interrupt request
    (DIRQ). Event Handler Routine Tasks are created when an ISR is
    associated with the device interrupt request (DIRQ). These Event
    Handler Routine tasks will be the code of the task to be released
    each time that the associated, and enabled, device interrupt request
    (DIRQ) occurs.

-   **Idle Task**: this task is executed when neither user tasks nor
    interrupt tasks are ready for execution. An idle task may turn a
    processor to a low power consumption state until the next event that
    requires processing.

Tasks and events are closely related because events change the status of
the tasks.

## Events 

The GRTOS supports external and internal events. External events come
from the GRTOS controller to the processors through the processor
interrupt requests (PIRQs) and are processed by the GRTOS in the
interrupt handler routine of the system.

Internal events are produced by different conditions, such as when an
executing task is deallocated, an executing task completes, etc. The
GRTOS uses ECB data structures to handle all the events of the system.
ECB data structures are linked with TCB data structures to properly
manage changes in the states of tasks.

## Time

Time is important because it marks when events occur. The R_TM_CNT
register of the GRTOS controller holds the system time and the
R_FRZ_TM_CNT register holds the length of time the system stayed in
frozen mode. The GRTOS controller produces a time event (C1_EVN_TM_OCC
signal) each time the R_TM_CNT register reaches the value stored in the
R_NXT_EVN_CNT register.

## Resources

In a multitasking system, tasks may share the utilization of the
system's resources, such as data memory sections, code memory sections
or input/output devices as well as other special resources. To manage
the utilization of these resources and share them among tasks
efficiently, an operating system should offer an adequate resource
management mechanism. This mechanism offers data structures and
functions to handle the sharing of system resources among the tasks that
require their utilization.

The GRTOS implements the data structure of a resource in the RCB data
structure. The RCB data structure supports a linked list to ECB data
structures to manage the events associated with resources, in addition
to the data fields required to implement the functionalities of specific
kinds of resources. When a task requests a resource, (1) the resource
may be granted to the task if it is available or (2) the task may be
blocked if it must wait until the resource becomes available. Moreover,
a timeout may need to be specified in order to avoid a task either
granting or waiting for a resource for a long time, which would impede
the adequate sharing of resources among tasks.

FIG. 3 shows a RCB structure to support semaphore or message queue
functionality that can be extended to different kinds of system
resources.

FIG. 3: Resource extensions of RCB data structure.

## Signals

Signals in operating systems handle exceptional conditions that may
happen during runtime. Signals allow tasks to execute associated code
when a certain condition takes place. Some of these conditions may be
defined, for instance, as when a task is created, a resource is granted
or a task is blocked. The SCB data structure is defined to store the
signal information and it is linked to data structures accordingly.

The SCB data structure points to the code to be executed when the
condition of the signal occurs. This code is executed prior to the
execution of the code of the task with which the signal is associated.

## Ready and Running Linked Lists (TCBRDYL list and TCBRUNL list)

Tasks in the ready state are grouped in the Ready Linked Lists. LCB data
structures allow holding information about the tasks ready for execution
and the tasks currently executing in a determined TCBRDYL list. Several
ready linked lists may be defined in order to group the tasks of the
system according to each one's function and the function of the group.
FIG. 3 the LCBL list that links all the LCB data structures of the
system. When a task is in the ready state, it should be linked to a
TCBRDYL list; when a task is in the running state, it should be linked
to a TCBRUNL list.

FIG. 7: Ready and Running LCB lists.

## Processors

The processors executes the code of the operating system, tasks, and
signals. The GRTOS is in charge of sharing the utilization of the
processors among the system tasks. Each processor should be assigned to
execute the highest priority tasks in the ready state. A PCB data
structure stores the state of a processor and the ready linked lists
that the processor may serve. A processor may serve several TCBRDYL
lists and several processors may serve to the same TCBRDYL list.

The LCBL list shown in FIG. 7 sorts the LCB data structures according to
the priority of the tasks executed by each processor. In this way, it is
easy to determine the lowest priority processor to be defined in the
R_LOW_PRI_PRC register of the GRTOS controller.

## RRDS data structure

Each request for a resource may include an RRDS data structure that
contains the specific information for that request. If the pointer to an
RRDS data structure 1210 in the RCBType field is NULL, then the default
values of the resource are considered.

# Control block fields

Each of the control blocks holds all the information required to perform
its functions and the links required for the linked lists in fields of
the data structure.

## KCB data structure 1200

The KCB data structure holds the main information about the computer
program. The KCB data structure includes the following fields:

-   KCB_NextOccTime field: this field holds the time that the earliest
    time event shall take place, if at all. When the KCB_NextOccTime
    field changes its value, it is written to the R_NXT_EVN_TM register
    of the GRTOS controller.

-   \*KCB_NextECBTL field: this field is the root block of the ECBTL
    list. It points to the first-time ECB data structure in the ECBTL
    list or NULL if there is not any ECB data structure in the ECBTL
    list.

-   \*KCB_NextTCBWL field: this field is the root block of the TCBWL
    list. It points to the first-waiting TCB data structure in the TCBWL
    list or NULL if there is not any TCB data structure in the TCBWL
    list.

-   \*KCB_NextTCBFL field: this field is the root block of the TCBFL
    list. It points to the first-free TCB data structure in the TCBFL
    list or NULL if there is not any TCB data structure in the TCBFL
    list.

-   \*KCB_NextECBFL field: this field is the root block of the ECBFL
    list. It points to the first-free ECB data structure in the ECBFL
    list or NULL if there is not any ECB data structure in the ECBFL
    list.

-   \*KCB_NextRCBFL field: this field is the root block of the RCBFL
    list. It points to the first-free RCB data structure in the RCBFL
    list or NULL if there is not any RCB data structure in the RCBFL
    list.

-   \*KCB_NextRRDSFL field: this field is the root block of the RRDSFL
    list. It points to the first-free RRDS data structure in the RRDSFL
    list or NULL if there is not any RRDS data structure in the RRDSFL
    list.

-   \*KCB_NextSCBFL field: this field is the root block of the SCBFL
    list . It points to the first-free SCB data structure in the SCBFL
    list or NULL if there is not any SCB data structure in the SCBFL
    list.

-   \*KCB_NextMCBFL field: this field is the root block of the MCBFL
    list. It points to the first-free MCB data structure in the MCBFL
    list or NULL if there is not any MCB data structure in the MCBFL
    list.

-   \*KCB_NextLCBL field 1809: this field is the root block of the LCBL
    list. It points to the first-free LCB data structure in the LCBL
    list or NULL if there is not any LCB data structure in the LCBL
    list.

-   \*KCB_NextKCBASL field : this field is the root block of the KCBASL
    list. It points to the first SCB data structure associated with the
    KCB data structure.

## TCB data structure

The TCB data structure holds the support information required for the
tasks. The TCB data structure includes the following fields:

-   TCBState field: this field holds the state of the TCB data
    structure. The state may be used to determine the action to be
    performed during runtime. For instance, to unlink a TCB data
    structure from the current linked list, the TCBState field can be
    used to select the adequate unlink function.

-   TCBType field: this field holds the type of the TCB data structure.
    The type may be used to determine the action to be performed during
    runtime. For instance, if it is a periodic task, then the release
    time event should be updated periodically.

-   TCBReadyPriority field: this field holds the priority of the task
    associated with the TCB data structure when it is in the ready
    state. The task that is ready for execution with the lowest value in
    the TCBReadyPriority field (highest priority) is chosen for
    execution when a processor is available.

-   TCBInherPriority field: this field holds the priority that the task
    inherits when a resource is granted. The priority of the resource
    may be inherited by the task when the resource is granted to the
    task.

-   TCBRunPriority field: this field holds the priority of the task
    associated with the TCB data structure when it is in the running
    state.

-   TCBCurrentPriority field: this field holds the current priority of
    the task associated with the TCB data structure according to its
    state. For instance, when a task is in the running state, the
    current priority may be set according to the highest priority
    between the TCBRunPriority field and the TCBInherPriority field when
    it grants a resource.

-   TCBPeriod field: this field may hold the period of the task
    associated with the TCB data structure.

-   TCBDeadline field: this field may hold the deadline of the task
    associated with the TCB data structure.

-   \*TCB_StackPointer field: this field holds the current stack pointer
    of the task when it is not in the running state.

-   \*TCB_StackBottom field: this field holds a pointer to the bottom of
    the task stack.

-   \*TCB_StackTop field: this field holds a pointer to the top of the
    task stack.

-   \*TCB_TaskCode field: this field holds a pointer to the starting
    address of the task code.

-   \*TCB_TaskArg field: this field holds a pointer to the starting
    arguments of the task.

-   \*TCB_NextTCB field: this field points to the next element of the
    list that the TCB data structure is linked.

-   \*TCB_PrevTCB field: this field points to the previous element of
    the list that the TCB data structure is linked.

-   \*TCB_NextTCBAEL field: this field is the root of the TCBAEL list .
    It points to the first ECB data structure in the TCBAEL list or NULL
    if there is not any ECB data structure in the TCBAEL list.

-   \*TCB_NextTCBASL field 1916: this field is the root of the TCBASL
    list. It points to the first SCB data structure in the TCBASL list
    or NULL if there is not any SCB data structure in the TCBASL list.

-   \*TCB_NextTCBPSL field 1917: this field is the root of the TCBPSL
    list. It points to the first SCB data structure in the TCBPSL list
    or NULL if there is not any SCB data structure in the TCBPSL list.

-   TCB_AssocPCB field : this field holds the value of the internal
    identification register CPUID of the processor that is currently
    executing the task associated with the TCB data structure.

-   TCB_INTNumber field: this field holds the IRQ number of the device
    interrupt request (DIRQ) input when TCB data structure is associated
    with an Interrupt Task (TCBType field is equal to ISR).

-   TCB_RDY_LCB_Index field: this field holds the index of the LCBL list
    to be linked when it is in the ready state.

## ECB data structure

The ECB data structure holds information about events. The ECB data
structure includes the following fields:

-   ECBState field: this field holds the state of the ECB data
    structure. The state may be used to determine the action to be
    performed during runtime. For instance, to unlink an ECB data
    structure from the current linked list, the ECBState field is used
    to select the adequate unlink function.

-   ECBType field: this field holds the type of the ECB data structure.
    The type may be used to determine the action to be performed during
    runtime.

-   ECBValue field: this field holds the value to be used as a sorting
    criterion in several linked lists. For instance, it may hold time
    when the ECB data structure is used as a time event or the waiting
    priority when it is used to request a resource.

-   \*ECB_NextECB field: this field points to the next element of the
    list with which the ECB data structure is linked.

-   \*ECB_PrevECB field: this field points to the previous element of
    the list with which the ECB data structure is linked.

-   \*ECB_AssocTCB field 2005: this field points to the TCB data
    structure 1201 with which the ECB data structure 1202 is associated.

-   \*ECB_AssocRCB field: this field points to the RCB data structure
    with which the ECB data structure is associated.

-   \*ECB_NextTCBAEL field: this field points to the next element of the
    TCBAEL list.

-   \*ECB_PrevTCBAEL field: this field points to the previous element of
    the TCBAEL list.

-   \*ECB_NextECBAEL field: this field is the root block of the ECBAEL
    list. It is NULL if there is no ECB data structure associated with
    the ECB data structure.

-   \*ECB_NextECBASL field: this field is the root block of the ECBASL
    list. It is NULL if there is no SCB data structure associated with
    the ECB data structure.

-   \*ECB_RRDS field: this field points to the RRDS data structure
    associated with the ECB data structure. It is NULL if there is no
    RRDS data structure associated.

## RCB data structure

The RCB data structure holds information about a resource. The RCB data
structure includes the following fields:

-   RCBState field: this field holds the state of the RCB data
    structure. The state may be used to determine the action to be
    performed during runtime.

-   RCBType field: this field holds the type of the RCB data structure.
    The type is used to determine the actions performed on the resource
    during runtime.

-   RCBCount field: this field holds the count that determines the
    availability of the resource for requests.

-   RCBPriority field: this field holds the priority of the lowest
    priority task that is granting the resource. This field may be used,
    for instance, to determine if priority inversion exits between tasks
    granting and tasks waiting for the resource.

-   RCBGrantedPriority field: this field holds the priority that a task
    inherits when it grants the resource.

-   RCBWaitingTimeout field: this field holds the default waiting
    timeout for the resource associated with the RCB data structure.

-   RCBGrantedTimeout field: this field holds the default granted
    timeout for the resource associated with the RCB data structure.

-   \*RCB_NextRCBWEL field: this field is the root block of the RCBWEL
    list. It is NULL if there is no ECB data structure linked to the
    RCBWEL list.

-   \*RCB_NextRCBGEL field: this field is the root block of the RCBGEL
    list. It is NULL if there is no ECB data structure linked to the
    RCBGEL list.

-   \*RCB_NextRCB field: this field points to the next element of the
    list with which the RCB data structure is linked.

-   \*RCB_NextRCBASL field: this field points to the next element of the
    RCBASL list.

Different resources can be implemented according to the synchronization
features to be achieved. Each one of these synchronization mechanisms
requires data structures to hold its status. The resource-dependent
structure is appended to the RCB data structure to support different
synchronization mechanisms.

### Semaphore structure

A semaphore is a widely used synchronization resource for controlling
the access of different tasks to critical sections. The fields of this
data structure are appended to the RCB data structure. The semaphore
structure includes the field:

-   SEMMaxCount field: this field holds the maximum number of
    simultaneous requests that can be granted for the semaphore
    structure.

### Message queue structure

Messages are used as a communication resource among different tasks. The
messages are organized as a queue list in which the first message sent
is the first message to be delivered. The message queue structure
includes the following fields:

-   MESSize field: this field holds the maximum number of MCB data
    structures for the message queue structure.

-   MESEntries field: this field holds the current number of MCB data
    structures of the message queue structure.

-   \*MES_FirstMCB field: this field points to the first MCB data
    structure linked to the message queue structure.

-   \*MES_LastMCB field: this field points to the last MCB data
    structure linked to the message queue structure.

## SCB data structure

Signals are used to manage internal events that can be produced during
runtime. The SCB data structure includes the following fields

-   SCBState field: this field holds the state of the SCB data
    structure. The state may be used to determine some actions to be
    performed during runtime.

-   SCBType field: this field holds the type of the SCB data structure.
    The type is used to determine some actions to be performed during
    runtime.

-   SCBPriority field: this field holds the priority of the SCB data
    structure. This field is used to determine the order in which the
    pending signals are executed (highest priority first).

-   \*SCB_TaskCode field: this field holds a pointer to the starting
    address of the signal code.

-   \*SCB_TaskArg field: this field holds a pointer to the starting
    arguments of the signal.

-   \*SCB_NextSCB field: this field holds pointers to the next element
    of the list to which the SCB data structure is linked.

-   \*SCB_NextSCBAPSL field: this fields links all the pending signals
    with the signal that generates them.

-   \*SCB_AssocXCB field: points to the data structure with which this
    SCB data structure is associated.

## LCB data structure

In a multiprocessor system, tasks may be grouped to be executed by
either one or several processors in order to produce a certain
performance or function. The LCB data structure allows organizing the
group of tasks to be executed by one or several processors and the
sharing of processors among LCB data structures. The LCB data structure
includes the following fields:

-   LCBState field: this field holds the state of the LCB data
    structure. The state may be used to determine the action to be
    performed during runtime.

-   LCBType field: this field holds the type of the LCB data structure.
    The type is used to determine the actions to perform during runtime.

-   LCBRunPriority field: this field holds the lowest priority of the
    executing tasks linked to the LCB data structure or the lowest
    possible priority value if no task is in the running state.

-   \*LCB_NextTCBRUNL field: this field is the root block of the TCBRUNL
    list. It is NULL if there is no TCB data structure associated with
    the TCBRUNL list.

-   \*LCB_NextTCBRDYL field: this field is the root block of the TCBRDYL
    list. It is NULL if there is no TCB data structure associated with
    the TCBRDYL list.

-   \*LCB_NextLCBL field: this field holds a pointer to the next element
    in the LCBL list.

-   \*LCB_PrevLCBL field: this field holds a pointer to the previous
    element in the LCBL list.

-   \*LCB_NextLCBFPL field: this field is the root block of the LCBFPL
    list. It is NULL if there is no PCB data structure linked to the
    LCBFPL list.

## PCB data structure

Each processor of the system requires a data structure to preserve its
status during runtime. The PCB data structure holds the information of
one processor. The PCB data structure includes the following fields:

-   PCBID field: this field holds the value of the internal
    identification register CPUID of the processor associated with the
    PCB data structure.

-   PCBState field: this field holds the state of the PCB data
    structure. The state may be used to determine the action to be
    performed during runtime.

-   PCBType field: this field holds the type of the PCB data structure.
    The type is used to determine the actions performed during runtime.

-   PCB_RDY_LCBL\[\...\] field: this field holds an array that contains
    the index of the LCBL list that the processor, associated with the
    PCB data structure, uses to search for a task in the ready state to
    execute.

-   \*PCB_IDLETCB field: this field holds a pointer to the TCB data
    structure of the idle task associated with the PCB data structure.

-   \*PCB_EXECTCB field: this field holds a pointer to the TCB data
    structure of the currently executing task associated with the PCB
    data structure.

-   \*PCB_NextPCB field: this field holds a pointer to the next element
    of the list to which the PCB data structure is linked.

-   \*PCB_PrevPCB field: this field holds a pointer to the previous
    element of the list to which the PCB data structure is linked.

## RRDS data structure

The RRDS data structure is the structure that holds information of the
specific resource request. Fields of the RRDS data structure depend on
the resource it is associated.

## GRTOS structure

FIG. 7 shows the GRTOS code structure. Shadow sections are executed in
the critical section of the GRTOS controller. The GRTOS structure
includes the following sections:

-   **initialization section**: initializes the data structures of the
    computer program.

-   **task start section**: restores the state of the next task to be
    executed by the processor.

-   **interrupt handler section**: the code section that should be
    executed when the GRTOS controller asserts a processor interrupt
    request (PIRQ). The code of this section is executed in the critical
    section of the GRTOS controller.

-   **task switch section**: checks if task context switching is
    required and saves and restores the task status accordingly. It also
    checks when a pending signal must be executed and sets the task
    stack for its execution.

-   **system call section**: groups the functions that the computer
    program allows to call from tasks. System calls may be called from
    tasks but require granting the critical section of the GRTOS
    controller to be executed.

-   **tasks codes**: are executed by system processors. Tasks codes are
    executed outside the critical section of the GRTOS controller, but
    the critical section may be requested when a task code calls a
    system call. The task stack stores the data and information required
    for the task code to be executed during runtime. A TCB data
    structure is associated with each task to hold all the task statuses
    required by the GRTOS for execution.

-   **task suspend section**: some system calls produce blocking events
    that require saving the status of the task and restoring the status
    of the highest priority task in order to execute the next ready task
    while the calling task waits for a release event.

-   **task complete section**: is executed when a task completes its
    execution. The task state is changed to the waiting state and its
    status is saved. Then, the start task section is called.

FIG. 7: GRTOS structure.

## Initialization section

FIG. 7 shows the structure of the initialization section that sets the
initial value of the data structures of the GRTOS. At the beginning, the
stack pointer register (SP) of each processor is configured with a
unique stack memory address to avoid conflicts among processors. In
order to synchronize the initialization, only the processor whose
internal identification register CPUID is equal to 1 executes the
configuration code of the data structures and the GRTOS controller. The
rest of the processors waits, until the critical section is granted to
the processor with an internal identification, register CPUID equal to
1. Then, they request the critical section and wait for it to be
granted. This mechanism guarantees that: (1) the initialization code
section is executed before any other section and (2) the rest of
processors start executing tasks codes after the initialization of the
system.

FIG. 7: Initialization section structure.

The processor with an internal identification register CPUID equal to 1:

-   resets the GRTOS controller and the rest of the processors: sets
    R_PRC_RST register to 0 by executing a GRTOS_CMD_RST_GRTOS
    instruction. The internal registers of the GRTOS controller are set
    to their initial values and the processors with CPUID different from
    1 are reset.

-   gets access to the critical section of the GRTOS controller: to
    avoid consistency errors when data structures and linked lists are
    initialized.

-   initializes data structures: all the data structures of the computer
    program are created with their initial values.

-   initializes linked lists: all linked lists are created in their
    initial states.

-   initializes interrupt and idle tasks: creates the interrupt task
    associating the ISR of each device interrupt request (DIRQ). An idle
    task is created for each processor of the system.

-   executes the main routine (main()) defined in the user program. The
    user initializes and creates the user tasks and interrupt tasks in
    this routine.

The rest of the processors of the plurality of processors 110-112:

-   assign an unique stack for each processor of the system.

-   wait until the R_PRC_RST register is equal to CPUID -- 1.

-   get the critical section: processors are synchronized to start the
    execution of user tasks requesting the critical section of the GRTOS
    controller.

-   assign the value of CPUID to the R_PRC_RST register. This assignment
    enables the next processor to start its configuration.

When the initialization section is executed, the start task section is
executed.

## Task switch section

FIG. 7 shows the structure of the task switch section. This section
implements the methods for saving and restoring task statuses according
to the states of the tasks. Prior to executing the code of the next
task, the pending signals are executed and then, when there is no other
pending signal, the code of the task is resumed. The task switch section
presents three entry points:

-   **GRTOS_Suspend_Task**t: saves the status of the current task and
    restores the status of the idle task of the processor.

-   **GRTOS_irq_entry**: saves the status of the current task and
    restores the status of the idle task of the processor.

-   **GRTOS_Return_from_signal**: when there exists a pending signal for
    a task, the RETURN_FROM_SIGNAL is configured as the return entry
    point.

-   **GRTOS_Start_Task**: restore the status of the next task to be
    executed and return. If signal is pending, the task stack is modify
    to execute the signal code when returning from interrupt and the
    return address is set to GRTOS_Return_from_signal.

FIG. 7: Task switch section structure.

The task switch section is completed by executing a return from
interrupt in order to enable the processor interrupt. Before leaving the
section, the critical section is released and the corresponding
processor interrupt request (PIRQ) of the GRTOS controller is enabled.

## Interrupt handler section

The code of the interrupt handler is executed by the processors when
some of them receive a processor interrupt request (PIRQ) from the GRTOS
controller and produces a C1_IRQ_PND signal. When a processor receives
an interrupt request from a device interrupt request (DIRQ), the
processor disables the processor interrupt, and starts executing the
interrupt handler section. In the interrupt handler, the processor
performs the following procedures:

-   the status of the processor is saved into the task stack. All the
    processor registers are pushed into the task stack in order to
    restore them when the interrupted task is resumed.

-   the status of the idle task is restored from the idle task stack.
    The interrupt section is executed as the idle task of the processor
    in order to avoid task status inconsistencies.

-   the processor interrupt request (PIRQ) of the GRTOS controller
    associated with the processor is disabled. No new interrupt will be
    produced to the processor by the GRTOS controller while it is
    executing the interrupt handler section.

-   the processor interrupt request (PIRQ) of the GRTOS controller
    associated with the processor is cleared. The interrupt request is
    cleared to avoid a new request when it is enabled.

-   the critical section of the GRTOS controller is requested. The
    processor waits while requesting the critical section of the GRTOS
    controller until it is granted to the processor.

-   the R_LST_EVN register of the GRTOS controller is read to get the
    last event that occurred. According to the code read from the
    R_LST_EVN register of the GRTOS controller, the following is
    performed:

-   Code EVN_CODE_FROZEN: the gk_KERNEL_FROZEN_IRQ_HANDLER routine is
    called to perform the procedure according to the type of time event
    that produced the interrupt while the system time is frozen. This
    routine calls the gk_FROZEN_CALLBACK() routine in grtosfunctions.c
    file.

-   Code EVN_CODE_TIMED: the gk_KERNEL_TIME_IRQ_HANDLER() routine is
    called to perform the procedure according to the type of time event
    that produced the current EVN_CODE_TIMED event. This routine calls
    the gk_TIME_CALLBACK() routine in grtosfunctions.c file.

-   Code DIRQ*i*\_CODE: (1) the requesting device interrupt request
    (DIRQ) is disabled in the GRTOS controller, (2) the TCB data
    structures associated with the task interrupt of the device
    interrupt request (DIRQ) are unlinked from the interrupt list to
    which it is currently linked, (3) the task stack 1601 of the
    interrupt tasks are initialized and (4) the TCB data structures are
    linked to the TCBRDYL list.

-   Zero (0): there are no more events to process from the GRTOS
    controller and consequently it calls the SWITCH_SUSPEND_TASK to
    execute the next task.

The R_LST_EVN register of the GRTOS controller is read until the value
returned is equal to 0.

## System call section

System calls are called from tasks and are executed in the critical
section of the GRTOS controller . The execution of the system call
depends on the specific system call. Some system calls do not require a
context switch and consequently the critical section of the GRTOS
controller is released before returning to the caller task. In contrast,
when a task requires a context switch, the SWITCH_SUSPEND_TASK entry
point in the task switch section is called. When the task is resumed,
the system call should complete its execution but getting the critical
section first.

## Task suspend section

When a task is suspended, the following procedure is performed:

-   the TCB data structure of the task is unlinked from the TCBRUNL list
    and then linked to the TCBWL list. The task remains in the waiting
    state until a release event changes it to the ready state.

-   the next task to execute is chosen among tasks in the ready state
    that may be executed by the current processor.

-   the next task is linked to the TCBRUNL list and its state is changed
    to the running state.

-   the processor interrupt is set to the running priority of the next
    task, the LCBL list is updated accordingly and the lowest priority
    processor register is set in the GRTOS controller.

-   the priority inversion is checked in order to verify that there is
    no task in the ready state with higher priority than any task in the
    running state for the TCBRUNL list of the next task. If there is a
    task in the ready state with a higher priority than a task that is
    currently being executed for a processor in the TCBRUNL list, then
    the processor interrupt request (PIRQ) in the GRTOS controller is
    triggered.

-   the next time processor is set in the GRTOS controller .

-   the SWITCH_SUSPEND_TASK entry point in the task switch section is
    called to produce the context switching of a suspended task.

## Task complete section

The complete task section is executed when a task finishes its
execution. Then, the COMPLETE_TASK entry is configured as the returning
point when a task is started and consequently is the return address when
the task finishes its execution. The following steps are executed to
complete the execution of a task:

-   the TCB data structure of the current task is unlinked from the
    TCBRUNL list.

-   the TCB data structure of the task is linked to the TCBWL list.

-   if task is an Interrupt Task, then the corresponding device
    interrupt request (DIRQ) is enabled for further requests.

-   the highest priority task for the current processor is selected as
    the next executing task.

-   the TCB data structure of the next task is unlinked from the TCBRDYL
    list.

-   the TCB data structure of the next task is linked to the TCBRUNL
    list and its state is changed to the running state,

-   the R_LOW_PRI_PRC register of the GRTOS controller is set with the
    processor that is executing the lowest priority task.

-   the priority inversion is checked in order to verify that there is
    no task in the ready state with higher priority than any task in the
    running state for the TCBRUNL list of the next task. If there is a
    task in the ready state with a higher priority than a task that is
    being executed for a processor in the TCBRUNL list, then the
    processor interrupt request (PIRQ) in the GRTOS controller is
    triggered.

-   the R_NXT_TM_EVN_PRC register in the GRTOS controller is set with
    the processor that is executing the task that produces the next time
    event (0 if no task is executing the next time event).

```{=html}
<!-- -->
```
-   the gk_GRTOS_Start_Task is called to produce context switching of a
    suspended task.

# Priority Management

Priority discipline determines the sequence in which tasks are executed.
A priority discipline defines a total order among tasks in the ready
state. The TCBRDYL lists sort the TCB data structures of the tasks
according to the TCBReadyPriority field. By previously setting the
TCBReadyPriority field to link the TCB data structure to the TCBRDYL
lists (by executing the gk_TCBRDYL_Link(GS_TCB \*ptcb) function),
different priority disciplines may be implemented. For instance, leaving
a constant value in the TCBReadyPriority field implements a Fixed
Priority discipline, while assigning TCBReadyPriority field = gk_now +
TCBDeadline field 1907 implements an Earliest Deadline First priority
discipline.

# A "Hello World" for GRTOS

These section describes a simple example for setting an application
using some of the GRTOS system calls. In this example, four task are
programed to be executed in 3 processors, with the following features:

-   task 1:

-   task 2:

-   task 3:

-   task 4:

The following is the code that should be contained in the main
application, together with the main() function.

Include libraries for system function calls and GRTOS system calls:

\#include \<stdio.h\>

\#include \<grtosuserapi.h\>

\#include \<grtosdefinitions.h\>

Define a semaphore, called *semaphore1*, to synchronize between task1
and task2:

t_semaphore_resource \*semaphore1;

Define the default size of the task stacks:

\#define TASK_STACKSIZE 4096

Define the task1 as an infinite loop with a 15 seconds delay.

void task1(void\* pdata)

{

gt_tm sys_time;

while (1)

{

sys_time = gu_Clock(gu_get_now());

printf(\"Hello task1: \");

printf(\"Processor = %d, \", gu_Get_CPU_ID());

printf(\"year = %d, day = %d, \", sys_time.tm_year, sys_time.tm_day);

printf(\"hour = %d, min = %d, \", sys_time.tm_hour, sys_time.tm_min, );

printf(\"sec = %d, ms = %d\\n\\n\", sys_time.tm_sec, sys_time.tm_msec);

gu_TASK_Sleep(0, 0, 15, 0);

}

}

GRTOS API

# Configuration functions

## gu_Set_Default_Task_Type(unsigned int type)
