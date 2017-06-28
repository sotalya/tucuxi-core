
@echo off
if not defined VisualStudioVersion (
	echo *********************************************
	echo *** Running vcvarsall.bat
	call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64
)

set MAKE="C:\Program Files (x86)\GnuWin32\bin\make" %1

for %%i in (tucucommon tucucore tucucli) do (
	echo *********************************************
	echo *** Compiling %%i
	cd "%TUCUXI_ROOT%\Src\%%i"
	%MAKE% TARGET=VS2015 -f "%TUCUXI_ROOT%\src\%%i\makefile" clean
	%MAKE% TARGET=VS2015 -f "%TUCUXI_ROOT%\src\%%i\makefile"
)

for %%i in (tucucommon) do (
	echo *********************************************
	echo *** Compiling %%i
	cd "%TUCUXI_ROOT%\Test\%%i"
	%MAKE% TARGET=VS2015 -f "%TUCUXI_ROOT%\test\%%i\makefile" clean
	%MAKE% TARGET=VS2015 -f "%TUCUXI_ROOT%\test\%%i\makefile"
)

echo *********************************************
echo *** Generating documentation
doxygen "%TUCUXI_ROOT%\src\doxyfile"

pause