##
## \file
## \author Ricardo Cayssials
## \brief GeMRTOS_Multiprocessor top-level Platform Designer component descriptor
## \details Defines the GeMRTOS_Multiprocessor Qsys component: composes the
##          GeMRTOS controller, Nios processors, bridge, and monitor IP into a
##          single instantiable multiprocessor system. Implements the
##          elaborate{}/validate{}/compose{} callbacks used during system
##          generation. See ip/README.md for the parameter reference and
##          instance-naming contract this component establishes.
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

# _hw.tcl file for GeMRTOS_Multiprocessor
package require -exact qsys 14.0

# Global constants
# MAX_PROCESSORS is the single source of truth for this component's processor
# limit -- it drives NProcessors' allowed range below.
set MAX_PROCESSORS 32
set JTAG_READ_BUFFER_DEPTH 64
set JTAG_WRITE_BUFFER_DEPTH 512
set JTAG_IRQ_THRESHOLD 8
set MIN_ADDRESS_ALIGNMENT 0x8

# ConfiguraciÃ³n centralizada de tipos de procesador
array set PROCESSOR_CONFIGS {
    "Nios II/e (economy)" {
        component          "altera_nios2_gen2"
        implementation     "Tiny"
        max_irqs           32
        has_icache         0
        has_division       0
        has_exception      1
        debug_trace        1
        support_31bit_bypass 1
        icache_ranges      {}
    }
    "Nios II/f (full)" {
        component          "altera_nios2_gen2"
        implementation     "Fast"
        max_irqs           32
        has_icache         1
        has_division       0
        has_exception      1
        debug_trace        1
        support_31bit_bypass 1
        icache_ranges      {"None" "512 Bytes" "1 KB" "2 KB" "4 KB" "8 KB" "16 KB" "32 KB" "64 KB"}
        icache_values      {0 512 1024 2048 4096 8192 16384 32768 65536}
    }
    "Nios V/m" {
        component          "intel_niosv_m"
        implementation     ""
        max_irqs           16
        has_icache         0
        has_division       0
        has_exception      0
        has_pipeline       1
        num_gpr            32
        icache_ranges      {}
    }
    "Nios V/g" {
        component          "intel_niosv_g"
        implementation     ""
        max_irqs           16
        has_icache         1
        has_division       1
        has_exception      0
        enable_fpu         0
        enable_muldiv      1
        data_cache_size    4096
        icache_ranges      {"1 KB" "2 KB" "4 KB" "8 KB" "16 KB"}
        icache_values      {1024 2048 4096 8192 16384}
    }
}

# ParÃ¡metros comunes para todos los procesadores Nios II
array set NIOS2_COMMON_PARAMS {
    bht_ramBlockType                    "Automatic"
    breakOffset                         32
    cdx_enabled                         0
    cpuArchRev                          1
    cpuReset                            1
    data_master_high_performance_paddr_base 0
    data_master_high_performance_paddr_size 0.0
    data_master_paddr_base              0
    data_master_paddr_size              0.0
    dcache_bursts                       "false"
    dcache_numTCDM                      0
    dcache_ramBlockType                 "Automatic"
    dcache_size                         0
    dcache_tagramBlockType              "Automatic"
    dcache_victim_buf_impl              "ram"
    debug_OCIOnchipTrace                "_128"
    debug_assignJtagInstanceID          0
    debug_datatrigger                   0
    debug_debugReqSignals               0
    debug_enabled                       1
    debug_hwbreakpoint                  0
    debug_jtagInstanceID                0
    debug_traceStorage                  "onchip_trace"
    debug_traceType                     "none"
    debug_triggerArming                 1
    dividerType                         "no_div"
    fa_cache_line                       2
    fa_cache_linesize                   0
    flash_instruction_master_paddr_base 0
    flash_instruction_master_paddr_size 0.0
    icache_burstType                    "None"
    icache_numTCIM                      0
    icache_ramBlockType                 "Automatic"
    icache_tagramBlockType              "Automatic"
    instruction_master_high_performance_paddr_base 0
    instruction_master_high_performance_paddr_size 0.0
    instruction_master_paddr_base       0
    instruction_master_paddr_size       0.0
    io_regionbase                       0
    io_regionsize                       0
    master_addr_map                     0
    mmu_TLBMissExcOffset                0
    mmu_TLBMissExcSlave                 "None"
    mmu_autoAssignTlbPtrSz              1
    mmu_enabled                         0
    mmu_processIDNumBits                8
    mmu_ramBlockType                    "Automatic"
    mmu_tlbNumWays                      16
    mmu_tlbPtrSz                        7
    mmu_udtlbNumEntries                 6
    mmu_uitlbNumEntries                 4
    mpu_enabled                         0
    mpu_minDataRegionSize               12
    mpu_minInstRegionSize               12
    mpu_numOfDataRegion                 8
    mpu_numOfInstRegion                 8
    mpu_useLimit                        0
    mpx_enabled                         0
    mul_32_impl                         2
    mul_64_impl                         0
    mul_shift_choice                    0
    ocimem_ramBlockType                 "Automatic"
    ocimem_ramInit                      0
    regfile_ramBlockType                "Automatic"
    register_file_por                   0
    resetrequest_enabled                1
    setting_HBreakTest                  0
    setting_HDLSimCachesCleared         1
    setting_activateMonitors            1
    setting_activateTestEndChecker      0
    setting_activateTrace               1
    setting_allow_break_inst            0
    setting_alwaysEncrypt               1
    setting_asic_add_scan_mode_input    0
    setting_asic_enabled                0
    setting_asic_synopsys_translate_on_off 0
    setting_asic_third_party_synthesis  0
    setting_avalonDebugPortPresent      0
    setting_bhtPtrSz                    8
    setting_bigEndian                   0
    setting_branchpredictiontype        "Dynamic"
    setting_breakslaveoveride           0
    setting_clearXBitsLDNonBypass       1
    setting_dc_ecc_present              1
    setting_disable_tmr_inj             0
    setting_disableocitrace             0
    setting_dtcm_ecc_present            1
    setting_ecc_present                 0
    setting_ecc_sim_test_ports          0
    setting_exportHostDebugPort         0
    setting_exportPCB                   0
    setting_export_large_RAMs           0
    setting_exportdebuginfo             0
    setting_exportvectors               0
    setting_fast_register_read          0
    setting_ic_ecc_present              1
    setting_interruptControllerType     "Internal"
    setting_itcm_ecc_present            1
    setting_mmu_ecc_present             1
    setting_oci_export_jtag_signals     0
    setting_oci_version                 1
    setting_preciseIllegalMemAccessException 0
    setting_removeRAMinit               0
    setting_rf_ecc_present              1
    setting_shadowRegisterSets          0
    setting_showInternalSettings        0
    setting_showUnpublishedSettings     0
    setting_support31bitdcachebypass    1
    setting_tmr_output_disable          0
    setting_usedesignware               0
    shift_rot_impl                      1
    tightly_coupled_data_master_0_paddr_base 0
    tightly_coupled_data_master_0_paddr_size 0.0
    tightly_coupled_data_master_1_paddr_base 0
    tightly_coupled_data_master_1_paddr_size 0.0
    tightly_coupled_data_master_2_paddr_base 0
    tightly_coupled_data_master_2_paddr_size 0.0
    tightly_coupled_data_master_3_paddr_base 0
    tightly_coupled_data_master_3_paddr_size 0.0
    tightly_coupled_instruction_master_0_paddr_base 0
    tightly_coupled_instruction_master_0_paddr_size 0.0
    tightly_coupled_instruction_master_1_paddr_base 0
    tightly_coupled_instruction_master_1_paddr_size 0.0
    tightly_coupled_instruction_master_2_paddr_base 0
    tightly_coupled_instruction_master_2_paddr_size 0.0
    tightly_coupled_instruction_master_3_paddr_base 0
    tightly_coupled_instruction_master_3_paddr_size 0.0
    tmr_enabled                         0
    tracefilename                       ""
    userDefinedSettings                 ""
}

