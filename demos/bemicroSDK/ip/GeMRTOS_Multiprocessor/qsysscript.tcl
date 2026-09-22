##
##  \file
##  \author Ricardo Cayssials
##  \brief GeMRTOS Nios V Platform Designer system construction script
##  \version 1.0
##  \date      2013-2026
##
##  SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
##  Copyright (c) 2013-2026 GeMRTOS Project Contributors
##
##  Required Notice: Copyright GeMRTOS Project Contributors (https://gemrtos.com)
##
##  This file is part of GeMRTOS, licensed under the PolyForm Noncommercial
##  License 1.0.0. You may use, copy, modify, and distribute this file for
##  any noncommercial purpose. Commercial use requires a separate written
##  license from GeMRTOS. Full terms:
##  https://polyformproject.org/licenses/noncommercial/1.0.0
##

##
## \file
## \brief GeMRTOS Nios V Platform Designer system configuration extraction script (Quartus Prime Standard)
## \details Run via: qsys-script --system-file=<project>.qsys --script=qsysscript.tcl
##   Enumerates Qsys instances to locate the GeMRTOS_Multiprocessor component, identifies
##   the main processor (composed instance matching GEMRTOS_PROC_SUBINSTANCE) and its
##   JTAG UART sub-instances, then substitutes those names as placeholders into
##   bsp_niosv_settings.tcl and processor_set.bat (both created earlier by Platform
##   Designer's validate{} callback). See ip/README.md for the full instance-naming
##   contract this script depends on.
## \note Quartus Prime Standard only. The Pro counterpart is qsysscript_pro.tcl.
## \version 1.0
## \date 2026-07-14
##

package require -exact qsys 18.0


# === Constants: expected Qsys component/instance naming pattern ===
# Contract: the system must contain a GeMRTOS_Multiprocessor-family component with:
#   1. A composed instance named "<instance>_${GEMRTOS_PROC_SUBINSTANCE}" (main processor)
#   2. Two JTAG UART composed instances (stdio / stderr)
#   3. Optionally an altera_eth_tse instance (enables the lwIP software package)
set GEMRTOS_PROC_SUBINSTANCE   "gemrtos_proc_1"
set GEMRTOS_JTAG_UART_MARKER   "jtag_uart_0"
set GEMRTOS_ETH_CLASS          "altera_eth_tse"
set GEMRTOS_HPS_BRIDGE_MARKER  "mm_clock_crossing_bridge_0"
set GEMRTOS_HW_INSTANCE_MARKER "grtos_0"

set project_name [get_module_property name]
puts "Project name= ${project_name}"
puts "Project properties [get_project_properties]"

set lwip_package_niosII ""
set lwip_package_niosV ""

set generate_hps_bsp 0
set fp [open "logging.log" w+]
	# properties AUTHOR AUTO_EXPORT CLASS_NAME DESCRIPTION DISPLAY_NAME EDITABLE ENABLED GROUP INTERNAL NAME VERSION

    # === Enumerate instances and locate the GeMRTOS processor / JTAG UARTs / Ethernet ===
    set instances [ get_instances ]
    foreach instance $instances {
        puts $fp "Instance"
        puts $fp $instance
		puts $fp "CLASS_NAME [ get_instance_property $instance CLASS_NAME ] "
		set class_name [ get_instance_property $instance CLASS_NAME ]
		if { [ string equal $GEMRTOS_ETH_CLASS $class_name ] } {
			set lwip_package_niosII "--cmd \"enable_sw_package lwip\" "
			set lwip_package_niosV " enable_sw_package lwip "
		}
        foreach subinstance [get_composed_instances $instance] {
            puts $fp "subinstance"
            puts $fp $subinstance        
            if { [ string equal $GEMRTOS_PROC_SUBINSTANCE $subinstance ] } {
                set main_processor ${instance}_${subinstance}
            }
            if { [ string first $GEMRTOS_JTAG_UART_MARKER $subinstance ] != -1} {
                set stdio ${instance}_jtag_uart_0
                set stderr ${instance}_jtag_uart_1
            }
            if { [ string equal $GEMRTOS_HPS_BRIDGE_MARKER $subinstance ] } {
                set generate_hps_bsp 1
            }
			if { [ string equal $GEMRTOS_HW_INSTANCE_MARKER $subinstance ] } {
				set PROCESSOR_TYPE [ get_composed_instance_assignment $instance $subinstance embeddedsw.CMacro.PROCESSOR_TYPE ]
			}
        }
    }
    
