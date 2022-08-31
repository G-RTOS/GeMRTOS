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

# _hw.tcl file for GRTOS_Multiprocessor
package require -exact qsys 14.0

# module properties
set_module_property NAME GRTOS_Multiprocessor
set_module_property DISPLAY_NAME "GRTOS_Multiprocessor v0.9"

# default module properties
set_module_property VERSION 1.0
set_module_property GROUP gRTOS
set_module_property DESCRIPTION "GRTOS Multiprocessor system"
set_module_property AUTHOR "Ricardo Cayssials"
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true

set_module_property COMPOSITION_CALLBACK compose
set_module_property VALIDATION_CALLBACK validate
set_module_property ELABORATION_CALLBACK elaborate
set_module_property opaque_address_map false



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


# ############################################################################
# Processor version
add_parameter Processor_type STRING "Nios II/f (full)" "Processor type"
set_parameter_property Processor_type DISPLAY_NAME "Processor type"
set_parameter_property Processor_type UNITS None
set_parameter_property Processor_type ALLOWED_RANGES {"Nios II/e (economy)" "Nios II/f (full)"}
set_parameter_property Processor_type HDL_PARAMETER false

# ############################################################################
# Processor Instruction Cache
add_parameter icache_size_parameter STRING "None" "Instruction cache size"
set_parameter_property icache_size_parameter DISPLAY_NAME "Instruction cache size"
set_parameter_property icache_size_parameter UNITS None
set_parameter_property icache_size_parameter ALLOWED_RANGES {"None" "512 Bytes" "1 KB" "2 KB" "4 KB" "8 KB" "16 KB" "32 KB" "64 KB"}
set_parameter_property icache_size_parameter HDL_PARAMETER false

# ############################################################################
# Processor Instruction Cache burst enable
add_parameter icache_burst_parameter STRING "Disable" "Add burstcount signal to instruction_master"
set_parameter_property icache_burst_parameter DISPLAY_NAME "Add burstcount signal to instruction_master"
set_parameter_property icache_burst_parameter UNITS None
set_parameter_property icache_burst_parameter ALLOWED_RANGES {"Enable" "Disable"}
set_parameter_property icache_burst_parameter HDL_PARAMETER false


# ############################################################################
# Processor table for future detail
# add_parameter names STRING_LIST {"Nios II/e (economy)" "Nios II/f (full)"}
# set_parameter_property names ALLOWED_RANGES {"Nios II/e (economy)" "Nios II/f (full)"}
# add_parameter counts INTEGER_LIST
# add_display_item "" myTable GROUP TABLE
# add_display_item myTable names PARAMETER
# add_display_item myTable counts PARAMETER


# ############################################################################
# Enable HPS interface
add_parameter ENABLE_HPS_MAP_ACCESS BOOLEAN false
set_parameter_property ENABLE_HPS_MAP_ACCESS DISPLAY_NAME "Enable HPS internal access"
set_parameter_property ENABLE_HPS_MAP_ACCESS TYPE BOOLEAN
set_parameter_property ENABLE_HPS_MAP_ACCESS UNITS None
set_parameter_property ENABLE_HPS_MAP_ACCESS DESCRIPTION "Enable Avalon MM slave for HPS to access internal memory addresses"
set_parameter_property ENABLE_HPS_MAP_ACCESS HDL_PARAMETER false


# ############################################################################
# Input clock for processors
add_interface clk_processors clock sink
set_interface_property clk_processors clockRate 0
set_interface_property clk_processors ENABLED true
    
add_parameter CFrequency INTEGER  
set_parameter_property CFrequency SYSTEM_INFO {CLOCK_RATE clk_processors}
set_parameter_property CFrequency DISPLAY_NAME "Processor frequency"
set_parameter_property CFrequency UNITS Hertz
add_display_item "Information" CFrequency PARAMETER
set_parameter_property CFrequency HDL_PARAMETER false

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

# Esternal memory span (derived from BUS_WIDTH)
add_parameter EXTERNAL_MEMORY_SPAN INTEGER 4
set_parameter_property EXTERNAL_MEMORY_SPAN DERIVED true
set_parameter_property EXTERNAL_MEMORY_SPAN DISPLAY_NAME  "External Memory span"
set_parameter_property EXTERNAL_MEMORY_SPAN UNITS None
add_display_item "Information" EXTERNAL_MEMORY_SPAN PARAMETER
set_parameter_property EXTERNAL_MEMORY_SPAN HDL_PARAMETER false

# Esternal memory span (derived from BUS_WIDTH)
add_parameter EXTERNAL_MEMORY_UNIT STRING ""
set_parameter_property EXTERNAL_MEMORY_UNIT DERIVED true
set_parameter_property EXTERNAL_MEMORY_UNIT DISPLAY_NAME  "External Memory unit"
set_parameter_property EXTERNAL_MEMORY_UNIT UNITS None
add_display_item "Information" EXTERNAL_MEMORY_UNIT PARAMETER
set_parameter_property EXTERNAL_MEMORY_UNIT HDL_PARAMETER false


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
    set bus_width [get_parameter_value BUS_WIDTH]
    if { $bus_width == 0 } {
        send_message error "External memory device should be connected to processor buses"
    } 
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

