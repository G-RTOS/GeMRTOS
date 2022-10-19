#
# altera_avalon_jtag_uart_driver.tcl
#

# See create_os page 
# https://www.intel.co.jp/content/dam/altera-www/global/ja_JP/pdfs/literature/hb/nios2/n2sw_nii5v2_04.pdf
# https://github.com/EngineeringSpirit/FreeLwIP-Nios-II/blob/master/alt-freertos-lwip-do-install.sh


# Create a new driver
create_driver grtos_driver

# Associate it with some hardware known as "altera_avalon_jtag_uart"
set_sw_property hw_class_name grtos

# Set UI display name
set_sw_property display_name "GeMRTOS Controller 0.9"

# The version of this driver
set_sw_property version 0.9

# This driver may be incompatible with versions of hardware less
# than specified below. Updates to hardware and device drivers
# rendering the driver incompatible with older versions of
# hardware are noted with this property assignment.
#
# Multiple-Version compatibility was introduced in version 0.0;
# prior versions are therefore excluded.
set_sw_property min_compatible_hw_version 0.0

# Initialize the driver in alt_sys_init()
# read https://www.intel.com/content/www/us/en/docs/programmable/683282/current/enabling-software-initialization.html
set_sw_property auto_initialize false

# Initialize the driver in alt_irq_init() if this module
# is recognized as containing an interrupt controller.
set_sw_property irq_auto_initialize true

# Location in generated BSP that above sources will be copied into
set_sw_property bsp_subdirectory drivers

# set up module instance callbacks
set_sw_property callback_source_file callbacks.tcl
set_sw_property initialization_callback initialize
set_sw_property generation_callback generate
set_sw_property validation_callback validate

# set up per class callbacks
#set_sw_property class_initialization_callback class_initialize
#set_sw_property class_validation_callback class_validate

# Interrupt properties: This driver supports both legacy and enhanced
# interrupt APIs, as well as ISR preemption.
set_sw_property isr_preemption_supported true
set_sw_property supported_interrupt_apis "legacy_interrupt_api enhanced_interrupt_api"

#
# Source file listings...
#

# C source files
add_sw_property c_source HAL/src/gemrtos_core.c
# add_sw_property c_source HAL/src/grtos_init.c
add_sw_property c_source HAL/src/listfunctions.c
add_sw_property c_source HAL/src/grtos_kernel.c
add_sw_property c_source HAL/src/monitor.c
add_sw_property c_source HAL/src/grtosuserfunctions.c
# add_sw_property c_source HAL/src/tasks.c
# add_sw_property c_source HAL/src/ext_isr.c
add_sw_property c_source HAL/src/mq.c
add_sw_property c_source HAL/src/sem.c

# ASM source files
add_sw_property asm_source HAL/src/grtos_switch.S

# Include files
add_sw_property include_source inc/grtos_regs.h
add_sw_property include_source HAL/inc/gemrtos_core.h
# add_sw_property include_source HAL/inc/gemrtos_core_api.h
# add_sw_property include_source HAL/inc/grtos.h
add_sw_property include_source HAL/inc/gemrtos.h
add_sw_property include_source HAL/inc/grtosdefinitions.h
# add_sw_property include_source HAL/inc/grtosuserapi.h
# add_sw_property include_source HAL/inc/ext_isr.h
add_sw_property include_source HAL/inc/mq.h
add_sw_property include_source HAL/inc/sem.h

#Override HAL functions
# add_sw_property excluded_hal_source HAL/src/alt_iic.c
# add_sw_property excluded_hal_source HAL/src/alt_load.c
# add_sw_property excluded_hal_source HAL/src/alt_main.c
# 
# 
# add_sw_property c_source HAL/src/alt_iic.c
# add_sw_property c_source HAL/src/alt_load.c
# add_sw_property c_source HAL/src/alt_main.c



#add_sw_property systemh_generation_script copyfiles.tcl


#
# System settings
#

# set region_info [get_memory_region onchip_memory2_0]

# add_sw_setting unquoted_string system_h_define system.memory_region MEMORY_REGION $region_info "Sets the priority of the software timer service/daemon task. See the FreeRTOS software timers page for a full description."

add_sw_setting decimal_number system_h_define system.timer_task_priority OS_TIMER_TASK_PRIORITY 5 "Sets the priority of the software timer service/daemon task. See the FreeRTOS software timers page for a full description."


#
# GRTOS settings
#


# Only defines the parameter but no value is assigned
add_sw_setting boolean_define_only system_h_define gemrtos.semaphore GRTOS_INCLUDE_SEMAPHORE 1 "Set to include semaphore support to GeMRTOS."

add_sw_setting boolean system_h_define gemrtos.message_queue GRTOS_INCLUDE_MESSAGE_QUEUE 1 "Set to include message queue support to GeMRTOS."

# set module_name [get_module_name]
# add_sw_setting unquoted_string system_h_define gemrtos.IORD_GRTOS_SMP GRTOS_IORD_GRTOS_SMP "command" "Sets the priority of the software timer service/daemon task. See the FreeRTOS software timers page for a full description."



# See create_os page 
# https://www.intel.co.jp/content/dam/altera-www/global/ja_JP/pdfs/literature/hb/nios2/n2sw_nii5v2_04.pdf

# This driver supports HAL types
add_sw_property supported_bsp_type HAL




# Add the following per_driver configuration option to the BSP:
#  o Type of setting (boolean_define_only translates to "either
#    emit a #define if true, or don't if false"). Useful for
#    source code with "#ifdef" style build-options.
#  o Generated file to write to (public_mk_define -> public.mk)
#  o Name of setting for use with bsp command line settings tools
#    (enable_small_driver). This name will be combined with the
#    driver class to form a settings hierarchy to assure unique
#    settings names
#  o '#define' in driver code (and therefore string in generated
#     makefile): "ALTERA_AVALON_JTAG_UART_SMALL", which means: "emit
#     CPPFLAGS += ALTERA_AVALON_JTAG_UART_SMALL in generated makefile
#  o Default value (if the user doesn't specify at BSP creation): false
#    (which means: 'do not emit above CPPFLAGS string in generated makefile)
#  o Description text
#add_sw_setting boolean_define_only public_mk_define enable_small_driver ALTERA_AVALON_JTAG_UART_SMALL false "Small-footprint (polled mode) driver"





# End of file
