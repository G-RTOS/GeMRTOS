#!/bin/sh -x

sftp -o StrictHostKeyChecking=no ubuntu@192.168.0.197
cd sdfat
cp ./software/script/u-boot.scr ../../../e
cp soc_system.rbf ../../../e
cp socfpga.dtb ../../../e
cp soc_system.rbf ../../../e
cp *.h /cygdrive/e/headers

