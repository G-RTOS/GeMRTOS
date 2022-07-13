#!/bin/bash
# argumentos desde https://stackoverflow.com/questions/192249/how-do-i-parse-command-line-arguments-in-bash

# Get parameters from https://stackoverflow.com/questions/192249/how-do-i-parse-command-line-arguments-in-bash
# bash new_script.sh -bsp hellogrtos_bsp -app hellogrtos -qpr DE10_NANO_SoC_GHRD -qsys soc_system -dir software -sd e -fat f --board de10nano
# bash new_script.sh -bsp hellogrtos_bsp -app hellogrtos -qpr DE10_NANO_SoC_GHRD -qsys soc_system -dir software -gemdir ../gemrtos_ips -sd e -fat f --board de10nano -f -debug|& tee new_script.txt

BSP_NAME=""                   #hellogrtos_bsp
APP_NAME=""                   #hellogrtos
SOFTWARE_DIR_NAME="software"  #software
QUARTUS_PRJ="0"               # DE10_NANO_SoC_GHRD
QSYS_PRJ="0"                  #soc_system
SD_VOLUME="0"                 #e
FAT_VOLUME="0"                #f
BOARD="0"
APP_INCLUDE_DIR=""
APP_SRC_DIR=""
FULL_COMPILATION=0
EDIT_QSYS=0
DEBUG=0
GEM_IPS_DIR="$"

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
        -gemdir|--gemrtos-dir) GEM_IPS_DIR+=",$2/*/*"; shift ;;
        -f|--full-compilation) FULL_COMPILATION=1 ;;
        -e|--edit-qsys) EDIT_QSYS=1 ;;
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
                   echo "    Required. Name of the BSP project to be compiled/created.";
                   echo " ";
                   echo "[-app|--app-name] <APPLICATION_project_name>";
                   echo "    Required. Name of the application project to be compiled/created.";
                   echo " ";
                   echo "[-e]";
                   echo "    Optional. Edit Qsys project. The Platform Designer GUI is invoked";
                   echo  "   to edit the QSYS project"
                   echo " ";
                   echo "[-f]";
                   echo "    Optional. Force full project compilation.";
                   echo "    If omitted, only the outdated files are processed"
                   echo " ";
                   echo "[-brd|--board] <board_code_name>";
                   echo "    Optional. Name of the board.";
                   echo "    Valid options: \[de10nano\]";
                   echo " ";
                   echo "[-dir|--software-dir] <directory>";
                   echo "    Optional. Name of the directory in which software application shall"
                   echo "    be created. If omitter, \"software\" will be used";
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
                   echo " ";
                   echo "[-sdir|--src-dir] <source_path>";
                   echo "    Optional. If omitted, only the application path is considered";
                   echo "    If provided, source files (.c) will be searched recursively to be";
                   echo "    included to the application project. There may be as many -sdir as required";
                   echo " ";  
                   echo "[-gemdir|--gemrtos-dir] <gemrtos_ips_directory>";
                   echo "    Optional. If omitted, only the standard default path will be used.";
                   echo "    <gemrtos_ips_directory> is the root directory containing the GEMRTOS IPs";
                   echo " ";
                   echo "[-h|--help]";
                   echo "    Optional. Display help for this tool.";
                   echo " "
                   exit 1;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done

if [ "${BSP_NAME}" = "" ]; then
    echo "Error: -bsp | --bsp-name option is required"
    echo "Use ${0##*/} -h | --help for help"
    exit 1
fi

if [ "${APP_NAME}" = "" ]; then
    echo "Error: -app | --app-name option is required"
    echo "Use ${0##*/} -h | --help for help"
    exit 1
fi

if [ "${DEBUG}" = "1" ]; then
    echo "Print for tracking"
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
    echo "GEM_IPS_DIR : ${GEM_IPS_DIR}";

    # Turn echo commands on with variable replacement (set +x turns the echo off)
    set -x
fi

# Remove all the BSP to create from scratch
rm -rf ./${SOFTWARE_DIR_NAME}/${BSP_NAME}
mkdir ./${SOFTWARE_DIR_NAME}/${BSP_NAME}