# ParÃ¡metros comunes para todos los procesadores Nios V
array set NIOSV_COMMON_PARAMS {
    enableDebug                         1
    enableDebugReset                    1
    enableECCLite                       0
    useResetReq                         1
}

# ConfiguraciÃ³n de parÃ¡metros especÃ­ficos Nios V/g
array set NIOSV_G_SPECIFIC_PARAMS {
    CUSTOM_OP                           ""
    SUB_OP                              ""
    dtcm1Base                           0
    dtcm1InitFile                       ""
    dtcm1Size                           0
    dtcm2Base                           0
    dtcm2InitFile                       ""
    dtcm2Size                           0
    enableFPU                           0
    enableMulDiv                        1
    funct3                              ""
    funct7_l                            ""
    funct7_u                            ""
    itcm1Base                           0
    itcm1InitFile                       ""
    itcm1Size                           0
    itcm2Base                           0
    itcm2InitFile                       ""
    itcm2Size                           0
    mnemonic                            ""
    opcode                              ""
    peripheralRegionABase               0
    peripheralRegionASize               -2147483648
    peripheralRegionBBase               0
    peripheralRegionBSize               0
}

# Mapeo de tamaÃ±os de cachÃ© a valores numÃ©ricos (usado por ambas familias)
array set ICACHE_SIZE_MAP {
    "None"       0
    "512 Bytes"  512
    "1 KB"       1024
    "2 KB"       2048
    "4 KB"       4096
    "8 KB"       8192
    "16 KB"      16384
    "32 KB"      32768
    "64 KB"      65536
}
    

proc max {a b} {
    return [expr {$a > $b ? $a : $b}]
}


# module properties
set_module_property NAME GeMRTOS_Multiprocessor
set_module_property DISPLAY_NAME "GeMRTOS_Multiprocessor"

# default module properties
set_module_property VERSION 1.0
set_module_property GROUP GeMRTOS
set_module_property DESCRIPTION "GeMRTOS_Multiprocessor"
set_module_property AUTHOR "Ricardo Cayssials"
set_module_property ICON_PATH logoGeMRTOS.png
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true
set_module_property OPAQUE_ADDRESS_MAP false

set_module_property DATASHEET_URL "https://gemrtos.com"

set_module_property COMPOSITION_CALLBACK compose
set_module_property VALIDATION_CALLBACK validate


# 
# parameters
# 


add_parameter NProcessors INTEGER 1
set_parameter_property NProcessors DISPLAY_NAME "Number of Processors"
set_parameter_property NProcessors TYPE INTEGER
set_parameter_property NProcessors UNITS None
set_parameter_property NProcessors ALLOWED_RANGES "1:${MAX_PROCESSORS}"
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

# ############################################################################
# Processor Reset vector memory
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

# ############################################################################
# Processor Instruction Cache
add_parameter icache_size_parameter STRING "None" "Instruction cache size"
set_parameter_property icache_size_parameter DISPLAY_NAME "Instruction cache size"
set_parameter_property icache_size_parameter UNITS None
set_parameter_property icache_size_parameter ALLOWED_RANGES {"None" "512 Bytes" "1 KB" "2 KB" "4 KB" "8 KB" "16 KB" "32 KB" "64 KB"}
add_display_item "Parameters" icache_size_parameter PARAMETER
set_parameter_property icache_size_parameter HDL_PARAMETER false



# ############################################################################
# Processor Pipeline for Nios V/m
add_parameter pipelineArch_parameter STRING  "True"
set_parameter_property pipelineArch_parameter DISPLAY_NAME "Enable pipeline in CPU (Nios V/m only)"
set_parameter_property pipelineArch_parameter UNITS None
set_parameter_property pipelineArch_parameter HDL_PARAMETER false
set_parameter_property pipelineArch_parameter ALLOWED_RANGES {"True" "False"}
add_display_item "Parameters" pipelineArch_parameter PARAMETER
set_parameter_property pipelineArch_parameter DERIVED false


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
# Input clock for processors
add_interface clk_processors clock sink
set_interface_property clk_processors clockRate 0
set_interface_property clk_processors ENABLED true

add_display_item "" "Parameters" group tab
add_display_item "" "Information" group tab

add_parameter GeMRTOS_Frequency INTEGER  
set_parameter_property GeMRTOS_Frequency SYSTEM_INFO {CLOCK_RATE clk_processors}
set_parameter_property GeMRTOS_Frequency DISPLAY_NAME "Processor frequency"
set_parameter_property GeMRTOS_Frequency UNITS Hertz
add_display_item "Information" GeMRTOS_Frequency PARAMETER
set_parameter_property GeMRTOS_Frequency HDL_PARAMETER false

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

add_parameter exceptionAbsolute INTEGER 
set_parameter_property exceptionAbsolute DISPLAY_NAME "Exception Absolute memory base"
add_display_item "Information" exceptionAbsolute PARAMETER
set_parameter_property exceptionAbsolute "DISPLAY_HINT"  "hexadecimal"
set_parameter_property exceptionAbsolute HDL_PARAMETER false
set_parameter_property exceptionAbsolute DERIVED true

  
  


# ############################################################################
# External reset input
add_interface reset_in reset sink

# ############################################################################
# Interface for processor 1 and getting the bus width for external memory registers
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

# GeMRTOS version 
add_parameter GEMRTOS_VERSION_INTEGER INTEGER 1
set_parameter_property GEMRTOS_VERSION_INTEGER DERIVED true
set_parameter_property GEMRTOS_VERSION_INTEGER DISPLAY_NAME  "GeMRTOS version"
set_parameter_property GEMRTOS_VERSION_INTEGER UNITS None
add_display_item "Information" GEMRTOS_VERSION_INTEGER PARAMETER
set_parameter_property GEMRTOS_VERSION_INTEGER HDL_PARAMETER true


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

# Get the interrupts connected to the GRTOS
add_parameter GeMRTOS_INTERRUPTS LONG 
set_parameter_property GeMRTOS_INTERRUPTS DISPLAY_NAME "GeMRTOS interrupts"
set_parameter_property GeMRTOS_INTERRUPTS SYSTEM_INFO {INTERRUPTS_USED gemrtos_dirq_input}    
add_display_item "Information" GeMRTOS_INTERRUPTS PARAMETER 





proc validate_bus_width {bus_width} {
    if { $bus_width == 0 } {
        send_message error "External memory device must be connected to processor buses"
        return 0
    }
    if { $bus_width < 1 || $bus_width > 64 } {
        send_message error "Invalid bus width: $bus_width (valid range: 1-64)"
        return 0
    }
    return 1
}


proc calculate_memory_span {bus_width} {
    set total_bytes [expr {2**$bus_width}]
    
    # Tabla de conversiÃ³n
    set units [list \
        [list 10 "BYTES" 1] \
        [list 20 "KILOBYTES" 1024] \
        [list 30 "MEGABYTES" 1048576] \
        [list 64 "GIGABYTES" 1073741824] \
    ]
    
    foreach unit_info $units {
        lassign $unit_info threshold unit_name divisor
        if { $bus_width < $threshold } {
            set span [expr {$total_bytes / $divisor}]
            return [list $span $unit_name]
        }
    }
    
    # Por defecto GIGABYTES
    return [list [expr {$total_bytes / 1073741824}] "GIGABYTES"]
}

proc get_project_directory {} {
    set module_dir [get_module_property MODULE_DIRECTORY]
    set components [file split [file normalize $module_dir]]
    set ip_index [lsearch -exact $components "ip"]
    
    if {$ip_index == -1} {
        send_message error "Component must be located in an 'ip' subdirectory"
        return ""
    }
    
    if {$ip_index == 0} {
        send_message error "Invalid directory structure: 'ip' cannot be root"
        return ""
    }
    
    set project_components [lrange $components 0 [expr {$ip_index - 1}]]
    return [file normalize [file join {*}$project_components]]
}


proc elaborate { } {

}

