rem
rem \file
rem \author Ricardo Cayssials
rem \brief GeMRTOS build orchestration script (Nios V, Quartus Prime Standard & Pro)
rem \details Regenerates the Qsys system, compiles the FPGA bitstream, builds the
rem          BSP and application, then programs the board and downloads the ELF.
rem          See ip/README.md for the instance-naming contract this script and
rem          qsysscript(.tcl|_pro.tcl) rely on.
rem \version 0.9a
rem \date      2015-2025
rem
rem SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
rem Copyright (c) 2013-2026 GeMRTOS Project Contributors
rem
rem Required Notice: Copyright GeMRTOS Project Contributors (https://gemrtos.com)
rem
rem This file is part of GeMRTOS, licensed under the PolyForm Noncommercial
rem License 1.0.0. You may use, copy, modify, and distribute this file for
rem any noncommercial purpose. Commercial use requires a separate written
rem license from GeMRTOS. Full terms:
rem https://polyformproject.org/licenses/noncommercial/1.0.0
rem

rem echo off
setlocal ENABLEDELAYEDEXPANSION


set SD_VOLUME=0
set FAT_VOLUME=0
set BOARD=
set APP_INCLUDE_DIR=
set APP_SRC_DIR=
set FULL_COMPILATION=0
set EDIT_QSYS=
set DEBUG=
set QUARTUS_VERSION=

set SOFTWARE_DIR_NAME=software
set BSP_NAME=hellogemrtos_bsp
set APP_NAME=

rem Npcap SDK location, a user-installed prerequisite (same status as
rem Quartus itself -- never redistributed). Only
rem consulted when generating the Windows-simulation tasks.json below; has
rem no effect on the embedded build. Override if installed elsewhere.
set NPCAP_SDK_DIR=C:\npcap-sdk

rem Example application selection.  Empty -> DEFAULT_EXAMPLE below, which
rem reproduces the historical behaviour exactly (the full demo, built into
rem software\hellogemrtos\).  See ip\GeMRTOS_Multiprocessor\examples\.
set EXAMPLE=
set EXAMPLE_LIST=
set DEFAULT_EXAMPLE=10_full_demo
set EXAMPLES_DIR=ip\GeMRTOS_Multiprocessor\examples
set EXAMPLES_MANIFEST=%EXAMPLES_DIR%\manifest.txt


:looparg
	set value=%2
    IF /I "%1" == "-std" set QUARTUS_VERSION=std& shift
    IF /I "%1" == "-pro" set QUARTUS_VERSION=pro& shift
	IF /I "%1" == "-bsp" set BSP_NAME=%2& shift
	IF /I "%1" == "--bsp-name" set BSP_NAME=%2& shift
	IF /I "%1" == "-app" set APP_NAME=%2& shift
	IF /I "%1" == "--app-name" set APP_NAME=%2& shift
	IF /I "%1" == "-qpf" set QUARTUS_PRJ=%2& shift
	IF /I "%1" == "--quartus-prj" set QUARTUS_PRJ=%2& shift
	IF /I "%1" == "-qsys" set QSYS_PRJ=%2& shift
	IF /I "%1" == "--qsys-prj" set QSYS_PRJ=%2& shift
	IF /I "%1" == "-dir" set SOFTWARE_DIR_NAME=%2& shift
	IF /I "%1" == "--software-dir" set SOFTWARE_DIR_NAME=%2& shift
	IF /I "%1" == "-brd" set BOARD=%2& shift
	IF /I "%1" == "--board" set BOARD=%2& shift
	IF /I "%1" == "-ex" set EXAMPLE=%2& shift
	IF /I "%1" == "--example" set EXAMPLE=%2& shift
	IF /I "%1" == "-ex-list" set EXAMPLE_LIST=1
	IF /I "%1" == "--example-list" set EXAMPLE_LIST=1
	IF /I "%1" == "-sd" set SD_VOLUME=%2& shift
	IF /I "%1" == "--sd-volumne" set SD_VOLUME=%2& shift
	IF /I "%1" == "-fat" set FAT_VOLUME=%2& shift
	IF /I "%1" == "--fat-volumne" set FAT_VOLUME=%2& shift
	IF /I "%1" == "-idir" set APP_INCLUDE_DIR=%APP_INCLUDE_DIR% %2& shift
	IF /I "%1" == "--inc-dir" set APP_INCLUDE_DIR=%APP_INCLUDE_DIR% %2& shift
	IF /I "%1" == "-sdir" set APP_SRC_DIR=%APP_SRC_DIR%  %2& shift
	IF /I "%1" == "--src-dir" set APP_SRC_DIR=%APP_SRC_DIR%  %2& shift
	IF /I "%1" == "-f" set FULL_COMPILATION=1
	IF /I "%1" == "--full-compilation" set FULL_COMPILATION=1
	IF /I "%1" == "-e" set EDIT_QSYS=1
	IF /I "%1" == "--edit-qsys" set EDIT_QSYS=1
	IF /I "%1" == "-debug" set DEBUG=1
	IF /I "%1" == "-h" goto :Help
	IF /I "%1" == "--help" goto :Help
	shift
	if not "%1" == "" goto looparg
	
goto :follows
		
:Help
	echo "Use ${0##*/} -h | --help | [options]";
	echo "Options";
	echo "[-std]";
	echo "    Used version standard of Quartus Prime";
	echo " ";
	echo "[-pro]";
	echo "    Used version pro of Quartus Prime";
	echo " ";
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
	echo "    Optional. Free-form project or board identifier (e.g. de10nano, bemicrosdk,";
	echo "    de2_115, or any name of your choosing). A directory with this name";
	echo "    containing Quartus Prime files should exist in the current path.";
	echo " ";
	echo "[-ex|--example] <example_name>";
	echo "    Optional. Example application to build, from";
	echo "    ip\GeMRTOS_Multiprocessor\examples\. If omitted, 10_full_demo is";
	echo "    used, which reproduces the historical behaviour exactly.";
	echo "    The example's manifest entry decides the application directory";
	echo "    (10_full_demo -> hellogemrtos); an explicit -app overrides it.";
	echo " ";
	echo "[-ex-list|--example-list]";
	echo "    Optional. List the available examples and exit without building.";
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
	exit /b 1
	
:follows

if /I "%QUARTUS_VERSION%" == "" (
	echo "ERROR: define Quartus Prime version";
    echo "     use -std argument for Standard Version, or -pro argument for Pro Version"
	exit /b 1
)


rem ---------------------------------------------------------------------------
rem Example application deployment.
rem
rem This block is the SINGLE owner of application-source deployment.
rem new_script.bat must not also copy hellogemrtos.c into the application
rem directory: with example selection in play that would drop a second
rem main() next to the selected example.
rem
rem Semantics: copy the selected example's files into its own
rem application directory, OVERWRITING same-named files and leaving anything
rem else in place.  Nothing is deleted -- a build script must not remove files
rem from a directory a user may have put their own work into.  Cross-example
rem contamination is prevented by giving each example its own directory rather
rem than by deleting, so the only way to collide is an explicit -app that
rem deliberately points two examples at one directory; that case is warned
rem about below, not silently "repaired".
rem ---------------------------------------------------------------------------

if defined EXAMPLE_LIST (
	if not exist "%EXAMPLES_MANIFEST%" (
		echo ERROR: examples manifest not found: %EXAMPLES_MANIFEST%
		exit /b 1
	)
	echo Available examples:
	for /f "usebackq eol=# tokens=1,2,3 delims=|" %%A in ("%EXAMPLES_MANIFEST%") do echo   %%A  ^(-^> %SOFTWARE_DIR_NAME%\%%B^)  %%C
	exit /b 0
)

if /I "%EXAMPLE%" == "" set EXAMPLE=%DEFAULT_EXAMPLE%

if not exist "%EXAMPLES_MANIFEST%" (
	echo ERROR: examples manifest not found: %EXAMPLES_MANIFEST%
	exit /b 1
)

set EXAMPLE_FOUND=0
set EXAMPLE_APPDIR=
for /f "usebackq eol=# tokens=1,2 delims=|" %%A in ("%EXAMPLES_MANIFEST%") do (
	if /I "%%A" == "!EXAMPLE!" (
		set EXAMPLE_FOUND=1
		set EXAMPLE_APPDIR=%%B
	)
)
if "!EXAMPLE_FOUND!" == "0" (
	echo ERROR: unknown example '!EXAMPLE!'
	echo      : available examples are:
	for /f "usebackq eol=# tokens=1 delims=|" %%A in ("%EXAMPLES_MANIFEST%") do echo          %%A
	exit /b 1
)

rem An explicit -app always wins over the manifest's mapping.
if /I "%APP_NAME%" == "" set APP_NAME=!EXAMPLE_APPDIR!

set EXAMPLE_SRC_DIR=%EXAMPLES_DIR%\!EXAMPLE!
if not exist "!EXAMPLE_SRC_DIR!" (
	echo ERROR: example '!EXAMPLE!' is listed in the manifest but its directory
	echo      : is missing: !EXAMPLE_SRC_DIR!
	exit /b 1
)

echo Deploying example '!EXAMPLE!' into %SOFTWARE_DIR_NAME%\!APP_NAME!
if not exist ".\%SOFTWARE_DIR_NAME%\!APP_NAME!" mkdir ".\%SOFTWARE_DIR_NAME%\!APP_NAME!"

rem Warn (do not delete) if the target already holds sources from a different
rem example -- only reachable via an explicit, conflicting -app.
if exist ".\%SOFTWARE_DIR_NAME%\!APP_NAME!\.gemrtos_example" (
	for /f "usebackq delims=" %%P in (".\%SOFTWARE_DIR_NAME%\!APP_NAME!\.gemrtos_example") do (
		if /I not "%%P" == "!EXAMPLE!" (
			echo WARNING: %SOFTWARE_DIR_NAME%\!APP_NAME! previously held example '%%P'.
			echo WARNING: its source files are NOT deleted - if they define main^(^)
			echo WARNING: the link will fail with a duplicate symbol.  Use a
			echo WARNING: separate -app directory per example, or clean this one.
		)
	)
)

xcopy /E /I /Y "!EXAMPLE_SRC_DIR!" ".\%SOFTWARE_DIR_NAME%\!APP_NAME!" || (
	echo ERROR: could not deploy example '!EXAMPLE!'
	exit /b 1
)

rem Record which example this directory currently holds, so the next build can
rem warn on a mismatch.  Informational only -- nothing keys off it destructively.
echo !EXAMPLE!>".\%SOFTWARE_DIR_NAME%\!APP_NAME!\.gemrtos_example"

rem Copy the Windows-simulation exclusive files into the software directory on
rem first run, so the VS Code Windows version (no FPGA required) can be built
rem alongside the generated BSP and application. Only the port files
rem (gemrtos_controller_windows.c, grtos_fiber.c) and the VS Code project definition
rem (.vscode) live in ip\GeMRTOS_Multiprocessor\gemrtos_windows; the shared
rem kernel/HAL comes from the generated BSP (%SOFTWARE_DIR_NAME%\%BSP_NAME%) and
rem the application from %SOFTWARE_DIR_NAME%\%APP_NAME%. Its tasks.json builds
rem from this copied location (workspace root = %SOFTWARE_DIR_NAME%\gemrtos_windows).
rem This used to be a seed-once copy (`if not exist ... xcopy`), the same
rem pattern that caused an fs\ staleness bug -- edit a port file in
rem canonical and no existing build would ever pick it up, because the
rem directory already existed.  /D copies only files newer than the
rem destination, so running it unconditionally is a cheap no-op when nothing
rem changed and correctly refreshes gemrtos_controller_windows.c / grtos_fiber.c when
rem they have been edited.
echo Syncing Windows-simulation files into %SOFTWARE_DIR_NAME%\gemrtos_windows
if not exist ".\%SOFTWARE_DIR_NAME%\gemrtos_windows" mkdir ".\%SOFTWARE_DIR_NAME%\gemrtos_windows"
xcopy /E /I /Y /D ".\ip\GeMRTOS_Multiprocessor\gemrtos_windows" ".\%SOFTWARE_DIR_NAME%\gemrtos_windows" || (
	echo ERROR: could not sync gemrtos_windows Windows-simulation files
	exit /b 1
)


if exist "%SOFTWARE_DIR_NAME%\%APP_NAME%\%APP_NAME%.elf" rm %SOFTWARE_DIR_NAME%\%APP_NAME%\%APP_NAME%.elf


if exist "%QSYS_PRJ%.qsys" set FILE1=%QSYS_PRJ%.qsys
FOR %%i IN (%FILE1%) DO SET DATE1=%%~ti

if exist "output_files\%QUARTUS_PRJ%.sof" (
	set FILE2=output_files\%QUARTUS_PRJ%.sof
	FOR %%i IN (!FILE2!) DO SET DATE2=%%~ti
	)
if not exist "output_files\%QUARTUS_PRJ%.sof"  (
	set DATE2=01/01/1900 00:00
	set FULL_COMPILATION=1
)
set RUN_QSYS_SCRIPT=0
if not exist "processor_set.bat" (
	rem qsysscript_pro.tcl only substitutes placeholders into a processor_set.bat
	rem that validate{} already created during a full Platform Designer regeneration -
	rem it cannot create the file from nothing. qsysscript.tcl (std) has no such
	rem limitation. So a missing config file can only take the fast qsys-script-only
	rem path on std; pro must fall back to a full regeneration to recreate it.
	if /I "%QUARTUS_VERSION%" == "pro" (
		set DATE2=01/01/1900 00:00
		set FULL_COMPILATION=1
	) else (
		set RUN_QSYS_SCRIPT=1
	)
)

echo DATE1=%DATE1%
echo DATE2=%DATE2%

rem If the .qsys is newer than the compiled .sof, a full compilation is required.
rem Exit code 1 = stale (qsys newer), 0 = up to date; a PowerShell launch failure
rem also yields nonzero, conservatively forcing a full compilation.
rem PowerShell is invoked by absolute path (%SystemRoot% is always defined) so
rem this works in the Nios V Command Shell regardless of its PATH contents.
if exist "%QSYS_PRJ%.qsys" if exist "output_files\%QUARTUS_PRJ%.sof" (
	"%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe" -NoProfile -Command "exit [int]((Get-Item '%QSYS_PRJ%.qsys').LastWriteTime -gt (Get-Item 'output_files\%QUARTUS_PRJ%.sof').LastWriteTime)"
	if errorlevel 1 set FULL_COMPILATION=1
)


set command=%QSYS_ROOTDIR%/qsys-generate %QSYS_PRJ%.qsys --upgrade-ip-cores
if /I "%QUARTUS_VERSION%" == "pro" (
    set command=%QSYS_ROOTDIR%/qsys-generate %QSYS_PRJ%.qsys --quartus-project=%QUARTUS_PRJ%.qpf --upgrade-ip-cores
)
if /I "%FULL_COMPILATION%" == "1" (
%command% || (
	echo ERROR: %QSYS_PRJ%.qsys could not update
	exit /b 1
))

set command=%QSYS_ROOTDIR%/qsys-generate.exe %QSYS_PRJ%.qsys --synthesis=VERILOG
if /I "%QUARTUS_VERSION%" == "pro" (
    set command=%QSYS_ROOTDIR%/qsys-generate.exe %QSYS_PRJ%.qsys --quartus-project=%QUARTUS_PRJ%.qpf --synthesis=VERILOG
)
if /I "%FULL_COMPILATION%" == "1" (
%command% || (
	echo ERROR: %QSYS_PRJ%.qsys could not synthesises
	exit /b 1
))

rem set command=quartus_cmd.exe %QUARTUS_PRJ%.qpf -c %QUARTUS_PRJ%.qsf
set command=quartus_sh.exe --flow compile %QUARTUS_PRJ%.qpf 
if /I "%FULL_COMPILATION%" == "1" (
%command% || (
	echo "ERROR: with %command%"
	exit /b 1
))

set command=%QSYS_ROOTDIR%/qsys-script.exe --system-file=%QSYS_PRJ%.qsys --script=./ip/GeMRTOS_Multiprocessor/qsysscript.tcl
if /I "%QUARTUS_VERSION%" == "pro" (
    set command=%QSYS_ROOTDIR%/qsys-script.exe --system-file=%QSYS_PRJ%.qsys --quartus-project=%QUARTUS_PRJ%.qpf --script=./ip/GeMRTOS_Multiprocessor/qsysscript_pro.tcl
)
if /I "%FULL_COMPILATION%" == "1" set RUN_QSYS_SCRIPT=1
if /I "%RUN_QSYS_SCRIPT%" == "1" (
%command% || (
	echo ERROR: getting system information
	exit /b 1
))

call "%CD%\processor_set.bat"
if /I "%QSYS_PROC1%" == "" (
	echo ERROR: QSYS_PROC1 is empty after processor_set.bat - delete processor_set.bat and re-run to regenerate it
	exit /b 1
)
echo QSYS_PROC1 = %QSYS_PROC1%

set "lwip=--cmd="enable_sw_package lwip" "

set command=niosv-bsp -c --sopcinfo=%QSYS_PRJ%.sopcinfo -x=bsp_niosv_settings.tcl --type=hal -i=%QSYS_PROC1% %SOFTWARE_DIR_NAME%/%BSP_NAME%/settings.bsp
if /I "%QUARTUS_VERSION%" == "pro" (
    set command=niosv-bsp -c -t=hal -p=%QUARTUS_PRJ%.qpf -s=%QSYS_PRJ%.qsys -x=bsp_niosv_settings.tcl  -i=%QSYS_PROC1% %SOFTWARE_DIR_NAME%/%BSP_NAME%/settings.bsp %lwip%
)


%command% || (
	echo ERROR: could create BSP
	exit /b 1
)

rem Web page (LwIP httpd SSI) compilation. fs\*.html/*.gif/etc. (the editable
rem page sources) and makefsdata.exe only exist under
rem ip\GeMRTOS_Monitor\HAL\apps\http -- niosv-bsp above does not create a
rem HAL\apps\http\fs directory in the generated BSP on its own (only the
rem compiled-in .c/.h files: fsdata.c, httpd.c, fs.c, ...), but the web
rem server build compiles from %SOFTWARE_DIR_NAME%\%BSP_NAME%\HAL\apps\http\fs
rem (already-generated software-package variables, not a hardcoded path, so
rem this works for any project/board using this script, not just the one it
rem was written against).
rem
rem Regeneration is unconditional. Skipping it on an mtime comparison
rem (source fs\* vs. the BSP's fsdata.c) is fundamentally unreliable,
rem confirmed on real hardware: niosv-bsp drops its own stock lwip httpd package template
rem fsdata.c into the BSP on every run (unconditionally, with a "just now"
rem timestamp), REGARDLESS of whether the actual page content changed. That
rem stock template always looks "newer" than any real source edit, so the
rem old mtime check always concluded "up to date" and skipped regenerating
rem -- silently leaving niosv-bsp's stock (non-SSI-flagged, wrong
rem Content-Length) fsdata.c in place instead of GeMRTOS's real one. This
rem caused the web status page's response to be truncated (a static,
rem stale Content-Length baked from the stock template) even though the
rem canonical/ip\ fsdata.c was correct all along. Since regenerating
rem fsdata.c from a handful of KB of html/css/image sources takes well
rem under a second, there is no real cost to just doing it unconditionally
rem every build instead of trying to out-guess niosv-bsp's own timestamp
rem behavior.
rem
rem A board configuration without LwIP has no
rem ip\GeMRTOS_Monitor\HAL\apps\http\fs directory at all, and one with LwIP
rem disabled at the BSP level has no generated HAL\apps\http directory under
rem the BSP either -- both are no-ops here, not errors.
set HTTPD_SRC_DIR=ip\GeMRTOS_Monitor\HAL\apps\http
set HTTPD_SRC_FS_DIR=%HTTPD_SRC_DIR%\fs
set HTTPD_SRC_TOOL=%HTTPD_SRC_DIR%\makefsdata.exe
set HTTPD_BSP_DIR=%SOFTWARE_DIR_NAME%\%BSP_NAME%\HAL\apps\http
set HTTPD_BSP_FSDATA=%HTTPD_BSP_DIR%\fsdata.c
set HTTPD_BSP_FS_DIR=%HTTPD_BSP_DIR%\fs

if not exist "%HTTPD_SRC_FS_DIR%" (
	echo No web page sources found ^(%HTTPD_SRC_FS_DIR%^) - skipping web page compilation
) else if not exist "%HTTPD_SRC_TOOL%" (
	echo WARNING: %HTTPD_SRC_FS_DIR% exists but %HTTPD_SRC_TOOL% is missing - skipping web page compilation
) else if not exist "%HTTPD_BSP_DIR%" (
	echo No generated %HTTPD_BSP_DIR% - LwIP httpd not part of this BSP - skipping web page compilation
) else (
	rem Keep the BSP's fs\ copy in sync with the source on every build, not
	rem just seeded once when missing -- /D copies only files that are new
	rem or newer than what is already at the destination, so this is cheap
	rem when nothing changed and correctly refreshes anything edited since
	rem the last build (this directory is not managed by niosv-bsp at all,
	rem so nothing else keeps it current).
	if not exist "%HTTPD_BSP_FS_DIR%" mkdir "%HTTPD_BSP_FS_DIR%"
	xcopy /E /I /Y /D "%HTTPD_SRC_FS_DIR%" "%HTTPD_BSP_FS_DIR%" || (
		echo ERROR: could not sync %HTTPD_SRC_FS_DIR% into %HTTPD_BSP_FS_DIR%
		exit /b 1
	)

	echo Regenerating fsdata.c from current web page sources...

	rem makefsdata's own file-extension-based SSI detection does not
	rem recognize ".html" (only .shtml/.shtm/.ssi/.xml/.json by
	rem default), so every *.html under the source fs\ (there may be
	rem several pages) is passed explicitly via -ssi:<listfile>
	rem instead. Marking a
	rem plain page with no <!--#tag--> in it as SSI-eligible is
	rem harmless: the scan simply finds nothing to substitute.
	rem %CD% must not be substituted directly here (relying on it
	rem reflecting the pushd'd fs\ directory below): a real build run
	rem through niosv-shell (Ricardo's interactive Nios V shell, as opposed
	rem to a plain cmd.exe -- confirmed via a captured build log) showed
	rem %CD% still expanding to the *project root* even after the pushd,
	rem instead of the fs\ directory. That silently produced SSI-list
	rem entries like "ip/GeMRTOS_Monitor/HAL/apps/http/fs/status.html"
	rem instead of "status.html", which never matched anything inside
	rem makefsdata.exe (a real failure -- not the cosmetic "Failed to load
	rem list" message seen elsewhere), so every page silently
	rem fell back to being flagged as a plain non-SSI file. Capturing the
	rem post-pushd directory via `for /f ... in ('cd')` forces the shell to
	rem actually *run* `cd` and read its real output at this point in
	rem execution, rather than trusting a %CD%-style pseudo-variable whose
	rem expansion timing apparently differs between cmd.exe and niosv-shell.
	set SSI_LIST=%TEMP%\gemrtos_ssi_files_%RANDOM%.txt
	if exist "!SSI_LIST!" del "!SSI_LIST!"
	pushd "%HTTPD_SRC_FS_DIR%"
	for /f "delims=" %%D in ('cd') do set "HTTPD_SRC_FS_ABS=%%D"
	for /r %%F in (*.html) do (
		set "RELPATH=%%F"
		call set "RELPATH=%%RELPATH:!HTTPD_SRC_FS_ABS!\=%%"
		set "RELPATH=!RELPATH:\=/!"
		echo !RELPATH!>>"!SSI_LIST!"
	)
	popd

	rem A bare "makefsdata.exe" (no path prefix) is not found when
	rem this script runs under a non-interactive cmd.exe (e.g. spawned by an
	rem automation tool rather than a real interactive console) -- confirmed
	rem directly on this VM, same root cause and same fix as new_script.bat's
	rem EXAMPLES_MANIFEST path (see that file's own comment). ".\"
	rem always resolves correctly since the pushd two lines above guarantees
	rem the current directory is %HTTPD_SRC_DIR%, where the tool lives.
	pushd "%HTTPD_SRC_DIR%"
	.\makefsdata.exe fs -ssi:"!SSI_LIST!"
	set MAKEFSDATA_RESULT=!ERRORLEVEL!
	popd
	del "!SSI_LIST!" 2>NUL

	if not "!MAKEFSDATA_RESULT!" == "0" (
		echo ERROR: makefsdata.exe failed to regenerate %HTTPD_SRC_DIR%\fsdata.c
		exit /b 1
	)

	rem Copy the freshly-generated fsdata.c into the BSP directory that
	rem actually gets compiled, using the project's own
	rem %SOFTWARE_DIR_NAME%/%BSP_NAME% variables rather than a
	rem hardcoded path, so this works for any project/board this
	rem script is invoked against, not just the one it was written on.
	rem Unconditional overwrite -- see the rationale above for why this is
	rem no longer gated on a staleness check.
	copy /y "%HTTPD_SRC_DIR%\fsdata.c" "%HTTPD_BSP_FSDATA%" || (
		echo ERROR: could not copy fsdata.c into %HTTPD_BSP_DIR%
		exit /b 1
	)
)

rem ---------------------------------------------------------------------------
rem Windows-simulation project files.
rem
rem The Windows simulation must build the SAME example the FPGA build just
rem deployed.  A static tasks.json cannot express that: it hardcoded one
rem application source (../hellogemrtos/hellogemrtos.c) and a hand-maintained
rem list of BSP sources.  Such a list goes stale: renaming a kernel file
rem without updating it here silently breaks the Windows build.
rem
rem Generating both files at build time fixes the example-selection problem and
rem removes the staleness class entirely: the BSP sources are ENUMERATED from
rem the generated BSP rather than listed by hand, so a future rename cannot
rem silently break this again.
rem ---------------------------------------------------------------------------
set WIN_DIR=%SOFTWARE_DIR_NAME%\gemrtos_windows
set WIN_BSP_SRC=%SOFTWARE_DIR_NAME%\%BSP_NAME%\drivers\src
if not exist "%WIN_DIR%" (
	echo No %WIN_DIR% - skipping Windows-simulation project generation
) else if not exist "%WIN_BSP_SRC%" (
	echo No %WIN_BSP_SRC% - skipping Windows-simulation project generation
) else (
	echo Generating Windows-simulation project files for example '!EXAMPLE!'
	if not exist "%WIN_DIR%\.vscode" mkdir "%WIN_DIR%\.vscode"
	set WIN_TASKS=%WIN_DIR%\.vscode\tasks.json
	set WIN_CPROPS=%WIN_DIR%\.vscode\c_cpp_properties.json

	> "!WIN_TASKS!" echo {
	>>"!WIN_TASKS!" echo     "_generated": "by gemrtos_build.bat - do not edit; regenerated every build",
	>>"!WIN_TASKS!" echo     "_example": "!EXAMPLE!",
	>>"!WIN_TASKS!" echo     "version": "2.0.0",
	>>"!WIN_TASKS!" echo     "tasks": [
	>>"!WIN_TASKS!" echo         {
	>>"!WIN_TASKS!" echo             "type": "cppbuild",
	>>"!WIN_TASKS!" echo             "label": "C/C++: gcc.exe build active file",
	>>"!WIN_TASKS!" echo             "command": "gcc.exe",
	>>"!WIN_TASKS!" echo             "args": [
	>>"!WIN_TASKS!" echo                 "-fdiagnostics-color=always", "-g",

	rem Application sources: whatever the selected example actually deployed.
	for %%F in ("%SOFTWARE_DIR_NAME%\%APP_NAME%\*.c") do >>"!WIN_TASKS!" echo                 "../%APP_NAME%/%%~nxF",

	rem BSP kernel sources: enumerated, never hardcoded.
	for %%F in ("%WIN_BSP_SRC%\gemrtos_*.c") do >>"!WIN_TASKS!" echo                 "../%BSP_NAME%/drivers/src/%%~nxF",

	>>"!WIN_TASKS!" echo                 "gemrtos_controller_windows.c", "grtos_fiber.c", "pcap_netif.c", "jtag_uart_tcp.c",

	rem lwIP core/api/netif/httpd/arch sources: enumerated for the
	rem same anti-staleness reason as the BSP kernel sources above -- a future
	rem lwIP upgrade or reorganisation cannot silently break this list.
	rem
	rem Two deliberate exclusions, both direct-compiled-a-second-time
	rem bugs rather than anything Windows-specific:
	rem   - slipif.c: SLIP support, needs sio_open/sio_send/sio_tryread that
	rem     nothing in this BSP provides (SLIP is unused -- Ethernet only).
	rem   - fsdata.c: fs.c already pulls in its full content via
	rem     "#include HTTPD_FSDATA_FILE" (httpd_opts.h defaults that macro to
	rem     "fsdata.c"), so compiling it again as its own translation unit
	rem     produces duplicate-symbol link errors for every embedded file.
	set WIN_BSP_HAL=%SOFTWARE_DIR_NAME%\%BSP_NAME%\HAL
	for %%F in ("%WIN_BSP_HAL%\core\*.c") do >>"!WIN_TASKS!" echo                 "../%BSP_NAME%/HAL/core/%%~nxF",
	for %%F in ("%WIN_BSP_HAL%\core\ipv4\*.c") do >>"!WIN_TASKS!" echo                 "../%BSP_NAME%/HAL/core/ipv4/%%~nxF",
	for %%F in ("%WIN_BSP_HAL%\core\ipv6\*.c") do >>"!WIN_TASKS!" echo                 "../%BSP_NAME%/HAL/core/ipv6/%%~nxF",
	for %%F in ("%WIN_BSP_HAL%\api\*.c") do >>"!WIN_TASKS!" echo                 "../%BSP_NAME%/HAL/api/%%~nxF",
	for %%F in ("%WIN_BSP_HAL%\netif\*.c") do (
		if /I not "%%~nxF"=="slipif.c" >>"!WIN_TASKS!" echo                 "../%BSP_NAME%/HAL/netif/%%~nxF",
	)
	for %%F in ("%WIN_BSP_HAL%\netif\ppp\*.c") do >>"!WIN_TASKS!" echo                 "../%BSP_NAME%/HAL/netif/ppp/%%~nxF",
	for %%F in ("%WIN_BSP_HAL%\netif\ppp\polarssl\*.c") do >>"!WIN_TASKS!" echo                 "../%BSP_NAME%/HAL/netif/ppp/polarssl/%%~nxF",
	for %%F in ("%WIN_BSP_HAL%\apps\http\*.c") do (
		if /I not "%%~nxF"=="fsdata.c" >>"!WIN_TASKS!" echo                 "../%BSP_NAME%/HAL/apps/http/%%~nxF",
	)
	for %%F in ("%WIN_BSP_HAL%\arch\*.c") do >>"!WIN_TASKS!" echo                 "../%BSP_NAME%/HAL/arch/%%~nxF",

	>>"!WIN_TASKS!" echo                 "-I", "../%BSP_NAME%/",
	>>"!WIN_TASKS!" echo                 "-I", "../%BSP_NAME%/HAL/inc",
	>>"!WIN_TASKS!" echo                 "-I", "../%BSP_NAME%/drivers/inc",
	>>"!WIN_TASKS!" echo                 "-I", "../%APP_NAME%",
	>>"!WIN_TASKS!" echo                 "-I", ".",
	>>"!WIN_TASKS!" echo                 "-I", "../%BSP_NAME%/HAL/apps/http",
	>>"!WIN_TASKS!" echo                 "-I", "%NPCAP_SDK_DIR:\=/%/Include",
	>>"!WIN_TASKS!" echo                 "-L", "%NPCAP_SDK_DIR:\=/%/Lib",
	>>"!WIN_TASKS!" echo                 "-lwpcap", "-lPacket", "-lws2_32",
	>>"!WIN_TASKS!" echo                 "-lgcov",
	>>"!WIN_TASKS!" echo                 "-o", "${workspaceFolder}/%APP_NAME%.exe"
	>>"!WIN_TASKS!" echo             ],
	>>"!WIN_TASKS!" echo             "options": { "cwd": "${workspaceFolder}" },
	>>"!WIN_TASKS!" echo             "problemMatcher": [ "$gcc" ],
	>>"!WIN_TASKS!" echo             "group": { "kind": "build", "isDefault": true },
	>>"!WIN_TASKS!" echo             "detail": "Windows simulation of example !EXAMPLE!."
	>>"!WIN_TASKS!" echo         }
	>>"!WIN_TASKS!" echo     ]
	>>"!WIN_TASKS!" echo }

	> "!WIN_CPROPS!" echo {
	>>"!WIN_CPROPS!" echo     "_generated": "by gemrtos_build.bat - do not edit; regenerated every build",
	>>"!WIN_CPROPS!" echo     "version": 4,
	>>"!WIN_CPROPS!" echo     "configurations": [
	>>"!WIN_CPROPS!" echo         {
	>>"!WIN_CPROPS!" echo             "name": "Win32",
	>>"!WIN_CPROPS!" echo             "includePath": [
	>>"!WIN_CPROPS!" echo                 "${workspaceFolder}/**",
	>>"!WIN_CPROPS!" echo                 "${workspaceFolder}/../%BSP_NAME%/**",
	>>"!WIN_CPROPS!" echo                 "${workspaceFolder}/../%APP_NAME%/**",
	>>"!WIN_CPROPS!" echo                 "%NPCAP_SDK_DIR:\=/%/Include/**"
	>>"!WIN_CPROPS!" echo             ],
	>>"!WIN_CPROPS!" echo             "defines": [ "_DEBUG", "UNICODE", "_UNICODE" ],
	>>"!WIN_CPROPS!" echo             "compilerPath": "C:\\msys64\\mingw32\\bin\\gcc.exe",
	>>"!WIN_CPROPS!" echo             "cStandard": "c17",
	>>"!WIN_CPROPS!" echo             "cppStandard": "gnu++20",
	>>"!WIN_CPROPS!" echo             "intelliSenseMode": "windows-gcc-x86"
	>>"!WIN_CPROPS!" echo         }
	>>"!WIN_CPROPS!" echo     ]
	>>"!WIN_CPROPS!" echo }
)

rm -rf %SOFTWARE_DIR_NAME%/%APP_NAME%/CMakeFiles

set command=niosv-app --app-dir=%SOFTWARE_DIR_NAME%/%APP_NAME% --srcs-recursive=%SOFTWARE_DIR_NAME%/%APP_NAME% --incs-recursive=%SOFTWARE_DIR_NAME%/%APP_NAME% --bsp-dir=%SOFTWARE_DIR_NAME%/%BSP_NAME%
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

rem Reduce the JTAG speed to 16MHz to avoid instability issue on some board versions:
jtagconfig.exe --setparam 1 JtagClock 16M

set command=quartus_pgm ./%QUARTUS_PRJ%.cdf
if /I "%QUARTUS_VERSION%" == "pro" (
    
    !command! || (
        echo "*************************************************************"
        echo "ERROR: in Quartus Programmer                                 "
        echo "     : Create a .cdf file using Quartus Prime Programmer and "
        echo "     : named it as the project name in the main directory    "
        echo "     : (directory that holds the .qpf file of the project)   "
        echo "*************************************************************"
        exit /b 1
    )
)

if /I "%QUARTUS_VERSION%" == "std" (
    set command1=quartus_pgm -m JTAG -o "p;output_files/%QUARTUS_PRJ%.sof"
    set command2=quartus_pgm -m JTAG -o "p;output_files/%QUARTUS_PRJ%.sof@2"
    !command1! || (
    !command2! || (
        echo ERROR: Programming Quartus project
        exit /b 1
    )))

setlocal ENABLEDELAYEDEXPANSION
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
        if /I "%QUARTUS_VERSION%" == "pro" (
            timeout /t 9 > NULL
        )
	)
)

rem SETLOCAL ENABLEDELAYEDEXPANSION
rem SET count=0
rem FOR /F "tokens=* USEBACKQ" %%F IN (`jtagconfig -n`) DO (
rem     set varible=%%F
rem     rem @echo !varible!
rem     
rem     if not "!varible:JTAG UART=!" == "!varible!" (
rem         @echo !count!
rem         @echo !varible!
rem         set command=cmd.exe /c start juart-terminal.exe -v --flush --no-quit-on-ctrl-d --instance=!count!
rem         @echo !command!
rem         !command! || (
rem             echo ERROR: Opening JTAG terminal
rem         )
rem         
rem         SET /a count=!count!+1
rem         timeout /t 10 > NULL
rem     )
rem     rem SET var!count!=%%F
rem )

timeout /t 1 > NULL

openocd-cfg-gen -p
set command=niosv-download -g -i 0 %SOFTWARE_DIR_NAME%/%APP_NAME%/%APP_NAME%.elf
%command% || (
	echo ERROR: downloading .elf file
	exit /b 1
)

:follow

