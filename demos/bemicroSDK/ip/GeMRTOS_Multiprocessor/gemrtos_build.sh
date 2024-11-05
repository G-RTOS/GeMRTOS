#!/bin/bash
# argumentos desde https://stackoverflow.com/questions/192249/how-do-i-parse-command-line-arguments-in-bash

# Get parameters from https://stackoverflow.com/questions/192249/how-do-i-parse-command-line-arguments-in-bash
# bash new_script.sh -bsp hellogemrtos_bsp -app hellogemrtos -qpr DE10_NANO_SoC_GHRD -qsys soc_system -dir software -sd e -fat f --board de10nano
# FULL for DE10NANO
# bash new_script.sh -bsp hellogemrtos_bsp -app hellogemrtos -qpr DE10_NANO_SoC_GHRD -qsys soc_system -dir software -sd e -fat f --board de10nano -e -f -debug |& tee new_script.txt
# FULL for BEMICRO SDK
# bash new_script.sh -bsp hellogemrtos_bsp -app hellogemrtos -qpr grtosproject -qsys qsysgrtos -dir software -sd e -fat f --board bemicrosdk -debug -f -e |& tee new_script.txt
# FULL for DE2-115
# bash new_script.sh -bsp hellogemrtos_bsp -app hellogemrtos -qpr de2_115 -qsys qsys_gemrtos -dir software -sd e -fat f --board de2_115 -debug -f -e |& tee new_script.txt

BSP_NAME=""                   
APP_NAME=""
SOFTWARE_DIR_NAME="software"  
QUARTUS_PRJ="QUARTUS_PROJECT_FILE" 
QSYS_PRJ="QSYS_PROJECT_FILE"                  
SD_VOLUME="0" 
FAT_VOLUME="0"
BOARD=""
APP_INCLUDE_DIR=""
APP_SRC_DIR=""
FULL_COMPILATION=0
EDIT_QSYS=0
DEBUG=0
PROGRAMMING=1

# To run from command line for testing purposes
# BSP_NAME="hellogemrtos_bsp"                   
# APP_NAME="hellogemrtos"                   
# SOFTWARE_DIR_NAME="software"  
# QUARTUS_PRJ="grtosproject"               
# QSYS_PRJ="qsysgrtos"                 
# SD_VOLUME="0"                 
# FAT_VOLUME="0"                
# BOARD="bemicrosdk"
# APP_INCLUDE_DIR=""
# APP_SRC_DIR=""
# FULL_COMPILATION=1
# EDIT_QSYS=1


# Initialise the error log file in the current directory
error_log_file=$(pwd)/error_log.txt
rm -rf ${error_log_file}

