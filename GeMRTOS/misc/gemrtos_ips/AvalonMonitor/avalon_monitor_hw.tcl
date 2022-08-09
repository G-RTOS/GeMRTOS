# (C) 2001-2013 Altera Corporation. All rights reserved.
# Your use of Altera Corporation's design tools, logic functions and other 
# software and tools, and its AMPP partner logic functions, and any output 
# files any of the foregoing (including device programming or simulation 
# files), and any associated documentation or information are expressly subject 
# to the terms and conditions of the Altera Program License Subscription 
# Agreement, Altera MegaCore Function License Agreement, or other applicable 
# license agreement, including, without limitation, that your use is for the 
# sole purpose of programming logic devices manufactured by Altera and sold by 
# Altera or its authorized distributors.  Please refer to the applicable 
# agreement for further details.


# +-----------------------------------
# | 
# | avalon_monitor "Avalon-MM Pipeline Bridge"
# | 
# +-----------------------------------

# +-----------------------------------
# | request TCL package from ACDS 10.0
# | 
package require -exact sopc 10.0
# | 
# +-----------------------------------

# +-----------------------------------
# | module avalon_monitor
# | 
set_module_property DESCRIPTION "Nios II Avalon monitor module"
set_module_property NAME nios_avalon_monitor
set_module_property VERSION 13.0
set_module_property HIDE_FROM_SOPC true 
set_module_property GROUP gRTOS
set_module_property INTERNAL true
set_module_property AUTHOR "Ricardo Cayssials"
set_module_property DISPLAY_NAME "Nios II Avalon monitor module"
set_module_property TOP_LEVEL_HDL_FILE avalon_monitor.vhd
set_module_property TOP_LEVEL_HDL_MODULE avalon_monitor
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true
set_module_property ELABORATION_CALLBACK elaborate
set_module_property ANALYZE_HDL FALSE
set_module_property SIMULATION_MODEL_IN_VHDL true
set_module_property HIDE_FROM_SOPC true
#set_module_property DATASHEET_URL http://www.altera.com/literature/hb/qts/qsys_interconnect.pdf


# | 
# +-----------------------------------

# +-----------------------------------
# | files
# | 
add_file avalon_monitor.vhd {SYNTHESIS SIMULATION}
add_file STD_FIFO.vhd {SYNTHESIS SIMULATION}
# | 
# +-----------------------------------

# +-----------------------------------
# | parameters
# | 

# Number of Processors
add_parameter NProcessors INTEGER 1 "Number of System Processors"
set_parameter_property NProcessors DEFAULT_VALUE 1
set_parameter_property NProcessors DISPLAY_NAME "Number of Processors"
set_parameter_property NProcessors TYPE INTEGER
set_parameter_property NProcessors UNITS None
set_parameter_property NProcessors ALLOWED_RANGES {1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32}
set_parameter_property NProcessors DESCRIPTION "Number of System Processors"
set_parameter_property NProcessors HDL_PARAMETER true


add_parameter DATA_WIDTH INTEGER 32
set_parameter_property DATA_WIDTH DEFAULT_VALUE 32
set_parameter_property DATA_WIDTH DISPLAY_NAME {Data width}
set_parameter_property DATA_WIDTH TYPE INTEGER
set_parameter_property DATA_WIDTH UNITS None
set_parameter_property DATA_WIDTH DISPLAY_HINT ""
set_parameter_property DATA_WIDTH AFFECTS_GENERATION false
set_parameter_property DATA_WIDTH HDL_PARAMETER true
set_parameter_property DATA_WIDTH DESCRIPTION {Bridge data width}

add_parameter SYMBOL_WIDTH INTEGER 8
set_parameter_property SYMBOL_WIDTH DEFAULT_VALUE 8
set_parameter_property SYMBOL_WIDTH DISPLAY_NAME {Symbol width}
set_parameter_property SYMBOL_WIDTH TYPE INTEGER
set_parameter_property SYMBOL_WIDTH UNITS None
set_parameter_property SYMBOL_WIDTH DISPLAY_HINT ""
set_parameter_property SYMBOL_WIDTH AFFECTS_GENERATION false
set_parameter_property SYMBOL_WIDTH HDL_PARAMETER true
set_parameter_property SYMBOL_WIDTH DESCRIPTION {Symbol (byte) width}

