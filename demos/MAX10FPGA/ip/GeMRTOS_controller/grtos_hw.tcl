##
## \file
## \author Ricardo Cayssials
## \brief GeMRTOS controller Platform Designer component descriptor (Nios II/Nios V)
## \details Defines the gemrtos_controller Qsys component: parameters, interfaces,
##          and the validate{}/elaborate{} callbacks used during system generation.
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

#
# request TCL package from ACDS 12.1
#
package require -exact qsys 13.0


# 
# module grtos
# 
set_module_property DESCRIPTION "GeMRTOS controller"
set_module_property NAME gemrtos_controller
set_module_property VERSION 1.0
set_module_property INTERNAL true
set_module_property OPAQUE_ADDRESS_MAP true
set_module_property GROUP GeMRTOS
set_module_property AUTHOR "Ricardo Cayssials"
set_module_property DISPLAY_NAME "GeMRTOS controller"
set_module_property ICON_PATH logoGeMRTOS.png
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true
set_module_property ANALYZE_HDL AUTO
set_module_property REPORT_TO_TALKBACK false
set_module_property HIDE_FROM_SOPC true
set_module_property ALLOW_GREYBOX_GENERATION false


# 
# file sets
# 
add_fileset quartus_synth QUARTUS_SYNTH "" "Quartus Synthesis"
set_fileset_property quartus_synth TOP_LEVEL grtos
set_fileset_property quartus_synth ENABLE_RELATIVE_INCLUDE_PATHS false
add_fileset_file grtos.vhd VHDL PATH grtos.vhd
add_fileset_file grtos.sdc SDC PATH grtos.sdc


add_fileset sim_verilog SIM_VERILOG "" "Verilog Simulation"
set_fileset_property sim_verilog TOP_LEVEL grtos
set_fileset_property sim_verilog ENABLE_RELATIVE_INCLUDE_PATHS false
add_fileset_file grtos.vhd VERILOG PATH grtos.vhd
add_fileset_file grtos.sdc SDC PATH grtos.sdc

add_fileset sim_vhdl SIM_VHDL "" "VHDL Simulation"
set_fileset_property sim_vhdl TOP_LEVEL grtos
set_fileset_property sim_vhdl ENABLE_RELATIVE_INCLUDE_PATHS false
add_fileset_file grtos.vhd VHDL PATH grtos.vhd
add_fileset_file grtos.sdc SDC PATH grtos.sdc


# 
# parameters
# 
# Number of Processors
# MAX_PROCESSORS is the single source of truth for this component's processor
# limit -- it drives both NProcessors' allowed range below and the per-processor
# interface generation loop in elaborate{} (referenced there as $::MAX_PROCESSORS).
set MAX_PROCESSORS 32
add_parameter NProcessors INTEGER 1 "Number of System Processors"
set_parameter_property NProcessors DEFAULT_VALUE 1
set_parameter_property NProcessors DISPLAY_NAME "Number of Processors"
set_parameter_property NProcessors TYPE INTEGER
set_parameter_property NProcessors UNITS None
set_parameter_property NProcessors ALLOWED_RANGES "1:${MAX_PROCESSORS}"
set_parameter_property NProcessors DESCRIPTION "Number of System Processors"
set_parameter_property NProcessors HDL_PARAMETER true

# Prescaler 
add_parameter PreScale INTEGER 31 "Time Prescaler"
set_parameter_property PreScale DEFAULT_VALUE 31
set_parameter_property PreScale DISPLAY_NAME "Time Prescaler"
set_parameter_property PreScale TYPE INTEGER
set_parameter_property PreScale UNITS None
set_parameter_property PreScale ALLOWED_RANGES {1:1073741824}
set_parameter_property PreScale DESCRIPTION "Time Scale division"
set_parameter_property PreScale HDL_PARAMETER false

# GeMRTOS version
add_parameter GEMRTOS_VERSION_INTEGER INTEGER 1 "GeMRTOS version"
set_parameter_property GEMRTOS_VERSION_INTEGER DEFAULT_VALUE 1
set_parameter_property GEMRTOS_VERSION_INTEGER DISPLAY_NAME "GeMRTOS version"
set_parameter_property GEMRTOS_VERSION_INTEGER TYPE INTEGER
set_parameter_property GEMRTOS_VERSION_INTEGER UNITS None
set_parameter_property GEMRTOS_VERSION_INTEGER DESCRIPTION "GeMRTOS version"
set_parameter_property GEMRTOS_VERSION_INTEGER HDL_PARAMETER true


add_parameter PROCESSOR_TYPE STRING "" "Processor type"
set_parameter_property PROCESSOR_TYPE HDL_PARAMETER false

