
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64

set MAKE="C:\Program Files (x86)\GnuWin32\bin\make" %1

for %%i in (AAA BBB CCC) do (
	%MAKE% TARGET=VS2015 -f "%TUCUXI_ROOT%\Src\%%i\Makefile"
)
