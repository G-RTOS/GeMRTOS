#!/bin/bash

# /**
#  *  \file
#  *  \author Ricardo Cayssials
#  *  \brief GeMRTOS build orchestration script (Nios II, Quartus Prime Standard)
#  *  \details Regenerates the Qsys system, compiles the FPGA bitstream, builds the
#  *           BSP and application, then programs the board and downloads the ELF.
#  *           Nios II counterpart of gemrtos_build.bat (used when Processor_type
#  *           is Nios II instead of Nios V).
#  *  \version 0.9a
#  *  \date      2015-2025
#  *
#  *  SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
#  *  Copyright (c) 2013-2026 GeMRTOS Project Contributors
#  *
#  *  Required Notice: Copyright GeMRTOS Project Contributors (https://gemrtos.com)
#  *
#  *  This file is part of GeMRTOS, licensed under the PolyForm Noncommercial
#  *  License 1.0.0. You may use, copy, modify, and distribute this file for
#  *  any noncommercial purpose. Commercial use requires a separate written
#  *  license from GeMRTOS. Full terms:
#  *  https://polyformproject.org/licenses/noncommercial/1.0.0
#  */



BSP_NAME=""
APP_NAME=""
SOFTWARE_DIR_NAME="software"
QUARTUS_PRJ=""
QSYS_PRJ=""
SD_VOLUME="0"
FAT_VOLUME="0"
BOARD=""
APP_INCLUDE_DIR=""
APP_SRC_DIR=""
FULL_COMPILATION=0
EDIT_QSYS=0
DEBUG=0
PROGRAMMING=1

# Example application selection (ported from gemrtos_build.bat).
# Empty -> DEFAULT_EXAMPLE below, which reproduces the
# historical behaviour exactly (the full demo, built into software/hellogemrtos/).
# See ip/GeMRTOS_Multiprocessor/examples/.
EXAMPLE=""
EXAMPLE_LIST=0
DEFAULT_EXAMPLE="10_full_demo"
EXAMPLES_DIR="ip/GeMRTOS_Multiprocessor/examples"
EXAMPLES_MANIFEST="${EXAMPLES_DIR}/manifest.txt"



# Initialise the error log file in the current directory
error_log_file=$(pwd)/error_log.txt
rm -rf ${error_log_file}

while [[ "$#" -gt 0 ]]; do
    case $1 in
        -bsp|--bsp-name) BSP_NAME="$2"; shift ;;
        -app|--app-name) APP_NAME="$2"; shift ;;
        -qpf|--quartus-prj) QUARTUS_PRJ="$2"; shift ;;
        -qsys|--qsys-prj) QSYS_PRJ="$2"; shift ;;
        -dir|--software-dir) SOFTWARE_DIR_NAME="$2"; shift ;;
        -brd|--board) BOARD="$2"; shift ;;
        -ex|--example) EXAMPLE="$2"; shift ;;
        -ex-list|--example-list) EXAMPLE_LIST=1 ;;
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
                   echo "[-qpf|--quartus-prj] <quartus_project_name>";
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
                   echo "    Optional. Free-form project or board identifier (e.g. de10nano,";
                   echo "    bemicrosdk, de2_115, or any name of your choosing). A directory with";
                   echo "    this name containing Quartus Prime files should exist in the current path.";
                   echo " ";
                   echo "[-ex|--example] <example_name>";
                   echo "    Optional. Name of the example application to deploy, from";
                   echo "    ip/GeMRTOS_Multiprocessor/examples/manifest.txt. If omitted,";
                   echo "    ${DEFAULT_EXAMPLE} is used (reproduces the historical default:";
                   echo "    the full demo, built into software/hellogemrtos/).";
                   echo " ";
                   echo "[-ex-list|--example-list]";
                   echo "    Optional. List the available examples and exit -- builds nothing.";
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

# Example application selection (ported from gemrtos_build.bat's
# manifest-driven mechanism). Mirrors it field-for-field so both build
# paths read the one shared manifest identically -- see
# ip/GeMRTOS_Multiprocessor/examples/README.md "Adding an example".
if [ "${EXAMPLE_LIST}" == "1" ]; then
	if [ ! -f "${EXAMPLES_MANIFEST}" ]; then
		echo "ERROR: examples manifest not found: ${EXAMPLES_MANIFEST}"
		exit 1
	fi
	echo "Available examples:"
	while IFS='|' read -r ex_name ex_appdir ex_desc ex_reqs || [ -n "${ex_name}" ]; do
		case "${ex_name}" in ""|\#*) continue ;; esac
		echo "  ${ex_name}  (-> ${SOFTWARE_DIR_NAME}/${ex_appdir})  ${ex_desc}"
	done < "${EXAMPLES_MANIFEST}"
	exit 0
fi

if [ "${EXAMPLE}" == "" ]; then
	EXAMPLE="${DEFAULT_EXAMPLE}"
fi

if [ ! -f "${EXAMPLES_MANIFEST}" ]; then
	echo "ERROR: examples manifest not found: ${EXAMPLES_MANIFEST}"
	exit 1
fi

EXAMPLE_FOUND=0
EXAMPLE_APPDIR=""
while IFS='|' read -r ex_name ex_appdir ex_desc ex_reqs || [ -n "${ex_name}" ]; do
	case "${ex_name}" in ""|\#*) continue ;; esac
	if [ "${ex_name}" == "${EXAMPLE}" ]; then
		EXAMPLE_FOUND=1
		EXAMPLE_APPDIR="${ex_appdir}"
	fi
