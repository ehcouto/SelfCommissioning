# MKS MAKE default rules - must be modified for your compiler
#
# $Header: startup.mk 1.1 2015/07/30 18:15:36EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
# $Log: startup.mk  $
# Revision 1.1 2015/07/30 18:15:36EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) 
# Initial revision
# Member added to project d:/Data/Projects/Nucleus_Dev/Rx62TNucleusMCI/tools/_tools.pj
# Revision 1.1 2015/07/07 14:07:17EDT Dasar Hanumanth (DASARHB) 
# Initial revision
# Member added to project d:/Data/Projects/Nucleus_Dev/RX62xNucleus/tools/_tools.pj
# Revision 1.1 2012/11/12 13:30:22EST MANOEL GARBUIO DE SOUZA (SOUZAMG) 
# Initial revision
# Member added to project d:/Data/Projects/GPL/Basic_Files/Common/_Common.pj
# Revision 1.1 2008/09/23 16:46:25EDT Manoel G De Souza (SOUZAMG) 
# Initial revision
# Member added to project d:/Data/Projects/DevTools/Tools/_Tools.pj
# Revision 1.2 2008/05/28 17:29:40EDT SOUZAMG 
# 
# Revision 1.2 2008/02/16 17:11:35EST pearssl 
# 
# Revision 1.2 2008/01/07 09:11:40EST ebrommp 
# added build error parsing for eclipse
# Revision 1.2 2007/11/30 10:49:41EST ebrommp 
# added compile errors in eclipse
# Revision 1.2 2007/11/28 13:43:03EST ebrommp 
# modified ComSpec variable to declare cmd.exe as the command prompt for eclipse
# Revision 1.1 2007/10/04 08:33:33EDT souzamg 
# Initial revision
# Member added to project d:/Data/Projects/Atlas_MatadorIII_No_Folder_CortexM3/tools/_tools.pj
# Revision 1.1 2007/10/02 17:12:57BRT souzamg 
# Initial revision
# Member added to project d:/Data/Projects/Atlas_MatadorIII_No_Folder/tools/_tools.pj
# Revision 1.1 2007/09/20 09:13:49BRT souzamg 
# Initial revision
# Member added to project d:/Data/Projects/Region_LAR/Cross/Atlas_Hmi_Test/tools/_tools.pj
# Revision 1.1 2007/08/20 14:17:19BRT souzamg 
# Initial revision
# Member added to project d:/Data/Projects/Region_LAR/Cross/Atlas_S_Folder/tools/_tools.pj
# Revision 1.1 2007/08/20 09:16:46BRT souzamg 
# Initial revision
# Member added to project d:/Data/Projects/Atlas_MatadorIII/tools/project.pj
# Revision 1.1 2007/03/07 18:01:08BRT pearssl 
# Initial revision
# Member added to project d:/Data/Projects/IAR_STR_Dev/tools/_tools.pj
# Revision 1.3 2004/06/08 08:46:38EDT BAXTEM2 
# merged to main branch
# Revision 1.1.1.1 2004/06/08 08:40:57EDT BAXTEM2 
# This is the latest version
# Revision 1.1 2002/02/04 14:34:08EST PEARSSL 
# Initial revision
# Member added to project d:/Data/Projects/bats/tools/project.pj
# Revision 1.3  1999/05/03 13:40:21  hahnba
# Keyword format change
# Revision 1.2  1999/05/03 13:38:33  hahnba
# Added Keywords to file
#
# Startup.mk Ver. 1.0

HAVERCS=yes		# If you do not have MKS RCS, set to no

# Allow redefinition on command line of macros defined here,
# without warning messages
__SILENT:=$(.SILENT)
.SILENT:=yes

# suffix definitions
E:=.exe
O:=.obj
S:=.asm
A:=.lib
P:=.pas			# Pascal
F:=.for			# Fortran

# Other macros
TMPDIR	:=	$(ROOTDIR)/tmp

.IMPORT .IGNORE: TMPDIR SHELL ComSpec

# If SHELL not defined, use COMSPEC, assumed to be command.com.
# If SHELL is defined, it is assumed to be the MKS Toolkit Korn Shell

