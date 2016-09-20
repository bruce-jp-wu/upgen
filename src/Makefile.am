bin_PROGRAMS = upgen
upgen_SOURCES = coder/cmacro.cpp \
	coder/cmmgr.cpp \
	coder/coder.cpp \
	coder/dmmap.cpp \
	common/charmap.cpp \
	common/clhandler.cpp \
	common/filehelper.cpp \
	common/reporter.cpp \
	common/rulemgr.cpp \
	common/setsplitter.cpp \
	common/strhelper.cpp \
	common/unifind.cpp \
	common/uset.cpp \
	gcode/coderyac.cpp \
	gcode/ulyac.cpp \
	lexer/dgarc.cpp \
	lexer/dgnode.cpp \
	lexer/dgraph.cpp \
	lexer/dtable.cpp \
	lexer/lexer.cpp \
	lexer/retree.cpp \
	main/main.cpp \
	main/mreporter.cpp \
	main/upgmain.cpp \
	parser/grammar.cpp \
	parser/pgraph.cpp \
	parser/prod.cpp \
	parser/ptable.cpp \
	spec/cpplang.cpp \
	spec/paslang.cpp \
	spec/spec.cpp

UPGEN = upgen$(EXEEXT)

AM_CXXFLAGS = -std=c++11 $(DEPS_CXXFLAGS)

CLEANFILES = $(UPGEN) $(OBJS)
OBJS = *.$(OBJEXT) \
	coder/*.$(OBJEXT) \
	common/*.$(OBJEXT) \
	lexer/*.$(OBJEXT) \
	main/*.$(OBJEXT) \
	gcode/*.$(OBJEXT) \
	parser/*.$(OBJEXT) \
	spec/*.$(OBJEXT)