add_parameter ADDRESS_WIDTH INTEGER 32
set_parameter_property ADDRESS_WIDTH DEFAULT_VALUE 32
set_parameter_property ADDRESS_WIDTH DISPLAY_NAME {Address width}
set_parameter_property ADDRESS_WIDTH TYPE INTEGER
set_parameter_property ADDRESS_WIDTH UNITS None
set_parameter_property ADDRESS_WIDTH DISPLAY_HINT ""
set_parameter_property ADDRESS_WIDTH AFFECTS_GENERATION false
set_parameter_property ADDRESS_WIDTH HDL_PARAMETER true
set_parameter_property ADDRESS_WIDTH DESCRIPTION {Bridge address width}

add_parameter BURSTCOUNT_WIDTH INTEGER 1
set_parameter_property BURSTCOUNT_WIDTH DEFAULT_VALUE 1
set_parameter_property BURSTCOUNT_WIDTH DISPLAY_NAME {Burstcount width}
set_parameter_property BURSTCOUNT_WIDTH VISIBLE false
set_parameter_property BURSTCOUNT_WIDTH DERIVED true
set_parameter_property BURSTCOUNT_WIDTH TYPE INTEGER
set_parameter_property BURSTCOUNT_WIDTH UNITS None
set_parameter_property BURSTCOUNT_WIDTH DISPLAY_HINT ""
set_parameter_property BURSTCOUNT_WIDTH AFFECTS_GENERATION false
set_parameter_property BURSTCOUNT_WIDTH HDL_PARAMETER true
set_parameter_property BURSTCOUNT_WIDTH DESCRIPTION {Bridge burstcount width}

add_parameter AVALON_DATA_FIFO_DEPTH INTEGER 5 AVALON_DATA_FIFO_DEPTH
set_parameter_property AVALON_DATA_FIFO_DEPTH DEFAULT_VALUE 4
set_parameter_property AVALON_DATA_FIFO_DEPTH DISPLAY_NAME {Depth of the DATA FIFO}
set_parameter_property AVALON_DATA_FIFO_DEPTH TYPE INTEGER
set_parameter_property AVALON_DATA_FIFO_DEPTH UNITS None
set_parameter_property AVALON_DATA_FIFO_DEPTH DISPLAY_UNITS "Words"
set_parameter_property AVALON_DATA_FIFO_DEPTH ALLOWED_RANGES {2 4 8 16 32 64 128 256 512}
set_parameter_property AVALON_DATA_FIFO_DEPTH DESCRIPTION {Depth of the DATA FIFO}
set_parameter_property AVALON_DATA_FIFO_DEPTH HDL_PARAMETER true

# add_parameter AVALON_DATA_FIFO_WIDTHU INTEGER 
# set_parameter_property AVALON_DATA_FIFO_WIDTHU  DERIVED true
# set_parameter_property AVALON_DATA_FIFO_WIDTHU TYPE INTEGER
# set_parameter_property AVALON_DATA_FIFO_WIDTHU UNITS None
# set_parameter_property AVALON_DATA_FIFO_WIDTHU HDL_PARAMETER true
# add_display_item "" AVALON_DATA_FIFO_WIDTHU PARAMETER

add_parameter LINEWRAPBURSTS INTEGER 0
set_parameter_property LINEWRAPBURSTS DISPLAY_NAME "Line wrap bursts"
set_parameter_property LINEWRAPBURSTS TYPE INTEGER
set_parameter_property LINEWRAPBURSTS AFFECTS_ELABORATION true
set_parameter_property LINEWRAPBURSTS HDL_PARAMETER false
set_parameter_property LINEWRAPBURSTS DISPLAY_HINT BOOLEAN
set_parameter_property LINEWRAPBURSTS AFFECTS_GENERATION false
set_parameter_property LINEWRAPBURSTS DESCRIPTION "This parameter allows you to match the behavior of some memory devices that implement a wrapping burst instead of an incrementing burst. The difference between the two is that with a wrapping burst, when the address reaches a burst boundary, the address wraps back to the previous burst boundary so that only the low-order bits are required for address counting"

add_parameter DEVICE_FAMILY STRING 
set_parameter_property DEVICE_FAMILY SYSTEM_INFO {DEVICE_FAMILY}
set_parameter_property DEVICE_FAMILY DESCRIPTION DEVICE_FAMILY
set_parameter_property DEVICE_FAMILY HDL_PARAMETER true
add_display_item "" DEVICE_FAMILY PARAMETER

add_parameter BYTEEN_WIDTH INTEGER 
set_parameter_property BYTEEN_WIDTH DERIVED true
set_parameter_property BYTEEN_WIDTH TYPE INTEGER
set_parameter_property BYTEEN_WIDTH UNITS None
set_parameter_property BYTEEN_WIDTH HDL_PARAMETER true
add_display_item "" BYTEEN_WIDTH PARAMETER

