##
##  \file
##  \author Ricardo Cayssials
##  \brief GeMRTOS SDRAM controller Platform Designer component descriptor
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

# 
# gemrtos_sdram "GeMRTOS SDRAM controller" v1.0
# Ricardo Cayssials 2025.11.27.12:10:16
# SDRAM controller
# 

# 
# request TCL package from ACDS 16.1
# 
package require -exact qsys 16.1


# 
# module gemrtos_sdram
# 
set_module_property DESCRIPTION "SDRAM controller"
set_module_property NAME gemrtos_sdram
set_module_property VERSION 1.0
set_module_property INTERNAL false
set_module_property OPAQUE_ADDRESS_MAP true
set_module_property GROUP GeMRTOS
set_module_property AUTHOR "Ricardo Cayssials"
set_module_property ICON_PATH logoGeMRTOS.png
set_module_property DISPLAY_NAME "GeMRTOS SDRAM controller"
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true
set_module_property REPORT_TO_TALKBACK false
set_module_property ALLOW_GREYBOX_GENERATION false
set_module_property REPORT_HIERARCHY false
set_module_property ELABORATION_CALLBACK elaborate
set_module_property VALIDATION_CALLBACK validate
# set_module_property ACTION_CALLBACK editor



# 
# file sets
# 
add_fileset QUARTUS_SYNTH QUARTUS_SYNTH "" ""
set_fileset_property QUARTUS_SYNTH TOP_LEVEL gemrtos_sdram
set_fileset_property QUARTUS_SYNTH ENABLE_RELATIVE_INCLUDE_PATHS false
set_fileset_property QUARTUS_SYNTH ENABLE_FILE_OVERWRITE_MODE false
add_fileset_file gemrtos_sdram.vhd VHDL PATH ./gemrtos_sdram.vhd TOP_LEVEL_FILE

add_fileset sim_verilog SIM_VERILOG "" "Verilog Simulation"
set_fileset_property sim_verilog TOP_LEVEL gemrtos_sdram
set_fileset_property sim_verilog ENABLE_RELATIVE_INCLUDE_PATHS false
add_fileset_file gemrtos_sdram.vhd VERILOG PATH ./gemrtos_sdram.vhd


add_fileset sim_vhdl SIM_VHDL "" "VHDL Simulation"
set_fileset_property sim_vhdl TOP_LEVEL gemrtos_sdram
set_fileset_property sim_vhdl ENABLE_RELATIVE_INCLUDE_PATHS false
add_fileset_file gemrtos_sdram.vhd VHDL PATH ./gemrtos_sdram.vhd

# 
# parameters
# 
# +-----------------------------------
# Memory type
add_parameter Memory_type STRING 
set_parameter_property Memory_type DEFAULT_VALUE "Custom"
set_parameter_property Memory_type DISPLAY_NAME "Memory type"
set_parameter_property Memory_type UNITS None
set_parameter_property Memory_type ALLOWED_RANGES {"Custom" "micron - MT48LC4M32B2-6 CL3" "issi - IS42/45VM32160G-6"}
set_parameter_property Memory_type HDL_PARAMETER false
set_parameter_property Memory_type DERIVED false
add_display_item "Parameters" Memory_type PARAMETER


