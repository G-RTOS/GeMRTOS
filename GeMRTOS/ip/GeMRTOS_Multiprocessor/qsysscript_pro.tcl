##
##  \file
##  \author Ricardo Cayssials
##  \brief GeMRTOS Platform Designer system construction script (Quartus Prime Pro)
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
## \brief GeMRTOS Nios V Platform Designer configuration script (Quartus Prime Pro)
## \details Run via: qsys-script --system-file=<project>.qsys --quartus-project=<project>.qpf
##   --script=qsysscript_pro.tcl
##   Locates the GeMRTOS_Multiprocessor instance (identified by its GeMRTOS_Frequency
##   parameter) and derives the main processor / JTAG UART instance names, then
##   substitutes them as placeholders into bsp_niosv_settings.tcl and processor_set.bat.
## \note Unlike qsysscript.tcl (Quartus Standard), this script expects both config
##   files to already exist -- they are created by Platform Designer's validate{}
##   callback during a full regeneration. qsysscript_pro.tcl only substitutes
##   placeholders into pre-existing files; it cannot create them from nothing
##   (a genuine Quartus Prime API limitation, see PROPOSAL-003).
## \version 1.0
## \date 2026-07-14
##

package require -exact qsys 14.1

# === Constants: expected Qsys component/instance naming pattern ===
# Contract: the system must contain a GeMRTOS_Multiprocessor-family component,
# identified by having a "GeMRTOS_Frequency" instance parameter, with a main
# processor composed instance named "<instance>_gemrtos_proc_1" and JTAG UART
# composed instances "<instance>_jtag_uart_0" (stdio) / "<instance>_jtag_uart_1" (stderr).
set GEMRTOS_IDENTIFYING_PARAM  "GeMRTOS_Frequency"
set GEMRTOS_PROC_SUFFIX        "gemrtos_proc_1"
set GEMRTOS_STDIO_SUFFIX       "jtag_uart_0"
set GEMRTOS_STDERR_SUFFIX      "jtag_uart_1"

# === Placeholder tokens substituted into the config files below ===
set PLACEHOLDER_PROJECT_NAME   "GEMRTOS_PROJECT_NAME"
set PLACEHOLDER_MAIN_PROCESSOR "GEMRTOS_MAIN_PROCESSOR"
set PLACEHOLDER_STDERR         "GEMRTOS_STDERR"
set PLACEHOLDER_STDIO          "GEMRTOS_STDIO"
set PLACEHOLDER_LWIP           "GEMRTOS_LWIP"

# puts "Connections of clock [ get_connections clk_0.clk ] "

# puts "Connections of interface [ get_connections ] "

set fp [open "logging.log" w+]

    set gemrtos_project_name [get_module_property name]
    set lwip_package_niosII "--cmd \"enable_sw_package lwip\" "
    set lwip_package_niosV "enable_sw_package lwip "
    set lwip_package ""

    # === Find the GeMRTOS_Multiprocessor instance and derive processor/UART names ===
    set instances [ get_instances ]
    foreach instance $instances {
        set lista [ get_instance_parameters $instance ]
        if { [ lsearch -exact $lista $GEMRTOS_IDENTIFYING_PARAM ] >= 0 } {
            puts $fp "################################"
            puts $fp "Instance = $instance"

            set gemrtos_multiprocessor_name [ get_instance_property ${instance} NAME]
            set gemrtos_main_processor "${gemrtos_multiprocessor_name}_${GEMRTOS_PROC_SUFFIX}"
            set gemrtos_stderr "${gemrtos_multiprocessor_name}_${GEMRTOS_STDERR_SUFFIX}"
            set gemrtos_stdio "${gemrtos_multiprocessor_name}_${GEMRTOS_STDIO_SUFFIX}"
        }
    }

    if { ![info exists gemrtos_main_processor] } {
        puts "ERROR: no instance with a \"${GEMRTOS_IDENTIFYING_PARAM}\" parameter was found."
        puts "ERROR: is a GeMRTOS_Multiprocessor component present in this Qsys system?"
        close $fp
        exit 1
    }

    # === Substitute placeholders into the pre-existing config files ===
    set filenames {"bsp_niosv_settings.tcl" "processor_set.bat"}
    foreach filename $filenames {
        if {[file exists $filename]} {
            puts "${filename} opening"
            set f_in [open ${filename} r]
            set content [read $f_in]
            close $f_in
            
            set content [string map [list $PLACEHOLDER_PROJECT_NAME ${gemrtos_project_name}] $content]
            set content [string map [list $PLACEHOLDER_MAIN_PROCESSOR ${gemrtos_main_processor}] $content]
            set content [string map [list $PLACEHOLDER_STDERR ${gemrtos_stderr}] $content]
            set content [string map [list $PLACEHOLDER_STDIO ${gemrtos_stdio}] $content]
            set content [string map [list $PLACEHOLDER_LWIP ${lwip_package}] $content]
           
            
            set f_out [open ${filename} w]
            puts -nonewline $f_out $content
            close $f_out
            
        } else {
            puts "${filename} does not exists"
        }
    }
  
    
    # set project_name "GEMRTOS_PROJECT_NAME"
    # set main_processor "GEMRTOS_MAIN_PROCESSOR"
    # set stderr "GEMRTOS_STDERR"
    # set stdio "GEMRTOS_STDIO"
    # set lwip_package_niosII "GEMRTOS_LWIP_NIOSII"
    # set lwip_package_niosV "GEMRTOS_LWIP_NIOSV"     
    
close $fp





