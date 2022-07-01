# creacion del preloader from command line de
# https://rocketboards.org/foswiki/Documentation/SoCSWWS1IntroToAlteraSoCDevicesLab1Preloader#A_40Optional_41_Generating_the_Preloader_from_the_command_line

# bash grtos_hps_bsp_create.sh ${QUARTUS_PRJ} ${SD_VOLUME} ${FAT_VOLUME}
# bash create_hps_bsp.sh \$2 \$3
# $1 SD_VOLUME
# $2 FAT_VOLUME
# $3 SOFTWARE_DIR_NAME
# $4 QSYS_PRJ
# $5 QUARTUS_PRJ
# $6 BOARD

SD_VOLUME=$1
FAT_VOLUME=$2
SOFTWARE_DIR_NAME=$3
QSYS_PRJ=$4
QUARTUS_PRJ=$5
BOARD=$6

echo "Runing create_hps_bsp script"
echo "SD_VOLUME  : $SD_VOLUME"
echo "FAT_VOLUME  : $FAT_VOLUME"
echo "SOFTWARE_DIR_NAME  : $SOFTWARE_DIR_NAME"
echo "QSYS_PRJ  : $QSYS_PRJ"
echo "QUARTUS_PRJ  : $QUARTUS_PRJ"
echo "BOARD  : $BOARD"

if [ ! "${BOARD}" = "de10nano" ]; then
   echo "Error: -brd | --board <board_name> option is required when HPS is implemented"
   echo "Valid board_name: de10nano | de1soc"
   exit 1
fi


# bash bsp-createsettings.sh
# bsp-generate-files --bsp-dir "./software/spl_bsp" --settings "./software/spl_bsp/settings.bsp"

# if [ ! -e ./software/spl_bsp ]; then
#     echo "Select .\hps_isw_handoff\soc_system_hps_0 como preloader directory"
#     echo "Select FAT SUPPORT"
#     bsp-editor
# else
#     bsp-create-settings --type spl --bsp-dir software/spl_bsp --preloader-settings-dir hps_isw_handoff/soc_system_hps_0/ --settings software/spl_bsp/settings.bsp
# fi

# Remove the bsp directory to build it from scratch
rm -rf ./${SOFTWARE_DIR_NAME}/spl_bsp
mkdir ./${SOFTWARE_DIR_NAME}/spl_bsp

bsp-create-settings --type spl --bsp-dir "./${SOFTWARE_DIR_NAME}/spl_bsp" --preloader-settings-dir "./hps_isw_handoff/${QSYS_PRJ}_hps_0/" --settings "./${SOFTWARE_DIR_NAME}/spl_bsp/settings.bsp" --set spl.boot.FAT_SUPPORT 1

bsp-generate-files --bsp-dir "./${SOFTWARE_DIR_NAME}/spl_bsp" --settings "./${SOFTWARE_DIR_NAME}/spl_bsp/settings.bsp"

# Copy the uboot source !!! Check if it does exist and avoid copying
# cmd /c "mkdir .\\${SOFTWARE_DIR_NAME}\\spl_bsp\\uboot-socfpga"
# cmd /c "xcopy .\\misc\\uboot-socfpga .\\${SOFTWARE_DIR_NAME}\\spl_bsp\\uboot-socfpga\\ /E/H"

# Try to avoid if already made !!!!
cd ./${SOFTWARE_DIR_NAME}/spl_bsp/
make 
make uboot
cd ..
cd ..

# copiar preloader en la SD que debe estar como disco E
alt-boot-disk-util -p ./${SOFTWARE_DIR_NAME}/spl_bsp/preloader-mkpimage.bin -b ./${SOFTWARE_DIR_NAME}/spl_bsp/uboot-socfpga/u-boot.img -a write -d ${SD_VOLUME}
#alt-boot-disk-util -p ./software/spl_bsp/preloader-mkpimage.bin -a write -d e

mkimage -A arm -O linux -T script -C none -a 0 -e 0 -n "bootscript" -d ./misc/boards/${BOARD}/boot/u-boot.txt ./misc/boards/${BOARD}/boot/u-boot.scr
# ###################################################################
# cd ./${SOFTWARE_DIR_NAME}/script/
# mkimage -A arm -O linux -T script -C none -a 0 -e 0 -n "bootscript" -d u-boot.txt u-boot.scr
# cd ..
# cd ..
# cp ./${SOFTWARE_DIR_NAME}/script/u-boot.scr ../../../${SD_VOLUME}
# diff -qsr ./${SOFTWARE_DIR_NAME}/script/u-boot.scr ../../../${SD_VOLUME}
# cp ./${SOFTWARE_DIR_NAME}/script/u-boot.scr ../../${FAT_VOLUME}/sdfat
# diff -qsr ./${SOFTWARE_DIR_NAME}/script/u-boot.scr ../../${FAT_VOLUME}/sdfat
# 
# cp ./${SOFTWARE_DIR_NAME}/spl_bsp/uboot-socfpga/u-boot.img ../../${FAT_VOLUME}/sdfat
# sleep 10s
# ###################################################################


# Compile the Quartus project
# quartus_cmd DE10_NANO_SoC_GHRD.qpf -c DE10_NANO_SoC_GHRD.qsf
echo TRUE  

# Generar el rbf
# quartus_cpf -c ./output_files/DE10_NANO_SoC_GHRD.sof soc_system.rbf
# De https://rocketboards.org/foswiki/pub/Documentation/DE10Standard/DE10-Standard_Control_Panel.pdf
quartus_cpf -c -o bitstream_compression=on ./output_files/${QUARTUS_PRJ}.sof soc_system.rbf