# Avalon data width
add_parameter AVALON_DATA_WIDTH INTEGER
set_parameter_property AVALON_DATA_WIDTH DEFAULT_VALUE 32
set_parameter_property AVALON_DATA_WIDTH DISPLAY_NAME "Avalon data width"
# set_parameter_property AVALON_DATA_WIDTH ALLOWED_RANGES {8 16 32}
set_parameter_property AVALON_DATA_WIDTH UNITS bits
set_parameter_property AVALON_DATA_WIDTH DERIVED true
set_parameter_property AVALON_DATA_WIDTH HDL_PARAMETER true
add_display_item "Parameters" AVALON_DATA_WIDTH PARAMETER
# +-----------------------------------
# +-----------------------------------
# SDRAM rows
add_parameter SDRAM_ROWS INTEGER
set_parameter_property SDRAM_ROWS DISPLAY_NAME "SDRAM rows"
set_parameter_property SDRAM_ROWS DEFAULT_VALUE 4096
set_parameter_property SDRAM_ROWS TYPE INTEGER
set_parameter_property SDRAM_ROWS UNITS None
set_parameter_property SDRAM_ROWS DERIVED true
set_parameter_property SDRAM_ROWS HDL_PARAMETER true
add_display_item "Parameters" SDRAM_ROWS PARAMETER
# +-----------------------------------
# +-----------------------------------
# SDRAM columns
add_parameter SDRAM_COLS INTEGER
set_parameter_property SDRAM_COLS DISPLAY_NAME "SDRAM columns"
set_parameter_property SDRAM_COLS DEFAULT_VALUE 256
set_parameter_property SDRAM_COLS TYPE INTEGER
set_parameter_property SDRAM_COLS UNITS None
set_parameter_property SDRAM_COLS DERIVED true
set_parameter_property SDRAM_COLS HDL_PARAMETER true
add_display_item "Parameters" SDRAM_COLS PARAMETER
# +-----------------------------------
# +-----------------------------------
# SDRAM banks
add_parameter SDRAM_BANKS INTEGER
set_parameter_property SDRAM_BANKS DEFAULT_VALUE 4
set_parameter_property SDRAM_BANKS DISPLAY_NAME "SDRAM banks"
set_parameter_property SDRAM_BANKS TYPE INTEGER
set_parameter_property SDRAM_BANKS UNITS None
set_parameter_property SDRAM_BANKS DERIVED true
set_parameter_property SDRAM_BANKS HDL_PARAMETER true
add_display_item "Parameters" SDRAM_BANKS PARAMETER
# +-----------------------------------
# +-----------------------------------
# SDRAM address width
add_parameter SDRAM_ADDR_WIDTH INTEGER 
set_parameter_property SDRAM_ADDR_WIDTH DEFAULT_VALUE 5
set_parameter_property SDRAM_ADDR_WIDTH DISPLAY_NAME "SDRAM address width"
set_parameter_property SDRAM_ADDR_WIDTH ALLOWED_RANGES {5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25}
set_parameter_property SDRAM_ADDR_WIDTH TYPE INTEGER
set_parameter_property SDRAM_ADDR_WIDTH UNITS None
set_parameter_property SDRAM_ADDR_WIDTH DERIVED true
set_parameter_property SDRAM_ADDR_WIDTH HDL_PARAMETER true
add_display_item "Parameters" SDRAM_ADDR_WIDTH PARAMETER
# +-----------------------------------
# +-----------------------------------
# SDRAM data width
add_parameter SDRAM_DATA_WIDTH INTEGER 
set_parameter_property SDRAM_DATA_WIDTH DEFAULT_VALUE 32
set_parameter_property SDRAM_DATA_WIDTH DISPLAY_NAME "SDRAM data width"
# set_parameter_property SDRAM_DATA_WIDTH ALLOWED_RANGES {4 8 16 32}
set_parameter_property SDRAM_DATA_WIDTH UNITS bits
set_parameter_property SDRAM_DATA_WIDTH DERIVED true
set_parameter_property SDRAM_DATA_WIDTH HDL_PARAMETER true
add_display_item "Parameters" SDRAM_DATA_WIDTH PARAMETER
# +-----------------------------------
# +-----------------------------------
# 28-11-2025 # SDRAM data mask width
# 28-11-2025 add_parameter SDRAM_DATA_MASK_WIDTH INTEGER 
# 28-11-2025 set_parameter_property SDRAM_DATA_MASK_WIDTH DEFAULT_VALUE 4
# 28-11-2025 set_parameter_property SDRAM_DATA_MASK_WIDTH DISPLAY_NAME "SDRAM data mask width"
# 28-11-2025 set_parameter_property SDRAM_DATA_MASK_WIDTH UNITS None
# 28-11-2025 set_parameter_property SDRAM_DATA_MASK_WIDTH HDL_PARAMETER true
# 28-11-2025 add_display_item "Parameters" SDRAM_DATA_MASK_WIDTH PARAMETER
# +-----------------------------------
# +-----------------------------------
# SDRAM initialization time in us
add_parameter SDRAM_INIT_TIME_US INTEGER 
set_parameter_property SDRAM_INIT_TIME_US DEFAULT_VALUE 100
set_parameter_property SDRAM_INIT_TIME_US DISPLAY_NAME "SDRAM initialization time in us"
set_parameter_property SDRAM_INIT_TIME_US UNITS None
set_parameter_property SDRAM_INIT_TIME_US DERIVED true
set_parameter_property SDRAM_INIT_TIME_US HDL_PARAMETER true
add_display_item "Parameters" SDRAM_INIT_TIME_US PARAMETER
# +-----------------------------------
# +-----------------------------------
# Clock frequency in MHz units
add_parameter CLK_FREQUENCY_MHZ INTEGER 
set_parameter_property CLK_FREQUENCY_MHZ DISPLAY_NAME "Clock frequency in MHz units"
set_parameter_property CLK_FREQUENCY_MHZ TYPE INTEGER
set_parameter_property CLK_FREQUENCY_MHZ UNITS "Megahertz"
set_parameter_property CLK_FREQUENCY_MHZ HDL_PARAMETER true
set_parameter_property CLK_FREQUENCY_MHZ DERIVED true
set_parameter_property CLK_FREQUENCY_MHZ GROUP "Timing properties"
add_display_item "Parameters" CLK_FREQUENCY_MHZ PARAMETER
# +-----------------------------------
# +-----------------------------------
# 
add_parameter REFRESH_TIME_ms INTEGER 
set_parameter_property REFRESH_TIME_ms DEFAULT_VALUE 32
set_parameter_property REFRESH_TIME_ms DISPLAY_NAME REFRESH_TIME_ms
set_parameter_property REFRESH_TIME_ms UNITS "Milliseconds"
set_parameter_property REFRESH_TIME_ms DERIVED true
set_parameter_property REFRESH_TIME_ms HDL_PARAMETER true
add_display_item "Parameters" REFRESH_TIME_ms PARAMETER
# +-----------------------------------
# +-----------------------------------
# 
add_parameter REFRESH_COUNT INTEGER 8192
set_parameter_property REFRESH_COUNT DEFAULT_VALUE 8192
set_parameter_property REFRESH_COUNT DISPLAY_NAME REFRESH_COUNT
set_parameter_property REFRESH_COUNT TYPE INTEGER
set_parameter_property REFRESH_COUNT UNITS None
set_parameter_property REFRESH_COUNT DERIVED true
set_parameter_property REFRESH_COUNT HDL_PARAMETER true
add_display_item "Parameters" REFRESH_COUNT PARAMETER
# +-----------------------------------
# +-----------------------------------
# CAS latency cycles
add_parameter CAS_LATENCY INTEGER
set_parameter_property CAS_LATENCY DEFAULT_VALUE 3
set_parameter_property CAS_LATENCY DISPLAY_NAME "CAS latency cycles"
set_parameter_property CAS_LATENCY TYPE INTEGER
set_parameter_property CAS_LATENCY UNITS "cycles"
set_parameter_property CAS_LATENCY DERIVED true
set_parameter_property CAS_LATENCY HDL_PARAMETER true
add_display_item "Timing" CAS_LATENCY PARAMETER
# +-----------------------------------
# +-----------------------------------
# Access time from clock (ns)
add_parameter SHOWED_tAC FLOAT 5.500
set_parameter_property SHOWED_tAC DEFAULT_VALUE 5.500
set_parameter_property SHOWED_tAC DISPLAY_NAME "tAC - Access time from clock (ns)"
set_parameter_property SHOWED_tAC TYPE FLOAT
set_parameter_property SHOWED_tAC UNITS nanoseconds
set_parameter_property SHOWED_tAC DERIVED true
set_parameter_property SHOWED_tAC HDL_PARAMETER false
add_display_item "Timing" SHOWED_tAC PARAMETER
# +-----------------------------------
# +-----------------------------------
# Data-out High-Impedance time from CLK (ns)
add_parameter SHOWED_tHZ FLOAT 5.5
set_parameter_property SHOWED_tHZ DEFAULT_VALUE 5.5
set_parameter_property SHOWED_tHZ DISPLAY_NAME "tHZ - Data-out High-Impedance time from CLK (ns)"
set_parameter_property SHOWED_tHZ TYPE FLOAT
set_parameter_property SHOWED_tHZ UNITS nanoseconds
set_parameter_property SHOWED_tHZ DERIVED true
set_parameter_property SHOWED_tHZ HDL_PARAMETER false
add_display_item "Timing" SHOWED_tHZ PARAMETER
# +-----------------------------------
# +-----------------------------------
# Data-out hold time (load) (ns)
add_parameter SHOWED_tOH FLOAT 2.0
set_parameter_property SHOWED_tOH DEFAULT_VALUE 2.0
set_parameter_property SHOWED_tOH DISPLAY_NAME "tOH - Data-out hold time (load) (ns)"
set_parameter_property SHOWED_tOH TYPE FLOAT
set_parameter_property SHOWED_tOH UNITS nanoseconds
set_parameter_property SHOWED_tOH DERIVED true
set_parameter_property SHOWED_tOH HDL_PARAMETER false
add_display_item "Timing" SHOWED_tOH PARAMETER
# +-----------------------------------
# +-----------------------------------
# LOAD MODE REGISTER to ACTIVE or REFRESH (clk cycles)
add_parameter SHOWED_tMRD FLOAT 2.0
set_parameter_property SHOWED_tMRD DEFAULT_VALUE 2.0
set_parameter_property SHOWED_tMRD DISPLAY_NAME "tMRD - LOAD MODE REGISTER to ACTIVE or REFRESH (clk cycles)"
set_parameter_property SHOWED_tMRD TYPE FLOAT
set_parameter_property SHOWED_tMRD UNITS "Cycles"
set_parameter_property SHOWED_tMRD DERIVED true
set_parameter_property SHOWED_tMRD HDL_PARAMETER false
add_display_item "Timing" SHOWED_tMRD PARAMETER
# +-----------------------------------
# +-----------------------------------
# Active to precharge command (ns)
add_parameter SHOWED_tRAS FLOAT 42.0
set_parameter_property SHOWED_tRAS DEFAULT_VALUE 42.0
set_parameter_property SHOWED_tRAS DISPLAY_NAME "tRAS - Active to precharge command (ns)"
set_parameter_property SHOWED_tRAS TYPE FLOAT
set_parameter_property SHOWED_tRAS UNITS nanoseconds
set_parameter_property SHOWED_tRAS DERIVED true
set_parameter_property SHOWED_tRAS HDL_PARAMETER false
add_display_item "Timing" SHOWED_tRAS PARAMETER
# +-----------------------------------
# +-----------------------------------
# Active bank a to ACTIVE bank a command (ns)
add_parameter SHOWED_tRC FLOAT 60.0
set_parameter_property SHOWED_tRC DEFAULT_VALUE 60.0
set_parameter_property SHOWED_tRC DISPLAY_NAME "tRC - Active bank a to ACTIVE bank a command (ns)"
set_parameter_property SHOWED_tRC TYPE FLOAT
set_parameter_property SHOWED_tRC UNITS nanoseconds
set_parameter_property SHOWED_tRC DERIVED true
set_parameter_property SHOWED_tRC HDL_PARAMETER false
add_display_item "Timing" SHOWED_tRC PARAMETER
# +-----------------------------------
# +-----------------------------------
# Active to READ or WRITE delay (ns)
add_parameter SHOWED_tRCD FLOAT 18.0
set_parameter_property SHOWED_tRCD DEFAULT_VALUE 18.0
set_parameter_property SHOWED_tRCD DISPLAY_NAME "tRCD - Active to READ or WRITE delay (ns)"
set_parameter_property SHOWED_tRCD TYPE FLOAT
set_parameter_property SHOWED_tRCD UNITS nanoseconds
set_parameter_property SHOWED_tRCD DERIVED true
set_parameter_property SHOWED_tRCD HDL_PARAMETER false
add_display_item "Timing" SHOWED_tRCD PARAMETER
# +-----------------------------------
# +-----------------------------------
# Auto refresh period (ns)
add_parameter SHOWED_tRFC FLOAT 60.0
set_parameter_property SHOWED_tRFC DEFAULT_VALUE 60.0
set_parameter_property SHOWED_tRFC DISPLAY_NAME "tRFC - Auto refresh period (ns)"
set_parameter_property SHOWED_tRFC TYPE FLOAT
set_parameter_property SHOWED_tRFC UNITS nanoseconds
set_parameter_property SHOWED_tRFC DERIVED true
set_parameter_property SHOWED_tRFC HDL_PARAMETER false
add_display_item "Timing" SHOWED_tRFC PARAMETER
# +-----------------------------------
# +-----------------------------------
# Precharge command period (ns)
add_parameter SHOWED_tRP FLOAT 18.0
set_parameter_property SHOWED_tRP DEFAULT_VALUE 18.0
set_parameter_property SHOWED_tRP DISPLAY_NAME "tRP - Precharge command period (ns)"
set_parameter_property SHOWED_tRP TYPE FLOAT
set_parameter_property SHOWED_tRP UNITS nanoseconds
set_parameter_property SHOWED_tRP DERIVED true
set_parameter_property SHOWED_tRP HDL_PARAMETER false
add_display_item "Timing" SHOWED_tRP PARAMETER
# +-----------------------------------
# +-----------------------------------
# ACTIVE bank a to ACTIVE bank b (ns)
add_parameter SHOWED_tRRD FLOAT 12.0
set_parameter_property SHOWED_tRRD DEFAULT_VALUE 12.0
set_parameter_property SHOWED_tRRD DISPLAY_NAME "tRRD - ACTIVE bank a to ACTIVE bank b (ns)"
set_parameter_property SHOWED_tRRD TYPE FLOAT
set_parameter_property SHOWED_tRRD UNITS nanoseconds
set_parameter_property SHOWED_tRRD DERIVED true
set_parameter_property SHOWED_tRRD HDL_PARAMETER false
add_display_item "Timing" SHOWED_tRRD PARAMETER
# +-----------------------------------
# +-----------------------------------
# tWR - Write Recovery time (ns)
add_parameter SHOWED_tWR FLOAT 12.0
set_parameter_property SHOWED_tWR DEFAULT_VALUE 12.0
set_parameter_property SHOWED_tWR DISPLAY_NAME "tWR - Write Recovery time (ns)"
set_parameter_property SHOWED_tWR TYPE FLOAT
set_parameter_property SHOWED_tWR UNITS nanoseconds
set_parameter_property SHOWED_tWR DERIVED true
set_parameter_property SHOWED_tWR HDL_PARAMETER false
add_display_item "Timing" SHOWED_tWR PARAMETER
# +-----------------------------------
# +-----------------------------------
# Access time from clock (cycles)
add_parameter CYCLES_tAC INTEGER 
set_parameter_property CYCLES_tAC DISPLAY_NAME "CYCLES_tAC"
set_parameter_property CYCLES_tAC DESCRIPTION "Access time from clock (cycles)"
set_parameter_property CYCLES_tAC TYPE INTEGER
set_parameter_property CYCLES_tAC UNITS "cycles"
set_parameter_property CYCLES_tAC HDL_PARAMETER true
set_parameter_property CYCLES_tAC DERIVED true
add_display_item "Information" CYCLES_tAC PARAMETER
# +-----------------------------------
# +-----------------------------------
# Data-out High-Impedance time from CLK (cycles)
add_parameter CYCLES_tHZ INTEGER 
set_parameter_property CYCLES_tHZ DISPLAY_NAME "CYCLES_tHZ"
set_parameter_property CYCLES_tHZ DESCRIPTION "Data-out High-Impedance time from CLK (cycles)"
set_parameter_property CYCLES_tHZ TYPE INTEGER
set_parameter_property CYCLES_tHZ UNITS "cycles"
set_parameter_property CYCLES_tHZ HDL_PARAMETER true
set_parameter_property CYCLES_tHZ DERIVED true
add_display_item "Information" CYCLES_tHZ PARAMETER
# +-----------------------------------
# +-----------------------------------
# Data-out hold time (load) (cycles)
add_parameter CYCLES_tOH INTEGER 
set_parameter_property CYCLES_tOH DISPLAY_NAME "CYCLES_tOH"
set_parameter_property CYCLES_tOH DESCRIPTION "Data-out hold time (load) (cycles)"
set_parameter_property CYCLES_tOH TYPE INTEGER
set_parameter_property CYCLES_tOH UNITS "cycles"
set_parameter_property CYCLES_tOH HDL_PARAMETER true
set_parameter_property CYCLES_tOH DERIVED true
add_display_item "Information" CYCLES_tOH PARAMETER
# +-----------------------------------
# +-----------------------------------
# MODE REGISTER to ACTIVE or REFRESH (cycles)
add_parameter CYCLES_tMRD INTEGER 
set_parameter_property CYCLES_tMRD DISPLAY_NAME "CYCLES_tMRD"
set_parameter_property CYCLES_tMRD DESCRIPTION "MODE REGISTER to ACTIVE or REFRESH (cycles)"
set_parameter_property CYCLES_tMRD TYPE INTEGER
set_parameter_property CYCLES_tMRD UNITS "cycles"
set_parameter_property CYCLES_tMRD HDL_PARAMETER true
set_parameter_property CYCLES_tMRD DERIVED true
add_display_item "Information" CYCLES_tMRD PARAMETER
# +-----------------------------------
# +-----------------------------------
# Active to precharge command (cycles)
add_parameter CYCLES_tRAS INTEGER 
set_parameter_property CYCLES_tRAS DISPLAY_NAME "CYCLES_tRAS"
set_parameter_property CYCLES_tRAS DESCRIPTION "Active to precharge command (cycles)"
set_parameter_property CYCLES_tRAS TYPE INTEGER
set_parameter_property CYCLES_tRAS UNITS "cycles"
set_parameter_property CYCLES_tRAS HDL_PARAMETER true
set_parameter_property CYCLES_tRAS DERIVED true
add_display_item "Information" CYCLES_tRAS PARAMETER
# +-----------------------------------
# +-----------------------------------
# Active bank a to ACTIVE bank a command (cycles)
add_parameter CYCLES_tRC INTEGER 
set_parameter_property CYCLES_tRC DISPLAY_NAME "CYCLES_tRC"
set_parameter_property CYCLES_tRC DESCRIPTION "Active bank a to ACTIVE bank a command (cycles)"
set_parameter_property CYCLES_tRC TYPE INTEGER
set_parameter_property CYCLES_tRC UNITS "cycles"
set_parameter_property CYCLES_tRC HDL_PARAMETER true
set_parameter_property CYCLES_tRC DERIVED true
add_display_item "Information" CYCLES_tRC PARAMETER
# +-----------------------------------
# +-----------------------------------
# Active to READ or WRITE delay (cycles)
add_parameter CYCLES_tRCD INTEGER 
set_parameter_property CYCLES_tRCD DISPLAY_NAME "CYCLES_tRCD"
set_parameter_property CYCLES_tRCD DESCRIPTION "Active to READ or WRITE delay (cycles)"
set_parameter_property CYCLES_tRCD TYPE INTEGER
set_parameter_property CYCLES_tRCD UNITS "cycles"
set_parameter_property CYCLES_tRCD HDL_PARAMETER true
set_parameter_property CYCLES_tRCD DERIVED true
add_display_item "Information" CYCLES_tRCD PARAMETER
# +-----------------------------------
# +-----------------------------------
# Auto refresh period (cycles)
add_parameter CYCLES_tRFC INTEGER 
set_parameter_property CYCLES_tRFC DISPLAY_NAME "CYCLES_tRFC"
set_parameter_property CYCLES_tRFC DESCRIPTION "Auto refresh period (cycles)"
set_parameter_property CYCLES_tRFC TYPE INTEGER
set_parameter_property CYCLES_tRFC UNITS "cycles"
set_parameter_property CYCLES_tRFC HDL_PARAMETER true
set_parameter_property CYCLES_tRFC DERIVED true
add_display_item "Information" CYCLES_tRFC PARAMETER
# +-----------------------------------
# +-----------------------------------
# Precharge command period (cycles)
add_parameter CYCLES_tRP INTEGER
set_parameter_property CYCLES_tRP DISPLAY_NAME "CYCLES_tRP"
set_parameter_property CYCLES_tRP DESCRIPTION "Auto refresh period (cycles)"
set_parameter_property CYCLES_tRP TYPE INTEGER
set_parameter_property CYCLES_tRP UNITS "cycles"
set_parameter_property CYCLES_tRP HDL_PARAMETER true
set_parameter_property CYCLES_tRP DERIVED true
add_display_item "Information" CYCLES_tRP PARAMETER
# +-----------------------------------
# +-----------------------------------
# ACTIVE bank a to ACTIVE bank b (cycles)
add_parameter CYCLES_tRRD INTEGER 
set_parameter_property CYCLES_tRRD DISPLAY_NAME "CYCLES_tRRD"
set_parameter_property CYCLES_tRRD DESCRIPTION "ACTIVE bank a to ACTIVE bank b (cycles)"
set_parameter_property CYCLES_tRRD TYPE INTEGER
set_parameter_property CYCLES_tRRD UNITS "cycles"
set_parameter_property CYCLES_tRRD HDL_PARAMETER true
set_parameter_property CYCLES_tRRD DERIVED true
add_display_item "Information" CYCLES_tRRD PARAMETER
# +-----------------------------------
# +-----------------------------------
# tWR - Write Recovery time (ns)
add_parameter CYCLES_tWR INTEGER 
set_parameter_property CYCLES_tWR DISPLAY_NAME "CYCLES_tWR"
set_parameter_property CYCLES_tWR DESCRIPTION "tWR - Write Recovery time (ns)"
set_parameter_property CYCLES_tWR TYPE INTEGER
set_parameter_property CYCLES_tWR UNITS "cycles"
set_parameter_property CYCLES_tWR HDL_PARAMETER true
set_parameter_property CYCLES_tWR DERIVED true
add_display_item "Information" CYCLES_tWR PARAMETER
# +-----------------------------------

