# @date      2015-2020

# @copyright Ricardo Cayssials 
# @copyright All rights reserved.
# @warning This product was produced by Ricardo Cayssials in the hope that it 
# will be useful, but WITHOUT ANY WARRANTY; without even the implied 
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
# @warning DISCLAIMER OF WARRANTY. THIS PRODUCT IS LICENSED “AS IS.” YOU BEAR THE RISK OF USING IT. 
# @warning RICARDO CAYSSIALS GIVES NO EXPRESS WARRANTIES, GUARANTEES, OR CONDITIONS. 
# TO THE EXTENT PERMITTED UNDER APPLICABLE LAWS, RICARDO CAYSSIALS EXCLUDES ALL IMPLIED WARRANTIES,
# INCLUDING MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
# 
# 
# /******************************************************************************
# *                                                                             *
# * License Agreement                                                           *
# *                                                                             *
# * Copyright (c) Ricardo L. Cayssials                                          *
# * All rights reserved.                                                        *
# *                                                                             *
# ******************************************************************************/

# _hw.tcl file for GeMRTOS_Multiprocessor
package require -exact qsys 14.0


# module properties
set_module_property NAME GeMRTOS_Multiprocessor
set_module_property DISPLAY_NAME "GeMRTOS_Multiprocessor"

# default module properties
set_module_property VERSION 1.0
set_module_property GROUP GeMRTOS
set_module_property DESCRIPTION "GeMRTOS_Multiprocessor"
set_module_property AUTHOR "Ricardo Cayssials"
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true
set_module_property OPAQUE_ADDRESS_MAP false

set_module_property DATASHEET_URL "https://gemrtos.com"

set_module_property COMPOSITION_CALLBACK compose
set_module_property VALIDATION_CALLBACK validate
# set_module_property ELABORATION_CALLBACK elaborate
# set_module_property PARAMETER_UPGRADE_CALLBACK elaborate


# 
# parameters
# 
# Number of Processors
add_parameter NProcessors INTEGER 1
# set_parameter_property NProcessors DEFAULT_VALUE 3
set_parameter_property NProcessors DISPLAY_NAME "Number of Processors"
set_parameter_property NProcessors TYPE INTEGER
set_parameter_property NProcessors UNITS None
set_parameter_property NProcessors ALLOWED_RANGES {1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32}
set_parameter_property NProcessors DESCRIPTION "Number of System Processors"
set_parameter_property NProcessors HDL_PARAMETER false
add_display_item "Parameters" NProcessors PARAMETER


# ############################################################################
# Processor version
add_parameter Processor_type STRING "Nios II/f (full)" "Processor type"
set_parameter_property Processor_type DISPLAY_NAME "Processor type"
set_parameter_property Processor_type UNITS None
set_parameter_property Processor_type ALLOWED_RANGES {"Nios II/e (economy)" "Nios II/f (full)" "Nios V/m" "Nios V/g"}
set_parameter_property Processor_type HDL_PARAMETER false
add_display_item "Parameters" Processor_type PARAMETER

# ############################################################################
# Processor Reset vector memory
# 07-06-2024 add_parameter resetSlave STRING "Internal GeMRTOS (default)" "Reset vector memory"
add_parameter resetSlave STRING
set_parameter_property resetSlave DISPLAY_NAME "Reset vector memory"
set_parameter_property resetSlave UNITS None
add_display_item "Parameters" resetSlave PARAMETER
set_parameter_property resetSlave HDL_PARAMETER false

# ############################################################################
# Processor Reset vector address
add_parameter resetOffset INTEGER  0
set_parameter_property resetOffset DISPLAY_NAME "Reset vector offset"
set_parameter_property resetOffset "DISPLAY_HINT"  "hexadecimal"
set_parameter_property resetOffset UNITS None
set_parameter_property resetOffset HDL_PARAMETER false
add_display_item "Parameters" resetOffset PARAMETER
set_parameter_property resetOffset DERIVED false

# 28-06-2024 ###################
# ############################################################################
# Processor Reset vector memory
# 07-06-2024 add_parameter exceptionSlave STRING "Internal GeMRTOS (default)" "Reset vector memory"
add_parameter exceptionSlave STRING
set_parameter_property exceptionSlave DISPLAY_NAME "Exception vector memory (only for Nios II)"
set_parameter_property exceptionSlave UNITS None
add_display_item "Parameters" exceptionSlave PARAMETER
set_parameter_property exceptionSlave HDL_PARAMETER false

# ############################################################################
# Processor Reset vector address
add_parameter exceptionOffset INTEGER  0
set_parameter_property exceptionOffset DISPLAY_NAME "Exception vector offset (only for Nios II)"
set_parameter_property exceptionOffset "DISPLAY_HINT"  "hexadecimal"
set_parameter_property exceptionOffset UNITS None
set_parameter_property exceptionOffset HDL_PARAMETER false
add_display_item "Parameters" exceptionOffset PARAMETER
set_parameter_property exceptionOffset DERIVED false
# 28-06-2024 ###################

# ############################################################################
# Processor Instruction Cache
add_parameter icache_size_parameter STRING "None" "Instruction cache size"
set_parameter_property icache_size_parameter DISPLAY_NAME "Instruction cache size"
set_parameter_property icache_size_parameter UNITS None
set_parameter_property icache_size_parameter ALLOWED_RANGES {"None" "512 Bytes" "1 KB" "2 KB" "4 KB" "8 KB" "16 KB" "32 KB" "64 KB"}
add_display_item "Parameters" icache_size_parameter PARAMETER
set_parameter_property icache_size_parameter HDL_PARAMETER false


# ############################################################################
# Processor Instruction Cache burst enable
# add_parameter icache_burst_parameter STRING "Disable" "Add burstcount signal to instruction_master"
# set_parameter_property icache_burst_parameter DISPLAY_NAME "Add burstcount signal to instruction_master"
# set_parameter_property icache_burst_parameter UNITS None
# set_parameter_property icache_burst_parameter ALLOWED_RANGES {"Enable" "Disable"}
# set_parameter_property icache_burst_parameter HDL_PARAMETER false

# ############################################################################
# Processor Pipeline for Nios V/m
add_parameter pipelineArch_parameter STRING  "True"
set_parameter_property pipelineArch_parameter DISPLAY_NAME "Enable pipeline in CPU  (Nios V/m only)"
set_parameter_property pipelineArch_parameter UNITS None
set_parameter_property pipelineArch_parameter HDL_PARAMETER false
set_parameter_property pipelineArch_parameter ALLOWED_RANGES {"True" "False"}
add_display_item "Parameters" pipelineArch_parameter PARAMETER
set_parameter_property pipelineArch_parameter DERIVED false

# ############################################################################
# Processor table for future detail
# add_parameter names STRING_LIST {"Nios II/e (economy)" "Nios II/f (full)"}
# set_parameter_property names ALLOWED_RANGES {"Nios II/e (economy)" "Nios II/f (full)"}
# add_parameter counts INTEGER_LIST
# add_display_item "" myTable GROUP TABLE
# add_display_item myTable names PARAMETER
# add_display_item myTable counts PARAMETER

# ############################################################################
# Independent external processor buses
add_parameter INDEPENDENT_EXTERNAL_PROCESSOR_BUSES BOOLEAN true
set_parameter_property INDEPENDENT_EXTERNAL_PROCESSOR_BUSES DISPLAY_NAME "Independent external processor buses"
set_parameter_property INDEPENDENT_EXTERNAL_PROCESSOR_BUSES TYPE BOOLEAN
set_parameter_property INDEPENDENT_EXTERNAL_PROCESSOR_BUSES UNITS None
set_parameter_property INDEPENDENT_EXTERNAL_PROCESSOR_BUSES DESCRIPTION "Independent external processor buses"
add_display_item "Parameters" INDEPENDENT_EXTERNAL_PROCESSOR_BUSES PARAMETER
set_parameter_property INDEPENDENT_EXTERNAL_PROCESSOR_BUSES HDL_PARAMETER false

# ############################################################################
# Include JTAG-UART for STDIO interface
add_parameter INCLUDE_JTAG_UART_STDDIO BOOLEAN true
set_parameter_property INCLUDE_JTAG_UART_STDDIO DISPLAY_NAME "Include JTAG UART for STDIO"
set_parameter_property INCLUDE_JTAG_UART_STDDIO TYPE BOOLEAN
set_parameter_property INCLUDE_JTAG_UART_STDDIO UNITS None
set_parameter_property INCLUDE_JTAG_UART_STDDIO DESCRIPTION "Include JTAG UART for STDIO interface"
add_display_item "Parameters" INCLUDE_JTAG_UART_STDDIO PARAMETER
set_parameter_property INCLUDE_JTAG_UART_STDDIO HDL_PARAMETER false

# ############################################################################
# Include JTAG-UART for STDERR interface
add_parameter INCLUDE_JTAG_UART_STDERR BOOLEAN true
set_parameter_property INCLUDE_JTAG_UART_STDERR DISPLAY_NAME "Include JTAG UART for STDERR"
set_parameter_property INCLUDE_JTAG_UART_STDERR TYPE BOOLEAN
set_parameter_property INCLUDE_JTAG_UART_STDERR UNITS None
set_parameter_property INCLUDE_JTAG_UART_STDERR DESCRIPTION "Include JTAG UART for STDERR interface"
add_display_item "Parameters" INCLUDE_JTAG_UART_STDERR PARAMETER
set_parameter_property INCLUDE_JTAG_UART_STDERR HDL_PARAMETER false

# ############################################################################
# Include JTAG-UART for each processor
add_parameter INCLUDE_JTAG_UART_PROCESSOR BOOLEAN false
set_parameter_property INCLUDE_JTAG_UART_PROCESSOR DISPLAY_NAME "Include JTAG UART for each processor"
set_parameter_property INCLUDE_JTAG_UART_PROCESSOR TYPE BOOLEAN
set_parameter_property INCLUDE_JTAG_UART_PROCESSOR UNITS None
set_parameter_property INCLUDE_JTAG_UART_PROCESSOR DESCRIPTION "Include JTAG UART for each processor"
set_parameter_property INCLUDE_JTAG_UART_PROCESSOR HDL_PARAMETER false
add_display_item "Parameters" INCLUDE_JTAG_UART_PROCESSOR PARAMETER
set_parameter_property INCLUDE_JTAG_UART_PROCESSOR VISIBLE true

# ############################################################################
# Enable HPS interface
add_parameter ENABLE_HPS_MAP_ACCESS BOOLEAN false
set_parameter_property ENABLE_HPS_MAP_ACCESS DISPLAY_NAME "Enable HPS internal access"
set_parameter_property ENABLE_HPS_MAP_ACCESS TYPE BOOLEAN
set_parameter_property ENABLE_HPS_MAP_ACCESS UNITS None
set_parameter_property ENABLE_HPS_MAP_ACCESS DESCRIPTION "Enable Avalon MM slave for HPS to access internal memory addresses"
add_display_item "Parameters" ENABLE_HPS_MAP_ACCESS PARAMETER
set_parameter_property ENABLE_HPS_MAP_ACCESS HDL_PARAMETER false

# ############################################################################
# Quartus project file
add_parameter QUARTUS_PROJECT_FILE STRING "NOT DEFINED"
set_parameter_property QUARTUS_PROJECT_FILE DISPLAY_NAME "Quartus Prime project name"
set_parameter_property QUARTUS_PROJECT_FILE ALLOWED_RANGES {"NOT DEFINED"}
set_parameter_property QUARTUS_PROJECT_FILE UNITS None
set_parameter_property QUARTUS_PROJECT_FILE DESCRIPTION "Name of the Quartus Project in which the Platfrom Designer is instantiated"
add_display_item "Parameters" QUARTUS_PROJECT_FILE PARAMETER
set_parameter_property QUARTUS_PROJECT_FILE HDL_PARAMETER false

# ############################################################################
# QSYS project file
add_parameter QSYS_PROJECT_FILE STRING "NOT DEFINED"
set_parameter_property QSYS_PROJECT_FILE DISPLAY_NAME "Platform Designer project name"
set_parameter_property QSYS_PROJECT_FILE ALLOWED_RANGES {"NOT DEFINED"}
set_parameter_property QSYS_PROJECT_FILE UNITS None
set_parameter_property QSYS_PROJECT_FILE DESCRIPTION "Name of the Platform Designer Project in which the GeMRTOS Multiprocessor is instantiated"
add_display_item "Parameters" QSYS_PROJECT_FILE PARAMETER
set_parameter_property QSYS_PROJECT_FILE HDL_PARAMETER false

# ############################################################################
# Input clock for processors
add_interface clk_processors clock sink
set_interface_property clk_processors clockRate 0
set_interface_property clk_processors ENABLED true

add_display_item "" "Parameters" group tab
add_display_item "" "Information" group tab

add_parameter CFrequency INTEGER  
set_parameter_property CFrequency SYSTEM_INFO {CLOCK_RATE clk_processors}
set_parameter_property CFrequency DISPLAY_NAME "Processor frequency"
set_parameter_property CFrequency UNITS Hertz
add_display_item "Information" CFrequency PARAMETER
set_parameter_property CFrequency HDL_PARAMETER false


add_parameter instSlaveMapParam STRING "" "instSlaveMapParam"
set_parameter_property instSlaveMapParam DISPLAY_NAME "instSlaveMapParam"
set_parameter_property instSlaveMapParam SYSTEM_INFO {ADDRESS_MAP gemrtos_avalon_processor_m1}
add_display_item "Information" instSlaveMapParam PARAMETER
set_parameter_property instSlaveMapParam HDL_PARAMETER false
set_parameter_property instSlaveMapParam VISIBLE false
set_parameter_property instSlaveMapParam DERIVED true


add_parameter resestAbsolute INTEGER 
set_parameter_property resestAbsolute DISPLAY_NAME "Reset Absolute memory base"
add_display_item "Information" resestAbsolute PARAMETER
set_parameter_property resestAbsolute "DISPLAY_HINT"  "hexadecimal"
set_parameter_property resestAbsolute HDL_PARAMETER false
set_parameter_property resestAbsolute DERIVED true

# 28-06-2024 ###################

add_parameter exceptionAbsolute INTEGER 
set_parameter_property exceptionAbsolute DISPLAY_NAME "Reset Absolute memory base"
add_display_item "Information" exceptionAbsolute PARAMETER
set_parameter_property exceptionAbsolute "DISPLAY_HINT"  "hexadecimal"
set_parameter_property exceptionAbsolute HDL_PARAMETER false
set_parameter_property exceptionAbsolute DERIVED true

# 28-06-2024 ###################

# ############################################################################
# Input clock for external bus
add_interface clk_external_bus clock sink
set_interface_property clk_external_bus clockRate 0
set_interface_property clk_external_bus ENABLED true
  
  
add_parameter CEFrequency INTEGER  
set_parameter_property CEFrequency SYSTEM_INFO {CLOCK_RATE clk_external_bus}
set_parameter_property CEFrequency DISPLAY_NAME "External bus frequency"
set_parameter_property CEFrequency UNITS Hertz
add_display_item "Information" CEFrequency PARAMETER
set_parameter_property CEFrequency HDL_PARAMETER false


# ############################################################################
# External reset input
add_interface reset_in reset sink

# ############################################################################
# Interface for processor 1 and getting the bus width for external memory 
# registers
add_interface gemrtos_avalon_processor_m1 avalon start
set_interface_property gemrtos_avalon_processor_m1 ENABLED true

add_parameter BUS_WIDTH INTEGER  
set_parameter_property BUS_WIDTH SYSTEM_INFO {ADDRESS_WIDTH gemrtos_avalon_processor_m1}
set_parameter_property BUS_WIDTH DISPLAY_NAME "External bus width"
set_parameter_property BUS_WIDTH UNITS bits
add_display_item "Information" BUS_WIDTH PARAMETER
set_parameter_property BUS_WIDTH HDL_PARAMETER false