done < "${EXAMPLES_MANIFEST}"

if [ "${EXAMPLE_FOUND}" == "0" ]; then
	echo "ERROR: unknown example '${EXAMPLE}'"
	echo "     : available examples are:"
	while IFS='|' read -r ex_name ex_rest || [ -n "${ex_name}" ]; do
		case "${ex_name}" in ""|\#*) continue ;; esac
		echo "         ${ex_name}"
	done < "${EXAMPLES_MANIFEST}"
	exit 1
fi

# An explicit -app always wins over the manifest's mapping.
if [ "${APP_NAME}" == "" ]; then
	APP_NAME="${EXAMPLE_APPDIR}"
fi

EXAMPLE_SRC_DIR="${EXAMPLES_DIR}/${EXAMPLE}"
if [ ! -d "${EXAMPLE_SRC_DIR}" ]; then
	echo "ERROR: example '${EXAMPLE}' is listed in the manifest but its directory"
	echo "     : is missing: ${EXAMPLE_SRC_DIR}"
	exit 1
fi

echo "Deploying example '${EXAMPLE}' into ${SOFTWARE_DIR_NAME}/${APP_NAME}"
mkdir -p -m 777 ./${SOFTWARE_DIR_NAME}/${APP_NAME}

# Warn (do not delete) if the target already holds sources from a different
# example -- only reachable via an explicit, conflicting -app. Same
# non-destructive convention as gemrtos_build.bat: cross-example contamination
# is prevented by giving each example its own directory, not by deleting.
if [ -f "./${SOFTWARE_DIR_NAME}/${APP_NAME}/.gemrtos_example" ]; then
	PREV_EXAMPLE=$(cat "./${SOFTWARE_DIR_NAME}/${APP_NAME}/.gemrtos_example")
	if [ "${PREV_EXAMPLE}" != "${EXAMPLE}" ]; then
		echo "WARNING: ${SOFTWARE_DIR_NAME}/${APP_NAME} previously held example '${PREV_EXAMPLE}'."
		echo "WARNING: its source files are NOT deleted - if they define main()"
		echo "WARNING: the link will fail with a duplicate symbol.  Use a"
		echo "WARNING: separate -app directory per example, or clean this one."
	fi
fi

cp -r "${EXAMPLE_SRC_DIR}/." "./${SOFTWARE_DIR_NAME}/${APP_NAME}/" || {
	echo "ERROR: could not deploy example '${EXAMPLE}'"
	exit 1
}

echo "${EXAMPLE}" > "./${SOFTWARE_DIR_NAME}/${APP_NAME}/.gemrtos_example"

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



newest_file=`find ./ip/ -type f -name "*.vhd" -o -name "*.qsys" -o -name "*.qpf" -o -name "*.qsf" -o -name "*.v" -o -name "*.tcl" -printf '%T@ %p\n' | sort -n | tail -1 | cut -f2 -d " "`
newest_file_time=`stat -c "%Y" $newest_file`
generated=`stat -c "%Y" ${QSYS_PRJ}.sopcinfo`

if [ $newest_file_time -gt $generated ]; then
	FULL_COMPILATION=1
fi

if [ "${FULL_COMPILATION}" == "1" ]; then
    # Generate the Qsys SOPC
    qsys-generate.exe ${QSYS_PRJ}.qsys --upgrade-ip-cores 
    # 2>> ${error_log_file}
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


# Build the BSP
cd ./${SOFTWARE_DIR_NAME}/${BSP_NAME} 2>> ${error_log_file}
make all 
# 2>> ${error_log_file}
cd ../../ 
# 2>> ${error_log_file}


# Generate the Makefile of the application and build
nios2-app-generate-makefile.exe --bsp-dir ./${SOFTWARE_DIR_NAME}/${BSP_NAME} --app-dir ./${SOFTWARE_DIR_NAME}/${APP_NAME} --elf-name ${APP_NAME}.elf --set APP_CFLAGS_USER_FLAGS \"-Wa,-relax-all\" --inc-rdir ./${SOFTWARE_DIR_NAME}/${APP_NAME} --src-rdir ./${SOFTWARE_DIR_NAME}/${APP_NAME} ${APP_INCLUDE_DIR} ${APP_SRC_DIR} 2>> ${error_log_file}

# Build the application
cd ./${SOFTWARE_DIR_NAME}/${APP_NAME} 
# 2>> ${error_log_file}
make all 
# 2>> ${error_log_file}
cd ../../ 
# 2>> ${error_log_file}

    
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
jtagconfig.exe --setparam 1 JtagClock 9M 2>> ${error_log_file}
jtagconfig.exe --getparam 1 JtagClock 2>> ${error_log_file}

# Open terminal windows for standard outputs
mapfile -t StringArray <<< "$(jtagconfig.exe -n)"
for val in "${StringArray[@]}"; do
	# echo "##:$val"
	if [[ $val == *"JTAG UART"* ]]; then
		instance=$(awk -F'#' '{print $2}' <<< $val)
		cmd.exe /c start nios2-terminal.exe -v --flush --no-quit-on-ctrl-d --instance=$instance 
		sleep 1s 
	fi
done



nios2-download --go -r ./${SOFTWARE_DIR_NAME}/${APP_NAME}/${APP_NAME}.elf --instance=0 2>> ${error_log_file}


END=$(date +%s);
echo $((END-START)) | awk '{print "Total time elapsed: "int($1/3600)":"int(($1%3600)/60)":"int($1%60)}'