# ###########################################
# ###########################################

# Avalon data width
add_parameter CUSTOM_AVALON_DATA_WIDTH INTEGER
set_parameter_property CUSTOM_AVALON_DATA_WIDTH DEFAULT_VALUE 32
set_parameter_property CUSTOM_AVALON_DATA_WIDTH DISPLAY_NAME "Avalon data width"
set_parameter_property CUSTOM_AVALON_DATA_WIDTH ALLOWED_RANGES {8 16 32 64}
set_parameter_property CUSTOM_AVALON_DATA_WIDTH UNITS bits
set_parameter_property CUSTOM_AVALON_DATA_WIDTH DERIVED false
set_parameter_property CUSTOM_AVALON_DATA_WIDTH HDL_PARAMETER false
add_display_item "Custom Parameters" CUSTOM_AVALON_DATA_WIDTH PARAMETER
# +-----------------------------------
# +-----------------------------------
# SDRAM rows
add_parameter CUSTOM_SDRAM_ROWS INTEGER
set_parameter_property CUSTOM_SDRAM_ROWS DISPLAY_NAME "SDRAM rows"
set_parameter_property CUSTOM_SDRAM_ROWS DEFAULT_VALUE 4096
set_parameter_property CUSTOM_SDRAM_ROWS TYPE INTEGER
set_parameter_property CUSTOM_SDRAM_ROWS UNITS None
set_parameter_property CUSTOM_SDRAM_ROWS DERIVED false
set_parameter_property CUSTOM_SDRAM_ROWS HDL_PARAMETER false
add_display_item "Custom Parameters" CUSTOM_SDRAM_ROWS PARAMETER
# +-----------------------------------
# +-----------------------------------
# SDRAM columns
add_parameter CUSTOM_SDRAM_COLS INTEGER
set_parameter_property CUSTOM_SDRAM_COLS DISPLAY_NAME "SDRAM columns"
set_parameter_property CUSTOM_SDRAM_COLS DEFAULT_VALUE 256
set_parameter_property CUSTOM_SDRAM_COLS TYPE INTEGER
set_parameter_property CUSTOM_SDRAM_COLS UNITS None
set_parameter_property CUSTOM_SDRAM_COLS DERIVED false
set_parameter_property CUSTOM_SDRAM_COLS HDL_PARAMETER false
add_display_item "Custom Parameters" CUSTOM_SDRAM_COLS PARAMETER
# +-----------------------------------
# +-----------------------------------
# SDRAM banks
add_parameter CUSTOM_SDRAM_BANKS INTEGER
set_parameter_property CUSTOM_SDRAM_BANKS DEFAULT_VALUE 4
set_parameter_property CUSTOM_SDRAM_BANKS DISPLAY_NAME "SDRAM banks"
set_parameter_property CUSTOM_SDRAM_BANKS ALLOWED_RANGES {1 2 4 8}
set_parameter_property CUSTOM_SDRAM_BANKS TYPE INTEGER
set_parameter_property CUSTOM_SDRAM_BANKS UNITS None
set_parameter_property CUSTOM_SDRAM_BANKS DERIVED false
set_parameter_property CUSTOM_SDRAM_BANKS HDL_PARAMETER false
add_display_item "Custom Parameters" CUSTOM_SDRAM_BANKS PARAMETER
# +-----------------------------------
# +-----------------------------------
# SDRAM address width
add_parameter CUSTOM_SDRAM_ADDR_WIDTH INTEGER 
set_parameter_property CUSTOM_SDRAM_ADDR_WIDTH DEFAULT_VALUE 5
set_parameter_property CUSTOM_SDRAM_ADDR_WIDTH DISPLAY_NAME "SDRAM address width"
set_parameter_property CUSTOM_SDRAM_ADDR_WIDTH ALLOWED_RANGES {5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25}
set_parameter_property CUSTOM_SDRAM_ADDR_WIDTH TYPE INTEGER
set_parameter_property CUSTOM_SDRAM_ADDR_WIDTH UNITS None
set_parameter_property CUSTOM_SDRAM_ADDR_WIDTH DERIVED false
set_parameter_property CUSTOM_SDRAM_ADDR_WIDTH HDL_PARAMETER false
add_display_item "Custom Parameters" CUSTOM_SDRAM_ADDR_WIDTH PARAMETER
# +-----------------------------------
# +-----------------------------------
# SDRAM data width
add_parameter CUSTOM_SDRAM_DATA_WIDTH INTEGER 
set_parameter_property CUSTOM_SDRAM_DATA_WIDTH DEFAULT_VALUE 32
set_parameter_property CUSTOM_SDRAM_DATA_WIDTH DISPLAY_NAME "SDRAM address width"
set_parameter_property CUSTOM_SDRAM_DATA_WIDTH ALLOWED_RANGES {4 8 16 32}
set_parameter_property CUSTOM_SDRAM_DATA_WIDTH UNITS bits
set_parameter_property CUSTOM_SDRAM_DATA_WIDTH DERIVED false
set_parameter_property CUSTOM_SDRAM_DATA_WIDTH HDL_PARAMETER false
add_display_item "Custom Parameters" CUSTOM_SDRAM_DATA_WIDTH PARAMETER
# +-----------------------------------
# +-----------------------------------
# 28-11-2025 # SDRAM data mask width
# 28-11-2025 add_parameter CUSTOM_SDRAM_DATA_MASK_WIDTH INTEGER 
# 28-11-2025 set_parameter_property CUSTOM_SDRAM_DATA_MASK_WIDTH DEFAULT_VALUE 4
# 28-11-2025 set_parameter_property CUSTOM_SDRAM_DATA_MASK_WIDTH DISPLAY_NAME "SDRAM data mask width"
# 28-11-2025 set_parameter_property CUSTOM_SDRAM_DATA_MASK_WIDTH UNITS None
# 28-11-2025 set_parameter_property CUSTOM_SDRAM_DATA_MASK_WIDTH HDL_PARAMETER true
# 28-11-2025 add_display_item "Custom" CUSTOM_SDRAM_DATA_MASK_WIDTH PARAMETER
# +-----------------------------------
# +-----------------------------------
# SDRAM initialization time in us
add_parameter CUSTOM_SDRAM_INIT_TIME_US INTEGER 
set_parameter_property CUSTOM_SDRAM_INIT_TIME_US DEFAULT_VALUE 100
set_parameter_property CUSTOM_SDRAM_INIT_TIME_US DISPLAY_NAME "SDRAM initialization time in us"
set_parameter_property CUSTOM_SDRAM_INIT_TIME_US UNITS None
set_parameter_property CUSTOM_SDRAM_INIT_TIME_US DERIVED false
set_parameter_property CUSTOM_SDRAM_INIT_TIME_US HDL_PARAMETER false
add_display_item "Custom Parameters" CUSTOM_SDRAM_INIT_TIME_US PARAMETER
# +-----------------------------------
# +-----------------------------------
# 
add_parameter CUSTOM_REFRESH_TIME_ms INTEGER 
set_parameter_property CUSTOM_REFRESH_TIME_ms DEFAULT_VALUE 32
set_parameter_property CUSTOM_REFRESH_TIME_ms DISPLAY_NAME REFRESH_TIME_ms
set_parameter_property CUSTOM_REFRESH_TIME_ms UNITS "Milliseconds"
set_parameter_property CUSTOM_REFRESH_TIME_ms DERIVED false
set_parameter_property CUSTOM_REFRESH_TIME_ms HDL_PARAMETER false
add_display_item "Custom Parameters" CUSTOM_REFRESH_TIME_ms PARAMETER
# +-----------------------------------
# +-----------------------------------
# 
add_parameter CUSTOM_REFRESH_COUNT INTEGER 8192
set_parameter_property CUSTOM_REFRESH_COUNT DEFAULT_VALUE 8192
set_parameter_property CUSTOM_REFRESH_COUNT DISPLAY_NAME REFRESH_COUNT
set_parameter_property CUSTOM_REFRESH_COUNT TYPE INTEGER
set_parameter_property CUSTOM_REFRESH_COUNT UNITS None
set_parameter_property CUSTOM_REFRESH_COUNT DERIVED false
set_parameter_property CUSTOM_REFRESH_COUNT HDL_PARAMETER false
add_display_item "Custom Parameters" CUSTOM_REFRESH_COUNT PARAMETER
# +-----------------------------------
# +-----------------------------------
# CAS latency cycles
add_parameter CUSTOM_CAS_LATENCY INTEGER
set_parameter_property CUSTOM_CAS_LATENCY DEFAULT_VALUE 3
set_parameter_property CUSTOM_CAS_LATENCY DISPLAY_NAME "CAS latency cycles"
set_parameter_property CUSTOM_CAS_LATENCY ALLOWED_RANGES {1 2 3}
set_parameter_property CUSTOM_CAS_LATENCY TYPE INTEGER
set_parameter_property CUSTOM_CAS_LATENCY UNITS "cycles"
set_parameter_property CUSTOM_CAS_LATENCY DERIVED false
set_parameter_property CUSTOM_CAS_LATENCY HDL_PARAMETER false
add_display_item "Custom Timing" CUSTOM_CAS_LATENCY PARAMETER
# +-----------------------------------
# +-----------------------------------
# Access time from clock (ns)
add_parameter CUSTOM_tAC FLOAT 5.5
set_parameter_property CUSTOM_tAC DEFAULT_VALUE 5.5
set_parameter_property CUSTOM_tAC DISPLAY_NAME "tAC - Access time from clock (ns)"
set_parameter_property CUSTOM_tAC TYPE FLOAT
set_parameter_property CUSTOM_tAC UNITS nanoseconds
set_parameter_property CUSTOM_tAC DERIVED false
set_parameter_property CUSTOM_tAC HDL_PARAMETER false
add_display_item "Custom Timing" CUSTOM_tAC PARAMETER
# +-----------------------------------
# +-----------------------------------
# Data-out High-Impedance time from CLK (ns)
add_parameter CUSTOM_tHZ FLOAT 5.5
set_parameter_property CUSTOM_tHZ DEFAULT_VALUE 5.5
set_parameter_property CUSTOM_tHZ DISPLAY_NAME "tHZ - Data-out High-Impedance time from CLK (ns)"
set_parameter_property CUSTOM_tHZ TYPE FLOAT
set_parameter_property CUSTOM_tHZ UNITS nanoseconds
set_parameter_property CUSTOM_tHZ DERIVED false
set_parameter_property CUSTOM_tHZ HDL_PARAMETER false
add_display_item "Custom Timing" CUSTOM_tHZ PARAMETER
# +-----------------------------------
# +-----------------------------------
# Data-out hold time (load) (ns)
add_parameter CUSTOM_tOH FLOAT 2.0
set_parameter_property CUSTOM_tOH DEFAULT_VALUE 2.0
set_parameter_property CUSTOM_tOH DISPLAY_NAME "tOH - Data-out hold time (load) (ns)"
set_parameter_property CUSTOM_tOH TYPE FLOAT
set_parameter_property CUSTOM_tOH UNITS nanoseconds
set_parameter_property CUSTOM_tOH DERIVED false
set_parameter_property CUSTOM_tOH HDL_PARAMETER false
add_display_item "Custom Timing" CUSTOM_tOH PARAMETER
# +-----------------------------------
# +-----------------------------------
# LOAD MODE REGISTER to ACTIVE or REFRESH (clk cycles)
add_parameter CUSTOM_tMRD FLOAT 2.0
set_parameter_property CUSTOM_tMRD DEFAULT_VALUE 2.0
set_parameter_property CUSTOM_tMRD DISPLAY_NAME "tMRD - LOAD MODE REGISTER to ACTIVE or REFRESH (clk cycles)"
set_parameter_property CUSTOM_tMRD TYPE FLOAT
set_parameter_property CUSTOM_tMRD UNITS "Cycles"
set_parameter_property CUSTOM_tMRD DERIVED false
set_parameter_property CUSTOM_tMRD HDL_PARAMETER false
add_display_item "Custom Timing" CUSTOM_tMRD PARAMETER
# +-----------------------------------
# +-----------------------------------
# Active to precharge command (ns)
add_parameter CUSTOM_tRAS FLOAT 42.0
set_parameter_property CUSTOM_tRAS DEFAULT_VALUE 42.0
set_parameter_property CUSTOM_tRAS DISPLAY_NAME "tRAS - Active to precharge command (ns)"
set_parameter_property CUSTOM_tRAS TYPE FLOAT
set_parameter_property CUSTOM_tRAS UNITS nanoseconds
set_parameter_property CUSTOM_tRAS DERIVED false
set_parameter_property CUSTOM_tRAS HDL_PARAMETER false
add_display_item "Custom Timing" CUSTOM_tRAS PARAMETER
# +-----------------------------------
# +-----------------------------------
# Active bank a to ACTIVE bank a command (ns)
add_parameter CUSTOM_tRC FLOAT 60.0
set_parameter_property CUSTOM_tRC DEFAULT_VALUE 60.0
set_parameter_property CUSTOM_tRC DISPLAY_NAME "tRC - Active bank a to ACTIVE bank a command (ns)"
set_parameter_property CUSTOM_tRC TYPE FLOAT
set_parameter_property CUSTOM_tRC UNITS nanoseconds
set_parameter_property CUSTOM_tRC DERIVED false
set_parameter_property CUSTOM_tRC HDL_PARAMETER false
add_display_item "Custom Timing" CUSTOM_tRC PARAMETER
# +-----------------------------------
# +-----------------------------------
# Active to READ or WRITE delay (ns)
add_parameter CUSTOM_tRCD FLOAT 18.0
set_parameter_property CUSTOM_tRCD DEFAULT_VALUE 18.0
set_parameter_property CUSTOM_tRCD DISPLAY_NAME "tRCD - Active to READ or WRITE delay (ns)"
set_parameter_property CUSTOM_tRCD TYPE FLOAT
set_parameter_property CUSTOM_tRCD UNITS nanoseconds
set_parameter_property CUSTOM_tRCD DERIVED false
set_parameter_property CUSTOM_tRCD HDL_PARAMETER false
add_display_item "Custom Timing" CUSTOM_tRCD PARAMETER
# +-----------------------------------
# +-----------------------------------
# Auto refresh period (ns)
add_parameter CUSTOM_tRFC FLOAT 60.0
set_parameter_property CUSTOM_tRFC DEFAULT_VALUE 60.0
set_parameter_property CUSTOM_tRFC DISPLAY_NAME "tRFC - Auto refresh period (ns)"
set_parameter_property CUSTOM_tRFC TYPE FLOAT
set_parameter_property CUSTOM_tRFC UNITS nanoseconds
set_parameter_property CUSTOM_tRFC DERIVED false
set_parameter_property CUSTOM_tRFC HDL_PARAMETER false
add_display_item "Custom Timing" CUSTOM_tRFC PARAMETER
# +-----------------------------------
# +-----------------------------------
# Precharge command period (ns)
add_parameter CUSTOM_tRP FLOAT 18.0
set_parameter_property CUSTOM_tRP DEFAULT_VALUE 18.0
set_parameter_property CUSTOM_tRP DISPLAY_NAME "tRP - Precharge command period (ns)"
set_parameter_property CUSTOM_tRP TYPE FLOAT
set_parameter_property CUSTOM_tRP UNITS nanoseconds
set_parameter_property CUSTOM_tRP DERIVED false
set_parameter_property CUSTOM_tRP HDL_PARAMETER false
add_display_item "Custom Timing" CUSTOM_tRP PARAMETER
# +-----------------------------------
# +-----------------------------------
# ACTIVE bank a to ACTIVE bank b (ns)
add_parameter CUSTOM_tRRD FLOAT 12.0
set_parameter_property CUSTOM_tRRD DEFAULT_VALUE 12.0
set_parameter_property CUSTOM_tRRD DISPLAY_NAME "tRRD - ACTIVE bank a to ACTIVE bank b (ns)"
set_parameter_property CUSTOM_tRRD TYPE FLOAT
set_parameter_property CUSTOM_tRRD UNITS nanoseconds
set_parameter_property CUSTOM_tRRD DERIVED false
set_parameter_property CUSTOM_tRRD HDL_PARAMETER false
add_display_item "Custom Timing" CUSTOM_tRRD PARAMETER
# +-----------------------------------
# +-----------------------------------
# tWR - Write Recovery time (ns)
add_parameter CUSTOM_tWR FLOAT 12.0
set_parameter_property CUSTOM_tWR DEFAULT_VALUE 12.0
set_parameter_property CUSTOM_tWR DISPLAY_NAME "tWR - Write Recovery time (ns)"
set_parameter_property CUSTOM_tWR TYPE FLOAT
set_parameter_property CUSTOM_tWR UNITS nanoseconds
set_parameter_property CUSTOM_tWR DERIVED false
set_parameter_property CUSTOM_tWR HDL_PARAMETER false
add_display_item "Custom Timing" CUSTOM_tWR PARAMETER