# External memory span (derived from BUS_WIDTH)
add_parameter EXTERNAL_MEMORY_SPAN INTEGER 4
set_parameter_property EXTERNAL_MEMORY_SPAN DERIVED true
set_parameter_property EXTERNAL_MEMORY_SPAN DISPLAY_NAME  "External Memory span"
set_parameter_property EXTERNAL_MEMORY_SPAN UNITS None
add_display_item "Information" EXTERNAL_MEMORY_SPAN PARAMETER
set_parameter_property EXTERNAL_MEMORY_SPAN HDL_PARAMETER false

# External memory span (derived from BUS_WIDTH)
add_parameter EXTERNAL_MEMORY_UNIT STRING ""
set_parameter_property EXTERNAL_MEMORY_UNIT DERIVED true
set_parameter_property EXTERNAL_MEMORY_UNIT DISPLAY_NAME  "External Memory unit"

set_parameter_property EXTERNAL_MEMORY_UNIT UNITS None
add_display_item "Information" EXTERNAL_MEMORY_UNIT PARAMETER
set_parameter_property EXTERNAL_MEMORY_UNIT HDL_PARAMETER false

add_parameter DEVICE_NAME STRING ""  
set_parameter_property DEVICE_NAME SYSTEM_INFO {DEVICE}
set_parameter_property DEVICE_NAME DISPLAY_NAME "Device"
set_parameter_property DEVICE_NAME DESCRIPTION "FPGA device selected"
set_parameter_property DEVICE_NAME UNITS None
add_display_item "Information" DEVICE_NAME PARAMETER
set_parameter_property DEVICE_NAME HDL_PARAMETER false

# 02-07-2024 ############################
    # Get the interrupts connected to the GRTOS
    add_parameter GRTOS_INTERRUPTS LONG 
    set_parameter_property GRTOS_INTERRUPTS SYSTEM_INFO {INTERRUPTS_USED gemrtos_dirq_input}    
    add_display_item "Information" GRTOS_INTERRUPTS PARAMETER 
# 02-07-2024 ############################

# Address Width of Avalon Bridge
# # add_parameter BridgeAddressWidth INTEGER 26 "Address width of Avalon Bridge"
# # set_parameter_property BridgeAddressWidth DEFAULT_VALUE 26
# # set_parameter_property BridgeAddressWidth DISPLAY_NAME "Address width of Avalon Bridge"
# # set_parameter_property BridgeAddressWidth TYPE INTEGER
# # set_parameter_property BridgeAddressWidth UNITS None
# # set_parameter_property BridgeAddressWidth ALLOWED_RANGES {1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27}
# # set_parameter_property BridgeAddressWidth DESCRIPTION "Number of address bits for memory map addressing"
# # set_parameter_property BridgeAddressWidth HDL_PARAMETER false

# Number of IRQ signal width of IRQ_BRIDGE
# add_parameter ExtIRQs INTEGER 20 "Number of external IRQs"
# set_parameter_property ExtIRQs DEFAULT_VALUE 20
# set_parameter_property ExtIRQs DISPLAY_NAME "Number of external IRQs"
# set_parameter_property ExtIRQs TYPE INTEGER
# set_parameter_property ExtIRQs UNITS None
# set_parameter_property ExtIRQs ALLOWED_RANGES {0:25}
# set_parameter_property ExtIRQs DESCRIPTION "Number of external IRQs"
# set_parameter_property ExtIRQs HDL_PARAMETER false

proc elaborate { } {

}

proc validate { } {

	set error_stop 0
	# Get the information parameters of the GUI
    set bus_width [get_parameter_value BUS_WIDTH]
    if { $bus_width == 0 } {
        send_message error "External memory device should be connected to processor buses"
    } else {
		if { $bus_width < 10 } {
			set bus_width1 [expr 2**[get_parameter_value BUS_WIDTH]]
			set_parameter_value EXTERNAL_MEMORY_SPAN ${bus_width1}
			set_parameter_value EXTERNAL_MEMORY_UNIT "BYTES"
		} elseif { $bus_width < 20 } {
			set bus_width1 [expr (2**[get_parameter_value BUS_WIDTH]) / 1024]
			set_parameter_value EXTERNAL_MEMORY_SPAN ${bus_width1}
			set_parameter_value EXTERNAL_MEMORY_UNIT "KILOBYTES"
		} elseif { $bus_width < 30 } {
			set bus_width1 [expr (2**[get_parameter_value BUS_WIDTH]) / 1048576]
			set_parameter_value EXTERNAL_MEMORY_SPAN ${bus_width1}
			set_parameter_value EXTERNAL_MEMORY_UNIT "MEGABYTES"
		} else {
			set bus_width1 [expr (2**[get_parameter_value BUS_WIDTH]) / 1073741824]
			set_parameter_value EXTERNAL_MEMORY_SPAN ${bus_width1}
			set_parameter_value EXTERNAL_MEMORY_UNIT "GYGABYTES"
		}
    }
	
	
    set Type_of_Processor [get_parameter_value Processor_type]
	
	if { ($Type_of_Processor == "Nios II/f (full)") || ( $Type_of_Processor == "Nios II/e (economy)" ) } {
		set_parameter_property exceptionSlave VISIBLE true
		set_parameter_property exceptionOffset VISIBLE true
		set_parameter_property exceptionSlave ENABLED true
		set_parameter_property exceptionOffset ENABLED true
	} else {
		set_parameter_property exceptionSlave VISIBLE false
		set_parameter_property exceptionOffset VISIBLE false
		set_parameter_property exceptionSlave ENABLED false
		set_parameter_property exceptionOffset ENABLED false
	}
	

    if { ($Type_of_Processor == "Nios II/f (full)") || ( $Type_of_Processor == "Nios V/g" ) } {
		if { ($Type_of_Processor == "Nios II/f (full)") } {
			set_parameter_property icache_size_parameter ENABLED true
			set_parameter_property icache_size_parameter ALLOWED_RANGES {"None" "512 Bytes" "1 KB" "2 KB" "4 KB" "8 KB" "16 KB" "32 KB" "64 KB"}
		}
		if { ($Type_of_Processor == "Nios V/g") } {
			set_parameter_property icache_size_parameter ENABLED true
			set_parameter_property icache_size_parameter ALLOWED_RANGES {"1 KB" "2 KB" "4 KB" "8 KB" "16 KB"}
		}		
        # set_parameter_property icache_burst_parameter ENABLED true
    } else {
        set_parameter_property icache_size_parameter ENABLED false
        # set_parameter_property icache_burst_parameter ENABLED false
    }

    if { ( $Type_of_Processor == "Nios V/m" ) } {
		set_parameter_property pipelineArch_parameter ENABLED true
	} else {
		set_parameter_property pipelineArch_parameter ENABLED false
	}

	set resetSlave     [ get_parameter_value resetSlave ]
	if { ($resetSlave == "Internal GeMRTOS (default)" ) } {
		set_parameter_property resetOffset ENABLED false
		# set_parameter_value resetOffset 0
	} else {
		set_parameter_property resetOffset ENABLED true
		# set_parameter_value resetOffset 120
	}

	# Get the controller file according to the device selected
	# 17-05-2024 set DEVICE_NAME [get_parameter_value DEVICE_NAME]
	# 17-05-2024 if {[ file exists "./devices/${DEVICE_NAME}/grtos.qxp" ] == 1 } {	
	# 17-05-2024 	file copy -force ./devices/${DEVICE_NAME}/grtos.qxp ../GeMRTOS_controller
	# 17-05-2024 } else {
	# 17-05-2024 	send_message Error "Device ${DEVICE_NAME} not found for GeMRTOS. Please, request it to contact@gemrtos.com."
	# 17-05-2024 }
    
    # set module_name [get_module_property name]
    # set grtos_system_name [string map {_grtos_0 ""} $module_name]
    # set project_name [get_module_property name]
    # set DESCRIPTION [get_module_property DESCRIPTION]
    # set NAME [get_module_property NAME]
    # set VERSION [get_module_property VERSION]
    # set MODULE_DIRECTORY [get_module_property MODULE_DIRECTORY]
    # set INTERNAL [get_module_property INTERNAL]
    # set OPAQUE_ADDRESS_MAP [get_module_property OPAQUE_ADDRESS_MAP]
    # set GROUP [get_module_property GROUP]
    # set AUTHOR [get_module_property AUTHOR]
    # set ICON_PATH [get_module_property ICON_PATH]
    # set DISPLAY_NAME [get_module_property DISPLAY_NAME]
    # set DATASHEET_URL [get_module_property DATASHEET_URL]
    # set INSTANTIATE_IN_SYSTEM_MODULE [get_module_property INSTANTIATE_IN_SYSTEM_MODULE]
    # set EDITABLE [get_module_property EDITABLE]
    # set VALIDATION_CALLBACK [get_module_property VALIDATION_CALLBACK]
    # set EDITOR_CALLBACK [get_module_property EDITOR_CALLBACK]
    # set ELABORATION_CALLBACK [get_module_property ELABORATION_CALLBACK]
    # set COMPOSITION_CALLBACK [get_module_property COMPOSITION_CALLBACK]
    # set SUPPORTED_DEVICE_FAMILIES [get_module_property SUPPORTED_DEVICE_FAMILIES]
    # set REPORT_TO_TALKBACK [get_module_property REPORT_TO_TALKBACK]
    # set ALLOW_GREYBOX_GENERATION [get_module_property ALLOW_GREYBOX_GENERATION]
    # set SUPPRESS_WARNINGS [get_module_property SUPPRESS_WARNINGS]
    # set STRUCTURAL_COMPOSITION_CALLBACK [get_module_property STRUCTURAL_COMPOSITION_CALLBACK]
    # set NATIVE_INTERPRETER [get_module_property NATIVE_INTERPRETER]
    # set PREFERRED_SIMULATION_LANGUAGE [get_module_property PREFERRED_SIMULATION_LANGUAGE]
    # set REPORT_HIERARCHY [get_module_property REPORT_HIERARCHY]
    # set UPGRADEABLE_FROM [get_module_property UPGRADEABLE_FROM]
	# 
    # set fp [open "../../gemrtos_nios_bsp_create.rpt" w+]
    #     puts $fp "# module_name= ${module_name}"
    #     puts $fp "# grtos_system_name= ${grtos_system_name}"
    #     puts $fp "# project_name= ${project_name}"
	# 
    #     puts $fp "# DESCRIPTION= ${DESCRIPTION}"
    #     puts $fp "# NAME= ${NAME}"
    #     puts $fp "# VERSION= ${VERSION}"
	# 	puts $fp "# DEVICE_NAME= ${DEVICE_NAME}"
    #     puts $fp "# MODULE_DIRECTORY= ${MODULE_DIRECTORY}"
    #     puts $fp "# INTERNAL= ${INTERNAL}"
    #     puts $fp "# OPAQUE_ADDRESS_MAP= ${OPAQUE_ADDRESS_MAP}"
    #     puts $fp "# GROUP= ${GROUP}"
    #     puts $fp "# AUTHOR= ${AUTHOR}"
    #     puts $fp "# ICON_PATH= ${ICON_PATH}"
    #     puts $fp "# DISPLAY_NAME= ${DISPLAY_NAME}"
    #     puts $fp "# DATASHEET_URL= ${DATASHEET_URL}"
    #     puts $fp "# INSTANTIATE_IN_SYSTEM_MODULE= ${INSTANTIATE_IN_SYSTEM_MODULE}"
    #     puts $fp "# EDITABLE= ${EDITABLE}"
    #     puts $fp "# VALIDATION_CALLBACK= ${VALIDATION_CALLBACK}"
    #     puts $fp "# EDITOR_CALLBACK= ${EDITOR_CALLBACK}"
    #     puts $fp "# ELABORATION_CALLBACK= ${ELABORATION_CALLBACK}"
    #     puts $fp "# COMPOSITION_CALLBACK= ${COMPOSITION_CALLBACK}"
    #     puts $fp "# SUPPORTED_DEVICE_FAMILIES= ${SUPPORTED_DEVICE_FAMILIES}"
    #     puts $fp "# REPORT_TO_TALKBACK= ${REPORT_TO_TALKBACK}"
    #     puts $fp "# ALLOW_GREYBOX_GENERATION= ${ALLOW_GREYBOX_GENERATION}"
    #     puts $fp "# SUPPRESS_WARNINGS= ${SUPPRESS_WARNINGS}"
    #     puts $fp "# STRUCTURAL_COMPOSITION_CALLBACK= ${STRUCTURAL_COMPOSITION_CALLBACK}"
    #     puts $fp "# NATIVE_INTERPRETER= ${NATIVE_INTERPRETER}"
    #     puts $fp "# PREFERRED_SIMULATION_LANGUAGE= ${PREFERRED_SIMULATION_LANGUAGE}"
    #     puts $fp "# REPORT_HIERARCHY= ${REPORT_HIERARCHY}"
    #     puts $fp "# UPGRADEABLE_FROM= ${UPGRADEABLE_FROM}"
    #     puts $fp "# DEVICE: ${DEVICE_NAME}"
    # close $fp
    
	set folder [ pwd ] 
	set directory_component [ lrange [file split $folder] end-0 end-0]
	if {$directory_component != "GeMRTOS_Multiprocessor"} {
		send_message Error "Error in GeMRTOS_Multiprocessor directory name ${directory_component}"
		set error_stop 1
	}
	
	set directory_component [ lrange [file split $folder] end-1 end-1]
	if {$directory_component != "ip"} {
		send_message Error "Error: GeMRTOS_Multiprocessor should be in the ip subdirectory ${directory_component}/ip"
		set error_stop 1
	}

	set files_qpf {"NOT DEFINED"}	
	if {($error_stop == 0)} {
		set files_dir [file join [file normalize "../../"]]
		foreach file_qpf [glob -nocomplain -types f -directory $files_dir *.qpf] {
			set ftail [ file rootname $file_qpf ]
			set tail [ lrange [file split $ftail] end-0 end-0]
			lappend files_qpf $tail
		}
	}
	set_parameter_property QUARTUS_PROJECT_FILE ALLOWED_RANGES ${files_qpf}
	# send_message Warning "quartus project file is ${files_qpf}"

	set files_qpf {"NOT DEFINED"}
	if {($error_stop == 0)} {
		set files_dir [file join [file normalize "../../"]]
		foreach file_qpf [glob -nocomplain -types f -directory $files_dir *.qsys] {
			set ftail [ file rootname $file_qpf ]
			set tail [ lrange [file split $ftail] end-0 end-0]
			lappend files_qpf $tail
		}
	}
	set_parameter_property QSYS_PROJECT_FILE ALLOWED_RANGES ${files_qpf}
	# send_message Warning "quartus project file is ${files_qpf}"
	
	if {($error_stop == 0)} {
		if { ($Type_of_Processor == "Nios II/e (economy)") || ($Type_of_Processor == "Nios II/f (full)") } {
			if {[ file exists "../../gemrtos_build.bat" ] == 1 } {
				file delete -force ../../gemrtos_build.bat
			}
			set fd [open "./gemrtos_build.sh" r]
			set newfd [open "../../gemrtos_build.sh" w]
			send_message Warning "REMEMBER using Nios 2 Command Shell to execute \"bash gemrtos_build.sh\""
		}
		if { ($Type_of_Processor == "Nios V/m") || ($Type_of_Processor == "Nios V/g") } {
			if {[ file exists "../../gemrtos_build.sh" ] == 1 } {
				file delete -force ../../gemrtos_build.sh
			}		
			set fd [open "./gemrtos_build.bat" r]
			set newfd [open "../../gemrtos_build.bat" w]
			send_message Warning "REMEMBER using Nios V Command Shell to execute \gemrtos_build.bat\""
		}
		fconfigure $newfd -translation lf
		set quartus_project [get_parameter_value QUARTUS_PROJECT_FILE]
		set qsys_project [get_parameter_value QSYS_PROJECT_FILE]
		# send_message Error "Quartus file = ${quartus_project}"
		# send_message Error "Qsys file = ${qsys_project}"
		while {[gets $fd line] >= 0} {
			set newline [string map [list {QUARTUS_PROJECT_FILE} "$quartus_project" {QSYS_PROJECT_FILE} "$qsys_project"] $line]
			puts $newfd $newline
		}
		close $fd
		close $newfd
	}

# 10/03/2024 ###################
	set address_map_xml [get_parameter_value instSlaveMapParam]
	set address_map_dec [decode_address_map $address_map_xml]
	# set slaves_name [list "Internal GeMRTOS (default)"]
	set slaves_name [list]
	
	
	set resetSlave             [ get_parameter_value resetSlave ]
	set resetOffset            [ get_parameter_value resetOffset ]
	
	set_parameter_value resestAbsolute 0

	foreach i $address_map_dec {
		array set info $i
		lappend slaves_name "$info(name)"
		if { ( $resetSlave == $info(name) ) } {
			set resestAbsolute [expr { $info(start) + $resetOffset } ]
			set_parameter_value resestAbsolute $resestAbsolute
		}
		# send_message Warning "Connected to slave name=$info(name) start=$info(start) end=$info(end)"
	}
	set_parameter_property  resetSlave     "ALLOWED_RANGES" $slaves_name
	
# 28-06-2024 ###################

	set exceptionSlave             [ get_parameter_value exceptionSlave ]
	set exceptionOffset            [ get_parameter_value exceptionOffset ]
	
	set_parameter_value exceptionAbsolute 0

	foreach i $address_map_dec {
		array set info $i
		lappend slaves_name "$info(name)"
		if { ( $exceptionSlave == $info(name) ) } {
			set exceptionAbsolute [expr { $info(start) + $exceptionOffset } ]
			set_parameter_value exceptionAbsolute $exceptionAbsolute
		}
		# send_message Warning "Connected to slave name=$info(name) start=$info(start) end=$info(end)"
	}
	set_parameter_property  exceptionSlave     "ALLOWED_RANGES" $slaves_name	
	
# 28-06-2024 ###################

# 27/03/2023 ######################## 

}