# Added the following line to ensure proper DOS execution
ComSpec:=cmd.exe
SHELL:=$(ComSpec)

.IF $(SHELL)==$(NULL)
.IF $(ComSpec)==$(NULL)
    SHELL:=$(ROOTDIR)/bin/sh$E
.ELSE
    SHELL:=$(ComSpec)
.END
.END
GROUPSHELL := $(SHELL)

.IF $(SHELL)==$(ComSpec)
    SHELLMETAS := *?"<>|
    SHELLFLAGS := $(SWITCHAR)c
    GROUPFLAGS := $(SHELLFLAGS)
    GROUPSUFFIX := .bat
.ELSE
    SHELLMETAS := *?"<>|()&][$$\#`'
    SHELLFLAGS := -c
    GROUPSUFFIX := .ksh
    GROUPFLAGS := $(NULL)
.END

# Command names
CC	:= anycc		# Your compiler
LD	:= anylink		# Your linker
AS	:= anyasm		# Your assembler

MAKE	= $(MAKECMD)
CO	:= co		# MKS RCS Check out
AR	:= ar		# MKS AR librarian, provided with MAKE
RM	:= rm		# UNIX-style rm, provided with MAKE
YACC	:= yacc		# MKS LEX&YACC
LEX	:= lex

PC	:= anypc		# Your Pascal compiler
FC	:= anyf77		# Your Fortran compiler

LEXYY	:= lex_yy	# MKS LEX output
YTAB	:= ytab		# MKS YACC output

# Command flags and default args
ARFLAGS=ruv
CFLAGS=
ASFLAGS=
COFLAGS=-q			# check-out
LDFLAGS=
LDLIBS=				# additional libraries
LFLAGS=				# for LEX
YFLAGS=				# for YACC
RMFLAGS=			# for RM
FFLAGS=				# for FORTRAN
PFLAGS=				# for Pascal

# Implicit generation rules
# We don't provide .f rules here.  They may be added.

%$O: %.c;	$(CC) -c $(CFLAGS) $^
%$O: %$S;	$(AS) $(ASFLAGS) $^
%$O: %$P;	$(PC) -c $(PFLAGS) $^

# Your linker probably does not work this way!
%$E: %$O;	$(LD) $(LDFLAGS) -o $@ $& $(LDLIBS)
%$E:	;	$(LD) $(LDFLAGS) -o $@ $& $(LDLIBS)

%.c: %.y;	$(YACC) $(YFLAGS) $^
		mv $(YTAB).c $@

%.c: %.l;	$(LEX) $(LFLAGS) -o $@ $^

# Intermediate target file removal
.REMOVE:;	$(RM) $(RMFLAGS) $?

# RCS support
.IF $(HAVERCS)==yes
% .PRECIOUS: RCS/%;	-$(CO) $(COFLAGS) $^
%.c .PRECIOUS: RCS/%.c;	-$(CO) $(COFLAGS) $^
%.h .PRECIOUS: RCS/%.h;	-$(CO) $(COFLAGS) $^
%.l .PRECIOUS: RCS/%.l;	-$(CO) $(COFLAGS) $^
%.y .PRECIOUS: RCS/%.y;	-$(CO) $(COFLAGS) $^
%.p .PRECIOUS: RCS/%.p;	-$(CO) $(COFLAGS) $^
%$S .PRECIOUS: RCS/%$S;	-$(CO) $(COFLAGS) $^
%$F .PRECIOUS: RCS/%$F;	-$(CO) $(COFLAGS) $^
.END

# Archive support

LIBSUFFIX :=	$A
%$(LIBSUFFIX) .PRECIOUS .LIBRARY:;	$(AR) $(ARFLAGS) $@ $?

# augmake extensions
@B = $(@:b)
@D = $(@:d)
@F = $(@:f)
*B = $(*:b)
*D = $(*:d)
*F = $(*:f)
<B = $(<:b)
<D = $(<:d)
<F = $(<:f)
?B = $(?:b)
?F = $(?:f)
?D = $(?:d)
# Turn on warnings
.SILENT:=$(__SILENT)

# Include local startup.mk file, if present
.INCLUDE .IGNORE:	startup.mk