proc compose { } {
    # Parameters from GUI
    set Processors [get_parameter_value NProcessors]
    set Type_of_Processor [get_parameter_value Processor_type]
    set ClockFrequency [get_parameter_value CFrequency]
    set icache_size [get_parameter_value icache_size_parameter]
    set icache_burst [get_parameter_value icache_burst_parameter]    
    set PreScale [ expr { $ClockFrequency / 10000000} ]

    set Bridge_Address_Width [get_parameter_value BUS_WIDTH]
    set BaseAddress [expr {2**$Bridge_Address_Width}]
    set IntInterrupts [expr {$Processors + 4}]

    
    # remove_dangling_connections
    
    # Internal clock for devices in GRTOS
    add_instance clock_bridge_0 altera_clock_bridge 18.0
    set_instance_parameter_value clock_bridge_0 {NUM_CLOCK_OUTPUTS} {1}

    
    # Internal clock for devices in GRTOS
    add_instance clock_bridge_external_bus altera_clock_bridge 18.0
    set_instance_parameter_value clock_bridge_external_bus {NUM_CLOCK_OUTPUTS} {1}    
    
    add_instance reset_bridge_0 altera_reset_bridge 18.0
    set_instance_parameter_value reset_bridge_0 {ACTIVE_LOW_RESET} {0}
    set_instance_parameter_value reset_bridge_0 {NUM_RESET_OUTPUTS} {1}
    set_instance_parameter_value reset_bridge_0 {SYNCHRONOUS_EDGES} {deassert}
    set_instance_parameter_value reset_bridge_0 {USE_RESET_REQUEST} {0}    
    
    # Replace clk_0.clk_sacar for clock_bridge_0.out_clk
    # Replace clk_0.clk_reset_sacar for reset_bridge_0.out_reset
    add_connection clock_bridge_0.out_clk reset_bridge_0.clk clock

    
    # GRTOS Controller
    add_instance grtos_0 grtos 1.0
    set_instance_parameter_value grtos_0 {NProcessors} [get_parameter_value NProcessors]
    set_instance_parameter_value grtos_0 {PreScale} $PreScale
    # ######################
    set_instance_parameter_value grtos_0 {PROCESSOR_TYPE} [get_parameter_value Processor_type]
    set_instance_parameter_value grtos_0 {INSTRUCTION_CACHE_SIZE} [get_parameter_value icache_size_parameter]
    set_instance_parameter_value grtos_0 {INSTRUCTION_CACHE_BURST} [get_parameter_value icache_burst_parameter]
    set_instance_parameter_value grtos_0 {ENABLE_HPS_MAP_ACCESS} [get_parameter_value ENABLE_HPS_MAP_ACCESS]
    set_instance_parameter_value grtos_0 {NIOS_CLOCK_FREQUENCY} [get_parameter_value CFrequency]
    set_instance_parameter_value grtos_0 {BUS_CLOCK_FREQUENCY} [get_parameter_value CEFrequency]
    set_instance_parameter_value grtos_0 {BUS_WIDTH} [get_parameter_value BUS_WIDTH]
    set_instance_parameter_value grtos_0 {EXTERNAL_MEMORY_SPAN} [get_parameter_value EXTERNAL_MEMORY_SPAN]
    set_instance_parameter_value grtos_0 {EXTERNAL_MEMORY_UNIT} [get_parameter_value EXTERNAL_MEMORY_UNIT]
    # ######################
    
    # CLOCK and RESET
    add_connection clock_bridge_0.out_clk grtos_0.clock_reset clock
    add_connection reset_bridge_0.out_reset grtos_0.clock_reset_reset reset
    
    # ###########################################################
    # HPS internal access
    if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
        add_instance mm_clock_crossing_bridge_0 altera_avalon_mm_clock_crossing_bridge 18.0
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
    add_instance irq_bridge_0 altera_irq_bridge 18.0
    set_instance_parameter_value irq_bridge_0 {IRQ_N} {0}
    set Number_of_external_IRQ [expr {32 - $IntInterrupts}]
    set_instance_parameter_value irq_bridge_0 {IRQ_WIDTH} ${Number_of_external_IRQ}
    # CLOCK and RESET
    add_connection clock_bridge_0.out_clk irq_bridge_0.clk clock
    add_connection reset_bridge_0.out_reset irq_bridge_0.clk_reset reset
    
    # GRTOS Avalon monitor
    add_instance nios_avalon_monitor nios_avalon_monitor 13.0
    set_instance_parameter_value nios_avalon_monitor {ADDRESS_UNITS} {SYMBOLS}
    set_instance_parameter_value nios_avalon_monitor {ADDRESS_WIDTH} {27}
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
    add_instance onchip_memory2_0 altera_avalon_onchip_memory2 18.0
    set_instance_parameter_value onchip_memory2_0 {allowInSystemMemoryContentEditor} {0}
    set_instance_parameter_value onchip_memory2_0 {blockType} {AUTO}
    set_instance_parameter_value onchip_memory2_0 {copyInitFile} {0}
    set_instance_parameter_value onchip_memory2_0 {dataWidth} {32}
    set_instance_parameter_value onchip_memory2_0 {dataWidth2} {32}
    set_instance_parameter_value onchip_memory2_0 {dualPort} {0}
    set_instance_parameter_value onchip_memory2_0 {ecc_enabled} {0}
    set_instance_parameter_value onchip_memory2_0 {enPRInitMode} {0}
    set_instance_parameter_value onchip_memory2_0 {enableDiffWidth} {0}
    set_instance_parameter_value onchip_memory2_0 {initMemContent} {1}
    set_instance_parameter_value onchip_memory2_0 {initializationFileName} {GRTOS_Multiprocessor_0_onchip_memory2_0.hex}
    set_instance_parameter_value onchip_memory2_0 {instanceID} {NONE}
    set_instance_parameter_value onchip_memory2_0 {memorySize} {512.0}
    set_instance_parameter_value onchip_memory2_0 {readDuringWriteMode} {DONT_CARE}
    set_instance_parameter_value onchip_memory2_0 {resetrequest_enabled} {1}
    set_instance_parameter_value onchip_memory2_0 {simAllowMRAMContentsFile} {0}
    set_instance_parameter_value onchip_memory2_0 {simMemInitOnlyFilename} {0}
    set_instance_parameter_value onchip_memory2_0 {singleClockOperation} {0}
    set_instance_parameter_value onchip_memory2_0 {slave1Latency} {1}
    set_instance_parameter_value onchip_memory2_0 {slave2Latency} {1}
    set_instance_parameter_value onchip_memory2_0 {useNonDefaultInitFile} {0}
    set_instance_parameter_value onchip_memory2_0 {useShallowMemBlocks} {0}
    set_instance_parameter_value onchip_memory2_0 {writable} {1}
    # CLOCK and RESET
    add_connection clock_bridge_0.out_clk onchip_memory2_0.clk1 clock
    add_connection reset_bridge_0.out_reset onchip_memory2_0.reset1 reset

    # Memory for CPUID not 1 reset
    add_instance onchip_memory2_1 altera_avalon_onchip_memory2 18.0
    set_instance_parameter_value onchip_memory2_1 {allowInSystemMemoryContentEditor} {0}
    set_instance_parameter_value onchip_memory2_1 {blockType} {AUTO}
    set_instance_parameter_value onchip_memory2_1 {copyInitFile} {0}
    set_instance_parameter_value onchip_memory2_1 {dataWidth} {32}
    set_instance_parameter_value onchip_memory2_1 {dataWidth2} {32}
    set_instance_parameter_value onchip_memory2_1 {dualPort} {0}
    set_instance_parameter_value onchip_memory2_1 {ecc_enabled} {0}
    set_instance_parameter_value onchip_memory2_1 {enPRInitMode} {0}
    set_instance_parameter_value onchip_memory2_1 {enableDiffWidth} {0}
    set_instance_parameter_value onchip_memory2_1 {initMemContent} {1}
    set_instance_parameter_value onchip_memory2_1 {initializationFileName} {GRTOS_Multiprocessor_0_onchip_memory2_1.hex}
    set_instance_parameter_value onchip_memory2_1 {instanceID} {NONE}
    set_instance_parameter_value onchip_memory2_1 {memorySize} {2048.0}
    set_instance_parameter_value onchip_memory2_1 {readDuringWriteMode} {DONT_CARE}
    set_instance_parameter_value onchip_memory2_1 {resetrequest_enabled} {1}
    set_instance_parameter_value onchip_memory2_1 {simAllowMRAMContentsFile} {0}
    set_instance_parameter_value onchip_memory2_1 {simMemInitOnlyFilename} {0}
    set_instance_parameter_value onchip_memory2_1 {singleClockOperation} {0}
    set_instance_parameter_value onchip_memory2_1 {slave1Latency} {1}
    set_instance_parameter_value onchip_memory2_1 {slave2Latency} {1}
    set_instance_parameter_value onchip_memory2_1 {useNonDefaultInitFile} {0}
    set_instance_parameter_value onchip_memory2_1 {useShallowMemBlocks} {0}
    set_instance_parameter_value onchip_memory2_1 {writable} {1}
    # CLOCK and RESET
    add_connection clock_bridge_0.out_clk onchip_memory2_1.clk1 clock
    add_connection reset_bridge_0.out_reset onchip_memory2_1.reset1 reset    

    
    # # Memory for Code
    # add_instance onchip_memory2_2 altera_avalon_onchip_memory2 18.0
    # set_instance_parameter_value onchip_memory2_2 {allowInSystemMemoryContentEditor} {0}
    # set_instance_parameter_value onchip_memory2_2 {blockType} {AUTO}
    # set_instance_parameter_value onchip_memory2_2 {copyInitFile} {0}
    # set_instance_parameter_value onchip_memory2_2 {dataWidth} {32}
    # set_instance_parameter_value onchip_memory2_2 {dataWidth2} {32}
    # set_instance_parameter_value onchip_memory2_2 {dualPort} {0}
    # set_instance_parameter_value onchip_memory2_2 {ecc_enabled} {0}
    # set_instance_parameter_value onchip_memory2_2 {enPRInitMode} {0}
    # set_instance_parameter_value onchip_memory2_2 {enableDiffWidth} {0}
    # set_instance_parameter_value onchip_memory2_2 {initMemContent} {1}
    # set_instance_parameter_value onchip_memory2_2 {initializationFileName} {GRTOS_Multiprocessor_0_onchip_memory2_2.hex}
    # set_instance_parameter_value onchip_memory2_2 {instanceID} {NONE}
    # set_instance_parameter_value onchip_memory2_2 {memorySize} {2048.0}
    # set_instance_parameter_value onchip_memory2_2 {readDuringWriteMode} {DONT_CARE}
    # set_instance_parameter_value onchip_memory2_2 {resetrequest_enabled} {1}
    # set_instance_parameter_value onchip_memory2_2 {simAllowMRAMContentsFile} {0}
    # set_instance_parameter_value onchip_memory2_2 {simMemInitOnlyFilename} {0}
    # set_instance_parameter_value onchip_memory2_2 {singleClockOperation} {0}
    # set_instance_parameter_value onchip_memory2_2 {slave1Latency} {1}
    # set_instance_parameter_value onchip_memory2_2 {slave2Latency} {1}
    # set_instance_parameter_value onchip_memory2_2 {useNonDefaultInitFile} {0}
    # set_instance_parameter_value onchip_memory2_2 {useShallowMemBlocks} {0}
    # set_instance_parameter_value onchip_memory2_2 {writable} {1}
    # # CLOCK and RESET
    # add_connection clock_bridge_0.out_clk onchip_memory2_2.clk1 clock
    # add_connection reset_bridge_0.out_reset onchip_memory2_2.reset1 reset  

    # Processors
    for {set i 1} {$i <= $Processors} {incr i} {
        add_instance nios2_qsys_${i} altera_nios2_gen2 18.0
        set_instance_parameter_value nios2_qsys_${i} {bht_ramBlockType} {Automatic}
        set_instance_parameter_value nios2_qsys_${i} {breakOffset} {32}
        set_instance_parameter_value nios2_qsys_${i} {breakSlave} nios2_qsys_${i}.jtag_debug_module
        set_instance_parameter_value nios2_qsys_${i} {cdx_enabled} {0}
        set_instance_parameter_value nios2_qsys_${i} {cpuArchRev} {1}
        set_instance_parameter_value nios2_qsys_${i} {cpuID} ${i}
        set_instance_parameter_value nios2_qsys_${i} {cpuReset} {0}
        set_instance_parameter_value nios2_qsys_${i} {data_master_high_performance_paddr_base} {0}
        set_instance_parameter_value nios2_qsys_${i} {data_master_high_performance_paddr_size} {0.0}
        set_instance_parameter_value nios2_qsys_${i} {data_master_paddr_base} {0}
        set_instance_parameter_value nios2_qsys_${i} {data_master_paddr_size} {0.0}
        set_instance_parameter_value nios2_qsys_${i} {dcache_bursts} {false}
        set_instance_parameter_value nios2_qsys_${i} {dcache_numTCDM} {0}
        set_instance_parameter_value nios2_qsys_${i} {dcache_ramBlockType} {Automatic}
        set_instance_parameter_value nios2_qsys_${i} {dcache_size} {0}
        set_instance_parameter_value nios2_qsys_${i} {dcache_tagramBlockType} {Automatic}
        set_instance_parameter_value nios2_qsys_${i} {dcache_victim_buf_impl} {ram}
        set_instance_parameter_value nios2_qsys_${i} {debug_OCIOnchipTrace} {_128}
        set_instance_parameter_value nios2_qsys_${i} {debug_assignJtagInstanceID} {0}
        set_instance_parameter_value nios2_qsys_${i} {debug_datatrigger} {0}
        set_instance_parameter_value nios2_qsys_${i} {debug_debugReqSignals} {0}
        set_instance_parameter_value nios2_qsys_${i} {debug_enabled} {1}
        set_instance_parameter_value nios2_qsys_${i} {debug_hwbreakpoint} {0}
        set_instance_parameter_value nios2_qsys_${i} {debug_jtagInstanceID} {0}
        set_instance_parameter_value nios2_qsys_${i} {debug_traceStorage} {onchip_trace}
        set_instance_parameter_value nios2_qsys_${i} {debug_traceType} {none}
        set_instance_parameter_value nios2_qsys_${i} {debug_triggerArming} {1}
        # set_instance_parameter_value nios2_qsys_${i} {dividerType} {no_div}
        set_instance_parameter_value nios2_qsys_${i} {dividerType} {srt2}
        set_instance_parameter_value nios2_qsys_${i} {exceptionOffset} {32}
        set_instance_parameter_value nios2_qsys_${i} {exceptionSlave} {onchip_memory2_1.s1}
        set_instance_parameter_value nios2_qsys_${i} {fa_cache_line} {2}
        set_instance_parameter_value nios2_qsys_${i} {fa_cache_linesize} {0}
        set_instance_parameter_value nios2_qsys_${i} {flash_instruction_master_paddr_base} {0}
        set_instance_parameter_value nios2_qsys_${i} {flash_instruction_master_paddr_size} {0.0}
        if { $Type_of_Processor == "Nios II/f (full)" } {
            # ################
            if { $icache_burst == "Enable" } {
                set_instance_parameter_value nios2_qsys_${i} {icache_burstType} {Sequential} 
            }
            if { $icache_burst == "Disable" } {
                set_instance_parameter_value nios2_qsys_${i} {icache_burstType} {None} 
            }
        } else {
            set_instance_parameter_value nios2_qsys_${i} {icache_burstType} {None}
        }
        set_instance_parameter_value nios2_qsys_${i} {icache_numTCIM} {0}
        set_instance_parameter_value nios2_qsys_${i} {icache_ramBlockType} {Automatic}
        if { $Type_of_Processor == "Nios II/f (full)" } {
            # #########
            if { $icache_size == "None" } {
                set_instance_parameter_value nios2_qsys_${i} {icache_size} {0}
            }
            if { $icache_size == "512 Bytes" } {
                set_instance_parameter_value nios2_qsys_${i} {icache_size} {512}
            }
            if { $icache_size == "1 KB" } {
                set_instance_parameter_value nios2_qsys_${i} {icache_size} {1024}
            }
            if { $icache_size == "2 KB" } {
                set_instance_parameter_value nios2_qsys_${i} {icache_size} {2048}
            }
            if { $icache_size == "4 KB" } {
                set_instance_parameter_value nios2_qsys_${i} {icache_size} {4096}
            }
            if { $icache_size == "8 KB" } {
                set_instance_parameter_value nios2_qsys_${i} {icache_size} {8192}
            }
            if { $icache_size == "16 KB" } {
                set_instance_parameter_value nios2_qsys_${i} {icache_size} {16384}
            }
            if { $icache_size == "32 KB" } {
                set_instance_parameter_value nios2_qsys_${i} {icache_size} {32768}
            }
            if { $icache_size == "64 KB" } {
                set_instance_parameter_value nios2_qsys_${i} {icache_size} {65536}
            }
            # #########
        } else {
            set_instance_parameter_value nios2_qsys_${i} {icache_size} {0}
        }
        set_instance_parameter_value nios2_qsys_${i} {icache_tagramBlockType} {Automatic}
        if { $Type_of_Processor == "Nios II/e (economy)" } {
            set_instance_parameter_value nios2_qsys_${i} {impl} {Tiny}
        }
        if { $Type_of_Processor == "Nios II/f (full)" } {
            set_instance_parameter_value nios2_qsys_${i} {impl} {Fast}
        }        
        set_instance_parameter_value nios2_qsys_${i} {instruction_master_high_performance_paddr_base} {0}
        set_instance_parameter_value nios2_qsys_${i} {instruction_master_high_performance_paddr_size} {0.0}
        set_instance_parameter_value nios2_qsys_${i} {instruction_master_paddr_base} {0}
        set_instance_parameter_value nios2_qsys_${i} {instruction_master_paddr_size} {0.0}
        set_instance_parameter_value nios2_qsys_${i} {io_regionbase} {0}
        set_instance_parameter_value nios2_qsys_${i} {io_regionsize} {0}
        set_instance_parameter_value nios2_qsys_${i} {master_addr_map} {0}
        set_instance_parameter_value nios2_qsys_${i} {mmu_TLBMissExcOffset} {0}
        set_instance_parameter_value nios2_qsys_${i} {mmu_TLBMissExcSlave} {None}
        set_instance_parameter_value nios2_qsys_${i} {mmu_autoAssignTlbPtrSz} {1}
        set_instance_parameter_value nios2_qsys_${i} {mmu_enabled} {0}
        set_instance_parameter_value nios2_qsys_${i} {mmu_processIDNumBits} {8}
        set_instance_parameter_value nios2_qsys_${i} {mmu_ramBlockType} {Automatic}
        set_instance_parameter_value nios2_qsys_${i} {mmu_tlbNumWays} {16}
        set_instance_parameter_value nios2_qsys_${i} {mmu_tlbPtrSz} {7}
        set_instance_parameter_value nios2_qsys_${i} {mmu_udtlbNumEntries} {6}
        set_instance_parameter_value nios2_qsys_${i} {mmu_uitlbNumEntries} {4}
        set_instance_parameter_value nios2_qsys_${i} {mpu_enabled} {0}
        set_instance_parameter_value nios2_qsys_${i} {mpu_minDataRegionSize} {12}
        set_instance_parameter_value nios2_qsys_${i} {mpu_minInstRegionSize} {12}
        set_instance_parameter_value nios2_qsys_${i} {mpu_numOfDataRegion} {8}
        set_instance_parameter_value nios2_qsys_${i} {mpu_numOfInstRegion} {8}
        set_instance_parameter_value nios2_qsys_${i} {mpu_useLimit} {0}
        set_instance_parameter_value nios2_qsys_${i} {mpx_enabled} {0}
        set_instance_parameter_value nios2_qsys_${i} {mul_32_impl} {2}
        set_instance_parameter_value nios2_qsys_${i} {mul_64_impl} {0}
        set_instance_parameter_value nios2_qsys_${i} {mul_shift_choice} {0}
        set_instance_parameter_value nios2_qsys_${i} {ocimem_ramBlockType} {Automatic}
        set_instance_parameter_value nios2_qsys_${i} {ocimem_ramInit} {0}
        set_instance_parameter_value nios2_qsys_${i} {regfile_ramBlockType} {Automatic}
        set_instance_parameter_value nios2_qsys_${i} {register_file_por} {0}
        set_instance_parameter_value nios2_qsys_${i} {resetOffset} {0}
        if {$i == 1} {
            set_instance_parameter_value nios2_qsys_${i} {resetSlave} {onchip_memory2_1.s1}
        } else {
            set_instance_parameter_value nios2_qsys_${i} {resetSlave} {onchip_memory2_0.s1}
        }
        set_instance_parameter_value nios2_qsys_${i} {resetrequest_enabled} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_HBreakTest} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_HDLSimCachesCleared} {1}
        set_instance_parameter_value nios2_qsys_${i} {setting_activateMonitors} {1}
        set_instance_parameter_value nios2_qsys_${i} {setting_activateTestEndChecker} {0}
        # set_instance_parameter_value nios2_qsys_${i} {setting_activateTrace} {1}
        set_instance_parameter_value nios2_qsys_${i} {setting_activateTrace} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_allow_break_inst} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_alwaysEncrypt} {1}
        set_instance_parameter_value nios2_qsys_${i} {setting_asic_add_scan_mode_input} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_asic_enabled} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_asic_synopsys_translate_on_off} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_asic_third_party_synthesis} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_avalonDebugPortPresent} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_bhtPtrSz} {8}
        set_instance_parameter_value nios2_qsys_${i} {setting_bigEndian} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_branchpredictiontype} {Dynamic}
        set_instance_parameter_value nios2_qsys_${i} {setting_breakslaveoveride} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_clearXBitsLDNonBypass} {1}
        set_instance_parameter_value nios2_qsys_${i} {setting_dc_ecc_present} {1}
        set_instance_parameter_value nios2_qsys_${i} {setting_disable_tmr_inj} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_disableocitrace} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_dtcm_ecc_present} {1}
        set_instance_parameter_value nios2_qsys_${i} {setting_ecc_present} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_ecc_sim_test_ports} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_exportHostDebugPort} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_exportPCB} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_export_large_RAMs} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_exportdebuginfo} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_exportvectors} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_fast_register_read} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_ic_ecc_present} {1}
        set_instance_parameter_value nios2_qsys_${i} {setting_interruptControllerType} {Internal}
        set_instance_parameter_value nios2_qsys_${i} {setting_itcm_ecc_present} {1}
        set_instance_parameter_value nios2_qsys_${i} {setting_mmu_ecc_present} {1}
        set_instance_parameter_value nios2_qsys_${i} {setting_oci_export_jtag_signals} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_oci_version} {1}
        set_instance_parameter_value nios2_qsys_${i} {setting_preciseIllegalMemAccessException} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_removeRAMinit} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_rf_ecc_present} {1}
        set_instance_parameter_value nios2_qsys_${i} {setting_shadowRegisterSets} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_showInternalSettings} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_showUnpublishedSettings} {0}
        # set_instance_parameter_value nios2_qsys_${i} {setting_support31bitdcachebypass} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_support31bitdcachebypass} {1}
        set_instance_parameter_value nios2_qsys_${i} {setting_tmr_output_disable} {0}
        set_instance_parameter_value nios2_qsys_${i} {setting_usedesignware} {0}
        set_instance_parameter_value nios2_qsys_${i} {shift_rot_impl} {1}
        set_instance_parameter_value nios2_qsys_${i} {tightly_coupled_data_master_0_paddr_base} {0}
        set_instance_parameter_value nios2_qsys_${i} {tightly_coupled_data_master_0_paddr_size} {0.0}
        set_instance_parameter_value nios2_qsys_${i} {tightly_coupled_data_master_1_paddr_base} {0}
        set_instance_parameter_value nios2_qsys_${i} {tightly_coupled_data_master_1_paddr_size} {0.0}
        set_instance_parameter_value nios2_qsys_${i} {tightly_coupled_data_master_2_paddr_base} {0}
        set_instance_parameter_value nios2_qsys_${i} {tightly_coupled_data_master_2_paddr_size} {0.0}
        set_instance_parameter_value nios2_qsys_${i} {tightly_coupled_data_master_3_paddr_base} {0}
        set_instance_parameter_value nios2_qsys_${i} {tightly_coupled_data_master_3_paddr_size} {0.0}
        set_instance_parameter_value nios2_qsys_${i} {tightly_coupled_instruction_master_0_paddr_base} {0}
        set_instance_parameter_value nios2_qsys_${i} {tightly_coupled_instruction_master_0_paddr_size} {0.0}
        set_instance_parameter_value nios2_qsys_${i} {tightly_coupled_instruction_master_1_paddr_base} {0}
        set_instance_parameter_value nios2_qsys_${i} {tightly_coupled_instruction_master_1_paddr_size} {0.0}
        set_instance_parameter_value nios2_qsys_${i} {tightly_coupled_instruction_master_2_paddr_base} {0}
        set_instance_parameter_value nios2_qsys_${i} {tightly_coupled_instruction_master_2_paddr_size} {0.0}
        set_instance_parameter_value nios2_qsys_${i} {tightly_coupled_instruction_master_3_paddr_base} {0}
        set_instance_parameter_value nios2_qsys_${i} {tightly_coupled_instruction_master_3_paddr_size} {0.0}
        set_instance_parameter_value nios2_qsys_${i} {tmr_enabled} {0}
        set_instance_parameter_value nios2_qsys_${i} {tracefilename} {}
        set_instance_parameter_value nios2_qsys_${i} {userDefinedSettings} {}   
        # CLOCK and RESET
        add_connection clock_bridge_0.out_clk nios2_qsys_${i}.clk clock
        add_connection reset_bridge_0.out_reset nios2_qsys_${i}.reset reset
    }
    
    # JTAG UARTs
    for {set i 0} {$i <= $Processors + 1} {incr i} {
        add_instance jtag_uart_${i} altera_avalon_jtag_uart 18.0
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
        set_module_assignment embeddedsw.CMacro."/dev/jtg${i}" jtag_uart_${i}
    }
    
    # JTAG Avalon master
    add_instance master_0 altera_jtag_avalon_master 18.0
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
    add_instance GRTOS_Avalon_Bridge_1 GRTOS_Avalon_Bridge 13.0
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
    add_instance reset_bridge_1 altera_reset_bridge 18.0
    set_instance_parameter_value reset_bridge_1 {ACTIVE_LOW_RESET} {0}
    set_instance_parameter_value reset_bridge_1 {NUM_RESET_OUTPUTS} {1}
    set_instance_parameter_value reset_bridge_1 {SYNCHRONOUS_EDGES} {deassert}
    set_instance_parameter_value reset_bridge_1 {USE_RESET_REQUEST} {0}
    # CLOCK
    add_connection clock_bridge_0.out_clk reset_bridge_1.clk clock

    # SYSID
    add_instance sysid_qsys_0 altera_avalon_sysid_qsys 18.0
    set_instance_parameter_value sysid_qsys_0 {id} {1}
    # CLOCK and RESET
    add_connection clock_bridge_0.out_clk sysid_qsys_0.clk clock
    add_connection reset_bridge_0.out_reset sysid_qsys_0.reset reset
    
    # TIMER
    add_instance timer_0 altera_avalon_timer 18.0
    set_instance_parameter_value timer_0 {alwaysRun} {0}
    set_instance_parameter_value timer_0 {counterSize} {32}
    set_instance_parameter_value timer_0 {fixedPeriod} {0}
    set_instance_parameter_value timer_0 {period} {1}
    set_instance_parameter_value timer_0 {periodUnits} {MSEC}
    set_instance_parameter_value timer_0 {resetOutput} {0}
    set_instance_parameter_value timer_0 {snapshot} {1}
    set_instance_parameter_value timer_0 {timeoutPulseOutput} {0}
    set_instance_parameter_value timer_0 {watchdogPulse} {2} 
    # CLOCK and RESET
    add_connection clock_bridge_0.out_clk timer_0.clk clock
    add_connection reset_bridge_0.out_reset timer_0.reset reset
    
    ################################ DEC 2021
    for {set i 1} {$i <= $Processors} {incr i} {
        add_instance mm_clock_crossing_bridge_pro_${i} altera_avalon_mm_clock_crossing_bridge 18.0
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

    }
    ################################ DEC 2021    
    ################################ JAN 2022
    # HPS internal access
    if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
        add_instance mm_clock_crossing_bridge_HPS_OUT altera_avalon_mm_clock_crossing_bridge 18.0
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
    
    # NIOS CONNECTION TO MONITOR
    for {set i 1} {$i <= $Processors} {incr i} {
        add_connection nios2_qsys_${i}.data_master nios_avalon_monitor.s${i} avalon
        set_connection_parameter_value nios2_qsys_${i}.data_master/nios_avalon_monitor.s${i} arbitrationPriority {1}
        set_connection_parameter_value nios2_qsys_${i}.data_master/nios_avalon_monitor.s${i} baseAddress {0x0000}
        set_connection_parameter_value nios2_qsys_${i}.data_master/nios_avalon_monitor.s${i} defaultConnection {0}
        add_connection nios2_qsys_${i}.instruction_master nios_avalon_monitor.s${i} avalon
        set_connection_parameter_value nios2_qsys_${i}.instruction_master/nios_avalon_monitor.s${i} arbitrationPriority {1}
        set_connection_parameter_value nios2_qsys_${i}.instruction_master/nios_avalon_monitor.s${i} baseAddress {0x0000}
        set_connection_parameter_value nios2_qsys_${i}.instruction_master/nios_avalon_monitor.s${i} defaultConnection {0}    
    }

    
    # Connection of debug_mem_slave at $BaseAddress span 0x800 each
    for {set i 1} {$i <= $Processors} {incr i} {
        add_connection master_0.master nios2_qsys_${i}.debug_mem_slave avalon
        set_connection_parameter_value master_0.master/nios2_qsys_${i}.debug_mem_slave arbitrationPriority {1}
        set_connection_parameter_value master_0.master/nios2_qsys_${i}.debug_mem_slave baseAddress $BaseAddress
        set_connection_parameter_value master_0.master/nios2_qsys_${i}.debug_mem_slave defaultConnection {0}
    
        add_connection nios_avalon_monitor.m${i} nios2_qsys_${i}.debug_mem_slave avalon
        set_connection_parameter_value nios_avalon_monitor.m${i}/nios2_qsys_${i}.debug_mem_slave arbitrationPriority {1}
        set address_base [ get_connection_parameter_value master_0.master/nios2_qsys_${i}.debug_mem_slave baseAddress ]
        set_connection_parameter_value nios_avalon_monitor.m${i}/nios2_qsys_${i}.debug_mem_slave baseAddress $address_base
        set_connection_parameter_value nios_avalon_monitor.m${i}/nios2_qsys_${i}.debug_mem_slave defaultConnection {0}
        # HPS internal access
        # if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
        #     add_connection mm_clock_crossing_bridge_0.m0 nios2_qsys_${i}.debug_mem_slave avalon
        #     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/nios2_qsys_${i}.debug_mem_slave arbitrationPriority {1}
        #     set address_base [ get_connection_parameter_value master_0.master/nios2_qsys_${i}.debug_mem_slave baseAddress ]
        #     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/nios2_qsys_${i}.debug_mem_slave baseAddress $address_base
        #     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/nios2_qsys_${i}.debug_mem_slave defaultConnection {0}        
        # }
        set ancho [get_instance_interface_property nios2_qsys_${i} debug_mem_slave addressSpan ]
        send_message Warning "Span of nios2_qsys_${i} debug_mem_slave is $ancho, base address is $BaseAddress"
        set BaseAddress [expr {$BaseAddress + $ancho}]
        # set BaseAddress [expr {$BaseAddress + 0x800}]
    }
    

    # Connection of onchip_memory2_1 at $BaseAddress span 0x2000
    add_connection master_0.master onchip_memory2_1.s1 avalon
    set_connection_parameter_value master_0.master/onchip_memory2_1.s1 arbitrationPriority {1}
    set_connection_parameter_value master_0.master/onchip_memory2_1.s1 baseAddress $BaseAddress
    set_connection_parameter_value master_0.master/onchip_memory2_1.s1 defaultConnection {0}
    for {set i 1} {$i <= $Processors} {incr i} {
        add_connection nios_avalon_monitor.m$i onchip_memory2_1.s1 avalon
        set_connection_parameter_value nios_avalon_monitor.m${i}/onchip_memory2_1.s1 arbitrationPriority {1}
        set address_base [ get_connection_parameter_value master_0.master/onchip_memory2_1.s1 baseAddress ]
        set_connection_parameter_value nios_avalon_monitor.m${i}/onchip_memory2_1.s1 baseAddress $address_base
        set_connection_parameter_value nios_avalon_monitor.m${i}/onchip_memory2_1.s1 defaultConnection {0}
    }
    # HPS internal access
    if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
        add_connection mm_clock_crossing_bridge_0.m0 onchip_memory2_1.s1 avalon
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/onchip_memory2_1.s1 arbitrationPriority {1}
        set address_base [ get_connection_parameter_value master_0.master/onchip_memory2_1.s1 baseAddress ]
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/onchip_memory2_1.s1 baseAddress $address_base
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/onchip_memory2_1.s1 defaultConnection {0}        
    }
    set ancho [get_instance_interface_property onchip_memory2_1 s1 addressSpan ]
    send_message Warning "Span of onchip_memory2_1 s1 is $ancho, base address is $BaseAddress"
    set BaseAddress [expr {$BaseAddress + $ancho}]
    # set BaseAddress [expr {$BaseAddress + 0x0800}]    
    

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
    send_message Warning "Span of grtos_0 s_processor_monitor is $ancho, base address is $BaseAddress"
    set BaseAddress [expr {$BaseAddress + $ancho}]
    # set BaseAddress [expr {$BaseAddress + 0x400}]