proc compose { } {

    # Parameters from GUI
    set Processors [get_parameter_value NProcessors]
    set Type_of_Processor [get_parameter_value Processor_type]
	
	set resetSlave             [ get_parameter_value resetSlave ]
	set resestAbsolute         [ get_parameter_value resestAbsolute ]
	# 28-06-2024 set exceptionAbsolute      [expr { $resestAbsolute + 32 } ]
	set exceptionSlave             [ get_parameter_value exceptionSlave ]
	set exceptionAbsolute         [ get_parameter_value exceptionAbsolute ]	
	
	if { ($Type_of_Processor == "Nios II/e (economy)") || ($Type_of_Processor == "Nios II/f (full)") } {
		set proc_clock "clk"
		set proc_reset "reset"
		set data_manager "data_master"
		set instruction_manager "instruction_master"
		set debug_agent_reset_request "debug_reset_request"
		set irq_rx "irq"
		set debug_mem_slave "debug_mem_slave"
		set MAX_PROC_IRQs 32
	}
	if { ($Type_of_Processor == "Nios V/m") || ($Type_of_Processor == "Nios V/g") } {
		set proc_clock "clk"
		set proc_reset "reset"
		set data_manager "data_manager"
		set instruction_manager "instruction_manager"
		set debug_agent_reset_request "dbg_reset_out"
		set irq_rx "platform_irq_rx"
		set debug_mem_slave "dm_agent"
		set MAX_PROC_IRQs 16
	}	
	
	
    set ClockFrequency [get_parameter_value CFrequency]
    set icache_size [get_parameter_value icache_size_parameter]
	set pipelineArch_string [get_parameter_value pipelineArch_parameter]
	if { ($pipelineArch_string == "False" ) } {
		set pipelineArch 0
	} else {
		set pipelineArch 1
	}
	
	
    # set icache_burst [get_parameter_value icache_burst_parameter]    
    set PreScale [ expr { $ClockFrequency / 10000000} ]

    set Bridge_Address_Width [get_parameter_value BUS_WIDTH]
    set Bridge_Address_Width [expr {$Bridge_Address_Width + 0 }]
    set BaseAddress [expr {2**($Bridge_Address_Width)}]
    # STDIO, STDERR, GRTOS and 1 JTAG for each processor
    set IntInterrupts [expr {($Processors) + 3}]  
    send_message Debug "Number of internal interrupts is $IntInterrupts"
	
	# first interrupt is from GeMRTOS controller
	set IntInterrupts 1
	# Include STDIO, STDERR and processor JTAG UART and respective interrupts
	set incl_STDIO [get_parameter_value INCLUDE_JTAG_UART_STDDIO]
	if {($incl_STDIO == true)} { set IntInterrupts [expr {($IntInterrupts) + 1}] }
	
	set incl_STDERR [get_parameter_value INCLUDE_JTAG_UART_STDERR]
	if {($incl_STDERR == true)} { set IntInterrupts [expr {($IntInterrupts) + 1}] }
	
	set incl_STDPROC [get_parameter_value INCLUDE_JTAG_UART_PROCESSOR]
	if {($incl_STDPROC == true)} { set IntInterrupts [expr {($IntInterrupts) + ($Processors)}] }
	
	send_message Debug "Number of internal interrupts is $IntInterrupts"

	# Processor buses
	set independet_buses [get_parameter_value INDEPENDENT_EXTERNAL_PROCESSOR_BUSES]

    
    # remove_dangling_connections
    
    # Internal clock for devices in GRTOS
    add_instance clock_bridge_0 altera_clock_bridge
    set_instance_parameter_value clock_bridge_0 {NUM_CLOCK_OUTPUTS} {1}

    
    # Internal clock for devices in GRTOS
    add_instance clock_bridge_external_bus altera_clock_bridge
    set_instance_parameter_value clock_bridge_external_bus {NUM_CLOCK_OUTPUTS} {1}    
    
    # Reset bridge for reset input
    add_instance reset_bridge_0 altera_reset_bridge
    set_instance_parameter_value reset_bridge_0 {ACTIVE_LOW_RESET} {0}
    set_instance_parameter_value reset_bridge_0 {NUM_RESET_OUTPUTS} {1}
    set_instance_parameter_value reset_bridge_0 {SYNCHRONOUS_EDGES} {deassert}
    set_instance_parameter_value reset_bridge_0 {USE_RESET_REQUEST} {0}    
    
    # Replace clk_0.clk_sacar for clock_bridge_0.out_clk
    # Replace clk_0.clk_reset_sacar for reset_bridge_0.out_reset
    add_connection clock_bridge_0.out_clk reset_bridge_0.clk clock

    
    # GRTOS Controller
    add_instance grtos_0 grtos
    set_instance_parameter_value grtos_0 {NProcessors} [get_parameter_value NProcessors]
    set_instance_parameter_value grtos_0 {PreScale} $PreScale
    # ######################
    set_instance_parameter_value grtos_0 {PROCESSOR_TYPE} [get_parameter_value Processor_type]
    set_instance_parameter_value grtos_0 {INSTRUCTION_CACHE_SIZE} [get_parameter_value icache_size_parameter]
    # set_instance_parameter_value grtos_0 {INSTRUCTION_CACHE_BURST} [get_parameter_value icache_burst_parameter]
	set_instance_parameter_value grtos_0 {INSTRUCTION_CACHE_BURST} Disable
    set_instance_parameter_value grtos_0 {ENABLE_HPS_MAP_ACCESS} [get_parameter_value ENABLE_HPS_MAP_ACCESS]
    set_instance_parameter_value grtos_0 {NIOS_CLOCK_FREQUENCY} [get_parameter_value CFrequency]
    set_instance_parameter_value grtos_0 {BUS_CLOCK_FREQUENCY} [get_parameter_value CEFrequency]
    set_instance_parameter_value grtos_0 {BUS_WIDTH} [get_parameter_value BUS_WIDTH]
    set_instance_parameter_value grtos_0 {EXTERNAL_MEMORY_SPAN} [get_parameter_value EXTERNAL_MEMORY_SPAN]
    set_instance_parameter_value grtos_0 {EXTERNAL_MEMORY_UNIT} [get_parameter_value EXTERNAL_MEMORY_UNIT]
	set_instance_parameter_value grtos_0 {INDEPENDENT_EXTERNAL_PROCESSOR_BUSES} [get_parameter_value INDEPENDENT_EXTERNAL_PROCESSOR_BUSES]
	set_instance_parameter_value grtos_0 {INCLUDE_JTAG_UART_STDDIO} [get_parameter_value INCLUDE_JTAG_UART_STDDIO]
	set_instance_parameter_value grtos_0 {INCLUDE_JTAG_UART_STDERR} [get_parameter_value INCLUDE_JTAG_UART_STDERR]
	set_instance_parameter_value grtos_0 {INCLUDE_JTAG_UART_PROCESSOR} [get_parameter_value INCLUDE_JTAG_UART_PROCESSOR]
	set_instance_parameter_value grtos_0 {QUARTUS_PROJECT_FILE} [get_parameter_value QUARTUS_PROJECT_FILE]
	set_instance_parameter_value grtos_0 {QSYS_PROJECT_FILE} [get_parameter_value QSYS_PROJECT_FILE]
    # ######################

  
    # CLOCK and RESET
    add_connection clock_bridge_0.out_clk grtos_0.clock_reset clock
    add_connection reset_bridge_0.out_reset grtos_0.clock_reset_reset reset
    
    # ###########################################################
    # HPS internal access
    if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
        add_instance mm_clock_crossing_bridge_0 altera_avalon_mm_clock_crossing_bridge
        set_instance_parameter_value mm_clock_crossing_bridge_0 {ADDRESS_UNITS} {SYMBOLS}
        set_instance_parameter_value mm_clock_crossing_bridge_0 {ADDRESS_WIDTH} {10}
        set_instance_parameter_value mm_clock_crossing_bridge_0 {COMMAND_FIFO_DEPTH} {4}
        set_instance_parameter_value mm_clock_crossing_bridge_0 {DATA_WIDTH} {32}
        set_instance_parameter_value mm_clock_crossing_bridge_0 {MASTER_SYNC_DEPTH} {2}
        set_instance_parameter_value mm_clock_crossing_bridge_0 {MAX_BURST_SIZE} {1}
        set_instance_parameter_value mm_clock_crossing_bridge_0 {RESPONSE_FIFO_DEPTH} {4}
        set_instance_parameter_value mm_clock_crossing_bridge_0 {SLAVE_SYNC_DEPTH} {2}
        set_instance_parameter_value mm_clock_crossing_bridge_0 {SYMBOL_WIDTH} {8}
        set_instance_parameter_value mm_clock_crossing_bridge_0 {USE_AUTO_ADDRESS_WIDTH} {1}
        # CLOCK and RESET
        add_connection clock_bridge_0.out_clk mm_clock_crossing_bridge_0.m0_clk clock
        add_connection reset_bridge_0.out_reset mm_clock_crossing_bridge_0.m0_reset reset
        # add_connection clock_bridge_0.out_clk mm_clock_crossing_bridge_0.s0_clk clock        
        # add_connection reset_bridge_0.out_reset mm_clock_crossing_bridge_0.s0_reset reset

    }
    # ###########################################################    
    
    # IRQ Bridge
    add_instance irq_bridge_0 altera_irq_bridge
    set_instance_parameter_value irq_bridge_0 {IRQ_N} {0}
    set Number_of_external_IRQ [expr {${MAX_PROC_IRQs} - $IntInterrupts}]
    set_instance_parameter_value irq_bridge_0 {IRQ_WIDTH} ${Number_of_external_IRQ}
    # CLOCK and RESET
    add_connection clock_bridge_0.out_clk irq_bridge_0.clk clock
    add_connection reset_bridge_0.out_reset irq_bridge_0.clk_reset reset
    
    # GRTOS Avalon monitor
    add_instance nios_avalon_monitor nios_avalon_monitor
    set_instance_parameter_value nios_avalon_monitor {ADDRESS_UNITS} {SYMBOLS}
    set_instance_parameter_value nios_avalon_monitor {ADDRESS_WIDTH} {29}
    set_instance_parameter_value nios_avalon_monitor {AVALON_DATA_FIFO_DEPTH} {32}
    set_instance_parameter_value nios_avalon_monitor {DATA_WIDTH} {32}
    set_instance_parameter_value nios_avalon_monitor {LINEWRAPBURSTS} {0}
    set_instance_parameter_value nios_avalon_monitor {MAX_BURST_SIZE} {1}
    set_instance_parameter_value nios_avalon_monitor {MAX_PENDING_RESPONSES} {32}
    set_instance_parameter_value nios_avalon_monitor {NProcessors} $Processors
    set_instance_parameter_value nios_avalon_monitor {SYMBOL_WIDTH} {8}
    # CLOCK and RESET
    add_connection clock_bridge_0.out_clk nios_avalon_monitor.clk clock
    add_connection reset_bridge_0.out_reset nios_avalon_monitor.reset reset    
    
    # Memory for CPUID not 1 reset
    # 30-05-2024  add_instance rstaux_memory altera_avalon_onchip_memory2
    # 30-05-2024  set_instance_parameter_value rstaux_memory {allowInSystemMemoryContentEditor} {0}
    # 30-05-2024  set_instance_parameter_value rstaux_memory {blockType} {AUTO}
    # 30-05-2024  set_instance_parameter_value rstaux_memory {copyInitFile} {0}
    # 30-05-2024  set_instance_parameter_value rstaux_memory {dataWidth} {32}
    # 30-05-2024  set_instance_parameter_value rstaux_memory {dataWidth2} {32}
    # 30-05-2024  set_instance_parameter_value rstaux_memory {dualPort} {0}
    # 30-05-2024  set_instance_parameter_value rstaux_memory {ecc_enabled} {0}
    # 30-05-2024  set_instance_parameter_value rstaux_memory {enPRInitMode} {0}
    # 30-05-2024  set_instance_parameter_value rstaux_memory {enableDiffWidth} {0}
    # 30-05-2024  set_instance_parameter_value rstaux_memory {initMemContent} {1}
    # 30-05-2024  set_instance_parameter_value rstaux_memory {initializationFileName} {GRTOS_Multiprocessor_0_rstaux_memory.hex}
    # 30-05-2024  set_instance_parameter_value rstaux_memory {instanceID} {NONE}
    # 30-05-2024  set_instance_parameter_value rstaux_memory {memorySize} {512.0}
    # 30-05-2024  set_instance_parameter_value rstaux_memory {readDuringWriteMode} {DONT_CARE}
    # 30-05-2024  set_instance_parameter_value rstaux_memory {resetrequest_enabled} {1}
    # 30-05-2024  set_instance_parameter_value rstaux_memory {simAllowMRAMContentsFile} {0}
    # 30-05-2024  set_instance_parameter_value rstaux_memory {simMemInitOnlyFilename} {0}
    # 30-05-2024  set_instance_parameter_value rstaux_memory {singleClockOperation} {0}
    # 30-05-2024  set_instance_parameter_value rstaux_memory {slave1Latency} {1}
    # 30-05-2024  set_instance_parameter_value rstaux_memory {slave2Latency} {1}
    # 30-05-2024  set_instance_parameter_value rstaux_memory {useNonDefaultInitFile} {0}
    # 30-05-2024  set_instance_parameter_value rstaux_memory {useShallowMemBlocks} {0}
    # 30-05-2024  set_instance_parameter_value rstaux_memory {writable} {1}
    # 30-05-2024  # CLOCK and RESET
    # 30-05-2024  add_connection clock_bridge_0.out_clk rstaux_memory.clk1 clock
    # 30-05-2024  add_connection reset_bridge_0.out_reset rstaux_memory.reset1 reset

	if { ($resetSlave == "Internal GeMRTOS (default)" ) } {
		# Memory for CPUID 1 reset and exceptions
		add_instance rst1_and_exception_memory altera_avalon_onchip_memory2
		set_instance_parameter_value rst1_and_exception_memory {allowInSystemMemoryContentEditor} {0}
		set_instance_parameter_value rst1_and_exception_memory {blockType} {AUTO}
		set_instance_parameter_value rst1_and_exception_memory {copyInitFile} {0}
		set_instance_parameter_value rst1_and_exception_memory {dataWidth} {32}
		set_instance_parameter_value rst1_and_exception_memory {dataWidth2} {32}
		set_instance_parameter_value rst1_and_exception_memory {dualPort} {0}
		set_instance_parameter_value rst1_and_exception_memory {ecc_enabled} {0}
		set_instance_parameter_value rst1_and_exception_memory {enPRInitMode} {0}
		set_instance_parameter_value rst1_and_exception_memory {enableDiffWidth} {0}
		set_instance_parameter_value rst1_and_exception_memory {initMemContent} {1}
		set_instance_parameter_value rst1_and_exception_memory {initializationFileName} {GRTOS_Multiprocessor_0_rst1_and_exception_memory.hex}
		set_instance_parameter_value rst1_and_exception_memory {instanceID} {NONE}
		set_instance_parameter_value rst1_and_exception_memory {memorySize} {2048.0}
		set_instance_parameter_value rst1_and_exception_memory {readDuringWriteMode} {DONT_CARE}
		set_instance_parameter_value rst1_and_exception_memory {resetrequest_enabled} {1}
		set_instance_parameter_value rst1_and_exception_memory {simAllowMRAMContentsFile} {0}
		set_instance_parameter_value rst1_and_exception_memory {simMemInitOnlyFilename} {0}
		set_instance_parameter_value rst1_and_exception_memory {singleClockOperation} {0}
		set_instance_parameter_value rst1_and_exception_memory {slave1Latency} {1}
		set_instance_parameter_value rst1_and_exception_memory {slave2Latency} {1}
		set_instance_parameter_value rst1_and_exception_memory {useNonDefaultInitFile} {0}
		set_instance_parameter_value rst1_and_exception_memory {useShallowMemBlocks} {0}
		set_instance_parameter_value rst1_and_exception_memory {writable} {1}
		# CLOCK and RESET
		add_connection clock_bridge_0.out_clk rst1_and_exception_memory.clk1 clock
		add_connection reset_bridge_0.out_reset rst1_and_exception_memory.reset1 reset
	}

    
    # 31-05-2024 # # Memory for Code on-chip
    # 31-05-2024 add_instance onchip_memory2_2 altera_avalon_onchip_memory2
    # 31-05-2024 set_instance_parameter_value onchip_memory2_2 {allowInSystemMemoryContentEditor} {0}
    # 31-05-2024 set_instance_parameter_value onchip_memory2_2 {blockType} {AUTO}
    # 31-05-2024 set_instance_parameter_value onchip_memory2_2 {copyInitFile} {0}
    # 31-05-2024 set_instance_parameter_value onchip_memory2_2 {dataWidth} {32}
    # 31-05-2024 set_instance_parameter_value onchip_memory2_2 {dataWidth2} {32}
    # 31-05-2024 set_instance_parameter_value onchip_memory2_2 {dualPort} {0}
    # 31-05-2024 set_instance_parameter_value onchip_memory2_2 {ecc_enabled} {0}
    # 31-05-2024 set_instance_parameter_value onchip_memory2_2 {enPRInitMode} {0}
    # 31-05-2024 set_instance_parameter_value onchip_memory2_2 {enableDiffWidth} {0}
    # 31-05-2024 set_instance_parameter_value onchip_memory2_2 {initMemContent} {1}
    # 31-05-2024 set_instance_parameter_value onchip_memory2_2 {initializationFileName} {GRTOS_Multiprocessor_0_onchip_memory2_2.hex}
    # 31-05-2024 set_instance_parameter_value onchip_memory2_2 {instanceID} {NONE}
    # 31-05-2024 set_instance_parameter_value onchip_memory2_2 {memorySize} {16384.0}
    # 31-05-2024 set_instance_parameter_value onchip_memory2_2 {readDuringWriteMode} {DONT_CARE}
    # 31-05-2024 set_instance_parameter_value onchip_memory2_2 {resetrequest_enabled} {1}
    # 31-05-2024 set_instance_parameter_value onchip_memory2_2 {simAllowMRAMContentsFile} {0}
    # 31-05-2024 set_instance_parameter_value onchip_memory2_2 {simMemInitOnlyFilename} {0}
    # 31-05-2024 set_instance_parameter_value onchip_memory2_2 {singleClockOperation} {0}
    # 31-05-2024 set_instance_parameter_value onchip_memory2_2 {slave1Latency} {1}
    # 31-05-2024 set_instance_parameter_value onchip_memory2_2 {slave2Latency} {1}
    # 31-05-2024 set_instance_parameter_value onchip_memory2_2 {useNonDefaultInitFile} {0}
    # 31-05-2024 set_instance_parameter_value onchip_memory2_2 {useShallowMemBlocks} {0}
    # 31-05-2024 set_instance_parameter_value onchip_memory2_2 {writable} {1}
    # 31-05-2024 # CLOCK and RESET
    # 31-05-2024 add_connection clock_bridge_0.out_clk onchip_memory2_2.clk1 clock
    # 31-05-2024 add_connection reset_bridge_0.out_reset onchip_memory2_2.reset1 reset  



    # Processors
    for {set i 1} {$i <= $Processors} {incr i} {
		# #########################################################
		# NIOS V PROCESSORS
		# #########################################################
		if { ($Type_of_Processor == "Nios V/m") || ($Type_of_Processor == "Nios V/g") } {
			if { ($Type_of_Processor == "Nios V/m") } {
				add_instance gemrtos_proc_${i} intel_niosv_m
				set_instance_parameter_value gemrtos_proc_${i} {pipelineArch} {$pipelineArch}
				# Number of General Purpose Registers 16, 32
				set_instance_parameter_value gemrtos_proc_${i} {numGpr} {32}				
			}
			if { ($Type_of_Processor == "Nios V/g") } {
				add_instance gemrtos_proc_${i} intel_niosv_g
				set_instance_parameter_value gemrtos_proc_${i} {CUSTOM_OP} {}
				set_instance_parameter_value gemrtos_proc_${i} {SUB_OP} {}
				set_instance_parameter_value gemrtos_proc_${i} {dataCacheSize} {4096}
				set_instance_parameter_value gemrtos_proc_${i} {dtcm1Base} {0}
				set_instance_parameter_value gemrtos_proc_${i} {dtcm1InitFile} {}
				set_instance_parameter_value gemrtos_proc_${i} {dtcm1Size} {0}
				set_instance_parameter_value gemrtos_proc_${i} {dtcm2Base} {0}
				set_instance_parameter_value gemrtos_proc_${i} {dtcm2InitFile} {}
				set_instance_parameter_value gemrtos_proc_${i} {dtcm2Size} {0}
				set_instance_parameter_value gemrtos_proc_${i} {enableFPU} {0}
				set_instance_parameter_value gemrtos_proc_${i} {enableMulDiv} {1}
				set_instance_parameter_value gemrtos_proc_${i} {funct3} {}
				set_instance_parameter_value gemrtos_proc_${i} {funct7_l} {}
				set_instance_parameter_value gemrtos_proc_${i} {funct7_u} {}
				if { $icache_size == "None" } {
					set_instance_parameter_value gemrtos_proc_${i} {instCacheSize} {0}
				}
				if { $icache_size == "512 Bytes" } {
					set_instance_parameter_value gemrtos_proc_${i} {instCacheSize} {512}
				}
				if { $icache_size == "1 KB" } {
					set_instance_parameter_value gemrtos_proc_${i} {instCacheSize} {1024}
				}
				if { $icache_size == "2 KB" } {
					set_instance_parameter_value gemrtos_proc_${i} {instCacheSize} {2048}
				}
				if { $icache_size == "4 KB" } {
					set_instance_parameter_value gemrtos_proc_${i} {instCacheSize} {4096}
				}
				if { $icache_size == "8 KB" } {
					set_instance_parameter_value gemrtos_proc_${i} {instCacheSize} {8192}
				}
				if { $icache_size == "16 KB" } {
					set_instance_parameter_value gemrtos_proc_${i} {instCacheSize} {16384}
				}
				set_instance_parameter_value gemrtos_proc_${i} {itcm1Base} {0}
				set_instance_parameter_value gemrtos_proc_${i} {itcm1InitFile} {}
				set_instance_parameter_value gemrtos_proc_${i} {itcm1Size} {0}
				set_instance_parameter_value gemrtos_proc_${i} {itcm2Base} {0}
				set_instance_parameter_value gemrtos_proc_${i} {itcm2InitFile} {}
				set_instance_parameter_value gemrtos_proc_${i} {itcm2Size} {0}
				set_instance_parameter_value gemrtos_proc_${i} {mnemonic} {}
				set_instance_parameter_value gemrtos_proc_${i} {opcode} {}
				set_instance_parameter_value gemrtos_proc_${i} {peripheralRegionABase} {0}
				set_instance_parameter_value gemrtos_proc_${i} {peripheralRegionASize} {-2147483648}
				set_instance_parameter_value gemrtos_proc_${i} {peripheralRegionBBase} {0}
				set_instance_parameter_value gemrtos_proc_${i} {peripheralRegionBSize} {0}
			}
			
			set_instance_parameter_value gemrtos_proc_${i} {enableDebug} {1}
			set_instance_parameter_value gemrtos_proc_${i} {enableDebugReset} {1}
			set_instance_parameter_value gemrtos_proc_${i} {enableECCLite} {0}
			
			
			if {$i == 1} {
				set resetSlave             [ get_parameter_value resetSlave ]
				set resestAbsolute             [ get_parameter_value resestAbsolute ]
				if { ($resetSlave == "Internal GeMRTOS (default)" ) } {
					set_instance_parameter_value gemrtos_proc_${i} {resetSlave} {rst1_and_exception_memory.s1}
					set_instance_parameter_value gemrtos_proc_${i} {resetOffset} {0}
				} else {
					set_instance_parameter_value gemrtos_proc_${i} {resetSlave} {Absolute}
					set_instance_parameter_value gemrtos_proc_${i} {resetOffset} {$resestAbsolute}
				}
			} else {
				# set_instance_parameter_value gemrtos_proc_${i} {resetSlave} {rstaux_memory.s1}
				set_instance_parameter_value gemrtos_proc_${i} {resetSlave} {grtos_0.s_Global}
				set_instance_parameter_value gemrtos_proc_${i} {resetOffset} {0}
			}

			set_instance_parameter_value gemrtos_proc_${i} {useResetReq} {0}
			add_connection gemrtos_proc_${i}.dbg_reset_out gemrtos_proc_${i}.ndm_reset_in reset
			
			# 09-05-2024 if {$i != 1} {
			# 09-05-2024 	add_connection gemrtos_proc_1.dbg_reset_out gemrtos_proc_${i}.ndm_reset_in reset
			# 09-05-2024 }
		}
		
		# #########################################################
		# NIOS II PROCESSORS
		# #########################################################
		if { ($Type_of_Processor == "Nios II/e (economy)") || ($Type_of_Processor == "Nios II/f (full)") } {
			add_instance gemrtos_proc_${i} altera_nios2_gen2

			set_instance_parameter_value gemrtos_proc_${i} {bht_ramBlockType} {Automatic}
			set_instance_parameter_value gemrtos_proc_${i} {breakOffset} {32}
			set_instance_parameter_value gemrtos_proc_${i} {breakSlave} {gemrtos_proc_${i}.jtag_debug_module}
			set_instance_parameter_value gemrtos_proc_${i} {cdx_enabled} {0}
			set_instance_parameter_value gemrtos_proc_${i} {cpuArchRev} {1}
			set_instance_parameter_value gemrtos_proc_${i} {cpuID} ${i}
			set_instance_parameter_value gemrtos_proc_${i} {cpuReset} {0}
			set_instance_parameter_value gemrtos_proc_${i} {data_master_high_performance_paddr_base} {0}
			set_instance_parameter_value gemrtos_proc_${i} {data_master_high_performance_paddr_size} {0.0}
			set_instance_parameter_value gemrtos_proc_${i} {data_master_paddr_base} {0}
			set_instance_parameter_value gemrtos_proc_${i} {data_master_paddr_size} {0.0}
			set_instance_parameter_value gemrtos_proc_${i} {dcache_bursts} {false}
			set_instance_parameter_value gemrtos_proc_${i} {dcache_numTCDM} {0}
			set_instance_parameter_value gemrtos_proc_${i} {dcache_ramBlockType} {Automatic}
			set_instance_parameter_value gemrtos_proc_${i} {dcache_size} {0}
			set_instance_parameter_value gemrtos_proc_${i} {dcache_tagramBlockType} {Automatic}
			set_instance_parameter_value gemrtos_proc_${i} {dcache_victim_buf_impl} {ram}
			set_instance_parameter_value gemrtos_proc_${i} {debug_OCIOnchipTrace} {_128}
			set_instance_parameter_value gemrtos_proc_${i} {debug_assignJtagInstanceID} {0}
			set_instance_parameter_value gemrtos_proc_${i} {debug_datatrigger} {0}
			set_instance_parameter_value gemrtos_proc_${i} {debug_debugReqSignals} {0}
			set_instance_parameter_value gemrtos_proc_${i} {debug_enabled} {1}
			set_instance_parameter_value gemrtos_proc_${i} {debug_hwbreakpoint} {0}
			set_instance_parameter_value gemrtos_proc_${i} {debug_jtagInstanceID} {0}
			set_instance_parameter_value gemrtos_proc_${i} {debug_traceStorage} {onchip_trace}
			set_instance_parameter_value gemrtos_proc_${i} {debug_traceType} {none}
			set_instance_parameter_value gemrtos_proc_${i} {debug_triggerArming} {1}
			set_instance_parameter_value gemrtos_proc_${i} {dividerType} {no_div}
			# set_instance_parameter_value gemrtos_proc_${i} {dividerType} {srt2}
			# 06-06-2024 set_instance_parameter_value gemrtos_proc_${i} {exceptionOffset} {32}
			# 06-06-2024 set_instance_parameter_value gemrtos_proc_${i} {exceptionSlave} {rst1_and_exception_memory.s1}
			set_instance_parameter_value gemrtos_proc_${i} {fa_cache_line} {2}
			set_instance_parameter_value gemrtos_proc_${i} {fa_cache_linesize} {0}
			set_instance_parameter_value gemrtos_proc_${i} {flash_instruction_master_paddr_base} {0}
			set_instance_parameter_value gemrtos_proc_${i} {flash_instruction_master_paddr_size} {0.0}
			# if { $Type_of_Processor == "Nios II/f (full)" } {
			#     # ################
			#     if { $icache_burst == "Enable" } {
			#         set_instance_parameter_value gemrtos_proc_${i} {icache_burstType} {Sequential} 
			#     }
			#     if { $icache_burst == "Disable" } {
			#         set_instance_parameter_value gemrtos_proc_${i} {icache_burstType} {None} 
			#     }
			# } else {
			#     set_instance_parameter_value gemrtos_proc_${i} {icache_burstType} {None}
			# }
			set_instance_parameter_value gemrtos_proc_${i} {icache_burstType} {None}
			set_instance_parameter_value gemrtos_proc_${i} {icache_numTCIM} {0}
			set_instance_parameter_value gemrtos_proc_${i} {icache_ramBlockType} {Automatic}
			if { $Type_of_Processor == "Nios II/f (full)" } {
				# #########
				if { $icache_size == "None" } {
					set_instance_parameter_value gemrtos_proc_${i} {icache_size} {0}
				}
				if { $icache_size == "512 Bytes" } {
					set_instance_parameter_value gemrtos_proc_${i} {icache_size} {512}
				}
				if { $icache_size == "1 KB" } {
					set_instance_parameter_value gemrtos_proc_${i} {icache_size} {1024}
				}
				if { $icache_size == "2 KB" } {
					set_instance_parameter_value gemrtos_proc_${i} {icache_size} {2048}
				}
				if { $icache_size == "4 KB" } {
					set_instance_parameter_value gemrtos_proc_${i} {icache_size} {4096}
				}
				if { $icache_size == "8 KB" } {
					set_instance_parameter_value gemrtos_proc_${i} {icache_size} {8192}
				}
				if { $icache_size == "16 KB" } {
					set_instance_parameter_value gemrtos_proc_${i} {icache_size} {16384}
				}
				if { $icache_size == "32 KB" } {
					set_instance_parameter_value gemrtos_proc_${i} {icache_size} {32768}
				}
				if { $icache_size == "64 KB" } {
					set_instance_parameter_value gemrtos_proc_${i} {icache_size} {65536}
				}
				# #########
			} else {
				set_instance_parameter_value gemrtos_proc_${i} {icache_size} {0}
			}
			set_instance_parameter_value gemrtos_proc_${i} {icache_tagramBlockType} {Automatic}
			if { $Type_of_Processor == "Nios II/e (economy)" } {
				set_instance_parameter_value gemrtos_proc_${i} {impl} {Tiny}
			}
			if { $Type_of_Processor == "Nios II/f (full)" } {
				set_instance_parameter_value gemrtos_proc_${i} {impl} {Fast}
			}        
			set_instance_parameter_value gemrtos_proc_${i} {instruction_master_high_performance_paddr_base} {0}
			set_instance_parameter_value gemrtos_proc_${i} {instruction_master_high_performance_paddr_size} {0.0}
			set_instance_parameter_value gemrtos_proc_${i} {instruction_master_paddr_base} {0}
			set_instance_parameter_value gemrtos_proc_${i} {instruction_master_paddr_size} {0.0}
			set_instance_parameter_value gemrtos_proc_${i} {io_regionbase} {0}
			set_instance_parameter_value gemrtos_proc_${i} {io_regionsize} {0}
			set_instance_parameter_value gemrtos_proc_${i} {master_addr_map} {0}
			set_instance_parameter_value gemrtos_proc_${i} {mmu_TLBMissExcOffset} {0}
			set_instance_parameter_value gemrtos_proc_${i} {mmu_TLBMissExcSlave} {None}
			set_instance_parameter_value gemrtos_proc_${i} {mmu_autoAssignTlbPtrSz} {1}
			set_instance_parameter_value gemrtos_proc_${i} {mmu_enabled} {0}
			set_instance_parameter_value gemrtos_proc_${i} {mmu_processIDNumBits} {8}
			set_instance_parameter_value gemrtos_proc_${i} {mmu_ramBlockType} {Automatic}
			set_instance_parameter_value gemrtos_proc_${i} {mmu_tlbNumWays} {16}
			set_instance_parameter_value gemrtos_proc_${i} {mmu_tlbPtrSz} {7}
			set_instance_parameter_value gemrtos_proc_${i} {mmu_udtlbNumEntries} {6}
			set_instance_parameter_value gemrtos_proc_${i} {mmu_uitlbNumEntries} {4}
			set_instance_parameter_value gemrtos_proc_${i} {mpu_enabled} {0}
			set_instance_parameter_value gemrtos_proc_${i} {mpu_minDataRegionSize} {12}
			set_instance_parameter_value gemrtos_proc_${i} {mpu_minInstRegionSize} {12}
			set_instance_parameter_value gemrtos_proc_${i} {mpu_numOfDataRegion} {8}
			set_instance_parameter_value gemrtos_proc_${i} {mpu_numOfInstRegion} {8}
			set_instance_parameter_value gemrtos_proc_${i} {mpu_useLimit} {0}
			set_instance_parameter_value gemrtos_proc_${i} {mpx_enabled} {0}
			set_instance_parameter_value gemrtos_proc_${i} {mul_32_impl} {2}
			set_instance_parameter_value gemrtos_proc_${i} {mul_64_impl} {0}
			set_instance_parameter_value gemrtos_proc_${i} {mul_shift_choice} {0}
			set_instance_parameter_value gemrtos_proc_${i} {ocimem_ramBlockType} {Automatic}
			set_instance_parameter_value gemrtos_proc_${i} {ocimem_ramInit} {0}
			set_instance_parameter_value gemrtos_proc_${i} {regfile_ramBlockType} {Automatic}
			set_instance_parameter_value gemrtos_proc_${i} {register_file_por} {0}
			

			# send_message Warning "resetSlave= $resetSlave"
			# send_message Warning "reset absolute= $resestAbsolute"
			# send_message Warning "exception Absolute= $exceptionAbsolute"

			if { ($resetSlave == "Internal GeMRTOS (default)" ) } {
				set_instance_parameter_value gemrtos_proc_${i} {exceptionSlave} {rst1_and_exception_memory.s1}
				set_instance_parameter_value gemrtos_proc_${i} {exceptionOffset} {32}
			} else {
				set_instance_parameter_value gemrtos_proc_${i} {exceptionSlave} {Absolute}
				set_instance_parameter_value gemrtos_proc_${i} {exceptionOffset} $exceptionAbsolute
			}

			if {$i == 1} {
				if { ($resetSlave == "Internal GeMRTOS (default)" ) } {
					set_instance_parameter_value gemrtos_proc_${i} {resetSlave} {rst1_and_exception_memory.s1}
					set_instance_parameter_value gemrtos_proc_${i} {resetOffset} {0}
					# send_message Warning "Using normal reset vector"
				} else {
					set_instance_parameter_value gemrtos_proc_${i} {resetSlave} {Absolute}
					set_instance_parameter_value gemrtos_proc_${i} {resetOffset} $resestAbsolute
				}
			} else {
				# set_instance_parameter_value gemrtos_proc_${i} {resetSlave} {rstaux_memory.s1}
				set_instance_parameter_value gemrtos_proc_${i} {resetSlave} {grtos_0.s_Global}
				set_instance_parameter_value gemrtos_proc_${i} {resetOffset} {0}	
			}
			
			
			# set_instance_parameter_value gemrtos_proc_${i} {resetOffset} {0}
			# if {$i == 1} {
			# 	set_instance_parameter_value gemrtos_proc_${i} {resetSlave} {rst1_and_exception_memory.s1}
			# } else {
			# 	set_instance_parameter_value gemrtos_proc_${i} {resetSlave} {rstaux_memory.s1}
			# }
			
			
			
			
			set_instance_parameter_value gemrtos_proc_${i} {resetrequest_enabled} {1}
			set_instance_parameter_value gemrtos_proc_${i} {setting_HBreakTest} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_HDLSimCachesCleared} {1}
			set_instance_parameter_value gemrtos_proc_${i} {setting_activateMonitors} {1}
			set_instance_parameter_value gemrtos_proc_${i} {setting_activateTestEndChecker} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_activateTrace} {1}
			# set_instance_parameter_value gemrtos_proc_${i} {setting_activateTrace} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_allow_break_inst} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_alwaysEncrypt} {1}
			set_instance_parameter_value gemrtos_proc_${i} {setting_asic_add_scan_mode_input} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_asic_enabled} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_asic_synopsys_translate_on_off} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_asic_third_party_synthesis} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_avalonDebugPortPresent} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_bhtPtrSz} {8}
			set_instance_parameter_value gemrtos_proc_${i} {setting_bigEndian} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_branchpredictiontype} {Dynamic}
			set_instance_parameter_value gemrtos_proc_${i} {setting_breakslaveoveride} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_clearXBitsLDNonBypass} {1}
			set_instance_parameter_value gemrtos_proc_${i} {setting_dc_ecc_present} {1}
			set_instance_parameter_value gemrtos_proc_${i} {setting_disable_tmr_inj} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_disableocitrace} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_dtcm_ecc_present} {1}
			set_instance_parameter_value gemrtos_proc_${i} {setting_ecc_present} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_ecc_sim_test_ports} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_exportHostDebugPort} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_exportPCB} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_export_large_RAMs} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_exportdebuginfo} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_exportvectors} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_fast_register_read} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_ic_ecc_present} {1}
			set_instance_parameter_value gemrtos_proc_${i} {setting_interruptControllerType} {Internal}
			set_instance_parameter_value gemrtos_proc_${i} {setting_itcm_ecc_present} {1}
			set_instance_parameter_value gemrtos_proc_${i} {setting_mmu_ecc_present} {1}
			set_instance_parameter_value gemrtos_proc_${i} {setting_oci_export_jtag_signals} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_oci_version} {1}
			set_instance_parameter_value gemrtos_proc_${i} {setting_preciseIllegalMemAccessException} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_removeRAMinit} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_rf_ecc_present} {1}
			set_instance_parameter_value gemrtos_proc_${i} {setting_shadowRegisterSets} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_showInternalSettings} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_showUnpublishedSettings} {0}
			# set_instance_parameter_value gemrtos_proc_${i} {setting_support31bitdcachebypass} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_support31bitdcachebypass} {1}
			set_instance_parameter_value gemrtos_proc_${i} {setting_tmr_output_disable} {0}
			set_instance_parameter_value gemrtos_proc_${i} {setting_usedesignware} {0}
			set_instance_parameter_value gemrtos_proc_${i} {shift_rot_impl} {1}
			set_instance_parameter_value gemrtos_proc_${i} {tightly_coupled_data_master_0_paddr_base} {0}
			set_instance_parameter_value gemrtos_proc_${i} {tightly_coupled_data_master_0_paddr_size} {0.0}
			set_instance_parameter_value gemrtos_proc_${i} {tightly_coupled_data_master_1_paddr_base} {0}
			set_instance_parameter_value gemrtos_proc_${i} {tightly_coupled_data_master_1_paddr_size} {0.0}
			set_instance_parameter_value gemrtos_proc_${i} {tightly_coupled_data_master_2_paddr_base} {0}
			set_instance_parameter_value gemrtos_proc_${i} {tightly_coupled_data_master_2_paddr_size} {0.0}
			set_instance_parameter_value gemrtos_proc_${i} {tightly_coupled_data_master_3_paddr_base} {0}
			set_instance_parameter_value gemrtos_proc_${i} {tightly_coupled_data_master_3_paddr_size} {0.0}
			set_instance_parameter_value gemrtos_proc_${i} {tightly_coupled_instruction_master_0_paddr_base} {0}
			set_instance_parameter_value gemrtos_proc_${i} {tightly_coupled_instruction_master_0_paddr_size} {0.0}
			set_instance_parameter_value gemrtos_proc_${i} {tightly_coupled_instruction_master_1_paddr_base} {0}
			set_instance_parameter_value gemrtos_proc_${i} {tightly_coupled_instruction_master_1_paddr_size} {0.0}
			set_instance_parameter_value gemrtos_proc_${i} {tightly_coupled_instruction_master_2_paddr_base} {0}
			set_instance_parameter_value gemrtos_proc_${i} {tightly_coupled_instruction_master_2_paddr_size} {0.0}
			set_instance_parameter_value gemrtos_proc_${i} {tightly_coupled_instruction_master_3_paddr_base} {0}
			set_instance_parameter_value gemrtos_proc_${i} {tightly_coupled_instruction_master_3_paddr_size} {0.0}
			set_instance_parameter_value gemrtos_proc_${i} {tmr_enabled} {0}
			set_instance_parameter_value gemrtos_proc_${i} {tracefilename} {}
			set_instance_parameter_value gemrtos_proc_${i} {userDefinedSettings} {}
		}

        # CLOCK and RESET
        add_connection clock_bridge_0.out_clk gemrtos_proc_${i}.${proc_clock} clock
        add_connection reset_bridge_0.out_reset gemrtos_proc_${i}.${proc_reset} reset
    }
    
    # JTAG UARTs
    for {set i 0} {$i <= $Processors + 1} {incr i} {
		if {(($incl_STDIO == true) && ($i == 0)) || (($incl_STDERR == true) && ($i == 1)) || (($incl_STDPROC == true) && ($i > 1))} {
			add_instance jtag_uart_${i} altera_avalon_jtag_uart
			set_instance_parameter_value jtag_uart_${i} {allowMultipleConnections} {1}
			set_instance_parameter_value jtag_uart_${i} {hubInstanceID} {0}
			set_instance_parameter_value jtag_uart_${i} {readBufferDepth} {64}
			set_instance_parameter_value jtag_uart_${i} {readIRQThreshold} {8}
			set_instance_parameter_value jtag_uart_${i} {simInputCharacterStream} {}
			set_instance_parameter_value jtag_uart_${i} {simInteractiveOptions} {NO_INTERACTIVE_WINDOWS}
			set_instance_parameter_value jtag_uart_${i} {useRegistersForReadBuffer} {0}
			set_instance_parameter_value jtag_uart_${i} {useRegistersForWriteBuffer} {0}
			set_instance_parameter_value jtag_uart_${i} {useRelativePathForSimFile} {0}
			set_instance_parameter_value jtag_uart_${i} {writeBufferDepth} {512}
			set_instance_parameter_value jtag_uart_${i} {writeIRQThreshold} {8} 
			# CLOCK and RESET
			add_connection clock_bridge_0.out_clk jtag_uart_${i}.clk clock
			add_connection reset_bridge_0.out_reset jtag_uart_${i}.reset reset
			# Device in system.h
			# set_module_assignment embeddedsw.CMacro."/dev/jtg${i}" jtag_uart_${i}
		}
    }
    
    # JTAG Avalon master
    add_instance master_0 altera_jtag_avalon_master
    set_instance_parameter_value master_0 {FAST_VER} {0}
    set_instance_parameter_value master_0 {FIFO_DEPTHS} {2}
    set_instance_parameter_value master_0 {PLI_PORT} {50000}
    set_instance_parameter_value master_0 {USE_PLI} {0} 
    # CLOCK and RESET
    add_connection clock_bridge_0.out_clk master_0.clk clock
    add_connection reset_bridge_0.out_reset master_0.clk_reset reset
    add_connection master_0.master_reset grtos_0.clock_reset_reset reset
    
    # GRTOS Avalon Bridge
    set Ports $Processors
    if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
        set Ports [expr $Ports + 1]
    }
    add_instance GRTOS_Avalon_Bridge_1 GRTOS_Avalon_Bridge
    set_instance_parameter_value GRTOS_Avalon_Bridge_1 {ADDRESS_UNITS} {SYMBOLS}
    set_instance_parameter_value GRTOS_Avalon_Bridge_1 {ADDRESS_WIDTH} $Bridge_Address_Width
    set_instance_parameter_value GRTOS_Avalon_Bridge_1 {DATA_WIDTH} {32}
    set_instance_parameter_value GRTOS_Avalon_Bridge_1 {LINEWRAPBURSTS} {0}
    set_instance_parameter_value GRTOS_Avalon_Bridge_1 {MAX_BURST_SIZE} {1}
    set_instance_parameter_value GRTOS_Avalon_Bridge_1 {MAX_PENDING_RESPONSES} {32}
    set_instance_parameter_value GRTOS_Avalon_Bridge_1 {NProcessors} $Ports
    set_instance_parameter_value GRTOS_Avalon_Bridge_1 {SYMBOL_WIDTH} {8} 
    # CLOCK and RESET
    add_connection clock_bridge_0.out_clk GRTOS_Avalon_Bridge_1.clk clock
    add_connection reset_bridge_0.out_reset GRTOS_Avalon_Bridge_1.reset reset
    
    # Reset Bridge
    add_instance reset_bridge_1 altera_reset_bridge
    set_instance_parameter_value reset_bridge_1 {ACTIVE_LOW_RESET} {1}
    set_instance_parameter_value reset_bridge_1 {NUM_RESET_OUTPUTS} {1}
    set_instance_parameter_value reset_bridge_1 {SYNCHRONOUS_EDGES} {deassert}
    set_instance_parameter_value reset_bridge_1 {USE_RESET_REQUEST} {0}
    # CLOCK
    add_connection clock_bridge_0.out_clk reset_bridge_1.clk clock

    # SYSID
    # 17-07-2024 add_instance sysid_qsys_0 altera_avalon_sysid_qsys
    # 17-07-2024 set_instance_parameter_value sysid_qsys_0 {id} {1}
    # 17-07-2024 # CLOCK and RESET
    # 17-07-2024 add_connection clock_bridge_0.out_clk sysid_qsys_0.clk clock
    # 17-07-2024 add_connection reset_bridge_0.out_reset sysid_qsys_0.reset reset
    
    # TIMER
    # 27/03/2023 add_instance timer_0 altera_avalon_timer
    # 27/03/2023 set_instance_parameter_value timer_0 {alwaysRun} {0}
    # 27/03/2023 set_instance_parameter_value timer_0 {counterSize} {32}
    # 27/03/2023 set_instance_parameter_value timer_0 {fixedPeriod} {0}
    # 27/03/2023 set_instance_parameter_value timer_0 {period} {1}
    # 27/03/2023 set_instance_parameter_value timer_0 {periodUnits} {MSEC}
    # 27/03/2023 set_instance_parameter_value timer_0 {resetOutput} {0}
    # 27/03/2023 set_instance_parameter_value timer_0 {snapshot} {1}
    # 27/03/2023 set_instance_parameter_value timer_0 {timeoutPulseOutput} {0}
    # 27/03/2023 set_instance_parameter_value timer_0 {watchdogPulse} {2} 
    # 27/03/2023 # CLOCK and RESET
    # 27/03/2023 add_connection clock_bridge_0.out_clk timer_0.clk clock
    # 27/03/2023 add_connection reset_bridge_0.out_reset timer_0.reset reset
    
    ################################ DEC 2021
    for {set i 1} {$i <= $Processors} {incr i} {
		if {($independet_buses == true) || ($i == 1)} {
			add_instance mm_clock_crossing_bridge_pro_${i} altera_avalon_mm_clock_crossing_bridge
			set_instance_parameter_value mm_clock_crossing_bridge_pro_${i} {ADDRESS_UNITS} {SYMBOLS}
			set_instance_parameter_value mm_clock_crossing_bridge_pro_${i} {ADDRESS_WIDTH} $Bridge_Address_Width
			set_instance_parameter_value mm_clock_crossing_bridge_pro_${i} {COMMAND_FIFO_DEPTH} {4}
			set_instance_parameter_value mm_clock_crossing_bridge_pro_${i} {DATA_WIDTH} {32}
			set_instance_parameter_value mm_clock_crossing_bridge_pro_${i} {MASTER_SYNC_DEPTH} {2}
			set_instance_parameter_value mm_clock_crossing_bridge_pro_${i} {MAX_BURST_SIZE} {1}
			set_instance_parameter_value mm_clock_crossing_bridge_pro_${i} {RESPONSE_FIFO_DEPTH} {4}
			set_instance_parameter_value mm_clock_crossing_bridge_pro_${i} {SLAVE_SYNC_DEPTH} {2}
			set_instance_parameter_value mm_clock_crossing_bridge_pro_${i} {SYMBOL_WIDTH} {8}
			set_instance_parameter_value mm_clock_crossing_bridge_pro_${i} {USE_AUTO_ADDRESS_WIDTH} {0}
			
			add_connection clock_bridge_external_bus.out_clk mm_clock_crossing_bridge_pro_${i}.m0_clk clock
			add_connection clock_bridge_0.out_clk mm_clock_crossing_bridge_pro_${i}.s0_clk clock

			add_connection reset_bridge_0.out_reset mm_clock_crossing_bridge_pro_${i}.m0_reset reset
			add_connection reset_bridge_0.out_reset mm_clock_crossing_bridge_pro_${i}.s0_reset reset
			
			add_connection GRTOS_Avalon_Bridge_1.m${i} mm_clock_crossing_bridge_pro_${i}.s0 avalon
			set_connection_parameter_value GRTOS_Avalon_Bridge_1.m${i}/mm_clock_crossing_bridge_pro_${i}.s0 arbitrationPriority {1}
			set_connection_parameter_value GRTOS_Avalon_Bridge_1.m${i}/mm_clock_crossing_bridge_pro_${i}.s0 baseAddress {0x0000}
			set_connection_parameter_value GRTOS_Avalon_Bridge_1.m${i}/mm_clock_crossing_bridge_pro_${i}.s0 defaultConnection {0}
		} else {
			# 18-09-2023
			add_connection GRTOS_Avalon_Bridge_1.m${i} mm_clock_crossing_bridge_pro_1.s0 avalon
			set_connection_parameter_value GRTOS_Avalon_Bridge_1.m${i}/mm_clock_crossing_bridge_pro_1.s0 arbitrationPriority {1}
			set_connection_parameter_value GRTOS_Avalon_Bridge_1.m${i}/mm_clock_crossing_bridge_pro_1.s0 baseAddress {0x0000}
			set_connection_parameter_value GRTOS_Avalon_Bridge_1.m${i}/mm_clock_crossing_bridge_pro_1.s0 defaultConnection {0}		
		}
    }
	
    ################################ DEC 2021    
    ################################ JAN 2022
    # HPS internal access
    if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
        add_instance mm_clock_crossing_bridge_HPS_OUT altera_avalon_mm_clock_crossing_bridge
        set_instance_parameter_value mm_clock_crossing_bridge_HPS_OUT {ADDRESS_UNITS} {SYMBOLS}
        set_instance_parameter_value mm_clock_crossing_bridge_HPS_OUT {ADDRESS_WIDTH} $Bridge_Address_Width
        set_instance_parameter_value mm_clock_crossing_bridge_HPS_OUT {COMMAND_FIFO_DEPTH} {4}
        set_instance_parameter_value mm_clock_crossing_bridge_HPS_OUT {DATA_WIDTH} {32}
        set_instance_parameter_value mm_clock_crossing_bridge_HPS_OUT {MASTER_SYNC_DEPTH} {2}
        set_instance_parameter_value mm_clock_crossing_bridge_HPS_OUT {MAX_BURST_SIZE} {1}
        set_instance_parameter_value mm_clock_crossing_bridge_HPS_OUT {RESPONSE_FIFO_DEPTH} {4}
        set_instance_parameter_value mm_clock_crossing_bridge_HPS_OUT {SLAVE_SYNC_DEPTH} {2}
        set_instance_parameter_value mm_clock_crossing_bridge_HPS_OUT {SYMBOL_WIDTH} {8}
        set_instance_parameter_value mm_clock_crossing_bridge_HPS_OUT {USE_AUTO_ADDRESS_WIDTH} {0}
        
        add_connection clock_bridge_external_bus.out_clk mm_clock_crossing_bridge_HPS_OUT.m0_clk clock
        add_connection clock_bridge_0.out_clk mm_clock_crossing_bridge_HPS_OUT.s0_clk clock

        add_connection reset_bridge_0.out_reset mm_clock_crossing_bridge_HPS_OUT.m0_reset reset
        add_connection reset_bridge_0.out_reset mm_clock_crossing_bridge_HPS_OUT.s0_reset reset
        
        add_connection GRTOS_Avalon_Bridge_1.m${Ports} mm_clock_crossing_bridge_HPS_OUT.s0 avalon
        set_connection_parameter_value GRTOS_Avalon_Bridge_1.m${Ports}/mm_clock_crossing_bridge_HPS_OUT.s0 arbitrationPriority {1}
        set_connection_parameter_value GRTOS_Avalon_Bridge_1.m${Ports}/mm_clock_crossing_bridge_HPS_OUT.s0 baseAddress {0x0000}
        set_connection_parameter_value GRTOS_Avalon_Bridge_1.m${Ports}/mm_clock_crossing_bridge_HPS_OUT.s0 defaultConnection {0}

    }
    ################################ JAN 2022    
    ##########################################

    # Connections
    ##########################################
    
    # ###################################### 06/10/2022
    # connections and connection parameters
    add_connection nios_avalon_monitor.bus_internal grtos_0.bus_internal conduit
    set_connection_parameter_value nios_avalon_monitor.bus_internal/grtos_0.bus_internal endPort {}
    set_connection_parameter_value nios_avalon_monitor.bus_internal/grtos_0.bus_internal endPortLSB {0}
    set_connection_parameter_value nios_avalon_monitor.bus_internal/grtos_0.bus_internal startPort {}
    set_connection_parameter_value nios_avalon_monitor.bus_internal/grtos_0.bus_internal startPortLSB {0}
    set_connection_parameter_value nios_avalon_monitor.bus_internal/grtos_0.bus_internal width {0}
    # ###################################### 06/10/2022
    
    
    # NIOS CONNECTION TO MONITOR
    for {set i 1} {$i <= $Processors} {incr i} {
        add_connection gemrtos_proc_${i}.${data_manager} nios_avalon_monitor.s${i} avalon
        set_connection_parameter_value gemrtos_proc_${i}.${data_manager}/nios_avalon_monitor.s${i} arbitrationPriority {1}
        set_connection_parameter_value gemrtos_proc_${i}.${data_manager}/nios_avalon_monitor.s${i} baseAddress {0x0000}
        set_connection_parameter_value gemrtos_proc_${i}.${data_manager}/nios_avalon_monitor.s${i} defaultConnection {0}
        add_connection gemrtos_proc_${i}.${instruction_manager} nios_avalon_monitor.s${i} avalon
        set_connection_parameter_value gemrtos_proc_${i}.${instruction_manager}/nios_avalon_monitor.s${i} arbitrationPriority {1}
        set_connection_parameter_value gemrtos_proc_${i}.${instruction_manager}/nios_avalon_monitor.s${i} baseAddress {0x0000}
        set_connection_parameter_value gemrtos_proc_${i}.${instruction_manager}/nios_avalon_monitor.s${i} defaultConnection {0}    
    }

	if { ($Type_of_Processor == "Nios V/m") || ($Type_of_Processor == "Nios V/g") } {
		# Connection of debug_mem_slave at $BaseAddress span 0x800 each
		for {set i 1} {$i <= $Processors} {incr i} {
			add_connection master_0.master gemrtos_proc_${i}.${debug_mem_slave} avalon
			set_connection_parameter_value master_0.master/gemrtos_proc_${i}.${debug_mem_slave} arbitrationPriority {1}
			set_connection_parameter_value master_0.master/gemrtos_proc_${i}.${debug_mem_slave} baseAddress $BaseAddress
			set_connection_parameter_value master_0.master/gemrtos_proc_${i}.${debug_mem_slave} defaultConnection {0}
		
			add_connection nios_avalon_monitor.m${i} gemrtos_proc_${i}.${debug_mem_slave} avalon
			set_connection_parameter_value nios_avalon_monitor.m${i}/gemrtos_proc_${i}.${debug_mem_slave} arbitrationPriority {1}
			set address_base [ get_connection_parameter_value master_0.master/gemrtos_proc_${i}.${debug_mem_slave} baseAddress ]
			set_connection_parameter_value nios_avalon_monitor.m${i}/gemrtos_proc_${i}.${debug_mem_slave} baseAddress $address_base
			set_connection_parameter_value nios_avalon_monitor.m${i}/gemrtos_proc_${i}.${debug_mem_slave} defaultConnection {0}
			# HPS internal access
			# if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
			#     add_connection mm_clock_crossing_bridge_0.m0 gemrtos_proc_${i}.${debug_mem_slave} avalon
			#     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/gemrtos_proc_${i}.${debug_mem_slave} arbitrationPriority {1}
			#     set address_base [ get_connection_parameter_value master_0.master/gemrtos_proc_${i}.${debug_mem_slave} baseAddress ]
			#     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/gemrtos_proc_${i}.${debug_mem_slave} baseAddress $address_base
			#     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/gemrtos_proc_${i}.${debug_mem_slave} defaultConnection {0}        
			# }
			set ancho [get_instance_interface_property gemrtos_proc_${i} ${debug_mem_slave} addressSpan ]
			send_message Debug "Span of gemrtos_proc_${i} debug_mem_slave is $ancho, base address is $BaseAddress"
			set BaseAddress [expr {$BaseAddress + $ancho}]
			# set BaseAddress [expr {$BaseAddress + 0x800}]
		}
	}