# ###########################################
# ###########################################


# ###########################################
# # GENERIC                                ##
# ###########################################
# ###########################################
# +-----------------------------------
# Access time from clock (picoseconds)
add_parameter tAC INTEGER 
set_parameter_property tAC DISPLAY_NAME "tAC - Access time from clock (ps)"
set_parameter_property tAC TYPE INTEGER
set_parameter_property tAC UNITS picoseconds
set_parameter_property tAC DERIVED true
set_parameter_property tAC HDL_PARAMETER true
add_display_item "Timing" tAC PARAMETER
# +-----------------------------------
# +-----------------------------------
# Data-out High-Impedance time from CLK (ps)
add_parameter tHZ INTEGER
set_parameter_property tHZ DISPLAY_NAME "tHZ - Data-out High-Impedance time from CLK (ps)"
set_parameter_property tHZ TYPE INTEGER
set_parameter_property tHZ UNITS picoseconds
set_parameter_property tHZ DERIVED true
set_parameter_property tHZ HDL_PARAMETER true
add_display_item "Timing" tHZ PARAMETER
# +-----------------------------------
# +-----------------------------------
# Data-out hold time (load) (ps)
add_parameter tOH INTEGER
set_parameter_property tOH DISPLAY_NAME "tOH - Data-out hold time (load) (ps)"
set_parameter_property tOH TYPE INTEGER
set_parameter_property tOH UNITS picoseconds
set_parameter_property tOH DERIVED true
set_parameter_property tOH HDL_PARAMETER true
add_display_item "Timing" tOH PARAMETER
# +-----------------------------------
# +-----------------------------------
# LOAD MODE REGISTER to ACTIVE or REFRESH (clk cycles)
add_parameter tMRD INTEGER
set_parameter_property tMRD DISPLAY_NAME "tMRD - LOAD MODE REGISTER to ACTIVE or REFRESH (clk cycles)"
set_parameter_property tMRD TYPE INTEGER
set_parameter_property tMRD UNITS "Cycles"
set_parameter_property tMRD DERIVED true
set_parameter_property tMRD HDL_PARAMETER true
add_display_item "Timing" tMRD PARAMETER
# +-----------------------------------
# +-----------------------------------
# Active to precharge command (ps)
add_parameter tRAS INTEGER 42
set_parameter_property tRAS DEFAULT_VALUE 42.0
set_parameter_property tRAS DISPLAY_NAME "tRAS - Active to precharge command (ps)"
set_parameter_property tRAS TYPE INTEGER
set_parameter_property tRAS UNITS picoseconds
set_parameter_property tRAS DERIVED true
set_parameter_property tRAS HDL_PARAMETER true
add_display_item "Timing" tRAS PARAMETER
# +-----------------------------------
# +-----------------------------------
# Active bank a to ACTIVE bank a command (ps)
add_parameter tRC INTEGER 60
set_parameter_property tRC DEFAULT_VALUE 60.0
set_parameter_property tRC DISPLAY_NAME "tRC - Active bank a to ACTIVE bank a command (ps)"
set_parameter_property tRC TYPE INTEGER
set_parameter_property tRC UNITS picoseconds
set_parameter_property tRC DERIVED true
set_parameter_property tRC HDL_PARAMETER true
add_display_item "Timing" tRC PARAMETER
# +-----------------------------------
# +-----------------------------------
# Active to READ or WRITE delay (ps)
add_parameter tRCD INTEGER 18
set_parameter_property tRCD DEFAULT_VALUE 18.0
set_parameter_property tRCD DISPLAY_NAME "tRCD - Active to READ or WRITE delay (ps)"
set_parameter_property tRCD TYPE INTEGER
set_parameter_property tRCD UNITS picoseconds
set_parameter_property tRCD DERIVED true
set_parameter_property tRCD HDL_PARAMETER true
add_display_item "Timing" tRCD PARAMETER
# +-----------------------------------
# +-----------------------------------
# Auto refresh period (ps)
add_parameter tRFC INTEGER 60
set_parameter_property tRFC DEFAULT_VALUE 60.0
set_parameter_property tRFC DISPLAY_NAME "tRFC - Auto refresh period (ps)"
set_parameter_property tRFC TYPE INTEGER
set_parameter_property tRFC UNITS picoseconds
set_parameter_property tRFC DERIVED true
set_parameter_property tRFC HDL_PARAMETER true
add_display_item "Timing" tRFC PARAMETER
# +-----------------------------------
# +-----------------------------------
# Precharge command period (ps)
add_parameter tRP INTEGER 18
set_parameter_property tRP DEFAULT_VALUE 18.0
set_parameter_property tRP DISPLAY_NAME "tRP - Precharge command period (ps)"
set_parameter_property tRP TYPE INTEGER
set_parameter_property tRP UNITS picoseconds
set_parameter_property tRP DERIVED true
set_parameter_property tRP HDL_PARAMETER true
add_display_item "Timing" tRP PARAMETER
# +-----------------------------------
# +-----------------------------------
# ACTIVE bank a to ACTIVE bank b (ps)
add_parameter tRRD INTEGER 12
set_parameter_property tRRD DISPLAY_NAME "tRRD - ACTIVE bank a to ACTIVE bank b (ps)"
set_parameter_property tRRD UNITS picoseconds
set_parameter_property tRRD DERIVED true
set_parameter_property tRRD HDL_PARAMETER true
add_display_item "Timing" tRRD PARAMETER
# +-----------------------------------
# +-----------------------------------
# tWR - Write Recovery time (ns)
add_parameter tWR INTEGER 12
set_parameter_property tWR DISPLAY_NAME "tWR - Write Recovery time (ns)"
set_parameter_property tWR UNITS picoseconds
set_parameter_property tWR DERIVED true
set_parameter_property tWR HDL_PARAMETER true
add_display_item "Timing" tWR PARAMETER
# +-----------------------------------
# ###########################################
# ###########################################
# ###########################################
# ###########################################