while [[ "$#" -gt 0 ]]; do
    case $1 in
        -bsp|--bsp-name) BSP_NAME="$2"; shift ;;
        -app|--app-name) APP_NAME="$2"; shift ;;
        -qpr|--quartus-prj) QUARTUS_PRJ="$2"; shift ;;
        -qsys|--qsys-prj) QSYS_PRJ="$2"; shift ;;
        -dir|--software-dir) SOFTWARE_DIR_NAME="$2"; shift ;;
        -brd|--board) BOARD="$2"; shift ;;
        -sd|--sd-volumne) SD_VOLUME="$2"; shift ;;
        -fat|--fat-volumne) FAT_VOLUME="$2"; shift ;;
        -idir|--inc-dir) APP_INCLUDE_DIR+=" --inc-rdir $2"; shift ;;
        -sdir|--src-dir) APP_SRC_DIR+=" --src-rdir $2"; shift ;;
        -f|--full-compilation) FULL_COMPILATION=1 ;;
        -e|--edit-qsys) EDIT_QSYS=1 ;;
		-nonprog) PROGRAMMING=0 ;;
        -debug) DEBUG=1 ;;
        -h|--help) echo "Use ${0##*/} -h | --help | [options]";
                   echo "Options";        
                   echo "[-qsys|--qsys-prj] <qsys_project_name>";
                   echo "    Required. Name of the QSYS project to generate WITHOUT extension.";
                   echo "    The <qsys_project_name>.qsys should be in the current path";
                   echo " ";
                   echo "[-qpr|--quartus-prj] <quartus_project_name>";
                   echo "    Required. Name of the QUARTUS project to compile WITHOUT extension.";
                   echo "    The <quartus_project_name>.qpf should be in the current path";
                   echo " ";
                   echo "[-bsp|--bsp-name] <BSP_project_name>";
                   echo "    Optional. Name of the BSP project to be compiled/created.";
                   echo " ";
                   echo "[-app|--app-name] <APPLICATION_project_name>";
                   echo "    Required. Name of the application project to be compiled/created.";
                   echo " ";
                   echo "[-brd|--board] <board_code_name>";
                   echo "    Optional. Name of the subdirectory in which the Quartus Prime directory is located."
				   echo "    A directory with this name should be in current path.";
                   echo "    Valid options: \[de10nano | bemicrosdk | de2_115 \]";
                   echo " ";                   
                   echo "[-e]";
                   echo "    Optional. Edit Qsys project. The Platform Designer GUI is invoked";
                   echo  "   to edit the QSYS project"
                   echo " ";
                   echo "[-f]";
                   echo "    Optional. Force full project compilation.";
                   echo "    If omitted, only the outdated files are processed"
                   echo " ";
                   echo "[-dir|--software-dir] <directory>";
                   echo "    Optional. Name of the directory in which software application shall"
                   echo "    be created. If omitted, \"software\" will be used";
                   echo "    <BSP_project_name> and <APPLICATION_project_name> directories"
                   echo "    will be created/appended"
                   echo " ";
                   echo "[-sd|--sd-volumne] <drive_letter>";
                   echo "    Optional. Drive letter in which the SD for HPS booting is found";
                   echo "    For example: \"e\" for SD mounted in \"E:\"";
                   echo " ";
                   echo "[-fat|--fat-volumne] <drive_letter>";
                   echo "    Optional. Drive letter in which the SD FAT partition for HPS booting is mounted";
                   echo "    FAT partition may be network mounted through ftps";
                   echo "    For example: \"f\" for SD FAT partition mounted in \"F:\"";
                   echo " ";
                   echo "[-idir|--inc-dir] <include_path>";
                   echo "    Optional. If omitted, only the application path is considered";
                   echo "    If provided, header files (.h) will be searched recursively to be";
                   echo "    included to the application project. There may be as many -idir as required";
                   echo "    Path relative to project directory";
                   echo " ";
                   echo "[-sdir|--src-dir] <source_path>";
                   echo "    Optional. If omitted, only the application path is considered";
                   echo "    If provided, source files (.c) will be searched recursively to be";
                   echo "    included to the application project. There may be as many -sdir as required";
                   echo "    Path relative to project directory";                   
                   echo " ";  
                   echo "[-h|--help]";
                   echo "    Optional. Display help for this tool.";
                   echo " "
                   exit 1;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done

if [ "${APP_NAME}" == "" ]; then
	APP_NAME="hellogemrtos"
	mkdir -p -m 777 ./${SOFTWARE_DIR_NAME}/${APP_NAME}
	cp ./ip/GeMRTOS_Multiprocessor/hellogemrtos.c ./${SOFTWARE_DIR_NAME}/${APP_NAME}
	echo "Setting default application to hellogemrtos.c"
fi

if [ "${BSP_NAME}" == "" ]; then
	BSP_NAME=${APP_NAME}_bsp
	echo "Info: ${BSP_NAME} will be used as BSP project name.";	
fi

if [ "${BOARD}" != "" ] && [ ! -d ./${BOARD} ]; then
    echo "ERROR: ${BOARD} board directory not found";
	echo "[-brd|--board] option should be defined";
    exit 1
fi