add_parameter MAX_BURST_SIZE INTEGER 1
set_parameter_property MAX_BURST_SIZE DISPLAY_NAME {Maximum burst size (words)}
set_parameter_property MAX_BURST_SIZE AFFECTS_GENERATION true
set_parameter_property MAX_BURST_SIZE HDL_PARAMETER false
set_parameter_property MAX_BURST_SIZE DESCRIPTION {Specifies the maximum burst size}
set_parameter_property MAX_BURST_SIZE ALLOWED_RANGES "1,2,4,8,16,32,64,128,256,512,1024"

add_parameter ADDRESS_UNITS STRING "SYMBOLS"
set_parameter_property ADDRESS_UNITS DISPLAY_NAME {Address units}
set_parameter_property ADDRESS_UNITS UNITS None
set_parameter_property ADDRESS_UNITS DISPLAY_HINT ""
set_parameter_property ADDRESS_UNITS AFFECTS_GENERATION false
set_parameter_property ADDRESS_UNITS HDL_PARAMETER false
set_parameter_property ADDRESS_UNITS ALLOWED_RANGES "SYMBOLS,WORDS"
set_parameter_property ADDRESS_UNITS DESCRIPTION {Address units (Symbols[bytes]/Words)}

add_parameter MAX_PENDING_RESPONSES INTEGER 32
set_parameter_property MAX_PENDING_RESPONSES DISPLAY_NAME {Maximum pending read transactions}
set_parameter_property MAX_PENDING_RESPONSES TYPE INTEGER
set_parameter_property MAX_PENDING_RESPONSES UNITS None
set_parameter_property MAX_PENDING_RESPONSES DISPLAY_HINT ""
set_parameter_property MAX_PENDING_RESPONSES AFFECTS_GENERATION false
set_parameter_property MAX_PENDING_RESPONSES DESCRIPTION {Controls the Avalon-MM maximum pending read transactions interface property of the bridge}


# | 
# +-----------------------------------

# +-----------------------------------
# | connection point clk
# | 
add_interface clk clock end
add_interface reset reset end

set_interface_property clk ENABLED true
set_interface_property reset ENABLED true
set_interface_property reset ASSOCIATED_CLOCK clk

add_interface_port clk clk clk Input 1
add_interface_port reset reset reset Input 1
# | 
# +-----------------------------------

# 
# connection point s_Global
# 
add_interface s_Global avalon end
set_interface_property s_Global addressUnits WORDS
set_interface_property s_Global associatedClock clk
set_interface_property s_Global associatedReset reset
set_interface_property s_Global bitsPerSymbol 8
set_interface_property s_Global burstOnBurstBoundariesOnly false
set_interface_property s_Global burstcountUnits WORDS
set_interface_property s_Global explicitAddressSpan 0
set_interface_property s_Global holdTime 0
set_interface_property s_Global linewrapBursts false
set_interface_property s_Global maximumPendingReadTransactions 0
set_interface_property s_Global readLatency 0
set_interface_property s_Global readWaitStates 0
set_interface_property s_Global readWaitTime 0
set_interface_property s_Global setupTime 0
set_interface_property s_Global timingUnits Cycles
set_interface_property s_Global writeWaitTime 0
set_interface_property s_Global ENABLED true

add_interface_port s_Global slave_AvalonMonitor_address address Input 6
add_interface_port s_Global slave_AvalonMonitor_read read Input 1
add_interface_port s_Global slave_AvalonMonitor_write write Input 1
add_interface_port s_Global slave_AvalonMonitor_readdata readdata Output 32
add_interface_port s_Global slave_AvalonMonitor_writedata writedata Input 32
add_interface_port s_Global slave_AvalonMonitor_chipselect chipselect Input 1
set_interface_assignment s_Global embeddedsw.configuration.isFlash 0
set_interface_assignment s_Global embeddedsw.configuration.isMemoryDevice 0
set_interface_assignment s_Global embeddedsw.configuration.isNonVolatileStorage 0
set_interface_assignment s_Global embeddedsw.configuration.isPrintableDevice 0



# | 
# +-----------------------------------



