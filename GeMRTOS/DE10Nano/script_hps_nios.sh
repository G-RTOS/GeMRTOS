#!/bin/sh -x
# Run as ADMINISTRATOR in SOC EDS Command Shell

# Ver de ejecutar esto para preserver IP
# Info: Running Quartus Prime Compiler Database Interface
#	Info: Version 18.0.0 Build 614 04/24/2018 SJ Standard Edition
#	Info: Processing started: Fri Apr 30 20:23:06 2021
# Info: Command: quartus_cdb grtos -c grtos --incremental_compilation_export=grtos.qxp --incremental_compilation_export_partition_name=Top --incremental_compilation_export_post_synth=on --incremental_compilation_export_post_fit=off --incremental_compilation_export_routing=on --incremental_compilation_export_flatten=on


# bash grtos_bsp_create.sh "./software/hellogrtos_bsp" 

# de https://stackoverflow.com/questions/5499472/specify-command-line-arguments-like-name-value-pairs-for-shell-script
# llamar bash script_hps_nios.sh QSYS=qsys QUARTUS_PROJECT="./DE10_NANO_SoC_GHRD" QSYS_PROJECT="./soc_system" NIOS_BSP_DIR="./software/hellogrtos_bsp" |& tee script.txt
# llamar para solo nios download 
# bash script_hps_nios.sh NIOS_BSP_DIR="./software/hellogrtos_bsp" |& tee script.txt

QSYS=""
QUARTUS_PROJECT=""
QSYS_PROJECT=""
NIOS_BSP_DIR=""
SOF=""

for ARGUMENT in "$@"
do
    KEY=$(echo $ARGUMENT | cut -f1 -d=)
    VALUE=$(echo $ARGUMENT | cut -f2 -d=)   
    case "$KEY" in
            QSYS)              QSYS=${VALUE} ;;
            QUARTUS_PROJECT)   QUARTUS_PROJECT=${VALUE} ;;
            QSYS_PROJECT)      QSYS_PROJECT=${VALUE} ;;
            NIOS_BSP_DIR)      NIOS_BSP_DIR=${VALUE} ;;
            SOF)               SOF=${VALUE} ;;
            *)   
    esac    
done

echo "QSYS = $QSYS"
echo "QUARTUS_PROJECT = ${QUARTUS_PROJECT}"
echo "QSYS_PROJECT = ${QSYS_PROJECT}"
echo "NIOS_BSP_DIR = ${NIOS_BSP_DIR}"
echo "SOF = $SOF"

# Get the starting time
START=$(date +%s);

# Open Qsys is argument is equal to al
if [ "$QSYS" = "qsys" ]; then 
    # Open Qsys to modify the SOPC
    qsys-edit ${QSYS_PROJECT}.qsys
elif [ "$QSYS" != "" ]; then
    # QSYS argument not valid
    echo "Only vaue for QSYS argument is qsys"
    exit 0
fi

# Update the GeMRTOS controller if ./Quartus/grtos/grtos.vhd or ./Quartus/grtos/STD_FIFO.vhd are newer than ./Quartus/grtos/grtos.qxp
gen_grtos=`stat -c "%Y" ./Quartus/grtos/grtos.vhd`
gen_STD_FIFO=`stat -c "%Y" ./Quartus/grtos/STD_FIFO.vhd`
com_qxp=`stat -c "%Y" ./Quartus/grtos/grtos.qxp`
if [ $gen_grtos -gt $com_qxp ] || [ $gen_STD_FIFO -gt $com_qxp ] || [ ! -e ./Quartus/grtos/grtos.qxp ]; then
    # Updata the encriptation of the grrtos
    echo "Updating grtos encryptation"
    quartus_cdb ./Quartus/grtos/grtos -c grtos --incremental_compilation_export=grtos.qxp --incremental_compilation_export_partition_name=Top --incremental_compilation_export_post_synth=on --incremental_compilation_export_post_fit=off --incremental_compilation_export_routing=on --incremental_compilation_export_flatten=on
    cp ./Quartus/grtos/grtos.qxp ./grtos
fi

