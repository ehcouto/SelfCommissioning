@echo off
cd..
cd..
cd Source
make -DMAKESTARTUP=../tools/startup.mk -f ..\tools\Lint_Dynamic\LintProjectCSV.mak 
cd..
cd tools
cd lint_dynamic
@echo ------------------------------------------------------------------
@echo Results have been saved in the Exe folder as LintProjectErrors.csv