add_parameter INSTRUCTION_CACHE_SIZE STRING "None" "Instruction cache size"
set_parameter_property INSTRUCTION_CACHE_SIZE HDL_PARAMETER false

add_parameter INSTRUCTION_CACHE_BURST STRING "Disable" "Add burstcount signal to instruction_master"
set_parameter_property INSTRUCTION_CACHE_BURST HDL_PARAMETER false

add_parameter ENABLE_HPS_MAP_ACCESS BOOLEAN false
set_parameter_property ENABLE_HPS_MAP_ACCESS HDL_PARAMETER false

add_parameter NIOS_CLOCK_FREQUENCY INTEGER
set_parameter_property NIOS_CLOCK_FREQUENCY HDL_PARAMETER false

add_parameter BUS_CLOCK_FREQUENCY INTEGER 
set_parameter_property BUS_CLOCK_FREQUENCY HDL_PARAMETER false

add_parameter BUS_WIDTH INTEGER
set_parameter_property BUS_WIDTH HDL_PARAMETER false

add_parameter EXTERNAL_MEMORY_SPAN INTEGER 4
set_parameter_property EXTERNAL_MEMORY_SPAN HDL_PARAMETER false

add_parameter EXTERNAL_MEMORY_UNIT STRING ""
set_parameter_property EXTERNAL_MEMORY_UNIT HDL_PARAMETER false

add_parameter INDEPENDENT_EXTERNAL_PROCESSOR_BUSES BOOLEAN true
set_parameter_property INDEPENDENT_EXTERNAL_PROCESSOR_BUSES HDL_PARAMETER false

add_parameter INCLUDE_JTAG_UART_STDDIO BOOLEAN true
set_parameter_property INCLUDE_JTAG_UART_STDDIO HDL_PARAMETER false

add_parameter INCLUDE_JTAG_UART_STDERR BOOLEAN true
set_parameter_property INCLUDE_JTAG_UART_STDERR HDL_PARAMETER false

add_parameter INCLUDE_JTAG_UART_PROCESSOR BOOLEAN true
set_parameter_property INCLUDE_JTAG_UART_PROCESSOR HDL_PARAMETER false




# 
# display items
# 
add_display_item "" "System Properties" GROUP ""
add_display_item "" Interrupt GROUP ""

# 
# connection point clock_reset
# 
add_interface clock_reset clock end
set_interface_property clock_reset clockRate 0
set_interface_property clock_reset ENABLED true

add_interface_port clock_reset clk clk Input 1

add_parameter GRTOS_CLOCK_FREQUENCY INTEGER 
set_parameter_property GRTOS_CLOCK_FREQUENCY SYSTEM_INFO {CLOCK_RATE clock_reset}
add_display_item "" GRTOS_CLOCK_FREQUENCY PARAMETER
set_parameter_property GRTOS_CLOCK_FREQUENCY HDL_PARAMETER false

add_parameter GRTOS_CLOCK_RESET_INFO STRING 
set_parameter_property GRTOS_CLOCK_RESET_INFO SYSTEM_INFO {CLOCK_RESET_INFO clock_reset}
add_display_item "" GRTOS_CLOCK_RESET_INFO PARAMETER
set_parameter_property GRTOS_CLOCK_RESET_INFO HDL_PARAMETER false

add_parameter GRTOS_GENERATION_ID INTEGER 
set_parameter_property GRTOS_GENERATION_ID SYSTEM_INFO {GENERATION_ID}
add_display_item "" GRTOS_GENERATION_ID PARAMETER
set_parameter_property GRTOS_GENERATION_ID HDL_PARAMETER false
# 
# connection point clock_reset_reset
# 
add_interface clock_reset_reset reset end
set_interface_property clock_reset_reset associatedClock clock_reset
set_interface_property clock_reset_reset synchronousEdges DEASSERT
set_interface_property clock_reset_reset ENABLED true

add_interface_port clock_reset_reset reset reset Input 1

# *************************************************************
# *************************************************************
# 
# connection point clk_out
# 
add_interface clk_out clock start
set_interface_property clk_out associatedDirectClock ""
set_interface_property clk_out clockRate 0
set_interface_property clk_out ENABLED true
set_interface_property clk_out EXPORT_OF ""
set_interface_property clk_out PORT_NAME_MAP ""
set_interface_property clk_out SVD_ADDRESS_GROUP ""

add_interface_port clk_out clk_out clk Output 1

