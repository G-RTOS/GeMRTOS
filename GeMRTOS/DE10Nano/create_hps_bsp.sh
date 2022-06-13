# creacion del preloader from command line de
# https://rocketboards.org/foswiki/Documentation/SoCSWWS1IntroToAlteraSoCDevicesLab1Preloader#A_40Optional_41_Generating_the_Preloader_from_the_command_line

# bash bsp-createsettings.sh
# bsp-generate-files --bsp-dir "./software/spl_bsp" --settings "./software/spl_bsp/settings.bsp"


bsp-create-settings --type spl --bsp-dir software/spl_bsp --preloader-settings-dir hps_isw_handoff/soc_system_hps_0/ --settings software/spl_bsp/settings.bsp --set spl.boot.FAT_SUPPORT 1

# if [ ! -e ./software/spl_bsp ]; then
#     echo "Select .\hps_isw_handoff\soc_system_hps_0 as preloader directory"
#     echo "Select FAT SUPPORT"
#     bsp-editor
# else
#     bsp-create-settings --type spl --bsp-dir software/spl_bsp --preloader-settings-dir hps_isw_handoff/soc_system_hps_0/ --settings software/spl_bsp/settings.bsp
# fi


# transferir el archivo Makefile de patch en Windows 10 !!!!!!!!!
cd ./software/spl_bsp/
make 
make uboot
cd ..
cd ..

# copiar preloader en la SD que debe estar como disco E
# Pasar como parametro la letra de la unidad !!!!!!!!!
alt-boot-disk-util -p ./software/spl_bsp/preloader-mkpimage.bin -b ./software/spl_bsp/uboot-socfpga/u-boot.img -a write -d $1
#alt-boot-disk-util -p ./software/spl_bsp/preloader-mkpimage.bin -a write -d e


cd ./software/script/
mkimage -A arm -O linux -T script -C none -a 0 -e 0 -n "bootscript" -d u-boot.txt u-boot.scr
#mkimage -A arm -O u-boot -T script -C none -a 0 -e 0 -n "bootscript" -d u-boot.txt u-boot.scr
cd ..
cd ..

# PONER cp soc_system.rbf /cygdrive/f !!!!!!!!!!!!
cp ./software/script/u-boot.scr /cygdrive/$1
diff -qsr ./software/script/u-boot.scr /cygdrive/$1

# Fijarse las unidades de red si se desea transferir estos archivos !!!!!!!
cp ./software/script/u-boot.scr ../../f/sdfat
diff -qsr ./software/script/u-boot.scr ../../f/sdfat

cp ./software/spl_bsp/uboot-socfpga/u-boot.img ../../f/sdfat

sleep 10s


# Pasar como parametro el nombre del archivo .SOF !!!!!!!!!!!!!!!
# Generar el rbf
# quartus_cpf -c ./output_files/DE10_NANO_SoC_GHRD.sof soc_system.rbf
# De https://rocketboards.org/foswiki/pub/Documentation/DE10Standard/DE10-Standard_Control_Panel.pdf
quartus_cpf -c -o bitstream_compression=on ./output_files/DE10_NANO_SoC_GHRD.sof soc_system.rbf
# Copiarlo a la FAT32 que esta en el drive E
cp soc_system.rbf /cygdrive/$1
diff -qsr soc_system.rbf /cygdrive/$1

# Comprobar si se desea conpartirlos a red !!!!!!!!
cp soc_system.rbf ../../f/sdfat
diff -qsr soc_system.rbf ../../f/sdfat

# Generar el archivo dts
#sopc2dts -i soc_system.sopcinfo -o socfpga.dts
#dtc -I dts -O dtb -o soc_system.dtb socfpga.dts

#sopc2dts -i soc_system.sopcinfo -o socfpga.dtb --type dtb --bridge-removal all --clocks

# Este era el primero que funciona sin crossing 
#sopc2dts --input soc_system.sopcinfo --output socfpga.dtb --type dtb --board hps_common_board_info.xml --board soc_system_board_info.xml --bridge-removal all --clocks -v
#sopc2dts --input soc_system.sopcinfo --output socfpga.dtb --type dtb --board hps_common_board_info.xml --board soc_system_board_info.xml --bridge-removal all --sopc-parameters cmacro --clocks -v
# pasar como parametro el nombre del soc_system !!!!!!!!!!!
sopc2dts --input soc_system.sopcinfo --output socfpga.dtb --type dtb --board hps_common_board_info.xml --board soc_system_board_info.xml --bridge-removal all --sopc-parameters node --clocks -v

cp socfpga.dtb /cygdrive/$1
diff -qsr socfpga.dtb /cygdrive/$1

# Fijarse si se desea transferir por red !!!!!!!!!
cp socfpga.dtb ../../f/sdfat
diff -qsr socfpga.dtb ../../f/sdfat

# version dts que es la version leible de dtb y se convierte con la aplicacion dtc
sopc2dts --input soc_system.sopcinfo --output socfpga.dts --type dts --board hps_common_board_info.xml --board soc_system_board_info.xml --bridge-removal all --clocks -v

# para que transfiera y lo saque de la cache
cp soc_system.rbf /cygdrive/$1
diff -qsr soc_system.rbf /cygdrive/$1

# Fijarse si se desea transferir por red !!!!!!!!!
cp soc_system.rbf ../../f/sdfat
diff -qsr soc_system.rbf ../../f/sdfat


# Obtengo todos los archivos cabeceras y los transfiero al directorio headers
sopc-create-header-files soc_system.sopcinfo > system_modules.h
mkdir -p /cygdrive/$1/headers
chmod -R 0777 /cygdrive/$1/headers/
cp *.h /cygdrive/$1/headers

# Fijarse si se desea transferir por red !!!!!!!!!
cp *.h /cygdrive/f/sdfat/headers

sync 

# cp soc_system.dtb ../../../e
# diff -qsr soc_system.dtb ../../../e

# Modificarse el bat para recibir la letra de la unidad !!!!!!!!!
cmd /c script.bat