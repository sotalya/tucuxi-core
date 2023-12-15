@ECHO off
SETLOCAL 

SET scriptpath=%~dp0

REM Check if the number of argument is correct
IF "%1" EQU "" (
  ECHO No argument given!! Defaulting to building all!
  SET build_config="all"
)
IF "%2" NEQ "" (
  ECHO Too many arguments given!!
  ECHO Rerun build.bat with only one of the following arguments
  ECHO    release : To build the library in release mode
  ECHO    debug   : To build the library in debug mode
  ECHO    all     : To build the library in both release and debug mode
  GOTO exit
)

ECHO ==============================================================================
ECHO ==================================== Botan ===================================
ECHO ==============================================================================
REM Check what argument was given
IF /i "%1" EQU "release" (
  SET build_config="release"
  ECHO Release selected!!!
) ELSE IF /i "%1" EQU "debug" (
    SET build_config="debug"
    ECHO Debug selected!!!
  ) ELSE IF "%1" EQU "all" (
      SET build_config="all"
      ECHO All selected!!!
    )

ECHO ==============================================================================
ECHO =============================== Building Botan ===============================
ECHO ==============================================================================
CD %scriptpath%/libs/botan

IF %build_config% EQU "debug" (
  GOTO debug
)

:release
ECHO ==============================================================================
ECHO =============================== Release library ==============================
ECHO ==============================================================================
PYTHON configure.py --disable-shared --without-documentation --build-targets=static 
NMAKE
ECHO ==============================================================================
ECHO ========================= Release library builded!!! =========================
ECHO ==============================================================================

IF %build_config% EQU "release" (
  GOTO end
)

:debug
ECHO ==============================================================================
ECHO ================================ Debug library ===============================
ECHO ==============================================================================
PYTHON configure.py --disable-shared --without-documentation --debug-mode --build-targets=static --library-suffix=D
NMAKE
ECHO ==============================================================================
ECHO ========================== Debug library builded!!! ==========================
ECHO ==============================================================================

ECHO ==============================================================================
ECHO ============================== Botan builded!!! ==============================
ECHO ==============================================================================

:end
ENDLOCAL 