proc elaborate { } {

    set data_width     [ get_parameter_value DATA_WIDTH ]
    set sym_width      [ get_parameter_value SYMBOL_WIDTH ]
    set byteen_width   [ expr $data_width / $sym_width ]
    set mprt           [ get_parameter_value MAX_PENDING_RESPONSES ]
    set aunits         [ get_parameter_value ADDRESS_UNITS ]
    set burst_size     [ get_parameter_value MAX_BURST_SIZE ]
    set linewrap       [ get_parameter_value LINEWRAPBURSTS ]
    set address_width  [ get_parameter_value ADDRESS_WIDTH ]
    

    set burstcount_width [ expr int (ceil (log($burst_size) / log(2))) + 1 ]
    set_parameter_value BURSTCOUNT_WIDTH $burstcount_width

    # Internal parameters elaboration
    set rx_fifo_data_depth   [ get_parameter_value AVALON_DATA_FIFO_DEPTH ]
    # set rx_fifo_data_widthu [ expr int (ceil (log($rx_fifo_data_depth) / log(2))) ]
    # set_parameter_value AVALON_DATA_FIFO_WIDTHU $rx_fifo_data_widthu

    set data_width   [ get_parameter_value DATA_WIDTH ]
    set symbol_width [ get_parameter_value SYMBOL_WIDTH ]
    set byteen_width [ expr int (ceil ($data_width / $symbol_width )) ]
    set_parameter_value BYTEEN_WIDTH $byteen_width 

    set Processors [get_parameter_value NProcessors]
    for {set i 1} {$i <= $Processors} {incr i} {   
        # ############################################################
        # +-----------------------------------
        # | connection point s0
        # | 
        add_interface s$i avalon end
        set_interface_property s$i addressAlignment DYNAMIC
        set_interface_property s$i associatedClock clk
        set_interface_property s$i bridgesToMaster m$i
        set_interface_property s$i burstOnBurstBoundariesOnly false
        set_interface_property s$i explicitAddressSpan 0
        set_interface_property s$i holdTime 0
        set_interface_property s$i isMemoryDevice false
        set_interface_property s$i isNonVolatileStorage false
        set_interface_property s$i linewrapBursts false
        set_interface_property s$i maximumPendingReadTransactions 4
        set_interface_property s$i printableDevice false
        set_interface_property s$i readLatency 0
        set_interface_property s$i readWaitTime 0
        set_interface_property s$i setupTime 0
        set_interface_property s$i timingUnits Cycles
        set_interface_property s$i writeWaitTime 0

        set_interface_property s$i ASSOCIATED_CLOCK clk
        set_interface_property s$i associatedReset reset
        set_interface_property s$i ENABLED true

        add_interface_port s$i s${i}_waitrequest waitrequest Output 1
        add_interface_port s$i s${i}_readdata readdata Output DATA_WIDTH
        add_interface_port s$i s${i}_readdatavalid readdatavalid Output 1
        add_interface_port s$i s${i}_burstcount burstcount Input BURSTCOUNT_WIDTH
        add_interface_port s$i s${i}_writedata writedata Input DATA_WIDTH
        add_interface_port s$i s${i}_address address Input address_width
        add_interface_port s$i s${i}_write write Input 1
        add_interface_port s$i s${i}_read read Input 1
        add_interface_port s$i s${i}_byteenable byteenable Input 4
        add_interface_port s$i s${i}_debugaccess debugaccess Input 1

        # +-----------------------------------
        # | connection point m0
        # | 
        add_interface m$i avalon start
        set_interface_property m$i associatedClock clk
        set_interface_property m$i burstOnBurstBoundariesOnly false
        set_interface_property m$i doStreamReads false
        set_interface_property m$i doStreamWrites false
        set_interface_property m$i linewrapBursts false

        set_interface_property m$i ASSOCIATED_CLOCK clk
        set_interface_property m$i associatedReset reset
        set_interface_property m$i ENABLED true

        add_interface_port m$i m${i}_waitrequest waitrequest Input 1
        add_interface_port m$i m${i}_readdata readdata Input DATA_WIDTH
        add_interface_port m$i m${i}_readdatavalid readdatavalid Input 1
        add_interface_port m$i m${i}_burstcount burstcount Output BURSTCOUNT_WIDTH
        add_interface_port m$i m${i}_writedata writedata Output DATA_WIDTH
        add_interface_port m$i m${i}_address address Output ADDRESS_WIDTH
        add_interface_port m$i m${i}_write write Output 1
        add_interface_port m$i m${i}_read read Output 1
        add_interface_port m$i m${i}_byteenable byteenable Output 4
        add_interface_port m$i m${i}_debugaccess debugaccess Output 1
        # | 
        # +-----------------------------------


        # ############################################################

        set_port_property m${i}_byteenable WIDTH $byteen_width
        set_port_property s${i}_byteenable WIDTH $byteen_width

        set_interface_property m${i} bitsPerSymbol $sym_width
        set_interface_property s${i} bitsPerSymbol $sym_width

        set_interface_property m$i addressUnits $aunits
        set_interface_property s$i addressUnits $aunits

        set_interface_property s$i maximumPendingReadTransactions $mprt

        set_interface_property m$i linewrapBursts $linewrap
        set_interface_property s$i linewrapBursts $linewrap
    }



}