if [ "${DEBUG}" == "1" ]; then
    echo "Printed for tracking"
    echo "BSP_NAME : ${BSP_NAME}";
    echo "APP_NAME : ${APP_NAME}";
    echo "SOFTWARE_DIR_NAME : ${SOFTWARE_DIR_NAME}";
    echo "QUARTUS_PRJ : ${QUARTUS_PRJ}";
    echo "QSYS_PRJ : ${QSYS_PRJ}";
    echo "SD_VOLUME : ${SD_VOLUME}";
    echo "FAT_VOLUME : ${FAT_VOLUME}";
    echo "BOARD : ${BOARD}";
    echo "APP_INCLUDE_DIR : ${APP_INCLUDE_DIR}";
    echo "APP_SRC_DIR : ${APP_SRC_DIR}";
    echo "FULL_COMPILATION : ${FULL_COMPILATION}";
    echo "EDIT_QSYS : ${EDIT_QSYS}";

    # Turn echo commands on with variable replacement (set +x turns the echo off)
    set -x
fi


# Get the starting time
START=$(date +%s);

# Open Qsys is argument is equal to al
if [ "${EDIT_QSYS}" == "1" ]; then 
    # Open Qsys to modify the SOPC
    qsys-edit.exe ${QSYS_PRJ}.qsys
fi

# if .qsys is newer than .sof, then full compilation
generated=`stat -c "%Y" ${QSYS_PRJ}.qsys`
compilated=`stat -c "%Y" ./output_files/${QUARTUS_PRJ}.sof`

if [ $generated -gt $compilated ] || [ ! -f ./output_files/${QUARTUS_PRJ}.sof ]; then
	FULL_COMPILATION=1
fi

if [ "${FULL_COMPILATION}" == "1" ]; then
    echo "Start a full compilation"
	# Remove previous db
    rm -rf ./db 
    rm -rf ./incremental_db
	
    # Remove previous qsys generation files	
    rm -rf ./${QSYS_PRJ}
	
    # Clean the previous Quartus compilation files
    rm -rf ./output_files
    mkdir -m 777 ./output_files

    # Remove the previous hps folder to create from scratch
    rm -rf ./hps_isw_handoff
fi

# Check if a source file was modified to generate the system
# from https://stackoverflow.com/questions/4561895/how-to-recursively-find-the-latest-modified-file-in-a-directory


newest_file=`find ./ip/ -type f -name "*.vhd" -o -name "*.qsys" -o -name "*.qpf" -o -name "*.qsf" -o -name "*.v" -o -name "*.tcl" -printf '%T@ %p\n' | sort -n | tail -1 | cut -f2 -d " "`
newest_file_time=`stat -c "%Y" $newest_file`
generated=`stat -c "%Y" ${QSYS_PRJ}.sopcinfo`

if [ $newest_file_time -gt $generated ]; then
	FULL_COMPILATION=1
fi

if [ "${FULL_COMPILATION}" == "1" ]; then
    # Generate the Qsys SOPC
    qsys-generate.exe ${QSYS_PRJ}.qsys --upgrade-ip-cores 2>> ${error_log_file}
    qsys-generate.exe ${QSYS_PRJ}.qsys --synthesis=VERILOG 
fi



if [ -e ./grtos_nios_bsp_create.sh ]; then
	generated=`stat -c "%Y" grtos_nios_bsp_create.sh`
fi

newest_file=`find ./ -type f -name "${QSYS_PRJ}.qsys" -o -wholename "./ip/GeMRTOS_Multiprocessor/qsysscript.tcl" -printf '%T@ %p\n' | sort -n | tail -1 | cut -f2 -d " "`
newest_file_time=`stat -c "%Y" $newest_file`


if [ ! -e ./grtos_nios_bsp_create.sh ] || [ $newest_file_time -gt $generated ] || [ "${FULL_COMPILATION}" == "1" ]; then 
    # Get  data to produce the BSP settings file for HPS BSP and Nios BSP
    qsys-script.exe --system-file=${QSYS_PRJ}.qsys --script=./ip/GeMRTOS_Multiprocessor/qsysscript.tcl 
fi


# Create and generate the BSP setting file
bash grtos_nios_bsp_create.sh ./${SOFTWARE_DIR_NAME}/${BSP_NAME} 2>> ${error_log_file}
# Execute something like: nios2-bsp hal ./software/hellogrtos_bsp soc_system.sopcinfo --cpu-name GRTOS_Multiprocessor_0_gemrtos_proc_1 --cmd "set_setting hal.enable_reduced_device_drivers true" --cmd "set_setting hal.stderr GRTOS_Multiprocessor_0_jtag_uart_1" --cmd "set_setting hal.stdin GRTOS_Multiprocessor_0_jtag_uart_0"  --cmd "set_setting hal.stdout GRTOS_Multiprocessor_0_jtag_uart_0" --cmd "add_section_mapping rstaux GRTOS_Multiprocessor_0_onchip_memory2_0" 