# 31/10/2023 #############################################	
    # 31-05-2024  # Connection of onchip_memory2_2 at $BaseAddress span 0x2000
    # 31-05-2024  add_connection master_0.master onchip_memory2_2.s1 avalon
    # 31-05-2024  set_connection_parameter_value master_0.master/onchip_memory2_2.s1 arbitrationPriority {1}
    # 31-05-2024  set_connection_parameter_value master_0.master/onchip_memory2_2.s1 baseAddress $BaseAddress
    # 31-05-2024  set_connection_parameter_value master_0.master/onchip_memory2_2.s1 defaultConnection {0}
    # 31-05-2024  for {set i 1} {$i <= $Processors} {incr i} {
    # 31-05-2024      add_connection nios_avalon_monitor.m$i onchip_memory2_2.s1 avalon
    # 31-05-2024      set_connection_parameter_value nios_avalon_monitor.m${i}/onchip_memory2_2.s1 arbitrationPriority {1}
    # 31-05-2024      set address_base [ get_connection_parameter_value master_0.master/onchip_memory2_2.s1 baseAddress ]
    # 31-05-2024      set_connection_parameter_value nios_avalon_monitor.m${i}/onchip_memory2_2.s1 baseAddress $address_base
    # 31-05-2024      set_connection_parameter_value nios_avalon_monitor.m${i}/onchip_memory2_2.s1 defaultConnection {0}
    # 31-05-2024  }
    # 31-05-2024  # HPS internal access
    # 31-05-2024  if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
    # 31-05-2024      add_connection mm_clock_crossing_bridge_0.m0 onchip_memory2_2.s1 avalon
    # 31-05-2024      set_connection_parameter_value mm_clock_crossing_bridge_0.m0/onchip_memory2_2.s1 arbitrationPriority {1}
    # 31-05-2024      set address_base [ get_connection_parameter_value master_0.master/onchip_memory2_2.s1 baseAddress ]
    # 31-05-2024      set_connection_parameter_value mm_clock_crossing_bridge_0.m0/onchip_memory2_2.s1 baseAddress $address_base
    # 31-05-2024      set_connection_parameter_value mm_clock_crossing_bridge_0.m0/onchip_memory2_2.s1 defaultConnection {0}        
    # 31-05-2024  }
    # 31-05-2024  set ancho [get_instance_interface_property onchip_memory2_2 s1 addressSpan ]
    # 31-05-2024  send_message Debug "Span of onchip_memory2_2 s1 is $ancho, base address is $BaseAddress"
    # 31-05-2024  set BaseAddress [expr {$BaseAddress + $ancho}]
  