# Remove all the application files to create from scratch
rm -rf ./${SOFTWARE_DIR_NAME}/${APP_NAME}/*.map
rm -rf ./${SOFTWARE_DIR_NAME}/${APP_NAME}/*.elf
rm -rf ./${SOFTWARE_DIR_NAME}/${APP_NAME}/*.objdump
rm -rf ./${SOFTWARE_DIR_NAME}/${APP_NAME}/Makefile
rm -rf ./${SOFTWARE_DIR_NAME}/${APP_NAME}/obj
rm -rf ./${SOFTWARE_DIR_NAME}/${APP_NAME}/mem_init


# Get the starting time
START=$(date +%s);

# Open Qsys is argument is equal to al
if [ "${EDIT_QSYS}" = "1" ]; then 
    # Open Qsys to modify the SOPC
    qsys-edit ${QSYS_PRJ}.qsys --search-path=${GEM_IPS_DIR}
fi



generated=`stat -c "%Y" ${QSYS_PRJ}.qsys`
compilated=`stat -c "%Y" ./output_files/${QUARTUS_PRJ}.sof`
if [ $generated -gt $compilated ] || [ ! -f ./output_files/${QUARTUS_PRJ}.sof ] || [ "${FULL_COMPILATION}" = "1" ]; then
    echo "Start a full compilation"
    rm -rf ./db
    rm -rf ./incremental_db

    # Clean the previous Quartus compilation files
    rm -rf ./output_files
    mkdir ./output_files

    # Remove previous qsys generation files
    rm -rf ./${QSYS_PRJ}

    # Remove the previous hps folder to create from scratch
    rm -rf ./hps_isw_handoff

    # Generate the Qsys SOPC
    qsys-generate ${QSYS_PRJ}.qsys --upgrade-ip-cores
    qsys-generate ${QSYS_PRJ}.qsys --synthesis=VERILOG

    echo TRUE

    # Get  data to produce the BSP settings file for HPS BSP and Nios BSP
    qsys-script --system-file=${QSYS_PRJ}.qsys --script=qsysscript.tcl
    echo TRUE

    # Create and generate the BSP setting file
    bash grtos_nios_bsp_create.sh ./${SOFTWARE_DIR_NAME}/${BSP_NAME}
    # Execute something like: nios2-bsp hal ./software/hellogrtos_bsp soc_system.sopcinfo --cpu-name GRTOS_Multiprocessor_0_nios2_qsys_1 --cmd "set_setting hal.enable_reduced_device_drivers true" --cmd "set_setting hal.stderr GRTOS_Multiprocessor_0_jtag_uart_1" --cmd "set_setting hal.stdin GRTOS_Multiprocessor_0_jtag_uart_0"  --cmd "set_setting hal.stdout GRTOS_Multiprocessor_0_jtag_uart_0" --cmd "add_section_mapping rstaux GRTOS_Multiprocessor_0_onchip_memory2_0" 
    echo TRUE

    # Build the BSP
    cd ./${SOFTWARE_DIR_NAME}/${BSP_NAME}
    make clean
    make all
    cd ../../
    echo TRUE

    # Generate the Makefile of the application and build
    nios2-app-generate-makefile --bsp-dir ./${SOFTWARE_DIR_NAME}/${BSP_NAME} --app-dir ./${SOFTWARE_DIR_NAME}/${APP_NAME} --elf-name ${APP_NAME}.elf --inc-rdir ./${SOFTWARE_DIR_NAME}/${APP_NAME} --src-rdir ./${SOFTWARE_DIR_NAME}/${APP_NAME} ${APP_INCLUDE_DIR} ${APP_SRC_DIR}
    echo TRUE

    # Build the application
    cd ./${SOFTWARE_DIR_NAME}/${APP_NAME}
    make clean
    make all
    cd ../../
    echo TRUE

    # Make memory initialization files
    cd ./${SOFTWARE_DIR_NAME}/${APP_NAME}
    make mem_init_generate
    cd ../../
    echo TRUE
    
    # Compile the Quartus project
    quartus_cmd ${QUARTUS_PRJ}.qpf -c ${QUARTUS_PRJ}.qsf
    echo TRUE  

    # #######################################
    # Compile the Quartus project and create BSP for HPS if required
    bash grtos_hps_bsp_create.sh ${QUARTUS_PRJ} ${SD_VOLUME} ${FAT_VOLUME} ${SOFTWARE_DIR_NAME} ${QSYS_PRJ} ${BOARD}
    echo TRUE
    # #######################################

    END=$(date +%s);
    echo $((END-START)) | awk '{print "Total time elapsed: "int($1/3600)":"int(($1%3600)/60)":"int($1%60)}'
    read -n 1 -s -r -p "Change SD to board and press any key to continue downloading nios ${SOFTWARE_DIR_NAME}"

else
    # Get  data to produce the BSP settings file for HPS BSP and Nios BSP
    qsys-script --system-file=${QSYS_PRJ}.qsys --script=qsysscript.tcl

    # Create and generate the BSP setting file
    bash grtos_nios_bsp_create.sh ./${SOFTWARE_DIR_NAME}/${BSP_NAME}
    # Execute something like: nios2-bsp hal ./software/hellogrtos_bsp soc_system.sopcinfo --cpu-name GRTOS_Multiprocessor_0_nios2_qsys_1 --cmd "set_setting hal.enable_reduced_device_drivers true" --cmd "set_setting hal.stderr GRTOS_Multiprocessor_0_jtag_uart_1" --cmd "set_setting hal.stdin GRTOS_Multiprocessor_0_jtag_uart_0"  --cmd "set_setting hal.stdout GRTOS_Multiprocessor_0_jtag_uart_0" --cmd "add_section_mapping rstaux GRTOS_Multiprocessor_0_onchip_memory2_0" 

    # Build the BSP
    cd ./${SOFTWARE_DIR_NAME}/${BSP_NAME}
    make clean
    make all
    cd ../../

    # Generate the Makefile of the application and build
    nios2-app-generate-makefile --bsp-dir ./${SOFTWARE_DIR_NAME}/${BSP_NAME} --app-dir ./${SOFTWARE_DIR_NAME}/${APP_NAME} --elf-name ${APP_NAME}.elf --inc-rdir ./${SOFTWARE_DIR_NAME}/${APP_NAME} --src-rdir ./${SOFTWARE_DIR_NAME}/${APP_NAME}

    # Build the application
    cd ./${SOFTWARE_DIR_NAME}/${APP_NAME}
    make clean
    make all
    cd ../../
    
    END=$(date +%s);
    echo $((END-START)) | awk '{print "Total time elapsed: "int($1/3600)":"int(($1%3600)/60)":"int($1%60)}'    
fi

jtagconfig -n
# from https://forum.trenz-electronic.de/index.php?topic=1228.0
jtagconfig --setparam 1 JtagClock 9M
jtagconfig.exe --getparam 1 JtagClock

# Open terminal windows for standard outputs
cygstart nios2-terminal -v --flush --no-quit-on-ctrl-d --instance=0
sleep 5s

#cygstart terminal_err.sh
cygstart nios2-terminal -v --flush --no-quit-on-ctrl-d --instance=1 
sleep 5s
#cygstart terminal_1.sh
cygstart nios2-terminal -v --flush --no-quit-on-ctrl-d --instance=2
sleep 5s
#cygstart terminal_2.sh
cygstart nios2-terminal -v --flush --no-quit-on-ctrl-d --instance=3
sleep 5s
#cygstart terminal_3.sh
cygstart nios2-terminal -v --flush --no-quit-on-ctrl-d --instance=4
sleep 5s
cygstart nios2-terminal -v --flush --no-quit-on-ctrl-d --instance=5
sleep 5s

nios2-download --go -r ./${SOFTWARE_DIR_NAME}/${APP_NAME}/${APP_NAME}.elf --instance=0
