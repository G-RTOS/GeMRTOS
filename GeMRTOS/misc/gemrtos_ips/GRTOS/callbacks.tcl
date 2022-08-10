#
# callbacks.tcl
#
# This script is set as a callback in the components _sw.tcl file.
# It is set using the command: 'set_sw_property callback_source_file callbacks.tcl'
#


# Query number of shadows register sets for the CPU
set cpu_name [get_cpu_name]


## ---------------------------------
#| * callback *
#| This is a callback proc set in the _sw.tcl file.
#| The callback is called from a context that maintains a reference to an
#| individual SOPC module instance, that uses the software element defined in
#| the _sw.tcl.
#| It is set using the command: 'set_sw_property initialization_callback initialize'
#|
#| Initialize the default value for the following settings (per VIC instance):
#| - VEC_SIZE
#| - VEC_TBL_BASE
#|
#| args 0 : module instance name
#|
proc initialize { args } {

}

## ---------------------------------
#| * callback *
#| This is a callback proc set in the _sw.tcl file.
#| The callback is called from a context that maintains a reference to an
#| individual SOPC module instance, that uses the software element defined in
#| the _sw.tcl.
#| It is set using the command: 'set_sw_property initialization_callback generate'
#|
#|
#|
#| args 0 : module instance name
#| args 1 : path to the BSP directory
#| args 2 : driver sub directory
#|
proc generate { args } {

    if {[llength $args] < 3} {
        error "Insufficient arguments for generate callback proc (expected 3, received [llength $args])."
    }

    set module_name [get_module_name]
    set NPROCESSORS [ get_module_assignment "embeddedsw.CMacro.NPROCESSORS" ]

    # #########################
    add_module_systemh_line $module_name [ get_module_assignment "embeddedsw.CMacro.NPROCESSORS" ]
    add_module_systemh_line $module_name [ get_module_assignment "embeddedsw.CMacro.PRESCALE" ]
    add_module_systemh_line $module_name [ get_module_assignment "embeddedsw.CMacro.GRTOSFREQUENCY" ]
    add_module_systemh_line $module_name [ get_module_assignment "embeddedsw.CMacro.GRTOSINTERRUPTS" ]    
    add_module_systemh_line $module_name [ get_module_assignment "embeddedsw.CMacro.GRTOSGENERATIONID" ]
    add_module_systemh_line $module_name \"[ get_module_assignment "embeddedsw.CMacro.PROCESSOR_TYPE" ]\"
    add_module_systemh_line $module_name \"[ get_module_assignment "embeddedsw.CMacro.INSTRUCTION_CACHE_SIZE" ]\"
    add_module_systemh_line $module_name \"[ get_module_assignment "embeddedsw.CMacro.INSTRUCTION_CACHE_BURST" ]\"
    add_module_systemh_line $module_name \"[ get_module_assignment "embeddedsw.CMacro.ENABLE_HPS_MAP_ACCESS" ]\"
    add_module_systemh_line $module_name [ get_module_assignment "embeddedsw.CMacro.NIOS_CLOCK_FREQUENCY" ]
    add_module_systemh_line $module_name [ get_module_assignment "embeddedsw.CMacro.BUS_CLOCK_FREQUENCY" ]
    add_module_systemh_line $module_name [ get_module_assignment "embeddedsw.CMacro.BUS_WIDTH" ]
    add_module_systemh_line $module_name [ get_module_assignment "embeddedsw.CMacro.EXTERNAL_MEMORY_SPAN" ]
    add_module_systemh_line $module_name \"[ get_module_assignment "embeddedsw.CMacro.EXTERNAL_MEMORY_UNIT" ]\"
    
    add_class_systemh_line [ string toupper "NPROCESSORS" ] [ get_module_assignment "embeddedsw.CMacro.NPROCESSORS" ]
    add_class_systemh_line [ string toupper "PRESCALE" ] [ get_module_assignment "embeddedsw.CMacro.PRESCALE" ]
    add_class_systemh_line [ string toupper "GRTOSFREQUENCY" ] [ get_module_assignment "embeddedsw.CMacro.GRTOSFREQUENCY" ]
    add_class_systemh_line [ string toupper "GRTOSINTERRUPTS" ] [ get_module_assignment "embeddedsw.CMacro.GRTOSINTERRUPTS" ]    
    add_class_systemh_line [ string toupper "GRTOSGENERATIONID" ] [ get_module_assignment "embeddedsw.CMacro.GRTOSGENERATIONID" ]
    add_class_systemh_line [ string toupper "PROCESSOR_TYPE" ] \"[ get_module_assignment "embeddedsw.CMacro.PROCESSOR_TYPE" ]\"
    add_class_systemh_line [ string toupper "INSTRUCTION_CACHE_SIZE" ] \"[ get_module_assignment "embeddedsw.CMacro.INSTRUCTION_CACHE_SIZE" ]\"
    add_class_systemh_line [ string toupper "INSTRUCTION_CACHE_BURST" ] \"[ get_module_assignment "embeddedsw.CMacro.INSTRUCTION_CACHE_BURST" ]\"
    add_class_systemh_line [ string toupper "ENABLE_HPS_MAP_ACCESS" ] \"[ get_module_assignment "embeddedsw.CMacro.ENABLE_HPS_MAP_ACCESS" ]\"
    add_class_systemh_line [ string toupper "NIOS_CLOCK_FREQUENCY" ] [ get_module_assignment "embeddedsw.CMacro.NIOS_CLOCK_FREQUENCY" ]
    add_class_systemh_line [ string toupper "BUS_CLOCK_FREQUENCY" ] [ get_module_assignment "embeddedsw.CMacro.BUS_CLOCK_FREQUENCY" ]
    add_class_systemh_line [ string toupper "BUS_WIDTH" ] [ get_module_assignment "embeddedsw.CMacro.BUS_WIDTH" ]
    add_class_systemh_line [ string toupper "EXTERNAL_MEMORY_SPAN" ] [ get_module_assignment "embeddedsw.CMacro.EXTERNAL_MEMORY_SPAN" ]
    add_class_systemh_line [ string toupper "EXTERNAL_MEMORY_UNIT" ] \"[ get_module_assignment "embeddedsw.CMacro.EXTERNAL_MEMORY_UNIT" ]\"
    # #########################
    
    set grtos_system_name [string map {_grtos_0 ""} $module_name]
    set grtos_on_chip_memory1 ${grtos_system_name}_onchip_memory2_0
    
    add_class_systemh_line SYSTEM_NAME \"${grtos_system_name}\"
    
    add_class_systemh_line GRTOS_BASE [ string toupper ${grtos_system_name}_GRTOS_0_S_GLOBAL_BASE ]
    
    add_class_systemh_line GRTOS_MONITOR_BASE [ string toupper ${grtos_system_name}_GRTOS_0_S_PROCESSOR_MONITOR_BASE  ]
    add_class_systemh_line GRTOS_S_PROCESSOR1_IRQ_INTERRUPT_CONTROLLER_ID [ string toupper ${grtos_system_name}_GRTOS_0_S_PROCESSOR1_IRQ_INTERRUPT_CONTROLLER_ID  ]
    add_class_systemh_line GRTOS_S_PROCESSOR1_IRQ [ string toupper ${grtos_system_name}_GRTOS_0_S_PROCESSOR1_IRQ  ]
    
    
    for {set i 1} {$i <= $NPROCESSORS} {incr i} {
        add_class_systemh_line GRTOS_PROCESSOR_BASE${i} [ string toupper ${grtos_system_name}_GRTOS_0_S_PROCESSOR${i}_BASE  ]
    }
    
    set list (int*)[ string toupper ${grtos_system_name}_GRTOS_0_S_PROCESSOR1_BASE  ]
    for {set i 2} {$i <= $NPROCESSORS} {incr i} {
        set list ${list},(int*)[ string toupper ${grtos_system_name}_GRTOS_0_S_PROCESSOR${i}_BASE  ]
    }
    add_class_systemh_line GRTOS_PROCESSOR_BASE (int*\[\])\{${list}\}
 
    
    # Get BSP generate directory path
    set bsp_dir [lindex $args 1]
    set bsp_subdir [lindex $args 2]

}

