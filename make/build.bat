@echo off

REM get current location to return to path where script got launched
set mypath=%cd%


REM Handle command line arguments
set MakeCmd=
set BuildLibs=1
set BuildDoc=1
FOR %%a IN (%*) do (
  if "%%a" == "nolib" (
    set BuildLibs=0
    echo "No libs!"
  )
  if "%%a" == "nodoc" (
    set BuildDoc=0
    echo "No doc!"
  )
  if "%%a" == "clean" (
    set MakeCmd=clean
    set BuildDoc=0
    echo "Clean!"
  )
)

if not exist ..\bin\ (
  mkdir ..\bin
)

if not defined VisualStudioVersion (
  echo *********************************************
  echo *** Running vcvarsall.bat
  call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
)

set MAKE="C:\Qt\Tools\mingw810_64\bin\mingw32-make.exe"

if "%BuildLibs%" == "1" (
  for %%i in (tiny-js-master-20170629 botan-2.19.3) do (
    echo *********************************************
    echo *** Compiling %%i
    cd "%TUCUXI_ROOT%\libs\%%i"
    %MAKE% TARGET=WINDOWS -f "%TUCUXI_ROOT%\libs\%%i\makefile" %MakeCmd%
  )
)

for %%i in (tucucommon tucucore tucuquery tucucli tucuvalidator tucucrypto tuculicense tucudrugfilechecker) do (
  echo *********************************************
  echo *** Compiling %%i
  cd "%TUCUXI_ROOT%\Src\%%i"
  %MAKE% TARGET=WINDOWS -f "%TUCUXI_ROOT%\src\%%i\makefile" %MakeCmd%
)

for %%i in (tucucommon tucucore) do (
  echo *********************************************
  echo *** Compiling %%i
  cd "%TUCUXI_ROOT%\Test\%%i"
  %MAKE% TARGET=WINDOWS -f "%TUCUXI_ROOT%\test\%%i\makefile" %MakeCmd%
)

if "%BuildDoc%" == "1" (
  echo *********************************************
  echo *** Generating documentation
  doxygen "%TUCUXI_ROOT%\src\doxyfile"
)

cd %mypath%