close $fp
    
 


set fp [open "grtos_hps_bsp_create.sh" w+]
    puts $fp "#!> /bin/bash"
    puts $fp "# Automatically created from qsys-script with qsysscript.tcl"
    if { $generate_hps_bsp  == 1 } {
        puts $fp "bash create_hps_bsp.sh \$2 \$3 \$4 \$5 \$1 \$6 \$7"
    } else {
        puts $fp "echo \"No HPS module to produce\""
    }
close $fp


if { ($PROCESSOR_TYPE == "Nios II/e (economy)") || ($PROCESSOR_TYPE == "Nios II/f (full)") } {
	set fp [open "grtos_nios_bsp_create.sh" w+]
		puts $fp "nios2-bsp hal ./\$1 ${project_name}.sopcinfo --cpu-name ${main_processor} --cmd \"set_setting hal.stderr ${stderr}\" --cmd \"set_setting hal.stdin ${stdio}\" --cmd \"set_setting hal.stdout ${stdio}\" --cmd \"set_setting hal.enable_reduced_device_drivers true\" $lwip_package_niosII "
	close $fp
puts "The command to create/modify the BSP setting files is:"
puts "nios2-bsp hal ./\$1 ${project_name}.sopcinfo --cpu-name ${main_processor} --cmd \"set_setting hal.stderr ${stderr}\" --cmd \"set_setting hal.stdin ${stdio}\" --cmd \"set_setting hal.stdout ${stdio}\" --cmd \"set_setting hal.enable_reduced_device_drivers true\" "
}

if { ($PROCESSOR_TYPE == "Nios V/m") || ($PROCESSOR_TYPE == "Nios V/g") } {
	set fp [open "grtos_nios_bsp_create.sh" w+]
		fconfigure $fp -translation lf
		
		puts $fp "#!/bin/bash"
		puts $fp " "
		puts $fp "FULL_BSP_DIR=\$1"
		puts $fp "NIOS_PATH=\$2"
		puts $fp "FULL_APP_DIR=\$3"
		puts $fp "CMAKE_PATH=\$4"
		puts $fp " "
		puts $fp "\$NIOS_PATH/niosv-bsp.exe -c --sopcinfo=${project_name}.sopcinfo -x=bsp_niosv_settings.tcl --type=hal -i=${main_processor} \$FULL_BSP_DIR/settings.bsp "
		puts $fp " "
		
		puts $fp "\$NIOS_PATH/niosv-app.exe --app-dir=\$FULL_APP_DIR --srcs=\$FULL_APP_DIR --bsp-dir=\$FULL_BSP_DIR "
		puts $fp "\$CMAKE_PATH/cmake -G \"Unix Makefiles\" -B \$FULL_APP_DIR -S \$FULL_APP_DIR "
		puts $fp "make -C \$FULL_APP_DIR"
	close $fp

	set fp [open "bsp_niosv_settings.tcl" w+]
		fconfigure $fp -translation lf
		puts $fp "set_setting hal.sys_clk_timer {none} "
		puts $fp "set_setting hal.timestamp_timer {none} "	
		puts $fp "set_setting hal.enable_reduced_device_drivers true "
		puts $fp "set_setting hal.enable_instruction_related_exceptions_api true "		
		puts $fp "set_setting hal.enable_c_plus_plus true "
		puts $fp "set_setting hal.linker.allow_code_at_reset {false} "
		puts $fp "set_setting hal.stderr ${stderr} "
		puts $fp "set_setting hal.stdin ${stdio} "
		puts $fp "set_setting hal.stdout ${stdio} "
		puts $fp $lwip_package_niosV
		puts $fp " "
	close $fp

	set fp [open "processor_set.bat" w+]
		puts $fp "rem NOTE: this file is created automatically. Do not execute it."
		puts $fp "set QSYS_PROC1=${main_processor} "
		puts $fp " "
	close $fp
puts "The command to create/modify the BSP setting files is:"
puts "niosv-bsp -c --sopcinfo=${project_name}.sopcinfo --type=hal -i=${main_processor} -â€“cmd=\"set_setting hal.enable_reduced_device_drivers true\" -â€“cmd=\"set_setting hal.enable_instruction_related_exceptions_api true\" <BSP_sub_folder>/settings.bsp " 
}





