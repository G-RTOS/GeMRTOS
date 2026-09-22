# PIN LOCATION ASSIGNMENTS
set_location_assignment PIN_B3 -to RAM_A0
set_location_assignment PIN_C3 -to RAM_A1
set_location_assignment PIN_B4 -to RAM_A2
set_location_assignment PIN_F8 -to RAM_A3
set_location_assignment PIN_D11 -to RAM_A4
set_location_assignment PIN_C9 -to RAM_D0
set_location_assignment PIN_D12 -to RAM_A5
set_location_assignment PIN_B14 -to RAM_A6
set_location_assignment PIN_A13 -to RAM_A7
set_location_assignment PIN_E11 -to RAM_A8
set_location_assignment PIN_A10 -to RAM_D1
set_location_assignment PIN_B10 -to RAM_D2
set_location_assignment PIN_A11 -to RAM_D3
set_location_assignment PIN_B12 -to RAM_D4
set_location_assignment PIN_D6 -to RAM_A10
set_location_assignment PIN_E10 -to RAM_A11
set_location_assignment PIN_B11 -to RAM_D5
set_location_assignment PIN_B13 -to RAM_D6
set_location_assignment PIN_A12 -to RAM_D7
set_location_assignment PIN_D9 -to RAM_LDM
set_location_assignment PIN_F9 -to RAM_A12
set_location_assignment PIN_C8 -to RAM_LDQS
set_location_assignment PIN_D8 -to RAM_A13
set_location_assignment PIN_E8 -to RAM_D8
set_location_assignment PIN_D3 -to RAM_BA1
set_location_assignment PIN_E7 -to RAM_D9
set_location_assignment PIN_E6 -to RAM_D10
set_location_assignment PIN_C6 -to RAM_CS_N
set_location_assignment PIN_A6 -to RAM_UDQS
set_location_assignment PIN_A7 -to RAM_D11
set_location_assignment PIN_B7 -to RAM_D12
set_location_assignment PIN_B6 -to RAM_D13
set_location_assignment PIN_B5 -to RAM_D14
set_location_assignment PIN_A5 -to RAM_D15
set_location_assignment PIN_D5 -to RAM_BA0
set_location_assignment PIN_A4 -to RAM_UDM
set_location_assignment PIN_E9 -to RAM_CKE
set_location_assignment PIN_A2 -to RAM_CK_N
set_location_assignment PIN_A3 -to RAM_CK_P
set_location_assignment PIN_A14 -to RAM_A9
set_location_assignment PIN_C14 -to RAM_WS_N
set_location_assignment PIN_D14 -to RAM_CAS_N
set_location_assignment PIN_A15 -to RAM_RAS_N
set_location_assignment PIN_E1 -to CLK_FPGA_50M



# PIN I/O STANDARDS
set_global_assignment -name STRATIX_DEVICE_IO_STANDARD "3.3-V LVTTL"
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_A0
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_A1
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_A2
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_A3
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_A4
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_A5
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_A6
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_A7
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_A8
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_A9
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_A10
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_A11
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_A12
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_A13
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_BA0
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_BA1
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_CAS_N
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_CK_N
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_CK_P
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_CKE
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_CS_N
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_D0
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_D1
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_D2
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_D3
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_D4
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_D5
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_D6
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_D7
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_D8
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_D9
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_D10
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_D11
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_D12
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_D13
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_D14
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_D15
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_LDM
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_LDQS
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_RAS_N
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_UDM
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_UDQS
set_instance_assignment -name IO_STANDARD "1.8 V" -to RAM_WS_N
set_instance_assignment -name IO_STANDARD "1.8 V" -to msdram_clk_out_p
set_instance_assignment -name IO_STANDARD "1.8 V" -to msdram_clk_out_n
set_instance_assignment -name IO_STANDARD "1.8 V" -to msdram_ba
set_instance_assignment -name IO_STANDARD "1.8 V" -to msdram_a
set_instance_assignment -name IO_STANDARD "1.8 V" -to msdram_cs
set_instance_assignment -name IO_STANDARD "1.8 V" -to msdram_cas
set_instance_assignment -name IO_STANDARD "1.8 V" -to msdram_ras
set_instance_assignment -name IO_STANDARD "1.8 V" -to msdram_we
set_instance_assignment -name IO_STANDARD "1.8 V" -to msdram_dqm
set_instance_assignment -name IO_STANDARD "1.8 V" -to msdram_dq
set_instance_assignment -name IO_STANDARD "1.8 V" -to msdram_dqs
set_instance_assignment -name IO_STANDARD "1.8 V" -to msdram_cke