# 31/10/2023 #############################################	
    
	
	if { ($Type_of_Processor == "Nios II/e (economy)") || ($Type_of_Processor == "Nios II/f (full)") } { 
		# Connection of debug_mem_slave at $BaseAddress span 0x800 each
		for {set i 1} {$i <= $Processors} {incr i} {
			add_connection master_0.master gemrtos_proc_${i}.${debug_mem_slave} avalon
			set_connection_parameter_value master_0.master/gemrtos_proc_${i}.${debug_mem_slave} arbitrationPriority {1}
			set_connection_parameter_value master_0.master/gemrtos_proc_${i}.${debug_mem_slave} baseAddress $BaseAddress
			set_connection_parameter_value master_0.master/gemrtos_proc_${i}.${debug_mem_slave} defaultConnection {0}
		
			add_connection nios_avalon_monitor.m${i} gemrtos_proc_${i}.${debug_mem_slave} avalon
			set_connection_parameter_value nios_avalon_monitor.m${i}/gemrtos_proc_${i}.${debug_mem_slave} arbitrationPriority {1}
			set address_base [ get_connection_parameter_value master_0.master/gemrtos_proc_${i}.${debug_mem_slave} baseAddress ]
			set_connection_parameter_value nios_avalon_monitor.m${i}/gemrtos_proc_${i}.${debug_mem_slave} baseAddress $address_base
			set_connection_parameter_value nios_avalon_monitor.m${i}/gemrtos_proc_${i}.${debug_mem_slave} defaultConnection {0}
			# HPS internal access
			# if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
			#     add_connection mm_clock_crossing_bridge_0.m0 gemrtos_proc_${i}.${debug_mem_slave} avalon
			#     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/gemrtos_proc_${i}.${debug_mem_slave} arbitrationPriority {1}
			#     set address_base [ get_connection_parameter_value master_0.master/gemrtos_proc_${i}.${debug_mem_slave} baseAddress ]
			#     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/gemrtos_proc_${i}.${debug_mem_slave} baseAddress $address_base
			#     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/gemrtos_proc_${i}.${debug_mem_slave} defaultConnection {0}        
			# }
			set ancho [get_instance_interface_property gemrtos_proc_${i} ${debug_mem_slave} addressSpan ]
			send_message Debug "Span of gemrtos_proc_${i} debug_mem_slave is $ancho, base address is $BaseAddress"
			set BaseAddress [expr {$BaseAddress + $ancho}]
			# set BaseAddress [expr {$BaseAddress + 0x800}]
		}
	}
    
	if { ($resetSlave == "Internal GeMRTOS (default)" ) } {
		# Connection of rst1_and_exception_memory at $BaseAddress span 0x2000
		add_connection master_0.master rst1_and_exception_memory.s1 avalon
		set_connection_parameter_value master_0.master/rst1_and_exception_memory.s1 arbitrationPriority {1}
		set_connection_parameter_value master_0.master/rst1_and_exception_memory.s1 baseAddress $BaseAddress
		set_connection_parameter_value master_0.master/rst1_and_exception_memory.s1 defaultConnection {0}
		for {set i 1} {$i <= $Processors} {incr i} {
			add_connection nios_avalon_monitor.m$i rst1_and_exception_memory.s1 avalon
			set_connection_parameter_value nios_avalon_monitor.m${i}/rst1_and_exception_memory.s1 arbitrationPriority {1}
			set address_base [ get_connection_parameter_value master_0.master/rst1_and_exception_memory.s1 baseAddress ]
			set_connection_parameter_value nios_avalon_monitor.m${i}/rst1_and_exception_memory.s1 baseAddress $address_base
			set_connection_parameter_value nios_avalon_monitor.m${i}/rst1_and_exception_memory.s1 defaultConnection {0}
		}
		# HPS internal access
		if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
			add_connection mm_clock_crossing_bridge_0.m0 rst1_and_exception_memory.s1 avalon
			set_connection_parameter_value mm_clock_crossing_bridge_0.m0/rst1_and_exception_memory.s1 arbitrationPriority {1}
			set address_base [ get_connection_parameter_value master_0.master/rst1_and_exception_memory.s1 baseAddress ]
			set_connection_parameter_value mm_clock_crossing_bridge_0.m0/rst1_and_exception_memory.s1 baseAddress $address_base
			set_connection_parameter_value mm_clock_crossing_bridge_0.m0/rst1_and_exception_memory.s1 defaultConnection {0}        
		}
		set ancho [get_instance_interface_property rst1_and_exception_memory s1 addressSpan ]
		send_message Debug "Span of rst1_and_exception_memory s1 is $ancho, base address is $BaseAddress"
		set BaseAddress [expr {$BaseAddress + $ancho}]
		# set BaseAddress [expr {$BaseAddress + 0x0800}]
    }


    # Connection of grtos_0.s_processor_monitor at $BaseAddress span 0x400  
    add_connection master_0.master grtos_0.s_processor_monitor avalon
    set_connection_parameter_value master_0.master/grtos_0.s_processor_monitor arbitrationPriority {1}
    set_connection_parameter_value master_0.master/grtos_0.s_processor_monitor baseAddress $BaseAddress
    set_connection_parameter_value master_0.master/grtos_0.s_processor_monitor defaultConnection {0}
    for {set i 1} {$i <= $Processors} {incr i} {
        add_connection nios_avalon_monitor.m${i} grtos_0.s_processor_monitor avalon
        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_processor_monitor arbitrationPriority {1}
        set address_base [ get_connection_parameter_value master_0.master/grtos_0.s_processor_monitor baseAddress ]
        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_processor_monitor baseAddress $address_base
        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_processor_monitor defaultConnection {0}    
    }
    # HPS internal access
    if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
        add_connection mm_clock_crossing_bridge_0.m0 grtos_0.s_processor_monitor avalon
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/grtos_0.s_processor_monitor arbitrationPriority {1}
        set address_base [ get_connection_parameter_value master_0.master/grtos_0.s_processor_monitor baseAddress ]
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/grtos_0.s_processor_monitor baseAddress $address_base
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/grtos_0.s_processor_monitor defaultConnection {0}        
    } 
    set ancho [get_instance_interface_property grtos_0 s_processor_monitor addressSpan ]
    send_message Debug "Span of grtos_0 s_processor_monitor is $ancho, base address is $BaseAddress"
    set BaseAddress [expr {$BaseAddress + $ancho}]
    # set BaseAddress [expr {$BaseAddress + 0x400}]