# 
# display items
# 
add_display_item "" "Parameters" group tab
add_display_item "" "Timing" group tab
add_display_item "" "Custom" group tab
add_display_item "Custom" "Custom Parameters" group tab
add_display_item "Custom" "Custom Timing" group tab
add_display_item "" "Information" group tab

# 
# connection point clock
# 
add_interface clock clock end
set_interface_property clock clockRate 0
set_interface_property clock ENABLED true
set_interface_property clock EXPORT_OF ""
set_interface_property clock PORT_NAME_MAP ""
set_interface_property clock CMSIS_SVD_VARIABLES ""
set_interface_property clock SVD_ADDRESS_GROUP ""

add_interface_port clock clk clk Input 1

add_parameter Clock_Frequency INTEGER  
set_parameter_property Clock_Frequency SYSTEM_INFO {CLOCK_RATE clock}
set_parameter_property Clock_Frequency DISPLAY_NAME "Processor frequency"
set_parameter_property Clock_Frequency UNITS Hertz
set_parameter_property Clock_Frequency VISIBLE false
add_display_item "Timing" Clock_Frequency PARAMETER
set_parameter_property Clock_Frequency HDL_PARAMETER false


# 
# connection point reset
# 
add_interface reset reset end
set_interface_property reset associatedClock clock
set_interface_property reset synchronousEdges DEASSERT
set_interface_property reset ENABLED true
set_interface_property reset EXPORT_OF ""
set_interface_property reset PORT_NAME_MAP ""
set_interface_property reset CMSIS_SVD_VARIABLES ""
set_interface_property reset SVD_ADDRESS_GROUP ""