# GeMRTOS Avalon-MM slave interfaces (s_Global, s_GMemory, s_Testing) share
# identical timing/property conventions and only differ in interface name,
# port-name prefix (must match the exact signal names in grtos.vhd), and
# address width, so one procedure covers all three.
proc add_gemrtos_avalon_slave {name port_prefix addr_width} {
    add_interface $name avalon end
    set_interface_property $name addressUnits WORDS
    set_interface_property $name associatedClock clock_reset
    set_interface_property $name associatedReset clock_reset_reset
    set_interface_property $name bitsPerSymbol 8
    set_interface_property $name burstOnBurstBoundariesOnly false
    set_interface_property $name burstcountUnits WORDS
    set_interface_property $name explicitAddressSpan 0
    set_interface_property $name holdTime 0
    set_interface_property $name linewrapBursts false
    set_interface_property $name maximumPendingReadTransactions 0
    set_interface_property $name readLatency 1
    set_interface_property $name readWaitStates 0
    set_interface_property $name readWaitTime 0
    set_interface_property $name setupTime 0
    set_interface_property $name timingUnits Cycles
    set_interface_property $name writeWaitTime 0
    set_interface_property $name ENABLED true

    add_interface_port $name ${port_prefix}address address Input $addr_width
    add_interface_port $name ${port_prefix}read read Input 1
    add_interface_port $name ${port_prefix}write write Input 1
    add_interface_port $name ${port_prefix}readdata readdata Output 32
    add_interface_port $name ${port_prefix}writedata writedata Input 32
    add_interface_port $name ${port_prefix}waitrequest waitrequest Output 1
    set_interface_assignment $name embeddedsw.configuration.isFlash 0
    set_interface_assignment $name embeddedsw.configuration.isMemoryDevice 0
    set_interface_assignment $name embeddedsw.configuration.isNonVolatileStorage 0
    set_interface_assignment $name embeddedsw.configuration.isPrintableDevice 0
}

#
# connection point s_Global
#
add_gemrtos_avalon_slave s_Global slave_gRTOS_ 3

#
# connection point s_GMemory
#
add_gemrtos_avalon_slave s_GMemory slave_gRTOS_Memory_ 4

#
# connection point s_Testing
#
add_gemrtos_avalon_slave s_Testing slave_gRTOS_Testing_ 4


# ---------------------------------------------------------------------------
# grtos_reset_req_roles -- conduit role names of the per-processor reset-request
# handshake, for the given processor family.
#
# Platform Designer matches conduit interfaces by ROLE name, and the two
# supported families name the same two signals differently:
#
#   Nios II   cpu_resetrequest_conduit : cpu_resetrequest / cpu_resettaken
#   Nios V    cpu_resetreq             : resetreq         / ack
#
# The list is exhaustive on purpose. An unrecognised PROCESSOR_TYPE is a hard
# error, never a silent default: wiring the wrong roles would either fail to
# connect or, worse, connect a processor family whose handshake semantics have
# not been reviewed. A newly supported family must be added here explicitly.
#
# Returns a two-element list {request_role acknowledge_role}. A value is still
# returned after an error so the caller does not fault before Platform Designer
# reports the message.
# ---------------------------------------------------------------------------
proc grtos_reset_req_roles { procType } {
    switch -exact -- $procType {
        "Nios II/e (economy)" -
        "Nios II/f (full)" {
            return [list "cpu_resetrequest" "cpu_resettaken"]
        }
        "Nios V/m" -
        "Nios V/g" {
            return [list "resetreq" "ack"]
        }
        "" {
            send_message error "GeMRTOS controller: PROCESSOR_TYPE is empty. The\
                controller is not meant to be instantiated on its own -- add it\
                through the GeMRTOS_Multiprocessor component, which supplies\
                this parameter."
            return [list "resetreq" "ack"]
        }
        default {
            send_message error "GeMRTOS controller: unsupported PROCESSOR_TYPE\
                '$procType'. Supported values are 'Nios II/e (economy)',\
                'Nios II/f (full)', 'Nios V/m' and 'Nios V/g'. To add a new\
                processor family, extend grtos_reset_req_roles in grtos_hw.tcl\
                with the role names of its reset-request conduit."
            return [list "resetreq" "ack"]
        }
    }
}


set_module_property VALIDATION_CALLBACK validate
set_module_property ELABORATION_CALLBACK elaborate

