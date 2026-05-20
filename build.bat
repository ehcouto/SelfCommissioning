@echo off  
cls
set PROJECT_DIR=%cd% 

if not exist tools\Log\LogPreprocessor.exe (
    @echo.
    @echo Please share the "d:\Data\Projects\DevTools\LogPreprocessor\Log" subproject in this project's
    @echo tools folder as a "Log" build sandbox and try to build again.
    goto end
)
tools\Log\LogPreprocessor.exe

cd exe
    
    attrib -r *.s19 > nul
    attrib -r *.out > nul
    attrib -r *.map > nul
    attrib -r *.bin > nul
    if exist LintProjectErrors.txt del /f LintProjectErrors.txt > nul
cd ..

if not "%1"=="-u" goto DontErase
cd exe
del *.s19
del *.out
cd ..

cd listings
del *.lst
cd ..

cd obj
del *.obj
cd..
:DontErase


cd tools

cd Utilities

Regenerate.exe 

PathBuilder.exe

cd ..

cd ..


cd source
if exist ..\exe\LintProjectErrors.txt attrib -r ..\exe\LintProjectErrors.txt
if exist ..\exe\LintProjectErrors.txt del ..\exe\LintProjectErrors.txt


if "%2"=="I" goto Iap
set DBUG_OPTION=-D ENABLE_JUMP_TO_APPLICATION_WITHOUT_IAP_FIRMWARE -D SET_FLASH_OPTION_CONFIGURATION
set EXT_NAME=

:Iap
if "%2"=="V" goto Verbose1
goto No_verb1

:Verbose1
make %1 -DARG0=%1 -DMAKESTARTUP=../tools/startup.mk -S -f source_makefile -D VERBOSE=1 
if ERRORLEVEL 1 goto end
goto Next1
 
:No_verb1 
make %1 -DARG0=%1 -DMAKESTARTUP=../tools/startup.mk -S -f source_makefile
if ERRORLEVEL 1 goto end

:Next1
cd ..

REM Generate list of all object files
cd obj
..\tools\echo "OBJECTS =" *.obj > ..\object_file_list
cd ..

REM Generate the H05, S19, MAP, and 695 files
if "%2"=="V" goto Verbose2
goto No_Verb2

:Verbose2
make -DMAKESTARTUP=tools/startup.mk -s -f makefile.dat -D VERBOSE=1 
if ERRORLEVEL 1 goto end
goto Next2

:No_Verb2
make -DMAKESTARTUP=tools/startup.mk -s -f makefile.dat
if ERRORLEVEL 1 goto end

:Next2
if "%1"=="-U" goto LintProject
if "%1"=="-u" goto LintProject
goto end
 
:LintProject
cd source
 
if exist ..\Linted\*.lnt attrib -r ..\Linted\*.lnt
if exist ..\Linted\*.lnt del ..\Linted\*.lnt
 
make -DMAKESTARTUP=../tools/startup.mk -DARG0=%1 -f ..\tools\Lint_Dynamic\LintProject.mak
move /Y LintProjectErrors.txt ..\Exe
type ..\Exe\LintProjectErrors.txt
cd..
:end
EXIT /B %ERRORLEVEL%