add_interface_port reset reset reset Input 1


# 
# connection point avalon
# 
add_interface avalon avalon end
set_interface_property avalon addressAlignment {DYNAMIC}
set_interface_property avalon addressGroup {0}
set_interface_property avalon addressUnits {WORDS}
set_interface_property avalon associatedClock {clock}
set_interface_property avalon associatedReset {reset}
set_interface_property avalon bitsPerSymbol {8}
set_interface_property avalon alwaysBurstMaxBurst {0}
set_interface_property avalon burstOnBurstBoundariesOnly {0}
set_interface_property avalon burstcountUnits WORDS
set_interface_property avalon constantBurstBehavior {0}
set_interface_property avalon explicitAddressSpan 0
set_interface_property avalon holdTime 0
set_interface_property avalon interleaveBursts {0}
set_interface_property avalon linewrapBursts {0}
set_interface_property avalon maximumPendingReadTransactions 0
set_interface_property avalon maximumPendingWriteTransactions 0
set_interface_property avalon isBigEndian {0}
set_interface_property avalon readLatency 0
set_interface_property avalon readWaitTime 1
set_interface_property avalon setupTime 0
set_interface_property avalon transparentBridge {0}
set_interface_property avalon timingUnits {Cycles}
set_interface_property avalon writeWaitTime 0
set_interface_property avalon wellBehavedWaitrequest {0}
set_interface_property avalon ENABLED true
set_interface_property avalon EXPORT_OF ""
set_interface_property avalon PORT_NAME_MAP ""
set_interface_property avalon CMSIS_SVD_VARIABLES ""
set_interface_property avalon SVD_ADDRESS_GROUP ""
set_interface_assignment avalon embeddedsw.configuration.isFlash {0}
set_interface_assignment avalon embeddedsw.configuration.isMemoryDevice {1}
set_interface_assignment avalon embeddedsw.configuration.isNonVolatileStorage {0}
set_interface_assignment avalon embeddedsw.configuration.isPrintableDevice {0}