# Build the BSP
cd ./${SOFTWARE_DIR_NAME}/${BSP_NAME} 2>> ${error_log_file}
# make clean 2>> ${error_log_file}
make all 2>> ${error_log_file}
cd ../../ 2>> ${error_log_file}


# Generate the Makefile of the application and build
nios2-app-generate-makefile.exe --bsp-dir ./${SOFTWARE_DIR_NAME}/${BSP_NAME} --app-dir ./${SOFTWARE_DIR_NAME}/${APP_NAME} --elf-name ${APP_NAME}.elf --set APP_CFLAGS_USER_FLAGS \"-Wa,-relax-all\" --inc-rdir ./${SOFTWARE_DIR_NAME}/${APP_NAME} --src-rdir ./${SOFTWARE_DIR_NAME}/${APP_NAME} ${APP_INCLUDE_DIR} ${APP_SRC_DIR} 2>> ${error_log_file}

# Build the application
cd ./${SOFTWARE_DIR_NAME}/${APP_NAME} 2>> ${error_log_file}
make all 2>> ${error_log_file}
cd ../../ 2>> ${error_log_file}

    # 24-08-2023 # Make memory initialization files
    # 24-08-2023 cd ./${SOFTWARE_DIR_NAME}/${APP_NAME} 2>> ${error_log_file}
    # 24-08-2023 make mem_init_generate 2>> ${error_log_file}
    # 24-08-2023 cd ../../ 2>> ${error_log_file}
    # 24-08-2023 
    # 24-08-2023 sleep 2s
    # 24-08-2023 sync
    # 24-08-2023 quartus_cdb.exe --update_mif ${QUARTUS_PRJ}.qpf 
    # 24-08-2023 quartus_asm.exe ${QUARTUS_PRJ}.qpf 
    # 24-08-2023 quartus_cdb.exe --update_mif ${QUARTUS_PRJ}.qpf 
    # 24-08-2023 quartus_asm.exe ${QUARTUS_PRJ}.qpf  
    
if [ "${FULL_COMPILATION}" == "1" ]; then
    # Compile the Quartus project
    quartus_cmd.exe ${QUARTUS_PRJ}.qpf -c ${QUARTUS_PRJ}.qsf 2>> ${error_log_file}

    # #######################################
    # Compile the Quartus project and create BSP for HPS if required
    bash grtos_hps_bsp_create.sh ${QUARTUS_PRJ} ${SD_VOLUME} ${FAT_VOLUME} ${SOFTWARE_DIR_NAME} ${QSYS_PRJ} ${BOARD} ${error_log_file} 2>> ${error_log_file}
    # #######################################
fi


END=$(date +%s);
echo $((END-START)) | awk '{print "Total time elapsed: "int($1/3600)":"int(($1%3600)/60)":"int($1%60)}'
if [ "${BOARD}" == "de10nano" ]; then
	read -n 1 -s -r -p "Change SD to board and press any key to continue downloading nios ${SOFTWARE_DIR_NAME}"
fi

# Programming the FPGA device. Try first and second JTAG instance
if [ "${PROGRAMMING}" == "1" ]; then
	# command1="nios2-configure-sof ./output_files/${QUARTUS_PRJ}.sof"
	# command2="nios2-configure-sof -d 2 output_files/${QUARTUS_PRJ}.sof"

	command1='quartus_pgm.exe -m JTAG -o '"p;./output_files/${QUARTUS_PRJ}.sof"' --erase_all'
	command2='quartus_pgm.exe -m JTAG -o '"p;./output_files/${QUARTUS_PRJ}.sof@2"' --erase_all'
	$command1 || { 
	$command2 
	} || {
		echo "ERROR when programming the FPGA device. You may try with Quartus Prime and -nonprog gemrtos_build option"
		exit 1
	}