# THE PINS THAT ARE ON THE BEMICRO SDK, BUT ARE NOT USED IN THIS DESIGN
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to ETH_COL
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to ETH_CRS
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to ETH_RESET_N
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to EXP_PRESENT
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING VCC" -to RESET_EXP_N
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING VCC" -to MDC
set_instance_assignment -name RESERVE_PIN "AS BIDIRECTIONAL" -to MDIO
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P1
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P3
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P2
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P4
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P5
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P6
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P7
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P8
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P9
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P10
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P11
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P13
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P12
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P14
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P15
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P16
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P17
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P18
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P19
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P20
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P21
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P22
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P23
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P24
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P25
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P26
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P27
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P28
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P29
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P35
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P36
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P37
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P38
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P39
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P40
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P41
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P42
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P43
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P44
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P45
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P46
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P47
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P48
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P49
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P50
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P51
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P52
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P53
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P54
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P55
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P56
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P57
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P58
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P59
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to P60
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to RAM_A0
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to RAM_A1
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to RAM_A2
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to RAM_A3
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to RAM_A4
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to RAM_A5
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to RAM_A6
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to RAM_A7
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to RAM_A8
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to RAM_A9
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to RAM_A10
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to RAM_A11
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to RAM_A12
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to RAM_A13
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to RAM_BA0
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to RAM_BA1
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to RAM_CAS_N
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to RAM_CKE
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to RAM_CK_N
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to RAM_CK_P
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to RAM_CS_N
set_instance_assignment -name RESERVE_PIN "AS BIDIRECTIONAL" -to RAM_D0
set_instance_assignment -name RESERVE_PIN "AS BIDIRECTIONAL" -to RAM_D1
set_instance_assignment -name RESERVE_PIN "AS BIDIRECTIONAL" -to RAM_D2
set_instance_assignment -name RESERVE_PIN "AS BIDIRECTIONAL" -to RAM_D3
set_instance_assignment -name RESERVE_PIN "AS BIDIRECTIONAL" -to RAM_D4
set_instance_assignment -name RESERVE_PIN "AS BIDIRECTIONAL" -to RAM_D5
set_instance_assignment -name RESERVE_PIN "AS BIDIRECTIONAL" -to RAM_D6
set_instance_assignment -name RESERVE_PIN "AS BIDIRECTIONAL" -to RAM_D7
set_instance_assignment -name RESERVE_PIN "AS BIDIRECTIONAL" -to RAM_D8
set_instance_assignment -name RESERVE_PIN "AS BIDIRECTIONAL" -to RAM_D9
set_instance_assignment -name RESERVE_PIN "AS BIDIRECTIONAL" -to RAM_D10
set_instance_assignment -name RESERVE_PIN "AS BIDIRECTIONAL" -to RAM_D11
set_instance_assignment -name RESERVE_PIN "AS BIDIRECTIONAL" -to RAM_D12
set_instance_assignment -name RESERVE_PIN "AS BIDIRECTIONAL" -to RAM_D13
set_instance_assignment -name RESERVE_PIN "AS BIDIRECTIONAL" -to RAM_D14
set_instance_assignment -name RESERVE_PIN "AS BIDIRECTIONAL" -to RAM_D15
set_instance_assignment -name RESERVE_PIN "AS BIDIRECTIONAL" -to RAM_LDQS
set_instance_assignment -name RESERVE_PIN "AS BIDIRECTIONAL" -to RAM_UDQS
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to RAM_LDM
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to RAM_RAS_N
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to RAM_UDM
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to RAM_WS_N
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to RXD_0
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to RXD_1
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to RXD_2
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to RXD_3
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to RX_CLK
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to RX_DV
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to RX_ER
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to SD_CLK
set_instance_assignment -name RESERVE_PIN "AS BIDIRECTIONAL" -to SD_CMD
set_instance_assignment -name RESERVE_PIN "AS BIDIRECTIONAL" -to SD_DAT0
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to SD_DAT1
set_instance_assignment -name RESERVE_PIN "AS BIDIRECTIONAL" -to SD_DAT2
set_instance_assignment -name RESERVE_PIN "AS BIDIRECTIONAL" -to SD_DAT3
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to TXD_0
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to TXD_1
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to TXD_2
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to TXD_3
set_instance_assignment -name RESERVE_PIN "AS OUTPUT DRIVING GROUND" -to TX_EN
set_instance_assignment -name RESERVE_PIN "AS INPUT TRI-STATED" -to TX_CLK

# SET UP SOME OF THE CONFIGURATION PIN OPTIONS
set_global_assignment -name CYCLONEII_RESERVE_NCEO_AFTER_CONFIGURATION "USE AS REGULAR IO"
set_global_assignment -name STRATIXIII_UPDATE_MODE REMOTE
# SET UP FITTER OPTIONS TO MEET TIMING FOR DDR MEMORY
set_global_assignment -name OPTIMIZE_HOLD_TIMING "IO PATHS AND MINIMUM TPD PATHS"
set_global_assignment -name OPTIMIZE_MULTI_CORNER_TIMING ON
set_global_assignment -name FITTER_EFFORT "STANDARD FIT"
