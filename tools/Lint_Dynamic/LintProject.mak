.INCLUDE : ..\include_path_list
.INCLUDE : ..\LintPath

ECHO := ..\tools\echo.exe

LINT_FILE : 
	@@ $(ECHO) \nLinting Project
	$(LINT) ..\tools\Lint_Dynamic\std_ProjectMode.lnt $(INCLUDE_PATH) Sources.lnt
	
	