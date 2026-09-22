# +-----------------------------------
# | 
# | mddr_ctrl "mobile DDR controller" v1.0
# | Harald Fluegel, Arrow Central Europe GmbH 2011.07.01.11:14:54
# | simple mobile DDR SDRAM controller for BeMicro SDK
# | 
# +-----------------------------------

# +-----------------------------------
# | request TCL package from ACDS 10.1
# | 
package require -exact sopc 10.1
# | 
# +-----------------------------------

# +-----------------------------------
# | module mddr_ctrl
# | 
set_module_property DESCRIPTION "simple mobile DDR SDRAM controller for BeMicro SDK"
set_module_property NAME mddr_ctrl
set_module_property VERSION 1.0
set_module_property INTERNAL false
set_module_property OPAQUE_ADDRESS_MAP true
set_module_property AUTHOR "Harald Fluegel, Arrow Central Europe GmbH"
set_module_property DISPLAY_NAME "MDDR controller"
set_module_property TOP_LEVEL_HDL_FILE mddr_ctrl.v
set_module_property TOP_LEVEL_HDL_MODULE mddr_ctrl
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true
set_module_property ANALYZE_HDL TRUE
# | 
# +-----------------------------------

# +-----------------------------------
# | files
# | 
add_file mddr_ctrl.v {SYNTHESIS SIMULATION}
# | 
# +-----------------------------------

# +-----------------------------------
# | parameters
# | 
add_parameter pRefreshCount INTEGER 390
set_parameter_property pRefreshCount DEFAULT_VALUE 390
set_parameter_property pRefreshCount DISPLAY_NAME pRefreshCount
set_parameter_property pRefreshCount TYPE INTEGER
set_parameter_property pRefreshCount UNITS None
set_parameter_property pRefreshCount AFFECTS_GENERATION false
set_parameter_property pRefreshCount HDL_PARAMETER true
# | 
# +-----------------------------------

# +-----------------------------------
# | display items
# | 
# | 
# +-----------------------------------

# +-----------------------------------
# | connection point clock
# | 
add_interface clock clock end
set_interface_property clock clockRate 0
set_interface_property clock ENABLED true
add_interface_port clock clk clk Input 1
# | 
# +-----------------------------------

# +-----------------------------------
# | connection point clock_reset
# | 
add_interface clock_reset reset end
set_interface_property clock_reset associatedClock clock
set_interface_property clock_reset synchronousEdges DEASSERT
set_interface_property clock_reset ENABLED true
add_interface_port clock_reset rst reset Input 1
# | 
# +-----------------------------------

# +-----------------------------------
# | connection point avalon_slave
# | 
add_interface avalon_slave avalon end
set_interface_property avalon_slave addressAlignment DYNAMIC
set_interface_property avalon_slave addressUnits WORDS
set_interface_property avalon_slave associatedClock clock
set_interface_property avalon_slave associatedReset clock_reset
set_interface_property avalon_slave burstOnBurstBoundariesOnly false
set_interface_property avalon_slave explicitAddressSpan 0
set_interface_property avalon_slave holdTime 0
set_interface_property avalon_slave isMemoryDevice 1
set_interface_property avalon_slave isNonVolatileStorage false
set_interface_property avalon_slave linewrapBursts false
set_interface_property avalon_slave maximumPendingReadTransactions 0
set_interface_property avalon_slave printableDevice false
set_interface_property avalon_slave readLatency 0
# readWaitTime was 1
set_interface_property avalon_slave readWaitTime 50
set_interface_property avalon_slave setupTime 0
set_interface_property avalon_slave timingUnits Cycles
# writeWaitTime was 0
set_interface_property avalon_slave writeWaitTime 50
set_interface_property avalon_slave ENABLED true
add_interface_port avalon_slave read read Input 1
add_interface_port avalon_slave readdata readdata Output 32
add_interface_port avalon_slave write write Input 1
add_interface_port avalon_slave writedata writedata Input 32
add_interface_port avalon_slave byteenable byteenable Input 4
add_interface_port avalon_slave begintransfer begintransfer Input 1
add_interface_port avalon_slave waitrequest waitrequest Output 1
add_interface_port avalon_slave address address Input 24
# | 
# +-----------------------------------

# +-----------------------------------
# | connection point mddr
# | 
add_interface mddr conduit end
set_interface_property mddr ENABLED true
add_interface_port mddr RAM_A0 export Output 1
add_interface_port mddr RAM_A1 export Output 1
add_interface_port mddr RAM_A2 export Output 1
add_interface_port mddr RAM_A3 export Output 1
add_interface_port mddr RAM_A4 export Output 1
add_interface_port mddr RAM_A5 export Output 1
add_interface_port mddr RAM_A6 export Output 1
add_interface_port mddr RAM_A7 export Output 1
add_interface_port mddr RAM_A8 export Output 1
add_interface_port mddr RAM_A9 export Output 1
add_interface_port mddr RAM_A10 export Output 1
add_interface_port mddr RAM_A11 export Output 1
add_interface_port mddr RAM_A12 export Output 1
add_interface_port mddr RAM_A13 export Output 1
add_interface_port mddr RAM_BA0 export Output 1
add_interface_port mddr RAM_BA1 export Output 1
add_interface_port mddr RAM_CK_N export Output 1
add_interface_port mddr RAM_CK_P export Output 1
add_interface_port mddr RAM_CKE export Output 1
add_interface_port mddr RAM_CS_N export Output 1
add_interface_port mddr RAM_WS_N export Output 1
add_interface_port mddr RAM_RAS_N export Output 1
add_interface_port mddr RAM_CAS_N export Output 1
add_interface_port mddr RAM_LDM export Output 1
add_interface_port mddr RAM_UDM	export Output 1
add_interface_port mddr RAM_D0 export bidir 1
add_interface_port mddr RAM_D1 export bidir 1
add_interface_port mddr RAM_D2 export bidir 1
add_interface_port mddr RAM_D3 export bidir 1
add_interface_port mddr RAM_D4 export bidir 1
add_interface_port mddr RAM_D5 export bidir 1
add_interface_port mddr RAM_D6 export bidir 1
add_interface_port mddr RAM_D7 export bidir 1
add_interface_port mddr RAM_D8 export bidir 1
add_interface_port mddr RAM_D9 export bidir 1
add_interface_port mddr RAM_D10 export bidir 1
add_interface_port mddr RAM_D11 export bidir 1
add_interface_port mddr RAM_D12 export bidir 1
add_interface_port mddr RAM_D13 export bidir 1
add_interface_port mddr RAM_D14 export bidir 1
add_interface_port mddr RAM_D15 export bidir 1
add_interface_port mddr RAM_LDQS export bidir 1
add_interface_port mddr RAM_UDQS export bidir 1

# | 
# +-----------------------------------