# #########################################################    

    # 30-05-2024 # Connection of rstaux_memory at $BaseAddress span 0x800
    # 30-05-2024 add_connection master_0.master rstaux_memory.s1 avalon
    # 30-05-2024 set_connection_parameter_value master_0.master/rstaux_memory.s1 arbitrationPriority {1}
    # 30-05-2024 set_connection_parameter_value master_0.master/rstaux_memory.s1 baseAddress $BaseAddress
    # 30-05-2024 set_connection_parameter_value master_0.master/rstaux_memory.s1 defaultConnection {0}
    # 30-05-2024 for {set i 1} {$i <= $Processors} {incr i} {
    # 30-05-2024     add_connection nios_avalon_monitor.m$i rstaux_memory.s1 avalon
    # 30-05-2024     set_connection_parameter_value nios_avalon_monitor.m${i}/rstaux_memory.s1 arbitrationPriority {1}
    # 30-05-2024     set address_base [ get_connection_parameter_value master_0.master/rstaux_memory.s1 baseAddress ]
    # 30-05-2024     set_connection_parameter_value nios_avalon_monitor.m${i}/rstaux_memory.s1 baseAddress $address_base
    # 30-05-2024     set_connection_parameter_value nios_avalon_monitor.m${i}/rstaux_memory.s1 defaultConnection {0}
    # 30-05-2024 }
    # 30-05-2024 # HPS internal access
    # 30-05-2024 if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
    # 30-05-2024     add_connection mm_clock_crossing_bridge_0.m0 rstaux_memory.s1 avalon
    # 30-05-2024     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/rstaux_memory.s1 arbitrationPriority {1}
    # 30-05-2024     set address_base [ get_connection_parameter_value master_0.master/rstaux_memory.s1 baseAddress ]
    # 30-05-2024     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/rstaux_memory.s1 baseAddress $address_base
    # 30-05-2024     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/rstaux_memory.s1 defaultConnection {0}        
    # 30-05-2024 }
    # set ancho [get_instance_interface_properties rstaux_memory s1 ]
    # send_message Debug $ancho
    # 30-05-2024 set ancho [get_instance_interface_property rstaux_memory s1 addressSpan ]
    # 30-05-2024 send_message Debug "Span of rstaux_memory is $ancho, base address is $BaseAddress"
    # set BaseAddress [expr {$BaseAddress + 0x0200}]
    set BaseAddress [expr {$BaseAddress + $ancho}]

    # Connection of grtos_0.s_Global avalon at $BaseAddress span 0x200 
    add_connection master_0.master grtos_0.s_Global avalon
    set_connection_parameter_value master_0.master/grtos_0.s_Global arbitrationPriority {1}
    set_connection_parameter_value master_0.master/grtos_0.s_Global baseAddress $BaseAddress
    set_connection_parameter_value master_0.master/grtos_0.s_Global defaultConnection {0}
    for {set i 1} {$i <= $Processors} {incr i} {
        add_connection nios_avalon_monitor.m${i} grtos_0.s_Global avalon
        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_Global arbitrationPriority {1}
        set address_base [ get_connection_parameter_value master_0.master/grtos_0.s_Global baseAddress ]
        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_Global baseAddress $address_base
        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_Global defaultConnection {0}        
    }
    # HPS internal access
    if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
        add_connection mm_clock_crossing_bridge_0.m0 grtos_0.s_Global avalon
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/grtos_0.s_Global arbitrationPriority {1}
        set address_base [ get_connection_parameter_value master_0.master/grtos_0.s_Global baseAddress ]
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/grtos_0.s_Global baseAddress $address_base
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/grtos_0.s_Global defaultConnection {0}        
    }
    set ancho [get_instance_interface_property grtos_0 s_Global addressSpan ]
    send_message Debug "Span of grtos_0 s_Global is $ancho, base address is $BaseAddress"
    set BaseAddress [expr {$BaseAddress + $ancho}]    
    # set BaseAddress [expr {$BaseAddress + 0x200}]
    
    # Connection of nios_avalon_monitor.s_Global avalon at $BaseAddress span 0x100 
    add_connection master_0.master nios_avalon_monitor.s_Global avalon
    set_connection_parameter_value master_0.master/nios_avalon_monitor.s_Global arbitrationPriority {1}
    set_connection_parameter_value master_0.master/nios_avalon_monitor.s_Global baseAddress $BaseAddress
    set_connection_parameter_value master_0.master/nios_avalon_monitor.s_Global defaultConnection {0}
    for {set i 1} {$i <= $Processors} {incr i} {
        add_connection nios_avalon_monitor.m${i} nios_avalon_monitor.s_Global avalon
        set_connection_parameter_value nios_avalon_monitor.m${i}/nios_avalon_monitor.s_Global arbitrationPriority {1}
        set address_base [ get_connection_parameter_value master_0.master/nios_avalon_monitor.s_Global baseAddress ]
        set_connection_parameter_value nios_avalon_monitor.m${i}/nios_avalon_monitor.s_Global baseAddress $address_base
        set_connection_parameter_value nios_avalon_monitor.m${i}/nios_avalon_monitor.s_Global defaultConnection {0}
    }
    # HPS internal access
    if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
        add_connection mm_clock_crossing_bridge_0.m0 nios_avalon_monitor.s_Global avalon
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/nios_avalon_monitor.s_Global arbitrationPriority {1}
        set address_base [ get_connection_parameter_value master_0.master/nios_avalon_monitor.s_Global baseAddress ]
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/nios_avalon_monitor.s_Global baseAddress $address_base
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/nios_avalon_monitor.s_Global defaultConnection {0}        
    }    
    set ancho [get_instance_interface_property nios_avalon_monitor s_Global addressSpan ]
    # 27/03/2023 set unidades [get_instance_interface_property timer_0 s1 addressUnits ]
    # 27/03/2023 send_message Warning "Span of nios_avalon_monitor s_Global is $ancho, $unidades, base address is $BaseAddress"
    set BaseAddress [expr {$BaseAddress + $ancho}]      
    # set BaseAddress [expr {$BaseAddress + 0x100}]

    
    # Connection of grtos_0.s_processor$i at $BaseAddress span 0x8 each 
    for {set i 1} {$i <= $Processors} {incr i} {
		if {$i == 1} { 
			add_connection grtos_0.dummy_master grtos_0.s_processor$i avalon
			set_connection_parameter_value grtos_0.dummy_master/grtos_0.s_processor${i} arbitrationPriority {1}
			set_connection_parameter_value grtos_0.dummy_master/grtos_0.s_processor${i} baseAddress $BaseAddress
			set_connection_parameter_value grtos_0.dummy_master/grtos_0.s_processor${i} defaultConnection {0}
		} 
        
        # 04-12-2023 # The processor 1 connects to all the ports
        # 04-12-2023 if {$i != 1} {
        # 04-12-2023     add_connection nios_avalon_monitor.m1 grtos_0.s_processor$i avalon
        # 04-12-2023     set_connection_parameter_value nios_avalon_monitor.m1/grtos_0.s_processor${i} arbitrationPriority {1}
        # 04-12-2023     set address_base [ get_connection_parameter_value grtos_0.dummy_master/grtos_0.s_processor${i} baseAddress ]
        # 04-12-2023     set_connection_parameter_value nios_avalon_monitor.m1/grtos_0.s_processor${i} baseAddress $address_base
        # 04-12-2023     set_connection_parameter_value nios_avalon_monitor.m1/grtos_0.s_processor${i} defaultConnection {0}
        # 04-12-2023 } 

        add_connection nios_avalon_monitor.m$i grtos_0.s_processor$i avalon
        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_processor${i} arbitrationPriority {1}
        set address_base [ get_connection_parameter_value grtos_0.dummy_master/grtos_0.s_processor1 baseAddress ]
        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_processor${i} baseAddress $address_base
        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_processor${i} defaultConnection {0}
        
    }  
	set ancho [get_instance_interface_property grtos_0 s_processor1 addressSpan ]
	send_message Debug "Span of grtos_0 s_processor1 is $ancho, base address is $BaseAddress"
	set BaseAddress [expr {$BaseAddress + $ancho}]          
	# set BaseAddress [expr {$BaseAddress + 0x20}]



    