# #########################################################    

    # Connection of onchip_memory2_0 at $BaseAddress span 0x800
    add_connection master_0.master onchip_memory2_0.s1 avalon
    set_connection_parameter_value master_0.master/onchip_memory2_0.s1 arbitrationPriority {1}
    set_connection_parameter_value master_0.master/onchip_memory2_0.s1 baseAddress $BaseAddress
    set_connection_parameter_value master_0.master/onchip_memory2_0.s1 defaultConnection {0}
    for {set i 1} {$i <= $Processors} {incr i} {
        add_connection nios_avalon_monitor.m$i onchip_memory2_0.s1 avalon
        set_connection_parameter_value nios_avalon_monitor.m${i}/onchip_memory2_0.s1 arbitrationPriority {1}
        set address_base [ get_connection_parameter_value master_0.master/onchip_memory2_0.s1 baseAddress ]
        set_connection_parameter_value nios_avalon_monitor.m${i}/onchip_memory2_0.s1 baseAddress $address_base
        set_connection_parameter_value nios_avalon_monitor.m${i}/onchip_memory2_0.s1 defaultConnection {0}
    }
    # HPS internal access
    if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
        add_connection mm_clock_crossing_bridge_0.m0 onchip_memory2_0.s1 avalon
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/onchip_memory2_0.s1 arbitrationPriority {1}
        set address_base [ get_connection_parameter_value master_0.master/onchip_memory2_0.s1 baseAddress ]
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/onchip_memory2_0.s1 baseAddress $address_base
        set_connection_parameter_value mm_clock_crossing_bridge_0.m0/onchip_memory2_0.s1 defaultConnection {0}        
    }
    # set ancho [get_instance_interface_properties onchip_memory2_0 s1 ]
    # send_message Warning $ancho
    set ancho [get_instance_interface_property onchip_memory2_0 s1 addressSpan ]
    send_message Warning "Span of onchip_memory2_0 is $ancho, base address is $BaseAddress"
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
    send_message Warning "Span of grtos_0 s_Global is $ancho, base address is $BaseAddress"
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
    set unidades [get_instance_interface_property timer_0 s1 addressUnits ]
    send_message Warning "Span of nios_avalon_monitor s_Global is $ancho, $unidades, base address is $BaseAddress"
    set BaseAddress [expr {$BaseAddress + $ancho}]      
    # set BaseAddress [expr {$BaseAddress + 0x100}]
    
    
    
    # Connection of timer_0.s1 avalon at $BaseAddress span 0x20 
    add_connection master_0.master timer_0.s1 avalon
    set_connection_parameter_value master_0.master/timer_0.s1 arbitrationPriority {1}
    set_connection_parameter_value master_0.master/timer_0.s1 baseAddress $BaseAddress
    set_connection_parameter_value master_0.master/timer_0.s1 defaultConnection {0}    
    for {set i 1} {$i <= $Processors} {incr i} {
        add_connection nios_avalon_monitor.m${i} timer_0.s1 avalon
        set_connection_parameter_value nios_avalon_monitor.m${i}/timer_0.s1 arbitrationPriority {1}
        set address_base [ get_connection_parameter_value master_0.master/timer_0.s1 baseAddress ]
        set_connection_parameter_value nios_avalon_monitor.m${i}/timer_0.s1 baseAddress $address_base
        set_connection_parameter_value nios_avalon_monitor.m${i}/timer_0.s1 defaultConnection {0}    
    }
    # HPS internal access
    # if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
    #     add_connection mm_clock_crossing_bridge_0.m0 timer_0.s1 avalon
    #     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/timer_0.s1 arbitrationPriority {1}
    #     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/timer_0.s1 baseAddress $BaseAddress
    #     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/timer_0.s1 defaultConnection {0}        
    # } 
    set ancho [get_instance_interface_property timer_0 s1 addressSpan ]
    set unidades [get_instance_interface_property timer_0 s1 addressUnits ]
    send_message Warning "Span of timer_0 s1 is $ancho, $unidades, base address is $BaseAddress"
    # set BaseAddress [expr {$BaseAddress + $ancho}]       
    # set BaseAddress [expr {$BaseAddress + 0x20}]
    set BaseAddress [expr {$BaseAddress + max(0x20, $ancho)}]
    
    # Connection of grtos_0.s_processor$i at $BaseAddress span 0x8 each 
    for {set i 1} {$i <= $Processors} {incr i} {
        add_connection grtos_0.dummy_master grtos_0.s_processor$i avalon
        set_connection_parameter_value grtos_0.dummy_master/grtos_0.s_processor${i} arbitrationPriority {1}
        set_connection_parameter_value grtos_0.dummy_master/grtos_0.s_processor${i} baseAddress $BaseAddress
        set_connection_parameter_value grtos_0.dummy_master/grtos_0.s_processor${i} defaultConnection {0}
        
        # The processor 1 connects to all the ports
        if {$i != 1} {
            add_connection nios_avalon_monitor.m1 grtos_0.s_processor$i avalon
            set_connection_parameter_value nios_avalon_monitor.m1/grtos_0.s_processor${i} arbitrationPriority {1}
            set address_base [ get_connection_parameter_value grtos_0.dummy_master/grtos_0.s_processor${i} baseAddress ]
            set_connection_parameter_value nios_avalon_monitor.m1/grtos_0.s_processor${i} baseAddress $address_base
            set_connection_parameter_value nios_avalon_monitor.m1/grtos_0.s_processor${i} defaultConnection {0}        
        } 

        add_connection nios_avalon_monitor.m$i grtos_0.s_processor$i avalon
        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_processor${i} arbitrationPriority {1}
        set address_base [ get_connection_parameter_value grtos_0.dummy_master/grtos_0.s_processor${i} baseAddress ]
        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_processor${i} baseAddress $address_base
        set_connection_parameter_value nios_avalon_monitor.m${i}/grtos_0.s_processor${i} defaultConnection {0}
        
        # HPS internal access does not require acces to idle port
        # if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
        #     add_connection mm_clock_crossing_bridge_0.m0 grtos_0.s_processor${i} avalon
        #     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/grtos_0.s_processor${i} arbitrationPriority {1}
        #     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/grtos_0.s_processor${i} baseAddress $BaseAddress
        #     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/grtos_0.s_processor${i} defaultConnection {0}        
        # }   
        set ancho [get_instance_interface_property grtos_0 s_processor${i} addressSpan ]
        send_message Warning "Span of grtos_0 s_processor${i} is $ancho, base address is $BaseAddress"
        set BaseAddress [expr {$BaseAddress + $ancho}]          
        # set BaseAddress [expr {$BaseAddress + 0x20}]
    }

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

    # Connection of jtag_uart_0.avalon_jtag_slave at $BaseAddress span 0x8 
    add_connection master_0.master sysid_qsys_0.control_slave avalon
    set_connection_parameter_value master_0.master/sysid_qsys_0.control_slave arbitrationPriority {1}
    set_connection_parameter_value master_0.master/sysid_qsys_0.control_slave baseAddress $BaseAddress
    set_connection_parameter_value master_0.master/sysid_qsys_0.control_slave defaultConnection {0}
    for {set i 1} {$i <= $Processors} {incr i} {
        add_connection nios_avalon_monitor.m${i} sysid_qsys_0.control_slave avalon
        set_connection_parameter_value nios_avalon_monitor.m${i}/sysid_qsys_0.control_slave arbitrationPriority {1}
        set address_base [ get_connection_parameter_value master_0.master/sysid_qsys_0.control_slave baseAddress ]
        set_connection_parameter_value nios_avalon_monitor.m${i}/sysid_qsys_0.control_slave baseAddress $address_base
        set_connection_parameter_value nios_avalon_monitor.m${i}/sysid_qsys_0.control_slave defaultConnection {0}    
    }
    # HPS internal access
    # if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
    #     add_connection mm_clock_crossing_bridge_0.m0 sysid_qsys_0.control_slave avalon
    #     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/sysid_qsys_0.control_slave arbitrationPriority {1}
    #     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/sysid_qsys_0.control_slave baseAddress $BaseAddress
    #     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/sysid_qsys_0.control_slave defaultConnection {0}        
    # } 
    set ancho [get_instance_interface_property sysid_qsys_0 control_slave addressSpan ]
    set unidades [get_instance_interface_property sysid_qsys_0 control_slave addressUnits ]
    send_message Warning "Span of sysid_qsys_0 control_slave is $ancho, $unidades, base address is $BaseAddress"
    set BaseAddress [expr {$BaseAddress + $ancho}]      
    # set BaseAddress [expr {$BaseAddress + 0x8}]


    
    # Connection of jtag_uart_0.avalon_jtag_slave at $BaseAddress span 0x8 each for the NProcessors 
    for {set j 0} {$j <= $Processors + 1} {incr j} {
        add_connection master_0.master jtag_uart_${j}.avalon_jtag_slave avalon
        set_connection_parameter_value master_0.master/jtag_uart_${j}.avalon_jtag_slave arbitrationPriority {1}
        set_connection_parameter_value master_0.master/jtag_uart_${j}.avalon_jtag_slave baseAddress $BaseAddress
        set_connection_parameter_value master_0.master/jtag_uart_${j}.avalon_jtag_slave defaultConnection {0}     
        for {set i 1} {$i <= $Processors} {incr i} {
 
            add_connection nios_avalon_monitor.m${i} jtag_uart_${j}.avalon_jtag_slave avalon
            set_connection_parameter_value nios_avalon_monitor.m${i}/jtag_uart_${j}.avalon_jtag_slave arbitrationPriority {1}
            set address_base [ get_connection_parameter_value master_0.master/jtag_uart_${j}.avalon_jtag_slave baseAddress ]
            set_connection_parameter_value nios_avalon_monitor.m${i}/jtag_uart_${j}.avalon_jtag_slave baseAddress $address_base
            set_connection_parameter_value nios_avalon_monitor.m${i}/jtag_uart_${j}.avalon_jtag_slave defaultConnection {0}        
        }
        # HPS internal access
        # if { [get_parameter_value ENABLE_HPS_MAP_ACCESS ] } {
        #     add_connection mm_clock_crossing_bridge_0.m0 jtag_uart_${j}.avalon_jtag_slave avalon
        #     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/jtag_uart_${j}.avalon_jtag_slave arbitrationPriority {1}
        #     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/jtag_uart_${j}.avalon_jtag_slave baseAddress $BaseAddress
        #     set_connection_parameter_value mm_clock_crossing_bridge_0.m0/jtag_uart_${j}.avalon_jtag_slave defaultConnection {0}        
        # }
        set ancho [get_instance_interface_property jtag_uart_${j} avalon_jtag_slave addressSpan ]
        set unidades [get_instance_interface_property jtag_uart_${j} avalon_jtag_slave addressUnits ]
        send_message Warning "Span of jtag_uart_${j} avalon_jtag_slave is $ancho, $unidades, base address is $BaseAddress"
        # set BaseAddress [expr {$BaseAddress + $ancho}]  
        set BaseAddress [expr {$BaseAddress + max(0x8, $ancho)}]
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
    for {set i $IntInterrupts} {$i < 32} {incr i} {
        add_connection grtos_0.interrupt_receiver irq_bridge_0.sender[expr {$i - $IntInterrupts}]_irq interrupt
        set_connection_parameter_value grtos_0.interrupt_receiver/irq_bridge_0.sender[expr {$i - $IntInterrupts}]_irq irqNumber $i

        add_connection nios2_qsys_1.irq irq_bridge_0.sender[expr {$i - $IntInterrupts}]_irq interrupt
        set_connection_parameter_value nios2_qsys_1.irq/irq_bridge_0.sender[expr {$i - $IntInterrupts}]_irq irqNumber $i
    }
 
    # Interruptions from grtos_0
    for {set i 1} {$i <= $Processors} {incr i} {
        add_connection nios2_qsys_${i}.irq grtos_0.slv_irq${i} interrupt
        set_connection_parameter_value nios2_qsys_${i}.irq/grtos_0.slv_irq${i} irqNumber {0} 
    }
    
    # Interruptions from timer_0    
    add_connection nios2_qsys_1.irq timer_0.irq interrupt
    set_connection_parameter_value nios2_qsys_1.irq/timer_0.irq irqNumber {1}    
    add_connection grtos_0.interrupt_receiver timer_0.irq interrupt
    set_connection_parameter_value grtos_0.interrupt_receiver/timer_0.irq irqNumber {1}    
    
    # Interruptions from jtag_uart_$i 
    for {set i 0} {$i <= $Processors + 1} {incr i} {
        add_connection nios2_qsys_1.irq jtag_uart_${i}.irq interrupt
        set_connection_parameter_value nios2_qsys_1.irq/jtag_uart_${i}.irq irqNumber [expr {$i+2}]
        
        add_connection grtos_0.interrupt_receiver jtag_uart_${i}.irq interrupt
        set_connection_parameter_value grtos_0.interrupt_receiver/jtag_uart_${i}.irq irqNumber [expr {$i+2}]
    }
    
    # ####################################################
    # RESETs
    
    # nios from debug and grtos
    for {set i 1} {$i <= $Processors} {incr i} {
        # if {$i != 1} {
        #     add_connection nios2_qsys_1.debug_reset_request nios2_qsys_${i}.reset reset        
        # }        
        add_connection nios2_qsys_${i}.debug_reset_request nios2_qsys_${i}.reset reset
        add_connection grtos_0.slv_rst${i} nios2_qsys_${i}.reset reset    
    }

    # jtag_uart from debug and grtos
    for {set i 0} {$i <= $Processors + 1} {incr i} { 
        add_connection nios2_qsys_1.debug_reset_request jtag_uart_${i}.reset reset
        add_connection grtos_0.slv_rst1 jtag_uart_${i}.reset reset
    }

    # grtos controller from debug
    add_connection nios2_qsys_1.debug_reset_request grtos_0.clock_reset_reset reset
    
    # GRTOS Avalon bridge from grtos
    add_connection grtos_0.slv_rst1 GRTOS_Avalon_Bridge_1.reset reset
    
    # IRG bridge from grtos
    add_connection grtos_0.slv_rst1 irq_bridge_0.clk_reset reset
    
    # GRTOS Avalon monitor from grtos and debug
    add_connection grtos_0.slv_rst1 nios_avalon_monitor.reset reset
    add_connection nios2_qsys_1.debug_reset_request nios_avalon_monitor.reset reset
    
    # onchip memories from grtos and debug
    add_connection grtos_0.slv_rst1 onchip_memory2_0.reset1 reset
    add_connection grtos_0.slv_rst1 onchip_memory2_1.reset1 reset
    # add_connection grtos_0.slv_rst1 onchip_memory2_2.reset1 reset
    
    # Reset bridge from grtos
    add_connection grtos_0.slv_rst1 reset_bridge_1.in_reset reset
    
    # Sysid from grtos and debug
    add_connection grtos_0.slv_rst1 sysid_qsys_0.reset reset
    
    # timer from grtos and debug
    add_connection grtos_0.slv_rst1 timer_0.reset reset
    add_connection nios2_qsys_1.debug_reset_request timer_0.reset reset
    
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
        if {$i != 1} {
            add_interface gemrtos_avalon_processor_m${i} avalon start
        }
        set_interface_property gemrtos_avalon_processor_m${i} EXPORT_OF mm_clock_crossing_bridge_pro_${i}.m0        
    }
 
    # Output for leds
    add_interface gemrtos_phy conduit end
    set_interface_property gemrtos_phy EXPORT_OF grtos_0.phy
    
    # External to IRQ Bridge
    add_interface gemrtos_dirq_input interrupt receiver
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
        add_interface grtos_avalon_bridge_1_HPS avalon start
        set_interface_property grtos_avalon_bridge_1_HPS EXPORT_OF mm_clock_crossing_bridge_HPS_OUT.m0        
        ############################# JAN 2022
        
    }
    # ###########################################################
    
    # interconnect requirements
    set_interconnect_requirement {$system} {qsys_mm.clockCrossingAdapter} {HANDSHAKE}
    set_interconnect_requirement {$system} {qsys_mm.maxAdditionalLatency} {1}
    
}
    