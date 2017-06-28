
@echo off
doxygen "%TUCUXI_ROOT%\src\doxyfile
start iexplore.exe "%TUCUXI_ROOT%\doc\html\index.html"

pause