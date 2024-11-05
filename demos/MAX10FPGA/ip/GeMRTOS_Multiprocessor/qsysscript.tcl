# run with qsys-script --system-file=qsysgrtos.qsys --script=my_script.tcl > parameters.txt
package require -exact qsys 18.0
# get all instance names in the system and print one by one
# puts "Connections of clock [ get_connections clk_0.clk ] "
# puts "Connections of interface [ get_connections ] "

# Debo llegar a esto
# nios2-bsp hal my_bsp qsysgrtos.sopcinfo --cpu-name GRTOS_Multiprocessor_1_gemrtos_proc_1 --cmd "set_setting hal.enable_reduced_device_drivers true" --cmd "set_setting hal.stderr GRTOS_Multiprocessor_1_jtag_uart_1" --cmd add_section_mapping rstaux GRTOS_Multiprocessor_1_rstaux_memory


set project_name [get_module_property name]
puts "Project name= ${project_name}"
puts "Project properties [get_project_properties]"

set generate_hps_bsp 0
set fp [open "logging.log" w+]

    set instances [ get_instances ]
    foreach instance $instances {
        puts $fp "Instance"
        puts $fp $instance
        foreach subinstance [get_composed_instances $instance] {
            puts $fp "subinstance"
            puts $fp $subinstance        
            if { [ string equal "gemrtos_proc_1" $subinstance ] } {
                set main_processor ${instance}_${subinstance}
            }
            if { [ string first "jtag_uart_0" $subinstance ] != -1} {
                set stdio ${instance}_jtag_uart_0
                set stderr ${instance}_jtag_uart_1
            }
            # 30-05-2024 if { [ string first "rstaux_memory" $subinstance ] != -1} {
            # 30-05-2024     set rstmem "add_section_mapping rstaux ${instance}_${subinstance}"
            # 30-05-2024     puts " add_section_mapping rstaux ${instance}_${subinstance}"
            # 30-05-2024 }
            # 30-05.2024 if { [ string first "onchip_memory2_2" $subinstance ] != -1} {
			# 30-05.2024 	set command1 "delete_memory_region ${instance}_${subinstance}"
			# 30-05.2024 	set command2 "add_memory_region onchipdata ${instance}_${subinstance} 0 512"
			# 30-05.2024 	set command3 "add_memory_region onchipcode ${instance}_${subinstance} 512 15872"
			# 30-05.2024 	set command4 "add_section_mapping onchipdata onchipdata"
			# 30-05.2024 	set command5 "add_section_mapping onchipmem onchipcode"
            # 30-05.2024     # set onchipmem "add_section_mapping onchipmem ${instance}_${subinstance}"
            # 30-05.2024     puts " add_section_mapping onchipmem ${instance}_${subinstance}"
            # 30-05.2024 }
            # if { [ string first "rst1_and_exception_memory" $subinstance ] != -1} {
			# 	# only for nios v
			# 	set command6 "delete_memory_region reset"
			# 	set command7 "delete_memory_region ${instance}_${subinstance}"
			# 	set command8 "add_memory_region reset ${instance}_${subinstance} 0 512"
			# 	set command9 "add_memory_region ${instance}_${subinstance} ${instance}_${subinstance} 512 1536"
            # }	
            if { [ string equal "mm_clock_crossing_bridge_0" $subinstance ] } {
                set generate_hps_bsp 1
            }
			if { [ string equal "grtos_0" $subinstance ] } {
				set PROCESSOR_TYPE [ get_composed_instance_assignment $instance $subinstance embeddedsw.CMacro.PROCESSOR_TYPE ]
			}
            # puts "  $subinstance"
            # puts "    [get_composed_instance_assignments $instance $subinstance]"
        }
    }
    
close $fp
    
# --cmd \"set_setting hal.enable_reduced_device_drivers true\" --cmd \"set_setting hal.stderr ${stderr}\" --cmd ${rstmem} "
 
# nios2-bsp hal my_bsp qsysgrtos.sopcinfo --cpu-name GRTOS_Multiprocessor_1_gemrtos_proc_1 --cmd "set_setting hal.enable_reduced_device_drivers true" --cmd "set_setting hal.stderr GRTOS_Multiprocessor_1_jtag_uart_1" --cmd add_section_mapping rstaux GRTOS_Multiprocessor_1_rstaux_memory 


