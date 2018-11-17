# Skaarhoj/Makefile
############################################################################

# tools to use:
CXX = g++ -std=c++11 
MAKEDEPEND = ${CXX} -M
EXE =
RM = rm
TOUCH = touch

# the source files in this directory
CXXSOURCES = main.cpp
CSOURCES =
SOURCES = $(CXXSOURCES) $(CSOURCES)
OBJECTS = ${CXXSOURCES:.cpp=.o} ${CSOURCES:.c=.o}

LIBS = -lm -lstdc++ -lpthread

#default target: compile
all: skaarhoj

# generate source file dependencies
depend:
	$(MAKEDEPEND) $(MKDEPFLAGS) $(SOURCES) > deps

skaarhoj: $(OBJECTS)
	$(CXX) -o skaarhoj$(EXE) $(OBJECTS) $(LIBS)

cleanhere: cleangen

clean: cleanhere

distclean: cleanhere

cleangen:
	-$(RM) -rf core *.o *~ *% ii_files *.il trash_heap deps
	-$(TOUCH) deps

###############################################################################
# dependencies --- automatically generated with make depend !!
###############################################################################
-include deps
