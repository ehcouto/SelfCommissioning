.INCLUDE : ..\Include_path_list
.INCLUDE : ..\LintPath

ECHO := ..\tools\echo.exe

LINT_FILE : 
	@@ $(ECHO) \nLinting $(TFILE)
	$(LINT) ..\tools\Lint_Dynamic\std.lnt $(INCLUDE_PATH) $(TFILE)
	

	
	