# Generation of QSYS, Quartus and HPS projects
if [ "$QSYS_PROJECT" != "" ]; then
    # Geberate qsys if updated or not generated
    generated=`stat -c "%Y" ${QSYS_PROJECT}.qsys`
    compilated=`stat -c "%Y" ./output_files/$QUARTUS_PROJECT.sof`

    if [ $generated -gt $compilated ] || [ ! -e ./output_files/$QUARTUS_PROJECT.sof ]; then

        echo "QSYS PROJECT NOT UPDATE: GENERATING AND COMPILING"
        # Clean the previos Qsys generation files
        rm -rf -- ./$QSYS_PROJECT/synthesis/*

        # Generate the Qsys SOPC
        qsys-generate ${QSYS_PROJECT}.qsys --upgrade-ip-cores
        qsys-generate ${QSYS_PROJECT}.qsys --synthesis=VERILOG  

        # #########
        # Get  data to produce the BSP settings file for HPS
        qsys-script --system-file=${QSYS_PROJECT}.qsys --script=qsysscript.tcl
        # Run the BSP creation from the executable produced by the qsys-script
        # SACAR ESTE !!!!!!!!!!!
        bsp-generate-files --bsp-dir "./software/spl_bsp" --settings "./software/spl_bsp/settings.bsp"
        bash grtos_bsp_create.sh ${NIOS_BSP_DIR} 

        # #########
        # Clean previous compilations
        make clean --directory=${NIOS_BSP_DIR}
        make clean --directory=./software/hellogrtos
        # nios2-bsp-update-settings --bsp-dir ${NIOS_BSP_DIR} --settings ${NIOS_BSP_DIR}/settings.bsp

        # Poner si se quiere el codigo en memoria inerna
        # nios2-bsp-update-settings --bsp-dir ${NIOS_BSP_DIR} --settings ${NIOS_BSP_DIR}/settings.bsp --cmd add_section_mapping .text GRTOS_Multiprocessor_0_onchip_memory2_2

        # Build both projects
        make all --directory=${NIOS_BSP_DIR}
        make all --directory=./software/hellogrtos

        cd ./software/hellogrtos
        make mem_init_generate
        cd ..
        cd ..
        quartus_cdb DE10_NANO_SoC_GHRD -c DE10_NANO_SoC_GHRD --update_mif

        # #########

        # Clean the previous Quartus compilation files
        rm ./output_files/*

        # Compile the Quartus project
        quartus_cmd ${QUARTUS_PROJECT}.qpf -c ${QUARTUS_PROJECT}.qsf


    else
        echo "Qsys already updated\n"
    fi
    END=$(date +%s);
    echo $((END-START)) | awk '{print "Total time elapsed: "int($1/3600)":"int(($1%3600)/60)":"int($1%60)}'
    read -n 1 -s -r -p "Change SD to board and press any key to continue downloading nios software"
fi

# Show jtag devices connected
echo on

jtagconfig -n
# from https://forum.trenz-electronic.de/index.php?topic=1228.0
jtagconfig --setparam 1 JtagClock 9M
jtagconfig.exe --getparam 1 JtagClock

if [ "$SOF" = "true" ]; then 
    # Configure the FPGA device
    # nios2-configure-sof output_files/grtosproject.sof
    # PARA DE10-NANO
    nios2-configure-sof -d 2 output_files/$QUARTUS_PROJECT.sof
    # Otra paca
    # nios2-configure-sof output_files/$QUARTUS_PROJECT.sof
fi

## Start Software stage

# Clean previous compilations
make clean --directory=${NIOS_BSP_DIR}
make clean --directory=./software/hellogrtos

# Build the BSP project with the .sopcinfo file in ./
# nios2-bsp hal ./software/hellogrtos_bsp .
# nios2-bsp-generate-files --bsp-dir . --settings settings.bsp (C:\generic18\software\hellogrtos_bsp)
nios2-bsp-update-settings --bsp-dir ${NIOS_BSP_DIR} --settings ${NIOS_BSP_DIR}/settings.bsp 

# Poner si se quiere el codigo en memoria inerna
# nios2-bsp-update-settings --bsp-dir ${NIOS_BSP_DIR} --settings ${NIOS_BSP_DIR}/settings.bsp --cmd add_section_mapping .text GRTOS_Multiprocessor_0_onchip_memory2_2


# Para crear una seccion en el codgo. de https://www.intel.com/content/dam/www/programmable/us/en/pdfs/literature/hb/nios2/n2sw_nii52015.pdf
# nios2-bsp hal my_bsp --cmd add_section_mapping special_section onchip_special

# de https://www.intel.com/content/dam/www/programmable/us/en/pdfs/literature/hb/nios2/n2sw_nii52015.pdf pag 4-20
#nios2-bsp hal my_bsp --set hal.make.bsp_cflags_debug -g --set hal.make.bsp_cflags_optimization -O0r

# Build both projects
make all --directory=${NIOS_BSP_DIR}
make all --directory=./software/hellogrtos

# nios2-bsp-update-settings [--bsp-dir <directory>] 
#  [--cmd <tcl command>] [--cpu-name <cpu name>] 
#  [--debug] [--extended-help] [--help] [--jdi <filename>] 
#  [--librarian-factory-path <directory>] 
#  [--librarian-path <directory>] [--log <filename>]
#  [--script <filename>] [--set <name> <value>] 
#  --settings <filename> [--silent] 
#  [--sopc <filename>] [--verbose] [--version]

# Para poner en memora del chip
# cd ./software/hellogrtos
# make mem_init_generate
# cd ..
# cd ..
# quartus_cdb DE10_NANO_SoC_GHRD -c DE10_NANO_SoC_GHRD --update_mif

#make --directory=./software/hellogrtos

# Print all the symbol of the application
#nios2-elf-readelf.exe --syms ./software/hellogrtos/hellogrtos.elf
# Print a report of the mamoery and stack usage
#nios2-stackreport ./software/hellogrtos/hellogrtos.elf 

# Open terminal windows for standard outputs
cygstart nios2-terminal -v --flush --no-quit-on-ctrl-d --instance=0
sleep 5s
# cygstart nios2-terminal -v --flush --no-quit-on-ctrl-d --instance=1 
# cygstart nios2-terminal -v --flush --no-quit-on-ctrl-d --instance=1 | tee output.txt
# de https://askubuntu.com/questions/420981/how-do-i-save-terminal-output-to-a-file

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
#cygstart nios2-terminal --instance=1

# Download the application to each one of the processors, reseting them
#nios2-download --stop -r ./software/hellogrtos/hellogrtos.elf --instance=0
#nios2-download --stop -r ./software/hellogrtos/hellogrtos.elf --instance=1

##nios2-download -g ./software/hellogrtos/hellogrtos.elf --instance=1
##nios2-download -g ./software/hellogrtos/hellogrtos.elf --instance=0

# Wait for server to connect
# sleep 5s

# pause all processors
#nios2-download --stop -r --instance=0
#nios2-download --stop -r --instance=1
#nios2-download --stop -r --instance=2
#nios2-download --stop -r --instance=3

sleep 10s
# start processor 1 resetting the others ones
nios2-download --go -r ./software/hellogrtos/hellogrtos.elf --instance=0

#nios2-download --go -r ./software/hellogrtos/hellogrtos.elf --instance=0
# sleep 5s
# Connect the gdb server to each processor
#cygstart nios2-gdb-server --tcpport 2342 --tcppersist --instance=2
#cygstart nios2-gdb-server --tcpport 2343 --tcppersist --instance=1
#cygstart nios2-gdb-server --tcpport 2344 --tcppersist --instance=0
#cygstart nios2-gdb-server --tcpport 2345 --tcppersist --instance=3

# Wait for server to connect
# sleep 5

# Comando para imprimir la sesion de gdb
#cygstart bash -c \"nios2-elf-gdb --command=helpprint.txt ./software/hellogrtos/hellogrtos.elf  \> logging.txt \"

# Comandos para la ejecucion de los procesadores
#cygstart nios2-elf-gdb --command=target0.txt software/hellogrtos/hellogrtos.elf
#cygstart nios2-elf-gdb --command=target1.txt software/hellogrtos/hellogrtos.elf
#cygstart nios2-elf-gdb --command=target2.txt software/hellogrtos/hellogrtos.elf
#cygstart nios2-elf-gdb --command=target3.txt software/hellogrtos/hellogrtos.elf

# Comandos ejecutados dentro del cliente gdb
#target extended-remote 127.0.0.1:2343
#load
#jump __reset
# para logear la sesion
#set logging on
#set logging file myfile.txt


# Download the application to each one of the processors, without reseting them
#nios2-download --stop ./software/hellogrtos/hellogrtos.elf --instance=0
#nios2-download --stop ./software/hellogrtos/hellogrtos.elf --instance=1



# If gdb is desired to be executed
#nios2-elf-gdb ./software/hellogrtos/hellogrtos.elf 

#system-console --rc_script=debug.tcl --project_dir=./


#nios2-elf-addr2line --exe=./software/hellogrtos/hellogrtos.elf 0x0400c5a0


END=$(date +%s);
echo $((END-START)) | awk '{print "Total time elapsed: "int($1/3600)":"int(($1%3600)/60)":"int($1%60)}'