set fp [open "grtos_hps_bsp_create.sh" w+]
    puts $fp "#!> /bin/bash"
    puts $fp "# Automatically created from qsys-script with qsysscript.tcl"
    if { $generate_hps_bsp  == 1 } {
        puts $fp "bash create_hps_bsp.sh \$2 \$3 \$4 \$5 \$1 \$6 \$7"
    } else {
        puts $fp "echo \"No HPS module to produce\""
    }
close $fp

# --cmd \"set_setting hal.make.cflags_mgpopt \-mgpopt=none\" 
# for reentrant newlib
# --cmd \"set_setting hal.custom_newlib_flags \-DREENTRANT_SYSCALLS_PROVIDED\" 
# Este funciona  --cmd \"set_setting hal.custom_newlib_flags \-D__DYNAMIC_REENT__\"
# --cmd \"set_setting hal.make.bsp_cflags_defined_symbols \-D__DYNAMIC_REENT__ \"
# --cmd \"set_setting hal.make.bsp_cflags_user_flags \-Wl,\-\-wrap\=getreent\"

if { ($PROCESSOR_TYPE == "Nios II/e (economy)") || ($PROCESSOR_TYPE == "Nios II/f (full)") } {
	set fp [open "grtos_nios_bsp_create.sh" w+]
		# 30-05-2024 puts $fp "nios2-bsp hal ./\$1 ${project_name}.sopcinfo --cpu-name ${main_processor} --cmd \"set_setting hal.enable_reduced_device_drivers true\" --cmd \"set_setting hal.stderr ${stderr}\" --cmd \"set_setting hal.stdin ${stdio}\" --cmd \"set_setting hal.stdout ${stdio}\" --cmd \"${rstmem}\" --cmd \"${command1}\" --cmd \"${command2}\" --cmd \"${command3}\" --cmd \"${command4}\" --cmd \"${command5}\" --cmd \"set_setting hal.make.bsp_cflags_user_flags \-Wa,\-relax\-all\"  "
		puts $fp "nios2-bsp hal ./\$1 ${project_name}.sopcinfo --cpu-name ${main_processor} --cmd \"set_setting hal.stderr ${stderr}\" --cmd \"set_setting hal.stdin ${stdio}\" --cmd \"set_setting hal.stdout ${stdio}\" --cmd \"set_setting hal.enable_reduced_device_drivers true\" "
		# 08-08-2024 --cmd \"set_setting hal.enable_reduced_device_drivers true\"
		# 24-06-2024 --cmd \"set_setting hal.log_port ${stderr}\" --cmd \"set_setting hal.log_flags 0\"
		# 24-06-2024 --cmd \"set_setting hal.enable_small_c_library true\"
		# 27-06-2024 --cmd \"set_setting hal.make.bsp_cflags_user_flags \-Wa,\-relax\-all\"
		# 28-06-2024 --cmd \"set_setting hal.enable_reduced_device_drivers true\" 
		# 15-07-2024 --cmd \"set_setting hal.enable_small_c_library true\"
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
		# 24-06-2024 puts $fp "set_setting hal.use_picolibc true "
		# 24-06-2024 puts $fp "set_setting hal.log_port ${stderr} "
		# 24-06-2024 puts $fp "set_setting hal.log_flags 0 "
		# puts $fp "set_setting altera_avalon_jtag_uart_driver.enable_jtag_uart_ignore_fifo_full_error {true} "
		# puts $fp "set_setting altera_avalon_jtag_uart_driver.enable_small_driver {true} "		
		# puts $fp "set_setting hal.enable_lightweight_device_driver_api {true} "
		# puts $fp "set_setting hal.enable_clean_exit {false} "
		# puts $fp "set_setting hal.enable_exit {false} "
		# 30-05-2024 puts $fp ${rstmem}
		# 30-05-2024 puts $fp ${command1}
		# 30-05-2024 puts $fp ${command2}
		# 30-05-2024 puts $fp ${command3}
		# 30-05-2024 puts $fp ${command4}
		# 30-05-2024 puts $fp ${command5}
		# puts $fp ${command6}
		# puts $fp ${command7}
		# puts $fp ${command8}
		# puts $fp ${command9}
		puts $fp " "
	close $fp

	set fp [open "gemrtos_config.bat" w+]
		puts $fp "rem NOTE: this file is created automatically. Do not execute it."
		puts $fp "set QSYS_PROC1=${main_processor} "
		puts $fp " "
	close $fp
puts "The command to create/modify the BSP setting files is:"
puts "niosv-bsp -c --sopcinfo=${project_name}.sopcinfo --type=hal -i=${main_processor} -–cmd=\"set_setting hal.enable_reduced_device_drivers true\" -–cmd=\"set_setting hal.enable_instruction_related_exceptions_api true\" <BSP_sub_folder>/settings.bsp " 
}

