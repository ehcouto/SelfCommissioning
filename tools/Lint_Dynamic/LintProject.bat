cd..
cd..
cd Source
make -DMAKESTARTUP=../tools/startup.mk -f ..\tools\Lint_Dynamic\LintProject.mak 
move /Y LintProjectErrors.txt ..\Exe
cd..
cd tools\Lint_Dynamic

@echo Your projects lint file LintProjectErrors.txt has been saved in your Exe folder.