# ######################################## 01/09/2022
    # Connection of grtos_0.s_processor_monitor$i at $BaseAddress span 0x8 each 
    for {set i 1} {$i <= $Processors} {incr i} {
        add_connection master_0.master grtos_0.s_processor_monitor$i avalon
        set_connection_parameter_value master_0.master/grtos_0.s_processor_monitor${i} arbitrationPriority {1}
        set_connection_parameter_value master_0.master/grtos_0.s_processor_monitor${i} baseAddress $BaseAddress
        set_connection_parameter_value master_0.master/grtos_0.s_processor_monitor${i} defaultConnection {0}
        
        # The processor 1 connects to all the ports
        if {$i != 1} {
            add_connection nios_avalon_monitor.m1 grtos_0.s_processor_monitor$i avalon
            set_connection_parameter_value nios_avalon_monitor.m1/grtos_0.s_processor_monitor${i} arbitrationPriority {1}
            set address_base [ get_connection_parameter_value master_0.master/grtos_0.s_processor_monitor${i} baseAddress ]
            set_connection_parameter_value nios_avalon_monitor.m1/grtos_0.s_processor_monitor${i} baseAddress $address_base
            set_connection_parameter_value nios_avalon_monitor.m1/grtos_0.s_processor_monitor${i} defaultConnection {0}        
        } 

        add_connection nios_avalon_monitor.m$i grtos_0.s_processor_monitor$i avalon
        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_processor_monitor${i} arbitrationPriority {1}
        set address_base [ get_connection_parameter_value master_0.master/grtos_0.s_processor_monitor${i} baseAddress ]
        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_processor_monitor${i} baseAddress $address_base
        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_processor_monitor${i} defaultConnection {0}
        
        # HPS internal access does not require acces to idle port
        if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
            add_connection mm_clock_crossing_bridge_0.m0 grtos_0.s_processor_monitor${i} avalon
            set_connection_parameter_value mm_clock_crossing_bridge_0.m0/grtos_0.s_processor_monitor${i} arbitrationPriority {1}
            set address_base [ get_connection_parameter_value master_0.master/grtos_0.s_processor_monitor${i} baseAddress ]
            set_connection_parameter_value mm_clock_crossing_bridge_0.m0/grtos_0.s_processor_monitor${i} baseAddress $address_base
            set_connection_parameter_value mm_clock_crossing_bridge_0.m0/grtos_0.s_processor_monitor${i} defaultConnection {0}        
        }   
        set ancho [get_instance_interface_property grtos_0 s_processor_monitor${i} addressSpan ]
        send_message Debug "Span of grtos_0 s_processor_monitor${i} is $ancho, base address is $BaseAddress"
        set BaseAddress [expr {$BaseAddress + max(0x8, $ancho)}]          
        # set BaseAddress [expr {$BaseAddress + 0x20}]
    }

# ######################################## 01/09/2022    
    