proc validate {} {

  # Validate PROCESSOR_TYPE here, not only where the roles are consumed in
  # elaborate{}, so an unsupported family is reported in the parameter editor
  # instead of surfacing later during generation. Same single source of truth;
  # the returned roles are irrelevant at this point.
  grtos_reset_req_roles [get_parameter_value PROCESSOR_TYPE]

  set_module_assignment embeddedsw.CMacro.NPROCESSORS [get_parameter_value NProcessors]
  set_module_assignment embeddedsw.CMacro.PRESCALE [get_parameter_value PreScale]
  set_module_assignment embeddedsw.CMacro.GRTOSFREQUENCY [get_parameter_value GRTOS_CLOCK_FREQUENCY]
  set_module_assignment embeddedsw.CMacro.GRTOSCLOCKRESETINFO [get_parameter_value GRTOS_CLOCK_RESET_INFO]
  set_module_assignment embeddedsw.CMacro.GRTOSGENERATIONID [get_parameter_value GRTOS_GENERATION_ID]
  
  set_module_assignment embeddedsw.CMacro.PROCESSOR_TYPE [get_parameter_value PROCESSOR_TYPE]
  set_module_assignment embeddedsw.CMacro.INSTRUCTION_CACHE_SIZE [get_parameter_value INSTRUCTION_CACHE_SIZE]
  set_module_assignment embeddedsw.CMacro.INSTRUCTION_CACHE_BURST [get_parameter_value INSTRUCTION_CACHE_BURST]
  set_module_assignment embeddedsw.CMacro.ENABLE_HPS_MAP_ACCESS [get_parameter_value ENABLE_HPS_MAP_ACCESS]
  set_module_assignment embeddedsw.CMacro.NIOS_CLOCK_FREQUENCY [get_parameter_value NIOS_CLOCK_FREQUENCY]
  set_module_assignment embeddedsw.CMacro.BUS_CLOCK_FREQUENCY [get_parameter_value BUS_CLOCK_FREQUENCY]
  set_module_assignment embeddedsw.CMacro.BUS_WIDTH [get_parameter_value BUS_WIDTH]
  set_module_assignment embeddedsw.CMacro.EXTERNAL_MEMORY_SPAN [get_parameter_value EXTERNAL_MEMORY_SPAN]
  set_module_assignment embeddedsw.CMacro.EXTERNAL_MEMORY_UNIT [get_parameter_value EXTERNAL_MEMORY_UNIT]
  set_module_assignment embeddedsw.CMacro.INDEPENDENT_EXTERNAL_PROCESSOR_BUSES [get_parameter_value INDEPENDENT_EXTERNAL_PROCESSOR_BUSES]
  set_module_assignment embeddedsw.CMacro.INCLUDE_JTAG_UART_STDDIO [get_parameter_value INCLUDE_JTAG_UART_STDDIO]
  set_module_assignment embeddedsw.CMacro.INCLUDE_JTAG_UART_STDERR [get_parameter_value INCLUDE_JTAG_UART_STDERR]
  set_module_assignment embeddedsw.CMacro.INCLUDE_JTAG_UART_PROCESSOR [get_parameter_value INCLUDE_JTAG_UART_PROCESSOR]


 

}

