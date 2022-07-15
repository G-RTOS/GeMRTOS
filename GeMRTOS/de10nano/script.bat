pwd

del e:\u-boot.scr
del e:\soc_system.rbf
del e:\socfpga.dtb
del e:\u-boot.img

copy .\software\script\u-boot.scr e:
copy soc_system.rbf e:
copy socfpga.dtb e:
copy .\software\spl_bsp\uboot-socfpga\u-boot.img e:

copy .\software\script\u-boot.scr f:\sdfat\
copy soc_system.rbf f:\sdfat\
copy socfpga.dtb f:\sdfat\
copy .\software\spl_bsp\uboot-socfpga\u-boot.img f:\sdfat\