cd..
cd..
cd source
make -DTFILE=%1 -DMAKESTARTUP=../tools/startup.mk -f ..\tools\Lint_Dynamic\LintFile.mak 
ren LintErrors.txt %1.lnt
move /Y %1.lnt ..\lint
cd..
cd tools\lint_Dynamic

@echo Your lint file %1.lnt has been saved in your projects lint folder.