fi

jtagconfig.exe -n 2>> ${error_log_file}
# from https://forum.trenz-electronic.de/index.php?topic=1228.0
jtagconfig.exe --setparam 1 JtagClock 9M 2>> ${error_log_file}
jtagconfig.exe --getparam 1 JtagClock 2>> ${error_log_file}

# Open terminal windows for standard outputs
# SYSTEM=$(jtagconfig.exe -n)
mapfile -t StringArray <<< "$(jtagconfig.exe -n)"
for val in "${StringArray[@]}"; do
	# echo "##:$val"
	if [[ $val == *"JTAG UART"* ]]; then
		# echo "si:$val"
		instance=$(awk -F'#' '{print $2}' <<< $val)
		# echo $instance
		cmd.exe /c start nios2-terminal.exe -v --flush --no-quit-on-ctrl-d --instance=$instance 
		# cygstart nios2-terminal.exe -v --flush --no-quit-on-ctrl-d --instance=0 2>> ${error_log_file}
		sleep 1s 
	fi
done


# cmd.exe /c start nios2-terminal.exe -v --flush --no-quit-on-ctrl-d --instance=0 2>> ${error_log_file}
# # cygstart nios2-terminal.exe -v --flush --no-quit-on-ctrl-d --instance=0 2>> ${error_log_file}
# sleep 2s 2>> ${error_log_file}
# #cygstart terminal_err.sh
# cmd.exe /c start nios2-terminal.exe -v --flush --no-quit-on-ctrl-d --instance=1 2>> ${error_log_file}
# # cygstart nios2-terminal.exe -v --flush --no-quit-on-ctrl-d --instance=1 2>> ${error_log_file} 
# sleep 2s 2>> ${error_log_file}
# # cygstart terminal_1.sh
# cmd.exe /c start nios2-terminal.exe -v --flush --no-quit-on-ctrl-d --instance=2 2>> ${error_log_file}
# # cygstart nios2-terminal.exe -v --flush --no-quit-on-ctrl-d --instance=2 2>> ${error_log_file}
# sleep 2s 2>> ${error_log_file}
# # cygstart terminal_2.sh
# cmd.exe /c start nios2-terminal.exe -v --flush --no-quit-on-ctrl-d --instance=3 2>> ${error_log_file}
# # cygstart nios2-terminal.exe -v --flush --no-quit-on-ctrl-d --instance=3 2>> ${error_log_file}
# sleep 2s 2>> ${error_log_file}
# # cygstart terminal_3.sh
# cmd.exe /c start nios2-terminal.exe -v --flush --no-quit-on-ctrl-d --instance=4 2>> ${error_log_file}
# # cygstart nios2-terminal.exe -v --flush --no-quit-on-ctrl-d --instance=4 2>> ${error_log_file}
# sleep 2s 2>> ${error_log_file}
# cmd.exe /c start nios2-terminal.exe -v --flush --no-quit-on-ctrl-d --instance=5 2>> ${error_log_file}
# # cygstart nios2-terminal -v --flush --no-quit-on-ctrl-d --instance=5 2>> ${error_log_file}
# sleep 2s 2>> ${error_log_file}

nios2-download --go -r ./${SOFTWARE_DIR_NAME}/${APP_NAME}/${APP_NAME}.elf --instance=0 2>> ${error_log_file}
# nios2-download -r ./${SOFTWARE_DIR_NAME}/${APP_NAME}/${APP_NAME}.elf --instance=0 2>> ${error_log_file}

# cmd.exe /c start nios2-gdb-server.exe --go --tcpport 2342 --tcppersist -i 0
# cmd.exe /c start nios2-gdb-server.exe --tcpport 2344 --tcppersist -i 2
# cmd.exe /c start nios2-gdb-server.exe --tcpport 2343 --tcppersist -i 1
# cmd.exe /c start nios2-gdb-server.exe --tcpport 2342 --tcppersist -i 0

END=$(date +%s);
echo $((END-START)) | awk '{print "Total time elapsed: "int($1/3600)":"int(($1%3600)/60)":"int($1%60)}'

