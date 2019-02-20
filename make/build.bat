
@echo off

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

if not defined VisualStudioVersion (
  echo *********************************************
  echo *** Running vcvarsall.bat
  call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64
)

set MAKE="C:\Program Files (x86)\GnuWin32\bin\make"

if "%BuildLibs%" == "1" (
  for %%i in (tiny-js-master-20170629 botan-2.1.0) do (
    echo *********************************************
    echo *** Compiling %%i
    cd "%TUCUXI_ROOT%\libs\%%i"
    %MAKE% TARGET=VS2015 -f "%TUCUXI_ROOT%\libs\%%i\makefile" %MakeCmd%
  )
)

for %%i in (tucucommon tucucore tucucli tucuvalidator tuculicense) do (
  echo *********************************************
  echo *** Compiling %%i
  cd "%TUCUXI_ROOT%\Src\%%i"
  %MAKE% TARGET=VS2015 -f "%TUCUXI_ROOT%\src\%%i\makefile" %MakeCmd%
)

for %%i in (tucucommon tucucore) do (
  echo *********************************************
  echo *** Compiling %%i
  cd "%TUCUXI_ROOT%\Test\%%i"
  %MAKE% TARGET=VS2015 -f "%TUCUXI_ROOT%\test\%%i\makefile" %MakeCmd%
)

if "%BuildDoc%" == "1" (
  echo *********************************************
  echo *** Generating documentation
  doxygen "%TUCUXI_ROOT%\src\doxyfile"
)
