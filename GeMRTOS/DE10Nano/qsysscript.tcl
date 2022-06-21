# run with qsys-script --system-file=qsysgrtos.qsys --script=my_script.tcl > parameters.txt
package require -exact qsys 18.0
# get all instance names in the system and print one by one
# puts "Connections of clock [ get_connections clk_0.clk ] "
# puts "Connections of interface [ get_connections ] "

# Debo llegar a esto
# nios2-bsp hal my_bsp qsysgrtos.sopcinfo --cpu-name GRTOS_Multiprocessor_1_nios2_qsys_1 --cmd "set_setting hal.enable_reduced_device_drivers true" --cmd "set_setting hal.stderr GRTOS_Multiprocessor_1_jtag_uart_1" --cmd add_section_mapping rstaux GRTOS_Multiprocessor_1_onchip_memory2_0


set project_name [get_module_property name]
puts "Project name= ${project_name}"
puts "Project properties [get_project_properties]"

set generate_hps_bsp 0

set instances [ get_instances ]
foreach instance $instances {
    puts $instance
    foreach subinstance [get_composed_instances $instance] {
        if { [ string equal "nios2_qsys_1" $subinstance ] } {
            set main_processor ${instance}_${subinstance}
        }
        if { [ string first "jtag_uart_0" $subinstance ] != -1} {
            set stdio ${instance}_jtag_uart_0
            set stderr ${instance}_jtag_uart_1
        }
        if { [ string first "onchip_memory2_0" $subinstance ] != -1} {
            set rstmem "add_section_mapping rstaux ${instance}_${subinstance}"
            puts " add_section_mapping rstaux ${instance}_${subinstance}"
        }
        if { [ string equal "mm_clock_crossing_bridge_0" $subinstance ] } {
            set generate_hps_bsp 1
        }
        # puts "  $subinstance"
        # puts "    [get_composed_instance_assignments $instance $subinstance]"
    }
}
# --cmd \"set_setting hal.enable_reduced_device_drivers true\" --cmd \"set_setting hal.stderr ${stderr}\" --cmd ${rstmem} "
 
# nios2-bsp hal my_bsp qsysgrtos.sopcinfo --cpu-name GRTOS_Multiprocessor_1_nios2_qsys_1 --cmd "set_setting hal.enable_reduced_device_drivers true" --cmd "set_setting hal.stderr GRTOS_Multiprocessor_1_jtag_uart_1" --cmd add_section_mapping rstaux GRTOS_Multiprocessor_1_onchip_memory2_0 



set fp [open "grtos_hps_create.sh" w+]
    puts $fp "#!> /bin/bash"
    puts $fp "# Automatically created from qsys-script with qsysscript.tcl"
    if { $generate_hps_bsp  == 1 } {
        puts $fp "bash create_hps_bsp.sh \$1"
    } else {
        puts $fp "echo \"no hps found\""
    }
close $fp
set fp [open "grtos_bsp_create.sh" w+]
    puts $fp "nios2-bsp hal \$1 ${project_name}.sopcinfo --cpu-name ${main_processor} --cmd \"set_setting hal.enable_reduced_device_drivers true\" --cmd \"set_setting hal.stderr ${stderr}\" --cmd \"set_setting hal.stdin ${stdio}\"  --cmd \"set_setting hal.stdout ${stdio}\" --cmd ${rstmem} "
close $fp




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
#  set parameters [get_instance_interface_ports onchip_memory2_1 s1 ]
#  puts $parameters
#  puts [get_instance_interface_ports jtag_uart_0 avalon_jtag_slave ]
#  set properties [get_port_properties ]
#  puts $properties
#  set property [get_instance_interface_port_property onchip_memory2_1 s1 address WIDTH ]
#  puts $property
#  # foreach parameter $parameters {
#  #   puts  $instance $parameter
#  # }
#  #}

# clk_0 grtos_0 irq_bridge_0 jtag_uart_0 jtag_uart_1 jtag_uart_2 jtag_uart_3 jtag_uart_4 master_0 nios2_qsys_0 nios2_qsys_1 nios2_qsys_2 nios_avalon_monitor_0 nios_avalon_monitor_2 onchip_memory2_0 onchip_memory2_1 reset_bridge_0 sysid_qsys_0 timer_0


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