
rem echo off
setlocal ENABLEDELAYEDEXPANSION

rem set QUARTUS_PRJ=gemrtosproject
rem set QSYS_PRJ=qsysgemrtos
set SOFTWARE_DIR_NAME=software
set BSP_NAME=hellogemrtos_bsp
set APP_NAME=
set QUARTUS_PRJ=QUARTUS_PROJECT_FILE
set QSYS_PRJ=QSYS_PROJECT_FILE


if /I "%APP_NAME%" == "" (
	set APP_NAME=hellogemrtos
	mkdir .\\%SOFTWARE_DIR_NAME%\\!APP_NAME!
	copy .\\ip\\GeMRTOS_Multiprocessor\\hellogemrtos.c .\\%SOFTWARE_DIR_NAME%\\!APP_NAME!
	echo "Setting default application to hellogemrtos.c"
)


rem goto :follow

if exist "%SOFTWARE_DIR_NAME%\%APP_NAME%\%APP_NAME%.elf" rm %SOFTWARE_DIR_NAME%\%APP_NAME%\%APP_NAME%.elf


if exist "%QSYS_PRJ%.qsys" set FILE1=%QSYS_PRJ%.qsys
FOR %%i IN (%FILE1%) DO SET DATE1=%%~ti

if exist "output_files\%QUARTUS_PRJ%.sof" (
	set FILE2=output_files\%QUARTUS_PRJ%.sof
	FOR %%i IN (%FILE1%) DO SET DATE2=%%~ti
	)
if not exist "output_files\%QUARTUS_PRJ%.sof"  (
	set DATE2=01/01/1900 00:00
	set FULL_COMPILATION=1
)
if not exist "gemrtos_config.bat"  (
	set DATE2=01/01/1900 00:00
	set FULL_COMPILATION=1
)
rem echo %DATE2%

set DATE1=%DATE1:~6,4%%DATE1:~3,2%%DATE1:~0,2%%DATE1:~8,2%%DATE1:~14,2%%DATE1:~11,2%
set DATE2=%DATE2:~6,4%%DATE2:~3,2%%DATE2:~0,2%%DATE2:~8,2%%DATE2:~14,2%%DATE2:~11,2%
echo DATE1=%DATE1%
echo DATE2=%DATE2%

if /I "%DATE1%" gtr "%DATE2%" set FULL_COMPILATION=1


set command=%QSYS_ROOTDIR%/qsys-generate %QSYS_PRJ%.qsys --upgrade-ip-cores
if /I "%FULL_COMPILATION%" == "1" (
%command% || (
	echo ERROR: %QSYS_PRJ%.qsys could not update
	exit /b 1
))

set command=%QSYS_ROOTDIR%/qsys-generate.exe %QSYS_PRJ%.qsys --synthesis=VERILOG
if /I "%FULL_COMPILATION%" == "1" (
%command% || (
	echo ERROR: %QSYS_PRJ%.qsys could not synthesises
	exit /b 1
))

set command=quartus_cmd.exe %QUARTUS_PRJ%.qpf -c %QUARTUS_PRJ%.qsf
if /I "%FULL_COMPILATION%" == "1" (
%command% || (
	echo "ERROR: with %command%"
	exit /b 1
))

set command=%QSYS_ROOTDIR%/qsys-script.exe --system-file=%QSYS_PRJ%.qsys --script=./ip/GeMRTOS_Multiprocessor/qsysscript.tcl
if /I "%FULL_COMPILATION%" == "1" (
%command% || (
	echo ERROR: getting system information
	exit /b 1
))

rem GeMRTOS_Multiprocessor_0_gemrtos_proc_1
call gemrtos_config.bat
echo QSYS_PROC1 = %QSYS_PROC1%

set command=niosv-bsp -c --sopcinfo=%QSYS_PRJ%.sopcinfo -x=bsp_niosv_settings.tcl --type=hal -i=%QSYS_PROC1% %SOFTWARE_DIR_NAME%/%BSP_NAME%/settings.bsp
%command% || (
	echo ERROR: could create BSP
	exit /b 1
)

set command=niosv-app --app-dir=%SOFTWARE_DIR_NAME%/%APP_NAME% --srcs=%SOFTWARE_DIR_NAME%/%APP_NAME% --bsp-dir=%SOFTWARE_DIR_NAME%/%BSP_NAME%
%command% || (
	echo ERROR: could not create application
	exit /b 1
)

set command=cmake -G "Unix Makefiles" -B %SOFTWARE_DIR_NAME%/%APP_NAME% -S %SOFTWARE_DIR_NAME%/%APP_NAME%
%command% || (
	echo ERROR: could not produce cmake files
	exit /b 1
)

set command=make -C %SOFTWARE_DIR_NAME%/%APP_NAME%
%command% || (
	echo ERROR: in making
	exit /b 1
)

set command1=quartus_pgm -m JTAG -o "p;output_files/%QUARTUS_PRJ%.sof"
set command2=quartus_pgm -m JTAG -o "p;output_files/%QUARTUS_PRJ%.sof@2"
%command1% || (
%command2% || {
} || {
	echo ERROR: Programming Quartus project
	exit /b 1
)

rem in https://stackoverflow.com/questions/6359820/how-to-set-commands-output-as-a-variable-in-a-batch-file
rem in https://copyprogramming.com/howto/windows-bat-script-for-loop-if-substring-in-string
setlocal enabledelayedexpansion
set /A COUNTER=0
FOR /F "tokens=*" %%i IN ('jtagconfig -n') DO (
	set "VARIABLE=%%i"
	if not "!VARIABLE:JTAG UART=!" == "!VARIABLE!" (
		echo !COUNTER!
		rem set command=cmd.exe /c start nios2-terminal.exe -v --flush --no-quit-on-ctrl-d --instance=!COUNTER!
		set command=cmd.exe /c start juart-terminal.exe -v --flush --no-quit-on-ctrl-d --instance=!COUNTER!
		!command! || (
			echo ERROR: Opening JTAG terminal
			exit /b 1
		)
		set /A COUNTER=!COUNTER!+1
		timeout /t 1 > NULL
	)
)

rem jtagconfig.exe --setparam 1 JtagClock 3M
openocd-cfg-gen -p
set command=niosv-download -g -i 0 %SOFTWARE_DIR_NAME%/%APP_NAME%/%APP_NAME%.elf
%command% || (
	echo ERROR: downloading .elf file
	exit /b 1
)
rem jtagconfig.exe --setparam 1 JtagClock 24M

:follow