#	set_interface_property avalon addressSpan {16777216}
#	set_interface_property s1 printableDevice {0}	
#	set_interface_property s1 readWaitStates {1}
#	set_interface_property s1 registerIncomingSignals {0}
#	set_interface_property s1 registerOutgoingSignals {0}
#	set_interface_property s1 writeLatency {0}
#	set_interface_property s1 writeWaitStates {0}


add_interface_port avalon avalon_address address Input 1
add_interface_port avalon avalon_read read Input 1
add_interface_port avalon avalon_write write Input 1
add_interface_port avalon avalon_readdata readdata Output 32
add_interface_port avalon avalon_writedata writedata Input 32
add_interface_port avalon avalon_byteenable byteenable Input 4
add_interface_port avalon avalon_waitrequest waitrequest Output 1



# 
# connection point conduit_end
# 
add_interface conduit_end conduit end
set_interface_property conduit_end associatedClock clock
set_interface_property conduit_end associatedReset ""
set_interface_property conduit_end ENABLED true
set_interface_property conduit_end EXPORT_OF ""
set_interface_property conduit_end PORT_NAME_MAP ""
set_interface_property conduit_end CMSIS_SVD_VARIABLES ""
set_interface_property conduit_end SVD_ADDRESS_GROUP ""

add_interface_port conduit_end sdram_addr addr Output 12
add_interface_port conduit_end sdram_bank_addr bank Output 1 
add_interface_port conduit_end sdram_data dq Bidir 32
add_interface_port conduit_end sdram_clock_enable cke Output 1
add_interface_port conduit_end sdram_cs_n cs_n Output 1
add_interface_port conduit_end sdram_ras_n ras_n Output 1
add_interface_port conduit_end sdram_cas_n cas Output 1
add_interface_port conduit_end sdram_we_n we_n Output 1
add_interface_port conduit_end sdram_data_mask dqm Output 4