# ###################################################################
# # Copiarlo a la FAT32 que esta en el drive E
# cp soc_system.rbf ../../../${SD_VOLUME}
# diff -qsr soc_system.rbf ../../../${SD_VOLUME}
# cp soc_system.rbf ../../${FAT_VOLUME}/sdfat
# diff -qsr soc_system.rbf ../../${FAT_VOLUME}/sdfat
# ###################################################################

# Generar el archivo dts
#sopc2dts -i soc_system.sopcinfo -o socfpga.dts
#dtc -I dts -O dtb -o soc_system.dtb socfpga.dts

#sopc2dts -i soc_system.sopcinfo -o socfpga.dtb --type dtb --bridge-removal all --clocks

# Este era el primero que funciona sin crossing 
#sopc2dts --input soc_system.sopcinfo --output socfpga.dtb --type dtb --board hps_common_board_info.xml --board soc_system_board_info.xml --bridge-removal all --clocks -v
#sopc2dts --input soc_system.sopcinfo --output socfpga.dtb --type dtb --board hps_common_board_info.xml --board soc_system_board_info.xml --bridge-removal all --sopc-parameters cmacro --clocks -v
sopc2dts --input ${QSYS_PRJ}.sopcinfo --output socfpga.dtb --type dtb --board ./misc/boards/${BOARD}/hps_common_board_info.xml --board ./misc/boards/${BOARD}/soc_system_board_info.xml --bridge-removal all --sopc-parameters node --clocks -v

# ###################################################################
# cp socfpga.dtb ../../../${SD_VOLUME}
# diff -qsr socfpga.dtb ../../../${SD_VOLUME}
# cp socfpga.dtb ../../${FAT_VOLUME}/sdfat
# diff -qsr socfpga.dtb ../../${FAT_VOLUME}/sdfat
# ###################################################################

# version dts que es la version leible de dtb y se convierte con la aplicacion dtc
sopc2dts --input ${QSYS_PRJ}.sopcinfo --output socfpga.dts --type dts --board ./misc/boards/${BOARD}/hps_common_board_info.xml --board ./misc/boards/${BOARD}/soc_system_board_info.xml --bridge-removal all --clocks -v

# para que transfiera y lo saque de la cache
# ###################################################################
# cp soc_system.rbf ../../../${SD_VOLUME}
# diff -qsr soc_system.rbf ../../../${SD_VOLUME}
# cp soc_system.rbf ../../${FAT_VOLUME}/sdfat
# diff -qsr soc_system.rbf ../../${FAT_VOLUME}/sdfat
# ###################################################################


# Obtengo todos los archivos cabeceras y los transfiero al directorio headers
rm -rf ./${SOFTWARE_DIR_NAME}/headers/
mkdir ./${SOFTWARE_DIR_NAME}/headers/

# Create header files for GeMRTOS console debugging
sopc-create-header-files ${QSYS_PRJ}.sopcinfo --output-dir ./${SOFTWARE_DIR_NAME}/headers/     # system_modules.h

# ###################################################################
# mkdir -p /cygdrive/${SD_VOLUME}/headers
# chmod -R 0777 /cygdrive/${SD_VOLUME}/headers/
# cp *.h /cygdrive/${SD_VOLUME}/headers
# cp *.h /cygdrive/${FAT_VOLUME}/sdfat/headers
# sync 
# ###################################################################

# cp soc_system.dtb ../../../e
# diff -qsr soc_system.dtb ../../../e

# cmd /c script.bat

# Copy is done through CMD to avoid cygwin disk cache
if [ ! "${SD_VOLUME}" = "0" ]; then
    cmd /c "del ${SD_VOLUME}:\\u-boot.scr"
    cmd /c "del ${SD_VOLUME}:\\soc_system.rbf"
    cmd /c "del ${SD_VOLUME}:\\socfpga.dtb"
    cmd /c "del ${SD_VOLUME}:\\u-boot.img"

    cmd /c "copy .\\misc\boards\\${BOARD}\\boot\\u-boot.scr ${SD_VOLUME}:"
    cmd /c "copy soc_system.rbf ${SD_VOLUME}:"
    cmd /c "copy socfpga.dtb ${SD_VOLUME}:"
    cmd /c "copy .\\${SOFTWARE_DIR_NAME}\\spl_bsp\\uboot-socfpga\\u-boot.img ${SD_VOLUME}:"
    
    cmd /c "rmdir -r ${SD_VOLUME}:\\headers"
    cmd /c "copy .\\${SOFTWARE_DIR_NAME}\\headers\\* ${SD_VOLUME}:\\headers"
fi

if [ ! "${FAT_VOLUME}" = "0" ]; then
    cmd /c "copy .\\misc\boards\\${BOARD}\\boot\\u-boot.scr ${FAT_VOLUME}:\\sdfat\\"
    cmd /c "copy soc_system.rbf ${FAT_VOLUME}:\\sdfat\\"
    cmd /c "copy socfpga.dtb ${FAT_VOLUME}:\\sdfat\\"
    cmd /c "copy .\\${SOFTWARE_DIR_NAME}\\spl_bsp\\uboot-socfpga\\u-boot.img ${FAT_VOLUME}:\\sdfat\\"
    
    cmd /c "rmdir -r ${FAT_VOLUME}:\\sdfat\\headers"
    cmd /c "copy \\${SOFTWARE_DIR_NAME}\\headers\\* ${FAT_VOLUME}:\\sdfat\\headers"
fi
