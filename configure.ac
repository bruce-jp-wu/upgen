#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([the pattern-matching and parsing program generator], [0.5.2], [bruce.jp.wu@gmail.com], [upgen])
AC_PREREQ([2.12])
#AM_INIT_AUTOMAKE([])
AM_INIT_AUTOMAKE([subdir-objects])

AC_CONFIG_SRCDIR([src/main/mreporter.cpp])
AC_CONFIG_SRCDIR([src/coder/cmacro.cpp])
AC_CONFIG_SRCDIR([src/coder/cmacro.h])
AC_CONFIG_SRCDIR([src/coder/cmcond.h])
AC_CONFIG_SRCDIR([src/coder/cmmgr.cpp])
AC_CONFIG_SRCDIR([src/coder/cmmgr.h])
AC_CONFIG_SRCDIR([src/coder/coder.cpp])
AC_CONFIG_SRCDIR([src/coder/coderfwd.h])
AC_CONFIG_SRCDIR([src/coder/coder.h])
AC_CONFIG_SRCDIR([src/coder/ctoken.h])
AC_CONFIG_SRCDIR([src/coder/dmmap.cpp])
AC_CONFIG_SRCDIR([src/coder/dmmap.h])
AC_CONFIG_SRCDIR([src/coder/dmodel.h])
AC_CONFIG_SRCDIR([src/coder/metakeys.h])

AC_CONFIG_SRCDIR([src/common/action.h])
AC_CONFIG_SRCDIR([src/common/bitmap.h])
AC_CONFIG_SRCDIR([src/common/charmap.cpp])
AC_CONFIG_SRCDIR([src/common/charmap.h])
AC_CONFIG_SRCDIR([src/common/clhandler.cpp])
AC_CONFIG_SRCDIR([src/common/clhandler.h])
AC_CONFIG_SRCDIR([src/common/common.h])
AC_CONFIG_SRCDIR([src/common/filehelper.cpp])
AC_CONFIG_SRCDIR([src/common/filehelper.h])
AC_CONFIG_SRCDIR([src/common/reporter.cpp])
AC_CONFIG_SRCDIR([src/common/reporter.h])
AC_CONFIG_SRCDIR([src/common/rulemgr.cpp])
AC_CONFIG_SRCDIR([src/common/rulemgr.h])
AC_CONFIG_SRCDIR([src/common/setsplitter.cpp])
AC_CONFIG_SRCDIR([src/common/setsplitter.h])
AC_CONFIG_SRCDIR([src/common/strhelper.cpp])
AC_CONFIG_SRCDIR([src/common/strhelper.h])
AC_CONFIG_SRCDIR([src/common/types.h])
AC_CONFIG_SRCDIR([src/common/unifind.cpp])
AC_CONFIG_SRCDIR([src/common/unifind.h])
AC_CONFIG_SRCDIR([src/common/uniqueue.h])
AC_CONFIG_SRCDIR([src/common/uset.cpp])
AC_CONFIG_SRCDIR([src/common/uset.h])

AC_CONFIG_SRCDIR([src/gcode/coderyac.cpp])
AC_CONFIG_SRCDIR([src/gcode/coderyac.h])
AC_CONFIG_SRCDIR([src/gcode/ulyac.cpp])
AC_CONFIG_SRCDIR([src/gcode/ulyac.h])

AC_CONFIG_SRCDIR([src/lexer/dfafwd.h])
AC_CONFIG_SRCDIR([src/lexer/dgarc.cpp])
AC_CONFIG_SRCDIR([src/lexer/dgarc.h])
AC_CONFIG_SRCDIR([src/lexer/dgnode.cpp])
AC_CONFIG_SRCDIR([src/lexer/dgnode.h])
AC_CONFIG_SRCDIR([src/lexer/dgraph.cpp])
AC_CONFIG_SRCDIR([src/lexer/dgraph.h])
AC_CONFIG_SRCDIR([src/lexer/dtable.cpp])
AC_CONFIG_SRCDIR([src/lexer/dtable.h])
AC_CONFIG_SRCDIR([src/lexer/index.h])
AC_CONFIG_SRCDIR([src/lexer/lexer.cpp])
AC_CONFIG_SRCDIR([src/lexer/lexer.h])
AC_CONFIG_SRCDIR([src/lexer/macmgr.h])
AC_CONFIG_SRCDIR([src/lexer/retree.cpp])
AC_CONFIG_SRCDIR([src/lexer/retree.h])
AC_CONFIG_SRCDIR([src/lexer/scmgr.h])
AC_CONFIG_SRCDIR([src/lexer/token.h])

AC_CONFIG_SRCDIR([src/main/main.cpp])
AC_CONFIG_SRCDIR([src/main/mreporter.cpp])
AC_CONFIG_SRCDIR([src/main/mreporter.h])
AC_CONFIG_SRCDIR([src/main/upgmain.cpp])
AC_CONFIG_SRCDIR([src/main/upgmain.h])

AC_CONFIG_SRCDIR([src/meta/coderyac.upg])
AC_CONFIG_SRCDIR([src/meta/ulyac.upg])

AC_CONFIG_SRCDIR([src/parser/grammar.cpp])
AC_CONFIG_SRCDIR([src/parser/grammar.h])
AC_CONFIG_SRCDIR([src/parser/index.h])
AC_CONFIG_SRCDIR([src/parser/lalrtrp.h])
AC_CONFIG_SRCDIR([src/parser/lritem.h])
AC_CONFIG_SRCDIR([src/parser/parserfwd.h])
AC_CONFIG_SRCDIR([src/parser/pgraph.cpp])
AC_CONFIG_SRCDIR([src/parser/pgraph.h])
AC_CONFIG_SRCDIR([src/parser/prod.cpp])
AC_CONFIG_SRCDIR([src/parser/prod.h])
AC_CONFIG_SRCDIR([src/parser/ptable.cpp])
AC_CONFIG_SRCDIR([src/parser/ptable.h])
AC_CONFIG_SRCDIR([src/parser/symbol.h])

AC_CONFIG_SRCDIR([src/spec/cpplang.cpp])
AC_CONFIG_SRCDIR([src/spec/paslang.cpp])
AC_CONFIG_SRCDIR([src/spec/spec.cpp])
AC_CONFIG_SRCDIR([src/spec/spec.h])
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_LANG_PUSH([C++])

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
#AC_CHECK_HEADERS([stdio.h stdlib.h string.h sys/stat.h assert.h time.h])
AC_CONFIG_FILES(
src/Makefile
doc/Makefile
Makefile
)
# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE

# Checks for library functions.
AC_FUNC_STAT
AC_CHECK_FUNCS([memset sqrt stat])

AC_OUTPUT