# ########################################
# Lugar anterior de grtos_monitor
#    # Connection of grtos_0.s_processor_monitor at $BaseAddress span 0x8  
#    add_connection master_0.master grtos_0.s_processor_monitor avalon
#    set_connection_parameter_value master_0.master/grtos_0.s_processor_monitor arbitrationPriority {1}
#    set_connection_parameter_value master_0.master/grtos_0.s_processor_monitor baseAddress $BaseAddress
#    set_connection_parameter_value master_0.master/grtos_0.s_processor_monitor defaultConnection {0}
#    for {set i 1} {$i <= $Processors} {incr i} {
#        add_connection nios_avalon_monitor.m${i} grtos_0.s_processor_monitor avalon
#        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_processor_monitor arbitrationPriority {1}
#        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_processor_monitor baseAddress $BaseAddress
#        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_processor_monitor defaultConnection {0}    
#    }
#    # HPS internal access
#    if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
#        add_connection mm_clock_crossing_bridge_0.m0 grtos_0.s_processor_monitor avalon
#        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/grtos_0.s_processor_monitor arbitrationPriority {1}
#        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/grtos_0.s_processor_monitor baseAddress $BaseAddress
#        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/grtos_0.s_processor_monitor defaultConnection {0}        
#    }  
#    set BaseAddress [expr {$BaseAddress + 0x20}]
# ########################################

    # 17-07-2024 # Connection of sysid_qsys_0.control_slave at $BaseAddress span 0x8 
    # 17-07-2024 add_connection master_0.master sysid_qsys_0.control_slave avalon
    # 17-07-2024 set_connection_parameter_value master_0.master/sysid_qsys_0.control_slave arbitrationPriority {1}
    # 17-07-2024 set_connection_parameter_value master_0.master/sysid_qsys_0.control_slave baseAddress $BaseAddress
    # 17-07-2024 set_connection_parameter_value master_0.master/sysid_qsys_0.control_slave defaultConnection {0}
    # 17-07-2024 for {set i 1} {$i <= $Processors} {incr i} {
    # 17-07-2024     add_connection nios_avalon_monitor.m${i} sysid_qsys_0.control_slave avalon
    # 17-07-2024     set_connection_parameter_value nios_avalon_monitor.m${i}/sysid_qsys_0.control_slave arbitrationPriority {1}
    # 17-07-2024     set address_base [ get_connection_parameter_value master_0.master/sysid_qsys_0.control_slave baseAddress ]
    # 17-07-2024     set_connection_parameter_value nios_avalon_monitor.m${i}/sysid_qsys_0.control_slave baseAddress $address_base
    # 17-07-2024     set_connection_parameter_value nios_avalon_monitor.m${i}/sysid_qsys_0.control_slave defaultConnection {0}    
    # 17-07-2024 }
    # 17-07-2024 # HPS internal access
    # 17-07-2024 # if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
    # 17-07-2024 #     add_connection mm_clock_crossing_bridge_0.m0 sysid_qsys_0.control_slave avalon
    # 17-07-2024 #     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/sysid_qsys_0.control_slave arbitrationPriority {1}
    # 17-07-2024 #     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/sysid_qsys_0.control_slave baseAddress $BaseAddress
    # 17-07-2024 #     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/sysid_qsys_0.control_slave defaultConnection {0}        
    # 17-07-2024 # } 
    # 17-07-2024 set ancho [get_instance_interface_property sysid_qsys_0 control_slave addressSpan ]
    # 17-07-2024 set unidades [get_instance_interface_property sysid_qsys_0 control_slave addressUnits ]
    # 17-07-2024 send_message Debug "Span of sysid_qsys_0 control_slave is $ancho, $unidades, base address is $BaseAddress"
    # 17-07-2024 set BaseAddress [expr {$BaseAddress + $ancho}]      



    
    # Connection of jtag_uart_0.avalon_jtag_slave at $BaseAddress span 0x8 each for the NProcessors 
    for {set i 0} {$i <= $Processors + 1} {incr i} {
		if {(($incl_STDIO == true) && ($i == 0)) || (($incl_STDERR == true) && ($i == 1)) || (($incl_STDPROC == true) && ($i > 1))} {
			add_connection master_0.master jtag_uart_${i}.avalon_jtag_slave avalon
			set_connection_parameter_value master_0.master/jtag_uart_${i}.avalon_jtag_slave arbitrationPriority {1}
			set_connection_parameter_value master_0.master/jtag_uart_${i}.avalon_jtag_slave baseAddress $BaseAddress
			set_connection_parameter_value master_0.master/jtag_uart_${i}.avalon_jtag_slave defaultConnection {0}     
			for {set j 1} {$j <= $Processors} {incr j} {
	 
				add_connection nios_avalon_monitor.m${j} jtag_uart_${i}.avalon_jtag_slave avalon
				set_connection_parameter_value nios_avalon_monitor.m${j}/jtag_uart_${i}.avalon_jtag_slave arbitrationPriority {1}
				set address_base [ get_connection_parameter_value master_0.master/jtag_uart_${i}.avalon_jtag_slave baseAddress ]
				set_connection_parameter_value nios_avalon_monitor.m${j}/jtag_uart_${i}.avalon_jtag_slave baseAddress $address_base
				set_connection_parameter_value nios_avalon_monitor.m${j}/jtag_uart_${i}.avalon_jtag_slave defaultConnection {0}        
			}
			# HPS internal access
			# if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
			#     add_connection mm_clock_crossing_bridge_0.m0 jtag_uart_${i}.avalon_jtag_slave avalon
			#     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/jtag_uart_${i}.avalon_jtag_slave arbitrationPriority {1}
			#     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/jtag_uart_${i}.avalon_jtag_slave baseAddress $BaseAddress
			#     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/jtag_uart_${i}.avalon_jtag_slave defaultConnection {0}        
			# }
			set ancho [get_instance_interface_property jtag_uart_${i} avalon_jtag_slave addressSpan ]
			set unidades [get_instance_interface_property jtag_uart_${i} avalon_jtag_slave addressUnits ]
			send_message Debug "Span of jtag_uart_${i} avalon_jtag_slave is $ancho, $unidades, base address is $BaseAddress"
			# set BaseAddress [expr {$BaseAddress + $ancho}]  
			set BaseAddress [expr {$BaseAddress + max(0x8, $ancho)}]
		}
    }
    
    
    # Connection of GRTOS_Avalon_Bridge_1. at {0x00000000} span given by address_width
    add_connection master_0.master GRTOS_Avalon_Bridge_1.s1 avalon
    set_connection_parameter_value master_0.master/GRTOS_Avalon_Bridge_1.s1 arbitrationPriority {1}
    set_connection_parameter_value master_0.master/GRTOS_Avalon_Bridge_1.s1 baseAddress {0x0000}
    set_connection_parameter_value master_0.master/GRTOS_Avalon_Bridge_1.s1 defaultConnection {0}    
    for {set i 1} {$i <= $Processors} {incr i} {
        add_connection nios_avalon_monitor.m${i} GRTOS_Avalon_Bridge_1.s${i} avalon
        set_connection_parameter_value nios_avalon_monitor.m${i}/GRTOS_Avalon_Bridge_1.s${i} arbitrationPriority {1}
        set_connection_parameter_value nios_avalon_monitor.m${i}/GRTOS_Avalon_Bridge_1.s${i} baseAddress {0x0000}
        set_connection_parameter_value nios_avalon_monitor.m${i}/GRTOS_Avalon_Bridge_1.s${i} defaultConnection {0}    
    }
    if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
        add_connection mm_clock_crossing_bridge_0.m0 GRTOS_Avalon_Bridge_1.s${Ports} avalon
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/GRTOS_Avalon_Bridge_1.s${Ports} arbitrationPriority {2}
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/GRTOS_Avalon_Bridge_1.s${Ports} baseAddress {0x0000}
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/GRTOS_Avalon_Bridge_1.s${Ports} defaultConnection {0} 
    }
 
    # ##########################################################
    # Interruptions from irq_bridge_0
    for {set i $IntInterrupts} {$i < ${MAX_PROC_IRQs}} {incr i} {
        add_connection grtos_0.interrupt_receiver irq_bridge_0.sender[expr {$i - $IntInterrupts}]_irq interrupt
        set_connection_parameter_value grtos_0.interrupt_receiver/irq_bridge_0.sender[expr {$i - $IntInterrupts}]_irq irqNumber $i
        set j [expr {$i - $IntInterrupts}]
        send_message Debug "IRQ bridge receiver $j connected to grtos IRQ $i"

        add_connection gemrtos_proc_1.${irq_rx} irq_bridge_0.sender[expr {$i - $IntInterrupts}]_irq interrupt
        set_connection_parameter_value gemrtos_proc_1.${irq_rx}/irq_bridge_0.sender[expr {$i - $IntInterrupts}]_irq irqNumber $i
    }
 
    # Interruptions from grtos_0
    for {set i 1} {$i <= $Processors} {incr i} {
        add_connection gemrtos_proc_${i}.${irq_rx} grtos_0.slv_irq${i} interrupt
        set_connection_parameter_value gemrtos_proc_${i}.${irq_rx}/grtos_0.slv_irq${i} irqNumber {0} 
    }
    
    # Interruptions from timer_0    
    # 27/03/2023 add_connection gemrtos_proc_1.${irq_rx} timer_0.irq interrupt
    # 27/03/2023 set_connection_parameter_value gemrtos_proc_1.${irq_rx}/timer_0.irq irqNumber {1}    
    # 27/03/2023 add_connection grtos_0.interrupt_receiver timer_0.irq interrupt
    # 27/03/2023 set_connection_parameter_value grtos_0.interrupt_receiver/timer_0.irq irqNumber {1}    
    
    # Interruptions from jtag_uart_$i 
	set j 0
    for {set i 0} {$i <= $Processors + 1} {incr i} {
		if {(($incl_STDIO == true) && ($i == 0)) || (($incl_STDERR == true) && ($i == 1)) || (($incl_STDPROC == true) && ($i > 1))} {
			set j [expr {$j+1}]
			add_connection gemrtos_proc_1.${irq_rx} jtag_uart_${i}.irq interrupt
			set_connection_parameter_value gemrtos_proc_1.${irq_rx}/jtag_uart_${i}.irq irqNumber ${j}
			
			send_message Debug "JTAGUART $i connected to grtos IRQ $j"
			
			add_connection grtos_0.interrupt_receiver jtag_uart_${i}.irq interrupt
			set_connection_parameter_value grtos_0.interrupt_receiver/jtag_uart_${i}.irq irqNumber ${j}
		}
    }
    
    # ####################################################
    # RESETs
    
    # nios from debug and grtos
    for {set i 1} {$i <= $Processors} {incr i} {
		# 06-12-2023 if { ($Type_of_Processor == "Nios II/e (economy)") || ($Type_of_Processor == "Nios II/f (full)") } {
			if {$i != 1} {
				add_connection gemrtos_proc_1.${debug_agent_reset_request} gemrtos_proc_${i}.${proc_reset} reset        
			}        
			add_connection gemrtos_proc_${i}.${debug_agent_reset_request} gemrtos_proc_${i}.${proc_reset} reset
		# 06-12-2023 }
        add_connection grtos_0.slv_rst${i} gemrtos_proc_${i}.${proc_reset} reset    
    }

    # jtag_uart from debug and grtos
    for {set i 0} {$i <= $Processors + 1} {incr i} {
		if {(($incl_STDIO == true) && ($i == 0)) || (($incl_STDERR == true) && ($i == 1)) || (($incl_STDPROC == true) && ($i > 1))} {
			# 06/02/2023 add_connection gemrtos_proc_1.${debug_agent_reset_request} jtag_uart_${i}.reset reset
			add_connection gemrtos_proc_1.${debug_agent_reset_request} jtag_uart_${i}.reset reset
			add_connection grtos_0.slv_rst1 jtag_uart_${i}.reset reset
		}
    }

    # grtos controller from debug
	# if { ($Type_of_Processor == "Nios II/e (economy)") || ($Type_of_Processor == "Nios II/f (full)") } {
		add_connection gemrtos_proc_1.${debug_agent_reset_request} grtos_0.clock_reset_reset reset
	# }
    
    # GRTOS Avalon bridge from grtos
    add_connection grtos_0.slv_rst1 GRTOS_Avalon_Bridge_1.reset reset
    
    # IRG bridge from grtos
    add_connection grtos_0.slv_rst1 irq_bridge_0.clk_reset reset
    
    # GRTOS Avalon monitor from grtos and debug
    add_connection grtos_0.slv_rst1 nios_avalon_monitor.reset reset
    # 06/02/2023 add_connection gemrtos_proc_1.${debug_agent_reset_request} nios_avalon_monitor.reset reset
    add_connection gemrtos_proc_1.${debug_agent_reset_request} nios_avalon_monitor.reset reset
	 
    # onchip memories from grtos and debug
    # 30-05-2024 add_connection grtos_0.slv_rst1 rstaux_memory.reset1 reset
	if { ($resetSlave == "Internal GeMRTOS (default)" ) } {
		add_connection grtos_0.slv_rst1 rst1_and_exception_memory.reset1 reset
	}
    # 31-05-2024 add_connection grtos_0.slv_rst1 onchip_memory2_2.reset1 reset
    
    # Reset bridge from grtos
    # add_connection grtos_0.slv_rst1 reset_bridge_1.in_reset reset
	# 06-12-2023 if { ($Type_of_Processor == "Nios II/e (economy)") || ($Type_of_Processor == "Nios II/f (full)") } {
		add_connection gemrtos_proc_1.${debug_agent_reset_request} reset_bridge_1.in_reset reset
	# 06-12-2023 }
	# 06-12-2023 if { ($Type_of_Processor == "Nios V/m") } {
	# 06-12-2023 	add_connection grtos_0.slv_rst1 reset_bridge_1.in_reset reset
	# 06-12-2023 }
    
    # Sysid from grtos and debug
    # 17-07-2024 add_connection grtos_0.slv_rst1 sysid_qsys_0.reset reset
    
    # timer from grtos 
    # 27/03/2023 add_connection grtos_0.slv_rst1 timer_0.reset reset
    
    #  # #############################################
    #  # Exported interfaces
    #  # Input clock
    set_interface_property clk_processors EXPORT_OF clock_bridge_0.in_clk
    
    # Input clock for external bus
    set_interface_property clk_external_bus EXPORT_OF clock_bridge_external_bus.in_clk  

    # External reset input
    set_interface_property reset_in EXPORT_OF reset_bridge_0.in_reset
    # set_interface_property reset EXPORT_OF clk_0.clk_in_reset   

    # Nios avalon masters
    for {set i 1} {$i <= $Processors} {incr i} {
        # add_interface grtos_avalon_bridge_1_m${i} avalon start
        # set_interface_property grtos_avalon_bridge_1_m${i} EXPORT_OF GRTOS_Avalon_Bridge_1.m${i}
		if {($independet_buses == true) || ($i == 1)} {
			if {$i != 1} {
				add_interface gemrtos_avalon_processor_m${i} avalon start
			}
			set_interface_property gemrtos_avalon_processor_m${i} EXPORT_OF mm_clock_crossing_bridge_pro_${i}.m0
		} 
    }
 
    # Output for leds
    add_interface gemrtos_phy conduit end
    set_interface_property gemrtos_phy EXPORT_OF grtos_0.phy
    
    # External to IRQ Bridge
    # 02-07-2024 add_interface gemrtos_dirq_input interrupt receiver
# 02-07-2024 ############

    add_interface gemrtos_dirq_input interrupt start
    set_interface_property gemrtos_dirq_input irqScheme INDIVIDUAL_REQUESTS
	# 02-07-0224 set_interface_property gemrtos_dirq_input associatedClock clock_bridge_0.out_clk
    # 02-07-0224 set_interface_property gemrtos_dirq_input associatedAddressablePoint gemrtos_avalon_processor_m1
    set_interface_property gemrtos_dirq_input ENABLED true	
	
# 02-07-2024 ############	
    set_interface_property gemrtos_dirq_input EXPORT_OF irq_bridge_0.receiver_irq
    
    # External reset output
    add_interface reset_grtos reset source
    set_interface_property reset_grtos EXPORT_OF reset_bridge_1.out_reset

    # HPS internal access
    if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
        add_interface h2f_axi_master_in avalon slave
        set_interface_property h2f_axi_master_in EXPORT_OF mm_clock_crossing_bridge_0.s0
        # ############
        add_interface h2f_axi_clk_in clock sink
        set_interface_property h2f_axi_clk_in EXPORT_OF mm_clock_crossing_bridge_0.s0_clk
        add_interface h2f_axi_rst_in reset sink
        set_interface_property h2f_axi_rst_in EXPORT_OF mm_clock_crossing_bridge_0.s0_reset
        # ############
        
        ############################# JAN 2022
        # HPS external 
        # add_interface grtos_avalon_bridge_1_HPS avalon start
        # set_interface_property grtos_avalon_bridge_1_HPS EXPORT_OF GRTOS_Avalon_Bridge_1.m${Ports}
        

        # HPS external 
        add_interface gemrtos_avalon_bridge_HPS avalon start
        set_interface_property gemrtos_avalon_bridge_HPS EXPORT_OF mm_clock_crossing_bridge_HPS_OUT.m0        
        ############################# JAN 2022
        
    }
    # ###########################################################
    
    # interconnect requirements
    set_interconnect_requirement {$system} {qsys_mm.clockCrossingAdapter} {HANDSHAKE}
    set_interconnect_requirement {$system} {qsys_mm.maxAdditionalLatency} {1}
    

}

## Add documentation links for user guide and/or release notes
add_documentation_link "User Guide" https://gemrtos.com
add_documentation_link "Release Notes" https://gemrtos.com