proc validate { } {
    global MAX_PROCESSORS
    global JTAG_READ_BUFFER_DEPTH
    global JTAG_WRITE_BUFFER_DEPTH
    global JTAG_IRQ_THRESHOLD
    global MIN_ADDRESS_ALIGNMENT
    
	set error_stop 0
    
    # Validar NProcessors
    set nproc [get_parameter_value NProcessors]
    if { $nproc < 1 || $nproc > $MAX_PROCESSORS } {
        send_message error "Invalid number of processors: $nproc"
        return
    }
    
    # Validar frecuencia
    set freq [get_parameter_value GeMRTOS_Frequency]
    if { $freq <= 0 } {
        send_message error "Invalid clock frequency: $freq Hz"
        return
    }
    
    # Validar offsets
    set reset_offset [get_parameter_value resetOffset]
    if { $reset_offset < 0 } {
        send_message error "Reset offset cannot be negative"
        return
    }    
    
	# Get the information parameters of the GUI
    set bus_width [get_parameter_value BUS_WIDTH]
    if { ![validate_bus_width $bus_width] } {
        return
    } else {
        lassign [calculate_memory_span $bus_width] span unit
        set_parameter_value EXTERNAL_MEMORY_SPAN $span
        set_parameter_value EXTERNAL_MEMORY_UNIT $unit
        
		# if { $bus_width < 10 } {
		# 	set bus_width1 [expr 2**[get_parameter_value BUS_WIDTH]]
		# 	set_parameter_value EXTERNAL_MEMORY_SPAN ${bus_width1}
		# 	set_parameter_value EXTERNAL_MEMORY_UNIT "BYTES"
		# } elseif { $bus_width < 20 } {
		# 	set bus_width1 [expr (2**[get_parameter_value BUS_WIDTH]) / 1024]
		# 	set_parameter_value EXTERNAL_MEMORY_SPAN ${bus_width1}
		# 	set_parameter_value EXTERNAL_MEMORY_UNIT "KILOBYTES"
		# } elseif { $bus_width < 30 } {
		# 	set bus_width1 [expr (2**[get_parameter_value BUS_WIDTH]) / 1048576]
		# 	set_parameter_value EXTERNAL_MEMORY_SPAN ${bus_width1}
		# 	set_parameter_value EXTERNAL_MEMORY_UNIT "MEGABYTES"
		# } else {
		# 	set bus_width1 [expr (2**[get_parameter_value BUS_WIDTH]) / 1073741824]
		# 	set_parameter_value EXTERNAL_MEMORY_SPAN ${bus_width1}
		# 	set_parameter_value EXTERNAL_MEMORY_UNIT "GIGABYTES"
		# }
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

    
    
	set folder [get_module_property MODULE_DIRECTORY]
	set directory_component [ lrange [file split $folder] end-0 end-0]
	if {$directory_component != "GeMRTOS_Multiprocessor"} {
		send_message Warning "GeMRTOS_Multiprocessor component directory is named '${directory_component}', not 'GeMRTOS_Multiprocessor' â€” this is supported, but gemrtos_build.bat assumes the default name unless invoked with a matching -ip path. See ip/README.md."
	}

	set directory_component [ lrange [file split $folder] end-1 end-1]
	if {$directory_component != "ip"} {
		send_message Error "Error: GeMRTOS_Multiprocessor must be placed directly inside an 'ip' subdirectory of your Quartus project (found '${directory_component}' instead). See ip/README.md for the required layout."
		set error_stop 1
	}

    # Get the project folder as the parent of the ip folder
    set components [file split $folder]
    set index [lsearch -exact $components "ip"]
    
    set project_dir [get_project_directory]
    if {$project_dir eq ""} {
        return ;# Error ya reportado
    }



	
	if {($error_stop == 0)} {
        set gemrtos_buildsh [file join $project_dir "gemrtos_build.sh" ]
        set gemrtos_buildbat [file join $project_dir "gemrtos_build.bat" ]
        if { ($Type_of_Processor == "Nios II/e (economy)") || ($Type_of_Processor == "Nios II/f (full)") } {
            file delete -force $gemrtos_buildbat
            file copy -force "./gemrtos_build.sh" $gemrtos_buildsh
            send_message Info "REMEMBER using Nios 2 Command Shell to execute \"bash gemrtos_build.sh\""
        }
        if { ($Type_of_Processor == "Nios V/m") || ($Type_of_Processor == "Nios V/g") } {
            file delete -force $gemrtos_buildsh
            file copy -force "./gemrtos_build.bat" $gemrtos_buildbat
            send_message Info "REMEMBER using Nios V Command Shell to execute \gemrtos_build.bat\""
        }        
	}

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
	

	set exceptionSlave             [ get_parameter_value exceptionSlave ]
	set exceptionOffset            [ get_parameter_value exceptionOffset ]
	
	set_parameter_value exceptionAbsolute 0

    if { ($Type_of_Processor != "Nios II/e (economy)") && ($Type_of_Processor != "Nios II/f (full)") } {
        lappend slaves_name "None"
    }

	foreach i $address_map_dec {
		array set info $i
		lappend slaves_name "$info(name)"
		if { ( $exceptionSlave == $info(name) ) } {
			set exceptionAbsolute [expr { $info(start) + $exceptionOffset } ]
			set_parameter_value exceptionAbsolute $exceptionAbsolute
		}
		# send_message Error "Connected to slave name=$info(name) start=$info(start) end=$info(end)"
	}
	set_parameter_property  exceptionSlave     "ALLOWED_RANGES" $slaves_name
	

    # Create the scripts to create the system 
    if { ($Type_of_Processor == "Nios V/m") || ($Type_of_Processor == "Nios V/g") } {
    
        set bsp_niosv_settings [file join $project_dir "bsp_niosv_settings.tcl" ]
        file delete -force $bsp_niosv_settings
    	set fp [open $bsp_niosv_settings w+]
            fconfigure $fp -translation lf
            puts $fp "set_setting hal.sys_clk_timer {none} "
            puts $fp "set_setting hal.timestamp_timer {none} "	
            puts $fp "set_setting hal.enable_reduced_device_drivers true "
            puts $fp "set_setting hal.enable_instruction_related_exceptions_api true "		
            puts $fp "set_setting hal.enable_c_plus_plus true "
            puts $fp "set_setting hal.linker.allow_code_at_reset {false} "
            puts $fp "set_setting hal.stderr GEMRTOS_STDERR "
            puts $fp "set_setting hal.stdin GEMRTOS_STDIO "
            puts $fp "set_setting hal.stdout GEMRTOS_STDIO "
            puts $fp "GEMRTOS_LWIP"
            puts $fp " "
        close $fp

        set gemrtos_config [file join $project_dir "processor_set.bat" ]
        file delete -force $gemrtos_config
        set fp [open $gemrtos_config w+]
            puts $fp "rem NOTE: this file is created automatically. Do not execute it."
            puts $fp "set QSYS_PROC1=GEMRTOS_MAIN_PROCESSOR "
            puts $fp "set QSYS_PROJECT_NAME=GEMRTOS_PROJECT_NAME "
            puts $fp "set STDERR_NAME=GEMRTOS_STDERR "
            puts $fp "set STDIO_NAME=GEMRTOS_STDIO "
            puts $fp " "
        close $fp
    }
}

proc compose { } {

    global MAX_PROCESSORS
    global JTAG_READ_BUFFER_DEPTH
    global JTAG_WRITE_BUFFER_DEPTH
    global JTAG_IRQ_THRESHOLD
    global MIN_ADDRESS_ALIGNMENT
    global PROCESSOR_CONFIGS
    global NIOS2_COMMON_PARAMS
    global NIOSV_COMMON_PARAMS
    global NIOSV_G_SPECIFIC_PARAMS
    global ICACHE_SIZE_MAP

    # Parameters from GUI
    set Processors [get_parameter_value NProcessors]
    set Type_of_Processor [get_parameter_value Processor_type]
	
	set resetSlave             [ get_parameter_value resetSlave ]
	set resestAbsolute         [ get_parameter_value resestAbsolute ]
	set exceptionSlave             [ get_parameter_value exceptionSlave ]
	set exceptionAbsolute         [ get_parameter_value exceptionAbsolute ]	
	
	if { ($Type_of_Processor == "Nios II/e (economy)") || ($Type_of_Processor == "Nios II/f (full)") } {
		set proc_clock "clk"
		set proc_reset "reset"
        set proc_rstreq_intf "cpu_resetrequest_conduit"
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
        set proc_rstreq_intf "cpu_resetreq"
		set data_manager "data_manager"
		set instruction_manager "instruction_manager"
		set debug_agent_reset_request "dbg_reset_out"
		set irq_rx "platform_irq_rx"
		set debug_mem_slave "dm_agent"
		set MAX_PROC_IRQs 16
	}	

    set ClockFrequency [get_parameter_value GeMRTOS_Frequency]
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

	# Processor buses
	set independet_buses [get_parameter_value INDEPENDENT_EXTERNAL_PROCESSOR_BUSES]

    
    # Internal clock for devices in GRTOS
    add_instance clock_bridge_0 altera_clock_bridge
    set_instance_parameter_value clock_bridge_0 {NUM_CLOCK_OUTPUTS} {1}

    
    
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
    add_instance grtos_0 gemrtos_controller
    set_instance_parameter_value grtos_0 {NProcessors} [get_parameter_value NProcessors]
    set_instance_parameter_value grtos_0 {PreScale} $PreScale

    set_instance_parameter_value grtos_0 {PROCESSOR_TYPE} [get_parameter_value Processor_type]
    set_instance_parameter_value grtos_0 {INSTRUCTION_CACHE_SIZE} [get_parameter_value icache_size_parameter]
	set_instance_parameter_value grtos_0 {INSTRUCTION_CACHE_BURST} Disable
    set_instance_parameter_value grtos_0 {ENABLE_HPS_MAP_ACCESS} [get_parameter_value ENABLE_HPS_MAP_ACCESS]
    set_instance_parameter_value grtos_0 {NIOS_CLOCK_FREQUENCY} [get_parameter_value GeMRTOS_Frequency]
    set_instance_parameter_value grtos_0 {GEMRTOS_VERSION_INTEGER} [get_parameter_value  GEMRTOS_VERSION_INTEGER]

    set_instance_parameter_value grtos_0 {BUS_WIDTH} [get_parameter_value BUS_WIDTH]
    set_instance_parameter_value grtos_0 {EXTERNAL_MEMORY_SPAN} [get_parameter_value EXTERNAL_MEMORY_SPAN]
    set_instance_parameter_value grtos_0 {EXTERNAL_MEMORY_UNIT} [get_parameter_value EXTERNAL_MEMORY_UNIT]
	set_instance_parameter_value grtos_0 {INDEPENDENT_EXTERNAL_PROCESSOR_BUSES} [get_parameter_value INDEPENDENT_EXTERNAL_PROCESSOR_BUSES]
	set_instance_parameter_value grtos_0 {INCLUDE_JTAG_UART_STDDIO} [get_parameter_value INCLUDE_JTAG_UART_STDDIO]
	set_instance_parameter_value grtos_0 {INCLUDE_JTAG_UART_STDERR} [get_parameter_value INCLUDE_JTAG_UART_STDERR]
	set_instance_parameter_value grtos_0 {INCLUDE_JTAG_UART_PROCESSOR} [get_parameter_value INCLUDE_JTAG_UART_PROCESSOR]

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
    }
    # ###########################################################    

	# first interrupt is from GeMRTOS controller
	set IntInterrupts 1
	# Include STDIO, STDERR and processor JTAG UART and respective interrupts
	set incl_STDIO [get_parameter_value INCLUDE_JTAG_UART_STDDIO]
	if {($incl_STDIO == true)} { set IntInterrupts [expr {($IntInterrupts) + 1}] }
	
	set incl_STDERR [get_parameter_value INCLUDE_JTAG_UART_STDERR]
	if {($incl_STDERR == true)} { set IntInterrupts [expr {($IntInterrupts) + 1}] }
	
	set incl_STDPROC [get_parameter_value INCLUDE_JTAG_UART_PROCESSOR]
	if {($incl_STDPROC == true)} { set IntInterrupts [expr {($IntInterrupts) + ($Processors)}] }
	
	send_message Info "Number of internal interrupts is $IntInterrupts"
    
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
    set_instance_parameter_value nios_avalon_monitor {AVALON_DATA_FIFO_DEPTH} {4}
    set_instance_parameter_value nios_avalon_monitor {DATA_WIDTH} {32}
    set_instance_parameter_value nios_avalon_monitor {LINEWRAPBURSTS} {0}
    set_instance_parameter_value nios_avalon_monitor {MAX_BURST_SIZE} {1}
    set_instance_parameter_value nios_avalon_monitor {MAX_PENDING_RESPONSES} {32}
    set_instance_parameter_value nios_avalon_monitor {NProcessors} $Processors
    set_instance_parameter_value nios_avalon_monitor {SYMBOL_WIDTH} {8}
    # CLOCK and RESET
    add_connection clock_bridge_0.out_clk nios_avalon_monitor.clk clock
    add_connection reset_bridge_0.out_reset nios_avalon_monitor.reset reset    
    

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

    

    # Processors
    # Processors - table-driven instantiation, replacing the hardcoded
    # per-family loop archived above. Behavior differences relative to
    # that archived block:
    #  - pipelineArch (Nios V/m) is now actually substituted: the archived block wrote
    #    the literal braced string {$pipelineArch}, which Tcl never substitutes, so the
    #    instance parameter was always set to the 13-character string "$pipelineArch"
    #    instead of 0/1. Needs on-board verification for Nios V/m specifically.
    # Gaps found and closed while completing the draft (all data-only, no logic change):
    #  - NIOS2_COMMON_PARAMS was missing data_master_high_performance_paddr_base/_size,
    #    dividerType, instruction_master_high_performance_paddr_base/_size (added above).
    #  - cpuID and breakSlave are per-instance (depend on ${i}/$instance_name) and were
    #    missing from this block entirely; added under the Nios II branch only, matching
    #    where the archived block set them (Nios V never received cpuID/breakSlave).
    #  - debug_enabled (Nios II) is already provided by the NIOS2_COMMON_PARAMS foreach;
    #    dropped the redundant explicit set that had been misplaced as "common to all
    #    processors" (Nios V uses enableDebug, not debug_enabled -- setting the latter on
    #    a Nios V instance would target a parameter that component likely doesn't have).
    #  - Dropped redundant explicit enableFPU/enableMulDiv sets on Nios V/g: both are
    #    already provided by the NIOSV_G_SPECIFIC_PARAMS foreach with identical values.
    for {set i 1} {$i <= $Processors} {incr i} {

        # Obtener configuraciÃ³n del procesador
        array set proc_config $PROCESSOR_CONFIGS($Type_of_Processor)

        set instance_name "gemrtos_proc_${i}"

        # Crear instancia del componente
        add_instance $instance_name $proc_config(component)

        # =========================================================
        # CONFIGURACIÃ“N ESPECÃFICA NIOS II
        # =========================================================
        if { ($Type_of_Processor == "Nios II/e (economy)") || ($Type_of_Processor == "Nios II/f (full)") } {

            # Aplicar parÃ¡metros comunes Nios II
            foreach {param value} [array get NIOS2_COMMON_PARAMS] {
                set_instance_parameter_value $instance_name $param $value
            }

            # ParÃ¡metros que dependen del Ã­ndice de instancia (no tabulables)
            set_instance_parameter_value $instance_name {cpuID} ${i}
            set_instance_parameter_value $instance_name {breakSlave} "$instance_name.jtag_debug_module"

            # ParÃ¡metros especÃ­ficos por variante
            set_instance_parameter_value $instance_name {impl} $proc_config(implementation)

            # Configurar cachÃ© de instrucciones si estÃ¡ soportado
            if {$proc_config(has_icache)} {
                if {[info exists ICACHE_SIZE_MAP($icache_size)]} {
                    set_instance_parameter_value $instance_name {icache_size} $ICACHE_SIZE_MAP($icache_size)
                } else {
                    set_instance_parameter_value $instance_name {icache_size} {0}
                }
            } else {
                set_instance_parameter_value $instance_name {icache_size} {0}
            }

            # Configurar support31bitdcachebypass segÃºn configuraciÃ³n
            set_instance_parameter_value $instance_name {setting_support31bitdcachebypass} $proc_config(support_31bit_bypass)
        }

        # =========================================================
        # CONFIGURACIÃ“N ESPECÃFICA NIOS V
        # =========================================================
        if { ($Type_of_Processor == "Nios V/m") || ($Type_of_Processor == "Nios V/g") } {

            # Aplicar parÃ¡metros comunes Nios V
            foreach {param value} [array get NIOSV_COMMON_PARAMS] {
                set_instance_parameter_value $instance_name $param $value
            }

            # ConfiguraciÃ³n Nios V/m
            if { $Type_of_Processor == "Nios V/m" } {
                set_instance_parameter_value $instance_name {numGpr} $proc_config(num_gpr)
                set_instance_parameter_value $instance_name {pipelineArch} $pipelineArch
                send_message Debug "Pipeline of processor ${i} is ${pipelineArch}"
            }

            # ConfiguraciÃ³n Nios V/g
            if { $Type_of_Processor == "Nios V/g" } {

                # Aplicar parÃ¡metros especÃ­ficos Nios V/g
                foreach {param value} [array get NIOSV_G_SPECIFIC_PARAMS] {
                    set_instance_parameter_value $instance_name $param $value
                }

                # Configurar data cache
                set_instance_parameter_value $instance_name {dataCacheSize} $proc_config(data_cache_size)

                # Configurar instruction cache si estÃ¡ soportado
                if {$proc_config(has_icache)} {
                    if {[info exists ICACHE_SIZE_MAP($icache_size)]} {
                        set_instance_parameter_value $instance_name {instCacheSize} $ICACHE_SIZE_MAP($icache_size)
                    } else {
                        set_instance_parameter_value $instance_name {instCacheSize} {0}
                    }
                } else {
                    set_instance_parameter_value $instance_name {instCacheSize} {0}
                }
            }
        }

        # =========================================================
        # CONFIGURACIÃ“N DE VECTORES DE RESET Y EXCEPCIÃ“N
        # =========================================================
        if {$i == 1} {
            # Procesador principal (CPU 1)
            if { ($resetSlave == "Internal GeMRTOS (default)") } {
                set_instance_parameter_value $instance_name {resetSlave} {rst1_and_exception_memory.s1}
                set_instance_parameter_value $instance_name {resetOffset} {0}
            } else {
                set_instance_parameter_value $instance_name {resetSlave} {Absolute}
                set_instance_parameter_value $instance_name {resetOffset} $resestAbsolute
            }
        } else {
            # Procesadores secundarios (CPU 2..N)
            set_instance_parameter_value $instance_name {resetSlave} {grtos_0.s_GMemory}
            set_instance_parameter_value $instance_name {resetOffset} {0}
        }

        # Configurar vector de excepciÃ³n (solo Nios II)
        if {$proc_config(has_exception)} {
            if { ($resetSlave == "Internal GeMRTOS (default)") } {
                set_instance_parameter_value $instance_name {exceptionSlave} {rst1_and_exception_memory.s1}
                set_instance_parameter_value $instance_name {exceptionOffset} {32}
            } else {
                set_instance_parameter_value $instance_name {exceptionSlave} {Absolute}
                set_instance_parameter_value $instance_name {exceptionOffset} $exceptionAbsolute
            }
        }

        # =========================================================
        # CONFIGURACIÃ“N DE CONEXIONES (CLOCK Y RESET)
        # =========================================================
        add_connection clock_bridge_0.out_clk $instance_name.$proc_clock clock
        add_connection reset_bridge_0.out_reset $instance_name.$proc_reset reset

        # Self-reset connection para Nios V
        if { ($Type_of_Processor == "Nios V/m") || ($Type_of_Processor == "Nios V/g") } {
            add_connection $instance_name.dbg_reset_out $instance_name.ndm_reset_in reset
        }
    }

   
    # JTAG UARTs
    for {set i 0} {$i <= $Processors + 1} {incr i} {
		if {(($incl_STDIO == true) && ($i == 0)) || (($incl_STDERR == true) && ($i == 1)) || (($incl_STDPROC == true) && ($i > 1))} {
			add_instance jtag_uart_${i} altera_avalon_jtag_uart
			set_instance_parameter_value jtag_uart_${i} {allowMultipleConnections} {1}
			set_instance_parameter_value jtag_uart_${i} {hubInstanceID} {0}
			set_instance_parameter_value jtag_uart_${i} {readBufferDepth} $JTAG_READ_BUFFER_DEPTH
			set_instance_parameter_value jtag_uart_${i} {readIRQThreshold} $JTAG_IRQ_THRESHOLD
			set_instance_parameter_value jtag_uart_${i} {simInputCharacterStream} {}
			set_instance_parameter_value jtag_uart_${i} {simInteractiveOptions} {NO_INTERACTIVE_WINDOWS}
			set_instance_parameter_value jtag_uart_${i} {useRegistersForReadBuffer} {0}
			set_instance_parameter_value jtag_uart_${i} {useRegistersForWriteBuffer} {0}
			set_instance_parameter_value jtag_uart_${i} {useRelativePathForSimFile} {0}
			set_instance_parameter_value jtag_uart_${i} {writeBufferDepth} $JTAG_WRITE_BUFFER_DEPTH
			set_instance_parameter_value jtag_uart_${i} {writeIRQThreshold} $JTAG_IRQ_THRESHOLD 
			# CLOCK and RESET
			add_connection clock_bridge_0.out_clk jtag_uart_${i}.clk clock
			add_connection reset_bridge_0.out_reset jtag_uart_${i}.reset reset
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
	
	# Internal Bridge
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

	# Internal Bridge
    add_instance GRTOS_Avalon_Bridge_External GRTOS_Avalon_Bridge
    set_instance_parameter_value GRTOS_Avalon_Bridge_External {ADDRESS_UNITS} {SYMBOLS}
    set_instance_parameter_value GRTOS_Avalon_Bridge_External {ADDRESS_WIDTH} $Bridge_Address_Width
    set_instance_parameter_value GRTOS_Avalon_Bridge_External {DATA_WIDTH} {32}
    set_instance_parameter_value GRTOS_Avalon_Bridge_External {LINEWRAPBURSTS} {0}
    set_instance_parameter_value GRTOS_Avalon_Bridge_External {MAX_BURST_SIZE} {1}
    set_instance_parameter_value GRTOS_Avalon_Bridge_External {MAX_PENDING_RESPONSES} {32}
    set_instance_parameter_value GRTOS_Avalon_Bridge_External {NProcessors} $Ports
    set_instance_parameter_value GRTOS_Avalon_Bridge_External {SYMBOL_WIDTH} {8} 
    # CLOCK and RESET
    add_connection clock_bridge_0.out_clk GRTOS_Avalon_Bridge_External.clk clock
    add_connection reset_bridge_0.out_reset GRTOS_Avalon_Bridge_External.reset reset

   
    # Reset Bridge
    add_instance reset_bridge_1 altera_reset_bridge
    set_instance_parameter_value reset_bridge_1 {ACTIVE_LOW_RESET} {1}
    set_instance_parameter_value reset_bridge_1 {NUM_RESET_OUTPUTS} {1}
    set_instance_parameter_value reset_bridge_1 {SYNCHRONOUS_EDGES} {deassert}
    set_instance_parameter_value reset_bridge_1 {USE_RESET_REQUEST} {0}
    # CLOCK
    add_connection clock_bridge_0.out_clk reset_bridge_1.clk clock
    
    ################################ DEC 2021
    for {set i 1} {$i <= $Processors} {incr i} {
		if {($independet_buses == true) || ($i == 1)} {
			add_connection GRTOS_Avalon_Bridge_1.m${i} GRTOS_Avalon_Bridge_External.s${i} avalon
			set_connection_parameter_value GRTOS_Avalon_Bridge_1.m${i}/GRTOS_Avalon_Bridge_External.s${i} arbitrationPriority {1}
			set_connection_parameter_value GRTOS_Avalon_Bridge_1.m${i}/GRTOS_Avalon_Bridge_External.s${i} baseAddress {0x0000}
			set_connection_parameter_value GRTOS_Avalon_Bridge_1.m${i}/GRTOS_Avalon_Bridge_External.s${i} defaultConnection {0}			
		} else {
			add_connection GRTOS_Avalon_Bridge_1.m${i} GRTOS_Avalon_Bridge_External.s1 avalon
			set_connection_parameter_value GRTOS_Avalon_Bridge_1.m${i}/GRTOS_Avalon_Bridge_External.s1 arbitrationPriority {1}
			set_connection_parameter_value GRTOS_Avalon_Bridge_1.m${i}/GRTOS_Avalon_Bridge_External.s1 baseAddress {0x0000}
			set_connection_parameter_value GRTOS_Avalon_Bridge_1.m${i}/GRTOS_Avalon_Bridge_External.s1 defaultConnection {0}	
		}
    }
	
    # HPS internal access
    if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
	
		# HPS Bridge
		add_instance GRTOS_Avalon_Bridge_HPS GRTOS_Avalon_Bridge
		set_instance_parameter_value GRTOS_Avalon_Bridge_HPS {ADDRESS_UNITS} {SYMBOLS}
		set_instance_parameter_value GRTOS_Avalon_Bridge_HPS {ADDRESS_WIDTH} $Bridge_Address_Width
		set_instance_parameter_value GRTOS_Avalon_Bridge_HPS {DATA_WIDTH} {32}
		set_instance_parameter_value GRTOS_Avalon_Bridge_HPS {LINEWRAPBURSTS} {0}
		set_instance_parameter_value GRTOS_Avalon_Bridge_HPS {MAX_BURST_SIZE} {1}
		set_instance_parameter_value GRTOS_Avalon_Bridge_HPS {MAX_PENDING_RESPONSES} {32}
		set_instance_parameter_value GRTOS_Avalon_Bridge_HPS {NProcessors} {1}
		set_instance_parameter_value GRTOS_Avalon_Bridge_HPS {SYMBOL_WIDTH} {8} 
		# CLOCK and RESET
		add_connection clock_bridge_0.out_clk GRTOS_Avalon_Bridge_HPS.clk clock
		add_connection reset_bridge_0.out_reset GRTOS_Avalon_Bridge_HPS.reset reset
	
		add_connection GRTOS_Avalon_Bridge_1.m${Ports} GRTOS_Avalon_Bridge_HPS.s1 avalon
		set_connection_parameter_value GRTOS_Avalon_Bridge_1.m${Ports}/GRTOS_Avalon_Bridge_HPS.s1 arbitrationPriority {1}
		set_connection_parameter_value GRTOS_Avalon_Bridge_1.m${Ports}/GRTOS_Avalon_Bridge_HPS.s1 baseAddress {0x0000}
		set_connection_parameter_value GRTOS_Avalon_Bridge_1.m${Ports}/GRTOS_Avalon_Bridge_HPS.s1 defaultConnection {0}	


    }

    # Connections
    ##########################################

    # connections and connection parameters
    add_connection nios_avalon_monitor.bus_internal grtos_0.bus_internal conduit
    set_connection_parameter_value nios_avalon_monitor.bus_internal/grtos_0.bus_internal endPort {}
    set_connection_parameter_value nios_avalon_monitor.bus_internal/grtos_0.bus_internal endPortLSB {0}
    set_connection_parameter_value nios_avalon_monitor.bus_internal/grtos_0.bus_internal startPort {}
    set_connection_parameter_value nios_avalon_monitor.bus_internal/grtos_0.bus_internal startPortLSB {0}
    set_connection_parameter_value nios_avalon_monitor.bus_internal/grtos_0.bus_internal width {0}

    
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
			set ancho [get_instance_interface_property gemrtos_proc_${i} ${debug_mem_slave} addressSpan ]
			send_message Debug "Span of gemrtos_proc_${i} debug_mem_slave is $ancho, base address is $BaseAddress"
			set BaseAddress [expr {$BaseAddress + $ancho}]
		}
        
        # ##################################################
        for {set i 1} {$i <= $Processors} {incr i} {
			add_connection master_0.master gemrtos_proc_${i}.timer_sw_agent avalon
			set_connection_parameter_value master_0.master/gemrtos_proc_${i}.timer_sw_agent arbitrationPriority {1}
			set_connection_parameter_value master_0.master/gemrtos_proc_${i}.timer_sw_agent baseAddress $BaseAddress
			set_connection_parameter_value master_0.master/gemrtos_proc_${i}.timer_sw_agent defaultConnection {0}
		
			add_connection nios_avalon_monitor.m${i} gemrtos_proc_${i}.timer_sw_agent avalon
			set_connection_parameter_value nios_avalon_monitor.m${i}/gemrtos_proc_${i}.timer_sw_agent arbitrationPriority {1}
			set address_base [ get_connection_parameter_value master_0.master/gemrtos_proc_${i}.timer_sw_agent baseAddress ]
			set_connection_parameter_value nios_avalon_monitor.m${i}/gemrtos_proc_${i}.timer_sw_agent baseAddress $address_base
			set_connection_parameter_value nios_avalon_monitor.m${i}/gemrtos_proc_${i}.timer_sw_agent defaultConnection {0}
			set ancho [get_instance_interface_property gemrtos_proc_${i} timer_sw_agent addressSpan ]
			send_message Debug "Span of gemrtos_proc_${i} timer_sw_agent is $ancho, base address is $BaseAddress"
			set BaseAddress [expr {$BaseAddress + $ancho}]
		}
    # add_connection intel_niosv_g_0.data_manager intel_niosv_g_0.timer_sw_agent avalon
    # set_connection_parameter_value intel_niosv_g_0.data_manager/intel_niosv_g_0.timer_sw_agent arbitrationPriority {1}
    # set_connection_parameter_value intel_niosv_g_0.data_manager/intel_niosv_g_0.timer_sw_agent baseAddress {0x04010000}
    # set_connection_parameter_value intel_niosv_g_0.data_manager/intel_niosv_g_0.timer_sw_agent defaultConnection {0}        
        # ##################################################
        
	}

	
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
			set ancho [get_instance_interface_property gemrtos_proc_${i} ${debug_mem_slave} addressSpan ]
			send_message Debug "Span of gemrtos_proc_${i} debug_mem_slave is $ancho, base address is $BaseAddress"
			set BaseAddress [expr {$BaseAddress + $ancho}]
		}
	}
    
	if { ($resetSlave == "Internal GeMRTOS (default)" ) } {
		# Connection of rst1_and_exception_memory at $BaseAddress span 0x2000
		add_connection master_0.master rst1_and_exception_memory.s1 avalon
		set_connection_parameter_value master_0.master/rst1_and_exception_memory.s1 arbitrationPriority {1}
		set_connection_parameter_value master_0.master/rst1_and_exception_memory.s1 baseAddress $BaseAddress
		set_connection_parameter_value master_0.master/rst1_and_exception_memory.s1 defaultConnection {0}
        
		set address_base [ get_connection_parameter_value master_0.master/rst1_and_exception_memory.s1 baseAddress ]        
		for {set i 1} {$i <= $Processors} {incr i} {
			add_connection nios_avalon_monitor.m$i rst1_and_exception_memory.s1 avalon
			set_connection_parameter_value nios_avalon_monitor.m${i}/rst1_and_exception_memory.s1 arbitrationPriority {1}
			set_connection_parameter_value nios_avalon_monitor.m${i}/rst1_and_exception_memory.s1 baseAddress $address_base
			set_connection_parameter_value nios_avalon_monitor.m${i}/rst1_and_exception_memory.s1 defaultConnection {0}
		}
		# HPS internal access
		if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
			add_connection mm_clock_crossing_bridge_0.m0 rst1_and_exception_memory.s1 avalon
			set_connection_parameter_value mm_clock_crossing_bridge_0.m0/rst1_and_exception_memory.s1 arbitrationPriority {1}
			set_connection_parameter_value mm_clock_crossing_bridge_0.m0/rst1_and_exception_memory.s1 baseAddress $address_base
			set_connection_parameter_value mm_clock_crossing_bridge_0.m0/rst1_and_exception_memory.s1 defaultConnection {0}        
		}
		set ancho [get_instance_interface_property rst1_and_exception_memory s1 addressSpan ]
		send_message Debug "Span of rst1_and_exception_memory s1 is $ancho, base address is $BaseAddress"
		set BaseAddress [expr {$BaseAddress + $ancho}]
    }


    # Connection of grtos_0.s_GMemory avalon at $BaseAddress span 0x200 
    add_connection master_0.master grtos_0.s_GMemory avalon
    set_connection_parameter_value master_0.master/grtos_0.s_GMemory arbitrationPriority {1}
    set_connection_parameter_value master_0.master/grtos_0.s_GMemory baseAddress $BaseAddress
    set_connection_parameter_value master_0.master/grtos_0.s_GMemory defaultConnection {0}
    
    set address_base [ get_connection_parameter_value master_0.master/grtos_0.s_GMemory baseAddress ]
    for {set i 1} {$i <= $Processors} {incr i} {
        add_connection nios_avalon_monitor.m${i} grtos_0.s_GMemory avalon
        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_GMemory arbitrationPriority {1}
        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_GMemory baseAddress $address_base
        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_GMemory defaultConnection {0}        
    }
    # HPS internal access
    if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
        add_connection mm_clock_crossing_bridge_0.m0 grtos_0.s_GMemory avalon
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/grtos_0.s_GMemory arbitrationPriority {1}
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/grtos_0.s_GMemory baseAddress $address_base
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/grtos_0.s_GMemory defaultConnection {0}        
    }
    set ancho [get_instance_interface_property grtos_0 s_GMemory addressSpan ]
    send_message Debug "Span of grtos_0 s_GMemory is $ancho, base address is $BaseAddress"
    set BaseAddress [expr {$BaseAddress + $ancho}]    
    # set BaseAddress [expr {$BaseAddress + 0x200}]


    # Connection of grtos_0.s_Testing avalon at $BaseAddress span 0x200 
    add_connection master_0.master grtos_0.s_Testing avalon
    set_connection_parameter_value master_0.master/grtos_0.s_Testing arbitrationPriority {1}
    set_connection_parameter_value master_0.master/grtos_0.s_Testing baseAddress $BaseAddress
    set_connection_parameter_value master_0.master/grtos_0.s_Testing defaultConnection {0}
    
    set address_base [ get_connection_parameter_value master_0.master/grtos_0.s_Testing baseAddress ]
    for {set i 1} {$i <= $Processors} {incr i} {
        add_connection nios_avalon_monitor.m${i} grtos_0.s_Testing avalon
        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_Testing arbitrationPriority {1}
        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_Testing baseAddress $address_base
        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_Testing defaultConnection {0}        
    }
    # HPS internal access
    if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
        add_connection mm_clock_crossing_bridge_0.m0 grtos_0.s_Testing avalon
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/grtos_0.s_Testing arbitrationPriority {1}
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/grtos_0.s_Testing baseAddress $address_base
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/grtos_0.s_Testing defaultConnection {0}        
    }
    set ancho [get_instance_interface_property grtos_0 s_Testing addressSpan ]
    send_message Debug "Span of grtos_0 s_Testing is $ancho, base address is $BaseAddress"
    set BaseAddress [expr {$BaseAddress + $ancho}]    
    # set BaseAddress [expr {$BaseAddress + 0x200}]

	
    # Connection of grtos_0.s_Global avalon at $BaseAddress span 0x200 
    add_connection master_0.master grtos_0.s_Global avalon
    set_connection_parameter_value master_0.master/grtos_0.s_Global arbitrationPriority {1}
    set_connection_parameter_value master_0.master/grtos_0.s_Global baseAddress $BaseAddress
    set_connection_parameter_value master_0.master/grtos_0.s_Global defaultConnection {0}
    
    set address_base [ get_connection_parameter_value master_0.master/grtos_0.s_Global baseAddress ]
    for {set i 1} {$i <= $Processors} {incr i} {
        add_connection nios_avalon_monitor.m${i} grtos_0.s_Global avalon
        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_Global arbitrationPriority {1}
        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_Global baseAddress $address_base
        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_Global defaultConnection {0}        
    }
    # HPS internal access
    if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
        add_connection mm_clock_crossing_bridge_0.m0 grtos_0.s_Global avalon
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/grtos_0.s_Global arbitrationPriority {1}
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/grtos_0.s_Global baseAddress $address_base
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/grtos_0.s_Global defaultConnection {0}        
    }
    set ancho [get_instance_interface_property grtos_0 s_Global addressSpan ]
    send_message Debug "Span of grtos_0 s_Global is $ancho, base address is $BaseAddress"
    set BaseAddress [expr {$BaseAddress + $ancho}]    
    # set BaseAddress [expr {$BaseAddress + 0x200}]
    

    # Connection of grtos_0.s_processor$i at $BaseAddress span 0x8 each 
    for {set i 1} {$i <= $Processors} {incr i} {
        add_connection nios_avalon_monitor.m$i grtos_0.s_processor$i avalon
        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_processor${i} arbitrationPriority {1}
        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_processor${i} baseAddress $BaseAddress
        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_processor${i} defaultConnection {0}
        
    }  
	set ancho [get_instance_interface_property grtos_0 s_processor1 addressSpan ]
	send_message Debug "Span of grtos_0 s_processor1 is $ancho, base address is $BaseAddress"
	set BaseAddress [expr {$BaseAddress + $ancho}]          


    # Connection of nios_avalon_monitor.s_Global avalon at $BaseAddress span 0x100 
    add_connection master_0.master nios_avalon_monitor.s_Global avalon
    set_connection_parameter_value master_0.master/nios_avalon_monitor.s_Global arbitrationPriority {1}
    set_connection_parameter_value master_0.master/nios_avalon_monitor.s_Global baseAddress $BaseAddress
    set_connection_parameter_value master_0.master/nios_avalon_monitor.s_Global defaultConnection {0}
    
    set address_base [ get_connection_parameter_value master_0.master/nios_avalon_monitor.s_Global baseAddress ]
    for {set i 1} {$i <= $Processors} {incr i} {
        add_connection nios_avalon_monitor.m${i} nios_avalon_monitor.s_Global avalon
        set_connection_parameter_value nios_avalon_monitor.m${i}/nios_avalon_monitor.s_Global arbitrationPriority {1}
        set_connection_parameter_value nios_avalon_monitor.m${i}/nios_avalon_monitor.s_Global baseAddress $address_base
        set_connection_parameter_value nios_avalon_monitor.m${i}/nios_avalon_monitor.s_Global defaultConnection {0}
    }
    # HPS internal access
    if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
        add_connection mm_clock_crossing_bridge_0.m0 nios_avalon_monitor.s_Global avalon
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/nios_avalon_monitor.s_Global arbitrationPriority {1}
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/nios_avalon_monitor.s_Global baseAddress $address_base
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/nios_avalon_monitor.s_Global defaultConnection {0}   
    }    
    set ancho [get_instance_interface_property nios_avalon_monitor s_Global addressSpan ]
    send_message Debug "Span of nios_avalon_monitor.s_Global is $ancho, base address is $BaseAddress"    
    set BaseAddress [expr {$BaseAddress + $ancho}] 


    # Connection of jtag_uart_0.avalon_jtag_slave at $BaseAddress span 0x8 each for the NProcessors 
    for {set i 0} {$i <= $Processors + 1} {incr i} {
		if {(($incl_STDIO == true) && ($i == 0)) || (($incl_STDERR == true) && ($i == 1)) || (($incl_STDPROC == true) && ($i > 1))} {
			add_connection master_0.master jtag_uart_${i}.avalon_jtag_slave avalon
			set_connection_parameter_value master_0.master/jtag_uart_${i}.avalon_jtag_slave arbitrationPriority {1}
			set_connection_parameter_value master_0.master/jtag_uart_${i}.avalon_jtag_slave baseAddress $BaseAddress
			set_connection_parameter_value master_0.master/jtag_uart_${i}.avalon_jtag_slave defaultConnection {0}
            
            set address_base [ get_connection_parameter_value master_0.master/jtag_uart_${i}.avalon_jtag_slave baseAddress ]
			for {set j 1} {$j <= $Processors} {incr j} {
				add_connection nios_avalon_monitor.m${j} jtag_uart_${i}.avalon_jtag_slave avalon
				set_connection_parameter_value nios_avalon_monitor.m${j}/jtag_uart_${i}.avalon_jtag_slave arbitrationPriority {1}
				set_connection_parameter_value nios_avalon_monitor.m${j}/jtag_uart_${i}.avalon_jtag_slave baseAddress $address_base
				set_connection_parameter_value nios_avalon_monitor.m${j}/jtag_uart_${i}.avalon_jtag_slave defaultConnection {0}        
			}
			set ancho [get_instance_interface_property jtag_uart_${i} avalon_jtag_slave addressSpan ]
			set unidades [get_instance_interface_property jtag_uart_${i} avalon_jtag_slave addressUnits ]
			send_message Debug "Span of jtag_uart_${i} avalon_jtag_slave is $ancho, $unidades, base address is $BaseAddress"
			set BaseAddress [expr {$BaseAddress + max($MIN_ADDRESS_ALIGNMENT, $ancho)}]
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

    # Interruptions from jtag_uart_$i in the GeMRTOS controller
	set j 0
    for {set i 0} {$i <= $Processors + 1} {incr i} {
		if {(($incl_STDIO == true) && ($i == 0)) || (($incl_STDERR == true) && ($i == 1)) || (($incl_STDPROC == true) && ($i > 1))} {
			set j [expr {$j+1}]
			add_connection gemrtos_proc_1.${irq_rx} jtag_uart_${i}.irq interrupt
			set_connection_parameter_value gemrtos_proc_1.${irq_rx}/jtag_uart_${i}.irq irqNumber ${j}
			
			send_message Info "GeMRTOS JTAGUART $i connected to grtos IRQ $j"
            
            add_connection grtos_0.dirq${i}_receiver jtag_uart_${i}.irq interrupt
		}
    }

    # ##########################################################
    # Interruptions from irq_bridge_0
    for {set i $IntInterrupts} {$i < ${MAX_PROC_IRQs}} {incr i} {
        set dirq_pin [expr {$i - 1}]
        set j [expr {$i - $IntInterrupts}]
        add_connection grtos_0.dirq${dirq_pin}_receiver irq_bridge_0.sender${j}_irq interrupt
        
        send_message Info "IRQ bridge receiver $j connected to grtos IRQ $i"

        add_connection gemrtos_proc_1.${irq_rx} irq_bridge_0.sender${j}_irq interrupt
        set_connection_parameter_value gemrtos_proc_1.${irq_rx}/irq_bridge_0.sender${j}_irq irqNumber $i
    }
 
    # Interruptions from grtos_0
    for {set i 1} {$i <= $Processors} {incr i} {
        add_connection gemrtos_proc_${i}.${irq_rx} grtos_0.slv_irq${i} interrupt
        set_connection_parameter_value gemrtos_proc_${i}.${irq_rx}/grtos_0.slv_irq${i} irqNumber {0} 
    }

    
    # ####################################################
    # RESETs
    
    # nios from debug and grtos
    for {set i 1} {$i <= $Processors} {incr i} {
        if {$i != 1} {
            add_connection gemrtos_proc_1.${debug_agent_reset_request} gemrtos_proc_${i}.${proc_reset} reset        
        }        
        add_connection gemrtos_proc_${i}.${debug_agent_reset_request} gemrtos_proc_${i}.${proc_reset} reset
        add_connection grtos_0.slv_rst${i} gemrtos_proc_${i}.${proc_reset} reset
        add_connection grtos_0.cpu_rstreq${i} gemrtos_proc_${i}.${proc_rstreq_intf} conduit
    }

    # jtag_uart from debug and grtos
    for {set i 0} {$i <= $Processors + 1} {incr i} {
		if {(($incl_STDIO == true) && ($i == 0)) || (($incl_STDERR == true) && ($i == 1)) || (($incl_STDPROC == true) && ($i > 1))} {
			add_connection gemrtos_proc_1.${debug_agent_reset_request} jtag_uart_${i}.reset reset
		}
    }

    # grtos controller from debug
	add_connection gemrtos_proc_1.${debug_agent_reset_request} grtos_0.clock_reset_reset reset

    # GRTOS Avalon bridge from grtos
    
    # IRG bridge from grtos
    
    # GRTOS Avalon monitor from grtos and debug
    add_connection gemrtos_proc_1.${debug_agent_reset_request} nios_avalon_monitor.reset reset
	 
    # onchip memories from grtos and debug

    
    # Reset bridge from grtos
    add_connection gemrtos_proc_1.${debug_agent_reset_request} reset_bridge_1.in_reset reset

    
    #  # #############################################
    #  # Exported interfaces
    #  # Input clock
    set_interface_property clk_processors EXPORT_OF clock_bridge_0.in_clk
    

    # External reset input
    set_interface_property reset_in EXPORT_OF reset_bridge_0.in_reset

    # Nios avalon masters
    for {set i 1} {$i <= $Processors} {incr i} {
		if {($independet_buses == true) || ($i == 1)} {
			if {$i != 1} {
				add_interface gemrtos_avalon_processor_m${i} avalon start
			}
			set_interface_property gemrtos_avalon_processor_m${i} EXPORT_OF GRTOS_Avalon_Bridge_External.m${i}
		} 
    }
 
    # Output for leds
    add_interface gemrtos_phy conduit end
    set_interface_property gemrtos_phy EXPORT_OF grtos_0.phy
    

    add_interface gemrtos_dirq_input interrupt receiver
    set_interface_property gemrtos_dirq_input irqScheme INDIVIDUAL_REQUESTS
    set_interface_property gemrtos_dirq_input ENABLED true	
		
    set_interface_property gemrtos_dirq_input EXPORT_OF irq_bridge_0.receiver_irq
    
    # External reset output
    add_interface reset_grtos reset source
    set_interface_property reset_grtos EXPORT_OF reset_bridge_1.out_reset

    # HPS internal access
    if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
        add_interface h2f_axi_master_in avalon slave
        set_interface_property h2f_axi_master_in EXPORT_OF mm_clock_crossing_bridge_0.s0

        add_interface h2f_axi_clk_in clock sink
        set_interface_property h2f_axi_clk_in EXPORT_OF mm_clock_crossing_bridge_0.s0_clk
        add_interface h2f_axi_rst_in reset sink
        set_interface_property h2f_axi_rst_in EXPORT_OF mm_clock_crossing_bridge_0.s0_reset

        # HPS external 
        # add_interface grtos_avalon_bridge_1_HPS avalon start
        # set_interface_property grtos_avalon_bridge_1_HPS EXPORT_OF GRTOS_Avalon_Bridge_1.m${Ports}
        
        # HPS external 
        add_interface gemrtos_avalon_bridge_HPS avalon start
		set_interface_property gemrtos_avalon_bridge_HPS EXPORT_OF GRTOS_Avalon_Bridge_HPS.m1
    }
   
    # interconnect requirements
    set_interconnect_requirement {$system} {qsys_mm.clockCrossingAdapter} {HANDSHAKE}
    set_interconnect_requirement {$system} {qsys_mm.maxAdditionalLatency} {1}
}

## Add documentation links for user guide and/or release notes
add_documentation_link "User Guide" https://gemrtos.com
add_documentation_link "Release Notes" https://gemrtos.com
