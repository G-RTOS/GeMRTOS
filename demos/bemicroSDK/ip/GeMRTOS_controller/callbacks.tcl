##
## \file
## \author Ricardo Cayssials
## \brief GeMRTOS controller BSP driver callback implementations
## \details This script is set as a callback in grtos_sw.tcl (the component's
##          _sw.tcl file), via: 'set_sw_property callback_source_file callbacks.tcl'.
##          generate{} is the active callback: it populates the generated system.h
##          with GeMRTOS module/class defines from the Qsys instance's parameters.
## \note initialize{} and validate{} are wired (grtos_sw.tcl's
##       initialization_callback / validation_callback) but intentionally empty.
## \version 1.0
## \date 2026-07-14 (revised 2026-09-19)
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


# Name of the processor this BSP is being generated for.
set cpu_name [get_cpu_name]

## ---------------------------------
## Callback: initialization.
## Wired by grtos_sw.tcl (set_sw_property initialization_callback initialize).
## Intentionally empty: GeMRTOS needs no per-instance defaults at this stage.
##
## args 0 : module instance name
proc initialize { args } {

}

## ---------------------------------
## Callback: generation.
## Wired by grtos_sw.tcl (set_sw_property generation_callback generate).
## Writes the GeMRTOS module and class defines into the generated system.h from
## the embeddedsw.CMacro.* assignments of the controller instance.
##
## args 0 : module instance name
## args 1 : path to the BSP directory
## args 2 : driver subdirectory
proc generate { args } {

    if {[llength $args] < 3} {
        error "generate: expected 3 arguments (module name, BSP directory, driver subdirectory), got [llength $args]"
    }
	
    # The BSP generator passes these paths to the callback
    set bsp_dir [lindex $args 1]
    set bsp_subdir [lindex $args 2]
	
    set module_name [get_module_name]
    set NPROCESSORS [ get_module_assignment "embeddedsw.CMacro.NPROCESSORS" ]

    # #########################
    add_module_systemh_line $module_name [ get_module_assignment "embeddedsw.CMacro.NPROCESSORS" ]
    add_module_systemh_line $module_name [ get_module_assignment "embeddedsw.CMacro.PRESCALE" ]
    add_module_systemh_line $module_name [ get_module_assignment "embeddedsw.CMacro.GRTOSFREQUENCY" ]
    add_module_systemh_line $module_name [ get_module_assignment "embeddedsw.CMacro.GRTOSGENERATIONID" ]
    add_module_systemh_line $module_name \"[ get_module_assignment "embeddedsw.CMacro.PROCESSOR_TYPE" ]\"
    add_module_systemh_line $module_name \"[ get_module_assignment "embeddedsw.CMacro.INSTRUCTION_CACHE_SIZE" ]\"
    add_module_systemh_line $module_name \"[ get_module_assignment "embeddedsw.CMacro.INSTRUCTION_CACHE_BURST" ]\"
    add_module_systemh_line $module_name \"[ get_module_assignment "embeddedsw.CMacro.ENABLE_HPS_MAP_ACCESS" ]\"
    add_module_systemh_line $module_name [ get_module_assignment "embeddedsw.CMacro.NIOS_CLOCK_FREQUENCY" ]
    add_module_systemh_line $module_name [ get_module_assignment "embeddedsw.CMacro.BUS_WIDTH" ]
    add_module_systemh_line $module_name [ get_module_assignment "embeddedsw.CMacro.EXTERNAL_MEMORY_SPAN" ]
    add_module_systemh_line $module_name \"[ get_module_assignment "embeddedsw.CMacro.EXTERNAL_MEMORY_UNIT" ]\"
	add_module_systemh_line $module_name \"[ get_module_assignment "embeddedsw.CMacro.INDEPENDENT_EXTERNAL_PROCESSOR_BUSES" ]\"
	add_module_systemh_line $module_name \"[ get_module_assignment "embeddedsw.CMacro.INCLUDE_JTAG_UART_STDDIO" ]\"
    add_module_systemh_line $module_name \"[ get_module_assignment "embeddedsw.CMacro.INCLUDE_JTAG_UART_STDERR" ]\"
	add_module_systemh_line $module_name \"[ get_module_assignment "embeddedsw.CMacro.INCLUDE_JTAG_UART_PROCESSOR" ]\"
	
	
    add_class_systemh_line [ string toupper "NPROCESSORS" ] [ get_module_assignment "embeddedsw.CMacro.NPROCESSORS" ]
    add_class_systemh_line [ string toupper "PRESCALE" ] [ get_module_assignment "embeddedsw.CMacro.PRESCALE" ]
    add_class_systemh_line [ string toupper "GRTOSFREQUENCY" ] [ get_module_assignment "embeddedsw.CMacro.GRTOSFREQUENCY" ]
    add_class_systemh_line [ string toupper "GRTOSGENERATIONID" ] [ get_module_assignment "embeddedsw.CMacro.GRTOSGENERATIONID" ]
    add_class_systemh_line [ string toupper "PROCESSOR_TYPE" ] \"[ get_module_assignment "embeddedsw.CMacro.PROCESSOR_TYPE" ]\"
    add_class_systemh_line [ string toupper "INSTRUCTION_CACHE_SIZE" ] \"[ get_module_assignment "embeddedsw.CMacro.INSTRUCTION_CACHE_SIZE" ]\"
    add_class_systemh_line [ string toupper "INSTRUCTION_CACHE_BURST" ] \"[ get_module_assignment "embeddedsw.CMacro.INSTRUCTION_CACHE_BURST" ]\"
    add_class_systemh_line [ string toupper "ENABLE_HPS_MAP_ACCESS" ] \"[ get_module_assignment "embeddedsw.CMacro.ENABLE_HPS_MAP_ACCESS" ]\"
    add_class_systemh_line [ string toupper "NIOS_CLOCK_FREQUENCY" ] [ get_module_assignment "embeddedsw.CMacro.NIOS_CLOCK_FREQUENCY" ]
    add_class_systemh_line [ string toupper "BUS_WIDTH" ] [ get_module_assignment "embeddedsw.CMacro.BUS_WIDTH" ]
    add_class_systemh_line [ string toupper "EXTERNAL_MEMORY_SPAN" ] [ get_module_assignment "embeddedsw.CMacro.EXTERNAL_MEMORY_SPAN" ]
    add_class_systemh_line [ string toupper "EXTERNAL_MEMORY_UNIT" ] \"[ get_module_assignment "embeddedsw.CMacro.EXTERNAL_MEMORY_UNIT" ]\"
    add_class_systemh_line [ string toupper "INDEPENDENT_EXTERNAL_PROCESSOR_BUSES" ] \"[ get_module_assignment "embeddedsw.CMacro.INDEPENDENT_EXTERNAL_PROCESSOR_BUSES" ]\"
    add_class_systemh_line [ string toupper "INCLUDE_JTAG_UART_STDDIO" ] \"[ get_module_assignment "embeddedsw.CMacro.INCLUDE_JTAG_UART_STDDIO" ]\"
	add_class_systemh_line [ string toupper "INCLUDE_JTAG_UART_STDERR" ] \"[ get_module_assignment "embeddedsw.CMacro.INCLUDE_JTAG_UART_STDERR" ]\"
	add_class_systemh_line [ string toupper "INCLUDE_JTAG_UART_PROCESSOR" ] \"[ get_module_assignment "embeddedsw.CMacro.INCLUDE_JTAG_UART_PROCESSOR" ]\"
	
	
    # #########################
    
    set grtos_system_name [string map {_grtos_0 ""} $module_name]

    
	
    add_class_systemh_line SYSTEM_NAME \"${grtos_system_name}\"
    set grtos_system_name [ string toupper ${grtos_system_name} ]
	
	add_class_systemh_line BASE ${grtos_system_name}_GRTOS_0_S_GLOBAL_BASE
    add_class_systemh_line BASE_GMEMORY ${grtos_system_name}_GRTOS_0_S_GMEMORY_BASE
    add_class_systemh_line BASE_TESTING ${grtos_system_name}_GRTOS_0_S_TESTING_BASE
    add_class_systemh_line BASE_MONITOR ${grtos_system_name}_NIOS_AVALON_MONITOR_BASE

	
	add_class_systemh_line PROCESSOR_BASE [ string toupper ${grtos_system_name}_GRTOS_0_S_PROCESSOR1_BASE ]
    
    add_class_systemh_line GRTOS_S_PROCESSOR1_IRQ_INTERRUPT_CONTROLLER_ID [ string toupper ${grtos_system_name}_GRTOS_0_S_PROCESSOR1_IRQ_INTERRUPT_CONTROLLER_ID  ]
    add_class_systemh_line GRTOS_S_PROCESSOR1_IRQ [ string toupper ${grtos_system_name}_GRTOS_0_S_PROCESSOR1_IRQ  ]
    
    


   


	
}

## ---------------------------------
## Callback: validation.
## Wired by grtos_sw.tcl (set_sw_property validation_callback validate).
## Intentionally empty: the controller has no BSP-level settings to check.
##
## args 0 : module instance name
proc validate { args } {

}

# End of file
