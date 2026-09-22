##
## \file
## \author Ricardo Cayssials
## \brief GeMRTOS Monitor BSP driver descriptor (_sw.tcl)
## \details Registers the gemrtos_driver BSP driver: HAL C/ASM sources, include
##          files, and the initialize/generate/validate callbacks implemented
##          in callbacks.tcl.
## \version 1.0
## \date      2015-2025
##
## SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
## Copyright (c) 2013-2026 GeMRTOS Project Contributors
##
## Required Notice: Copyright GeMRTOS Project Contributors (https://gemrtos.com)
##
## This file is part of GeMRTOS, licensed under the PolyForm Noncommercial
## License 1.0.0. You may use, copy, modify, and distribute this file for
## any noncommercial purpose. Commercial use requires a separate written
## license from GeMRTOS. Full terms:
## https://polyformproject.org/licenses/noncommercial/1.0.0
##

# Create a new driver
create_driver gemrtos_monitor_driver

# Associate it with some hardware known as "altera_avalon_jtag_uart"
set_sw_property hw_class_name nios_avalon_monitor

# Set UI display name
set_sw_property display_name "GeMRTOS Monitor 1.0"

# The version of this driver
set_sw_property version 1.0

# This driver may be incompatible with versions of hardware less
# than specified below. Updates to hardware and device drivers
# rendering the driver incompatible with older versions of
# hardware are noted with this property assignment.
#
# Multiple-Version compatibility was introduced in version 0.0;
# prior versions are therefore excluded.
set_sw_property min_compatible_hw_version 0.0

# Initialize the driver in alt_sys_init()
set_sw_property auto_initialize false

# Initialize the driver in alt_irq_init() if this module
# is recognized as containing an interrupt controller.
set_sw_property irq_auto_initialize false

# Location in generated BSP that above sources will be copied into
set_sw_property bsp_subdirectory drivers

# set up module instance callbacks
# set_sw_property callback_source_file callbacks.tcl
# set_sw_property initialization_callback initialize
# set_sw_property generation_callback generate
# set_sw_property validation_callback validate


# Interrupt properties: This driver supports both legacy and enhanced
# interrupt APIs, as well as ISR preemption.
set_sw_property isr_preemption_supported true
set_sw_property supported_interrupt_apis "legacy_interrupt_api enhanced_interrupt_api"


# C source files
# add_sw_property c_source HAL/src/gemrtos_core.c


# Include files
add_sw_property include_source inc/avalon_monitor_regs.h


# This driver supports HAL types
add_sw_property supported_bsp_type HAL




# End of file