## ---------------------------------
#| * callback *
#| This is a callback proc set in the _sw.tcl file.
#| The callback is called from a context that maintains a reference to an
#| individual SOPC module instance, that uses the software element defined in
#| the _sw.tcl.
#| It is set using the command: 'set_sw_property initialization_callback validate'
#|
#| This callback validates the vector size, RIL, RRS and RNMI
#|
#| args 0 : module instance name
#|
proc validate { args } {

}

# Validate VEC_SIZE has one of these allowed values: 16, 32, 64, 128, 256, 512
proc sub_validate_vec_size { module_name } {

}




# Add warning and lincense agreement to vector table file.
proc add_file_header { fp } {

    # Add warning
    puts $fp "/*
 * DO NOT MODIFY THIS FILE
 *
 * Changing this file will have subtle consequences
 * which will almost certainly lead to a nonfunctioning
 * system. If you do modify this file, be aware that your
 * changes will be overwritten and lost when this file
 * is generated again.
 *
 * DO NOT MODIFY THIS FILE
 */"

    puts $fp "\n"

    # Add license agreement
    puts $fp "/*
 * License Agreement
 *
 * Copyright (c) 2009
 * Altera Corporation, San Jose, California, USA.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the \"Software\"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * This agreement shall be governed in all respects by the laws of the State
 * of California and by the laws of the United States of America.
 */"

    puts $fp "\n"
}