# puts $instances
#  foreach instance $instances {
#      puts "Parameters of instance $instance: [get_instance_parameters $instance] "
#      puts "Interfaces of $instance "
#      set interfaces [get_instance_interfaces $instance ] 
#      foreach interface $interfaces {
#          set connections [ get_connections $instance.$interface ]
#          foreach connection $connections {
#              puts "Connection $connection , property= [ get_connection_property $connection WIDTH ]"
#              puts "Properties [get_connection_properties $connection] "
#              puts "END=  [get_connection_property $connection END] "
#              puts "NAME=  [get_connection_property $connection NAME] "
#              puts "START=  [get_connection_property $connection START] "
#              puts "Type=  [get_connection_property $connection TYPE] "
#              set parameters [ get_connection_parameters $connection]
#              foreach parameter $parameters {
#                  puts "Parameter=  $parameter , value= [ get_connection_parameter_value $connection $parameter]"
#                  puts "Property=  $parameter , value= [ get_connection_parameter_property $connection $parameter UNITS ]"
#                  # puts "Parameter properties=  $parameter , value= [ get_parameter_properties]"
#              }
#          }
#          puts "Puertos of $interface "
#          set ports [get_instance_interface_ports $instance $interface ]
#          foreach port $ports {
#              puts "Width of port $port is [ get_instance_interface_port_property $instance $interface $port WIDTH ] "
#          }
#      }
#  }
#  set parameters [get_instance_interface_ports rst1_and_exception_memory s1 ]
#  puts $parameters
#  puts [get_instance_interface_ports jtag_uart_0 avalon_jtag_slave ]
#  set properties [get_port_properties ]
#  puts $properties
#  set property [get_instance_interface_port_property rst1_and_exception_memory s1 address WIDTH ]
#  puts $property
#  # foreach parameter $parameters {
#  #   puts  $instance $parameter
#  # }
#  #}

# clk_0 grtos_0 irq_bridge_0 jtag_uart_0 jtag_uart_1 jtag_uart_2 jtag_uart_3 jtag_uart_4 master_0 gemrtos_proc_0 gemrtos_proc_1 gemrtos_proc_2 nios_avalon_monitor_0 nios_avalon_monitor_2 rstaux_memory rst1_and_exception_memory reset_bridge_0 sysid_qsys_0 timer_0


# Ver ADDRESS_WIDTH in 584 del user de platform designer

# set Processors 3
# # [get_parameter_value NProcessors]
# # get JTAG_UART properties
# for {set i 0} {$i <= $Processors} {incr i} {
#     puts "Interfaces of jtag_uart_$i [get_instance_interfaces jtag_uart_$i ] "
#     puts "Width of av_address of jtag_uart_$i avalon_jtag_slave is [get_instance_interface_port_property jtag_uart_$i avalon_jtag_slave av_address WIDTH ] "
#     puts "Addres of jtag_uart_$i is [ get_connection_parameter_value nios_avalon_monitor_2.m3/jtag_uart_$i.avalon_jtag_slave baseAddress] "
#     set uart [expr {$i-1}]
#     puts $uart
#     
# }
# #puts [get_connection_parameters nios_avalon_monitor_2.m3/jtag_uart_0.avalon_jtag_slave ]