proc elaborate {} {

# 
# connection point bus_internal
# 
add_interface bus_internal conduit end
set_interface_property bus_internal associatedClock clock_reset
set_interface_property bus_internal associatedReset clock_reset_reset
set_interface_property bus_internal ENABLED true
set_interface_property bus_internal EXPORT_OF ""
set_interface_property bus_internal PORT_NAME_MAP ""
set_interface_property bus_internal SVD_ADDRESS_GROUP ""

# Monitor interface
add_interface_port bus_internal frozen_avalon_monitor_controller export Input 1
add_interface_port bus_internal frozen_avalon_controller_monitor export Output 1


# 
# connection point phy
# 
add_interface phy conduit end
set_interface_property phy associatedClock clock_reset
set_interface_property phy associatedReset clock_reset_reset
set_interface_property phy ENABLED true
set_interface_property phy EXPORT_OF ""
set_interface_property phy PORT_NAME_MAP ""
set_interface_property phy SVD_ADDRESS_GROUP ""

#Output and input port interface
add_interface_port phy GEMRTOS_OUTS GEMRTOS_OUTS Output 32
add_interface_port phy GEMRTOS_INS GEMRTOS_INS Input 32 

# ##########################################################
    # Interrupts receive signals
    for { set s 0 } { $s < 32 } { incr s } {
        add_interface "dirq${s}_receiver" interrupt receiver
        set_interface_property "dirq${s}_receiver" irqScheme individualRequests
        add_interface_port "dirq${s}_receiver" dirq${s}_receiver irq input 1
        set_interface_property "dirq${s}_receiver" associatedClock "clock_reset"
        set_interface_property "dirq${s}_receiver" associatedReset "clock_reset_reset"
        set_interface_assignment "dirq${s}_receiver" embeddedsw.configuration.isInterruptControllerReceiver 1
    }
# ##########################################################

# Conduit role names of the per-processor reset-request handshake. The mapping
# is exhaustive and errors on an unsupported family -- see grtos_reset_req_roles.
lassign [grtos_reset_req_roles [get_parameter_value PROCESSOR_TYPE]] ROLE_REQ ROLE_ACK

set Processors [get_parameter_value NProcessors]
    for {set i 1} {$i <= $::MAX_PROCESSORS} {incr i} {
		# 
		# connection point s_processor$i for each processor defined
		# 
		add_interface s_processor$i avalon end
		set_interface_property s_processor$i addressUnits WORDS
		set_interface_property s_processor$i associatedClock clock_reset
		set_interface_property s_processor$i associatedReset clock_reset_reset
		set_interface_property s_processor$i bitsPerSymbol 8
		set_interface_property s_processor$i burstOnBurstBoundariesOnly false
		set_interface_property s_processor$i burstcountUnits WORDS
		set_interface_property s_processor$i explicitAddressSpan 0
		set_interface_property s_processor$i holdTime 0
		set_interface_property s_processor$i linewrapBursts false
		set_interface_property s_processor$i maximumPendingReadTransactions 0
		set_interface_property s_processor$i readLatency 0
		set_interface_property s_processor$i readWaitStates 0
		set_interface_property s_processor$i readWaitTime 0
		set_interface_property s_processor$i setupTime 0
		set_interface_property s_processor$i timingUnits Cycles
		set_interface_property s_processor$i writeWaitTime 0

		add_interface_port s_processor$i slave_processor_address$i address Input 2
		add_interface_port s_processor$i slave_processor_read$i read Input 1
		add_interface_port s_processor$i slave_processor_write$i write Input 1
		add_interface_port s_processor$i slave_processor_readdata$i readdata Output 32
		add_interface_port s_processor$i slave_processor_writedata$i writedata Input 32
		add_interface_port s_processor$i slave_processor_waitrequest$i waitrequest Output 1 
		set_interface_assignment s_processor$i embeddedsw.configuration.isFlash 0
		set_interface_assignment s_processor$i embeddedsw.configuration.isMemoryDevice 0
		set_interface_assignment s_processor$i embeddedsw.configuration.isNonVolatileStorage 0
		set_interface_assignment s_processor$i embeddedsw.configuration.isPrintableDevice 0
		
		
		# ########################################## 
		# 
		# connection point slv_rst(1)
		# 
		add_interface slv_rst$i reset start
		set_interface_property slv_rst$i associatedClock clock_reset
		set_interface_property slv_rst$i associatedResetSinks clock_reset_reset
		set_interface_property slv_rst$i synchronousEdges DEASSERT

		add_interface_port slv_rst$i slave_rst$i reset Output 1
        
		# ########################################## 
		# 
		# connection point cpu_rstreq(i) -- handshake de reset por procesador
		#         
        # NOTE: deliberately NO associatedClock / associatedReset here.
        # The Nios II (cpu_resetrequest_conduit) and Nios V (cpu_resetreq)
        # reset-request conduits declare no associated clock either, and
        # Platform Designer rejects a connection whose two ends disagree
        # ("must be on the same clock domain, but ... has no associated
        # clock").  There is no clock crossing in practice: the controller
        # and every processor are driven from clock_bridge_0.out_clk.
        add_interface cpu_rstreq$i conduit end

        add_interface_port cpu_rstreq$i prc_rstreq$i  $ROLE_REQ Output 1
        add_interface_port cpu_rstreq$i prc_rstack$i  $ROLE_ACK Input  1
        
		# ##########################################        
		# 
		# connection point slv_irq(1)
		# 
		add_interface slv_irq$i interrupt end
		set_interface_property slv_irq$i associatedAddressablePoint s_processor$i
		set_interface_property slv_irq$i associatedClock clock_reset
		set_interface_property slv_irq$i associatedReset clock_reset_reset

		add_interface_port slv_irq$i slave_irq$i irq Output 1
			
		if { ($i <= $Processors) } {
			set_interface_property s_processor$i ENABLED true
			set_interface_property slv_irq$i ENABLED true
			set_interface_property slv_rst$i ENABLED true
            set_interface_property cpu_rstreq$i ENABLED true            
		} else {
			set_interface_property s_processor$i ENABLED false
			set_interface_property slv_irq$i ENABLED false
			set_interface_property slv_rst$i ENABLED false
            set_interface_property cpu_rstreq$i ENABLED false
		}
    }   
}