# ------------------------------------------------------------------------------
#                       VIC class callbacks
# ------------------------------------------------------------------------------

#| * Callback *
#| This is a callback proc set in the _sw.tcl file.
#| This is a callback for the driver class defined by a _sw.tcl file for the driver element
#|
proc class_initialize { args } {

}

#| * Callback *
#| This is a callback proc set in the _sw.tcl file.
#| This is a callback for the driver class defined by a _sw.tcl file for the driver element
#|
proc class_validate { args } {

}


# Validate RIL value in the allowed range, (RIL > 0) and
# (RIL <= ((1 << RIL_WIDTH) - 1))
proc sub_class_validate_ril { driver_name } {

}


# Validate RRS value less than NUM_OF_SHADOW_REG_SETS and greater than 0
proc sub_class_validate_rrs { driver_name } {

}

# Validate RNMI across all the VIC instances:
# 1. If the particular interrupt sets as non-maskable interrupt (RNMI=1),
#    no sharing register set with normal interrupts (interrupts with RNMI=0),
#    but multiple NMIs can share same shadow register set.
# 2. Issue warning if any interrupt with NMI enabled is
#    not the highest priority (RIL).
proc sub_class_validate_rnmi { driver_name module_list } {

}

# Validate "linker_section" is a valid linker section.
# Validate linker section's memory device is connected to data and instruction master.
# Validate linker section's memory device is writeable.
proc sub_class_validate_linker_section { driver_name } {
  
}

# Validate preemption
proc sub_class_validate_preemption { driver_name } {
  
}

# This proc returns a list of module instance name with specified driver name.
proc get_module_instance_list { driver_name } {

}

# Return 0 if any peripheral doesn't support preemption, else otherwise.
proc get_default_peripheral_preemption { driver_name } {

}

# Sort VIC list in controller ID order (ID 0 in first element).
proc sort_vic_order { vic_list } {

}

# Initialize default setting for RIL, RRS and RNMI for each interrupt in each VIC.
# 1. For each VIC, examining their RIL width, and then using the minimum RIL
#    width of all VICs as maximum RIL value.
# 2. Priority order: IRQ port 0 on the VIC connected to the CPU (i.e: VIC 0) is
#    highest priority, IRQ port (max) on VIC 0 is higher priority than IRQ 0 on
#    VIC 1, and so on.
# 3. Decrement the RIL value (start with RIL max) after being assigned to each
#    interrupt until you reach RIL = 1.
# 4. Uses the RIL values to assign shadow register sets, so that interrupts with
#    same RIL have same register set number.
proc initialize_default_per_intr_settings { driver_name } {

}

# Get minimum and maximum RIL assigned to each register set.
proc get_ril_min_max_list { driver_name module_list } {

}

# Validate RILs assigned to each register set do not overlap with any other
# register set.
proc sub_class_validate_rrs_ril { driver_name module_list } {

}

# Examine the minimum RIL_WIDTH of all VICs and return the maximum RIL of this RIL_WIDTH.
proc get_ril_max { vic_list } {

}
# End of file