proc elaborate { } {

    set frequency     [ get_parameter_value Clock_Frequency ]
    set frequencyMHz [ expr int ($frequency / 1000000) ]
    set_parameter_value CLK_FREQUENCY_MHZ $frequencyMHz
    set dataWidth [ get_parameter_value AVALON_DATA_WIDTH ]
    set byteenable_width [ expr { $dataWidth/8 } ]

    set Memory_of_Processor [get_parameter_value Memory_type]
    
    if { ($Memory_of_Processor == "Custom") } {
        set_parameter_value AVALON_DATA_WIDTH     [ get_parameter_value CUSTOM_AVALON_DATA_WIDTH ]
        set_parameter_value SDRAM_ROWS            [ get_parameter_value CUSTOM_SDRAM_ROWS  ]
        set_parameter_value SDRAM_COLS            [ get_parameter_value CUSTOM_SDRAM_COLS  ]
        set_parameter_value SDRAM_BANKS           [ get_parameter_value CUSTOM_SDRAM_BANKS  ]
        set_parameter_value SDRAM_ADDR_WIDTH      [ get_parameter_value CUSTOM_SDRAM_ADDR_WIDTH  ]
        set_parameter_value SDRAM_DATA_WIDTH      [ get_parameter_value CUSTOM_SDRAM_DATA_WIDTH  ]
        set_parameter_value SDRAM_INIT_TIME_US    [ get_parameter_value CUSTOM_SDRAM_INIT_TIME_US  ]
        set_parameter_value REFRESH_TIME_ms       [ get_parameter_value CUSTOM_REFRESH_TIME_ms  ]
        set_parameter_value REFRESH_COUNT         [ get_parameter_value CUSTOM_REFRESH_COUNT  ]
        set_parameter_value CAS_LATENCY           [ get_parameter_value CUSTOM_CAS_LATENCY  ]
        set_parameter_value SHOWED_tAC            [ get_parameter_value CUSTOM_tAC  ]
        set_parameter_value SHOWED_tHZ            [ get_parameter_value CUSTOM_tHZ  ]
        set_parameter_value SHOWED_tOH            [ get_parameter_value CUSTOM_tOH  ]
        set_parameter_value SHOWED_tMRD           [ get_parameter_value CUSTOM_tMRD  ]
        set_parameter_value SHOWED_tRAS           [ get_parameter_value CUSTOM_tRAS  ]
        set_parameter_value SHOWED_tRC            [ get_parameter_value CUSTOM_tRC  ]
        set_parameter_value SHOWED_tRCD           [ get_parameter_value CUSTOM_tRCD  ]
        set_parameter_value SHOWED_tRFC           [ get_parameter_value CUSTOM_tRFC  ]
        set_parameter_value SHOWED_tRP            [ get_parameter_value CUSTOM_tRP  ]
        set_parameter_value SHOWED_tRRD           [ get_parameter_value CUSTOM_tRRD  ]
        set_parameter_value SHOWED_tWR            [ get_parameter_value CUSTOM_tWR  ]  
    } 
    
    if { ($Memory_of_Processor == "micron - MT48LC4M32B2-6 CL3") } {
       
        # Set parameter values
        set_parameter_value AVALON_DATA_WIDTH     32
        set_parameter_value SDRAM_ROWS            4096
        set_parameter_value SDRAM_COLS            256
        set_parameter_value SDRAM_BANKS           4
        set_parameter_value SDRAM_ADDR_WIDTH      12
        set_parameter_value SDRAM_DATA_WIDTH      32
        set_parameter_value SDRAM_INIT_TIME_US    100
        set_parameter_value REFRESH_TIME_ms       64
        set_parameter_value REFRESH_COUNT         4096
        
        if { ($frequencyMHz <= 166) } { 
            set_parameter_value CAS_LATENCY       3
        }
        if { ($frequencyMHz <= 100) } { 
            set_parameter_value CAS_LATENCY       2
        }
        if { ($frequencyMHz <= 50) } { 
            set_parameter_value CAS_LATENCY       1
        }
        
        set_parameter_value SHOWED_tAC            5.5
        set_parameter_value SHOWED_tHZ            5.5
        set_parameter_value SHOWED_tOH            2.0
        set_parameter_value SHOWED_tMRD           2.0
        set_parameter_value SHOWED_tRAS           42.0
        set_parameter_value SHOWED_tRC            60.0
        set_parameter_value SHOWED_tRCD           18.0
        set_parameter_value SHOWED_tRFC           60.0
        set_parameter_value SHOWED_tRP            18.0
        set_parameter_value SHOWED_tRRD           12.0
        set_parameter_value SHOWED_tWR            12.0 
    } 

    if { ($Memory_of_Processor == "issi - IS42/45VM32160G-6") } {
       
        # Set parameter values
        set_parameter_value AVALON_DATA_WIDTH     32   
        set_parameter_value SDRAM_ROWS            8192 
        set_parameter_value SDRAM_COLS            512  
        set_parameter_value SDRAM_BANKS           4    
        set_parameter_value SDRAM_ADDR_WIDTH      13   
        set_parameter_value SDRAM_DATA_WIDTH      32   
        set_parameter_value SDRAM_INIT_TIME_US    100  
        set_parameter_value REFRESH_TIME_ms       64   
        set_parameter_value REFRESH_COUNT         8192 
        
        if { ($frequencyMHz <= 166) } { 
            set_parameter_value CAS_LATENCY       3    
            set_parameter_value SHOWED_tAC        5.5  
            set_parameter_value SHOWED_tHZ        5.5  
        }
        if { ($frequencyMHz <= 100) } { 
            set_parameter_value CAS_LATENCY       2    
            set_parameter_value SHOWED_tAC        8.0  
            set_parameter_value SHOWED_tHZ        8.0  
        }
        if { ($frequencyMHz <= 50) } { 
            send_message Warning "SDRAM frequency equal to ${frequencyMHz} MHz is very low"
        }
        
        set_parameter_value SHOWED_tOH            2.5  
        set_parameter_value SHOWED_tMRD           2.0  
        set_parameter_value SHOWED_tRAS           42.0 
        set_parameter_value SHOWED_tRC            60.0 
        set_parameter_value SHOWED_tRCD           18.0 
        set_parameter_value SHOWED_tRFC           80.0 
        set_parameter_value SHOWED_tRP            18.0 
        set_parameter_value SHOWED_tRRD           12.0 
        set_parameter_value SHOWED_tWR            15.0 
    }


    # SETTING THE PARAMETERS
  
    set time     [ get_parameter_value SHOWED_tAC ]
    set cycles   [ expr ceil ($time * $frequencyMHz / 1000) ]
    set_parameter_value CYCLES_tAC $cycles   

    set time     [ get_parameter_value SHOWED_tHZ ]
    set cycles   [ expr ceil ($time * $frequencyMHz / 1000) ]
    set_parameter_value CYCLES_tHZ $cycles

    set time     [ get_parameter_value SHOWED_tOH ]
    set cycles   [ expr ceil ($time * $frequencyMHz / 1000) ]
    set_parameter_value CYCLES_tOH $cycles

    set time     [ get_parameter_value SHOWED_tMRD ]
    set cycles   [ expr ceil ($time * $frequencyMHz / 1000) ]
    set_parameter_value CYCLES_tMRD $cycles

    set time     [ get_parameter_value SHOWED_tRAS ]
    set cycles   [ expr ceil ($time * $frequencyMHz / 1000) ]
    set_parameter_value CYCLES_tRAS $cycles

    set time     [ get_parameter_value SHOWED_tRC ]
    set cycles   [ expr ceil ($time * $frequencyMHz / 1000) ]
    set_parameter_value CYCLES_tRC $cycles

    set time     [ get_parameter_value SHOWED_tRCD ]
    set cycles   [ expr ceil ($time * $frequencyMHz / 1000) ]
    set_parameter_value CYCLES_tRCD $cycles

    set time     [ get_parameter_value SHOWED_tRFC ]
    set cycles   [ expr ceil ($time * $frequencyMHz / 1000) ]
    set_parameter_value CYCLES_tRFC $cycles

    set time     [ get_parameter_value SHOWED_tRP ]
    set cycles   [ expr ceil ($time * $frequencyMHz / 1000) ]
    set_parameter_value CYCLES_tRP $cycles

    set time     [ get_parameter_value SHOWED_tRRD ]
    set cycles   [ expr ceil ($time * $frequencyMHz / 1000) ]
    set_parameter_value CYCLES_tRRD $cycles

    set time     [ get_parameter_value SHOWED_tWR ]
    set cycles   [ expr ceil ($time * $frequencyMHz / 1000) ]
    set_parameter_value CYCLES_tWR $cycles    

    set size  [ get_parameter_value SDRAM_BANKS ]
	set SDRAM_BANK_WIDTH  [ expr int (ceil (log($size) / log(2))) ]

    set size  [ get_parameter_value SDRAM_ROWS ]
	set SDRAM_ROWS_WIDTH  [ expr int (ceil (log($size) / log(2))) ]    

    set size  [ get_parameter_value SDRAM_COLS ]
	set SDRAM_COLS_WIDTH  [ expr int (ceil (log($size) / log(2))) ]       

	set ADDRESS_WIDTH  [ expr int ($SDRAM_BANK_WIDTH + $SDRAM_ROWS_WIDTH + $SDRAM_COLS_WIDTH) ]
    set_port_property avalon_address WIDTH_EXPR $ADDRESS_WIDTH
    set_port_property avalon_readdata WIDTH_EXPR "$dataWidth"
    set_port_property avalon_writedata WIDTH_EXPR "$dataWidth"
    set_port_property avalon_byteenable WIDTH_EXPR "$byteenable_width"



    # ################################
    # GENERICS
    set_parameter_value tAC  [ expr ceil ([ get_parameter_value SHOWED_tAC ] * 1000 ) ]
    set_parameter_value tHZ  [ expr ceil ([ get_parameter_value SHOWED_tHZ ] * 1000 ) ]
    set_parameter_value tOH  [ expr ceil ([ get_parameter_value SHOWED_tOH ] * 1000 ) ]
    set_parameter_value tMRD [ expr ceil ([ get_parameter_value SHOWED_tMRD ] ) ]
    set_parameter_value tRAS [ expr ceil ([ get_parameter_value SHOWED_tRAS ] * 1000 ) ]
    set_parameter_value tRC  [ expr ceil ([ get_parameter_value SHOWED_tRC ] * 1000 ) ]
    set_parameter_value tRCD [ expr ceil ([ get_parameter_value SHOWED_tRCD ] * 1000 ) ]
    set_parameter_value tRFC [ expr ceil ([ get_parameter_value SHOWED_tRFC ] * 1000 ) ]
    set_parameter_value tRP  [ expr ceil ([ get_parameter_value SHOWED_tRP ] * 1000 ) ]
    set_parameter_value tRRD [ expr ceil ([ get_parameter_value SHOWED_tRRD ] * 1000 ) ]
    set_parameter_value tWR  [ expr ceil ([ get_parameter_value SHOWED_tWR ] * 1000 ) ]
    
    # ################################


    set_port_property avalon_readdata WIDTH_EXPR [ get_parameter_value AVALON_DATA_WIDTH ]
    set_port_property avalon_writedata WIDTH_EXPR [ get_parameter_value AVALON_DATA_WIDTH ]
    set_port_property sdram_bank_addr WIDTH_EXPR $SDRAM_BANK_WIDTH 
    set_port_property sdram_addr WIDTH_EXPR [ get_parameter_value SDRAM_ADDR_WIDTH ]
    set_port_property sdram_data WIDTH_EXPR [ get_parameter_value SDRAM_DATA_WIDTH ]
    set_port_property sdram_data_mask WIDTH_EXPR [ get_parameter_value SDRAM_BANKS ]    


   
}
 
proc validate { } {


}

proc editor { } {



}
