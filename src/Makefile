#
# Makefile for the RedBase project.
#
# Author: Hyunjung Park (hyunjung@cs.stanford.edu)
#


#
# Global variables
#
CC             = g++
BUILD_DIR      = ../build/
LIB_DIR        = ../lib/
#GTEST_ROOT     = ../gtest-1.5.0
INC_DIRS       = -I. -I$(GTEST_ROOT) -I$(GTEST_ROOT)/include
AR             = ar -rc
RANLIB         = ranlib
YACC           = bison -dy
LEX            = flex

# -m32 - Generates code that runs on any i386 system
#        sizeof(int) = sizeof(long) = sizeof(void *) = 4
# -ggdb - Debugging information
# -DDEBUG - Enables assert failures
# -O1 - Basic optimization
# -Wall - All warnings
# -DDEBUG_PF - This turns on the LOG file for lots of BufferMgr info
CFLAGS         = -m64 -ggdb -DDEBUG -O0 -Wall $(STATS_OPTION) $(INC_DIRS)

# The STATS_OPTION can be set to -DPF_STATS or to nothing to turn on and
# off buffer manager statistics.  The student should not modify this
# flag at all!
STATS_OPTION   = -DPF_STATS

#
# Students: Please modify SOURCES variables as needed.
#
PF_SOURCES     = pf_buffermgr.cc pf_error.cc pf_filehandle.cc \
                 pf_pagehandle.cc pf_hashtable.cc pf_manager.cc \
                 pf_statistics.cc statistics.cc
RM_SOURCES     = statistics.cc rm_manager.cc rm_error.cc rm_filescan.cc \
                 rm_record.cc rm_filehandle.cc \
		  bitmap.cc \
		 predicate.cc \
		 ix_error.cc sm_error.cc ql_error.cc
IX_SOURCES     = ix_indexhandle.cc \
		 ix_manager.cc \
		 ix_indexscan.cc \
		 btree_node.cc \
		 ix_error.cc statistics.cc predicate.cc
SM_SOURCES     = statistics.cc sm_error.cc sm_manager.cc printer.cc 
QL_SOURCES     = statistics.cc ql_manager.cc ql_error.cc file_scan.cc \
		 index_scan.cc \
		 nested_loop_join.cc \
		 projection.cc \
		 merge_join.cc

UTILS_SOURCES  = dbcreate.cc dbdestroy.cc thisadb.cc
PARSER_SOURCES = scan.c parse.c nodes.c interp.c
TESTER_SOURCES = pf_test1.cc pf_test2.cc pf_test3.cc rm_test.cc \
	         ix_test.cc parser_test.cc ix_testkpg_2.cc nested_scratch.cc \

#GTEST_SOURCES  = $(GTEST_ROOT)/src/gtest-all.cc

PF_OBJECTS     = $(addprefix $(BUILD_DIR), $(PF_SOURCES:.cc=.o))
RM_OBJECTS     = $(addprefix $(BUILD_DIR), $(RM_SOURCES:.cc=.o))
IX_OBJECTS     = $(addprefix $(BUILD_DIR), $(IX_SOURCES:.cc=.o))
SM_OBJECTS     = $(addprefix $(BUILD_DIR), $(SM_SOURCES:.cc=.o))
QL_OBJECTS     = $(addprefix $(BUILD_DIR), $(QL_SOURCES:.cc=.o))
UTILS_OBJECTS  = $(addprefix $(BUILD_DIR), $(UTILS_SOURCES:.cc=.o))
PARSER_OBJECTS = $(addprefix $(BUILD_DIR), $(PARSER_SOURCES:.c=.o))
TESTER_OBJECTS = $(addprefix $(BUILD_DIR), $(TESTER_SOURCES:.cc=.o))
#GTEST_OBJECTS  = $(addprefix $(BUILD_DIR), gtest-all.o)
OBJECTS        = $(sort $(PF_OBJECTS) $(RM_OBJECTS) $(IX_OBJECTS) \
                 $(SM_OBJECTS) $(QL_OBJECTS) $(PARSER_OBJECTS) \
                 $(TESTER_OBJECTS) $(UTILS_OBJECTS) )

LIBRARY_PF     = $(LIB_DIR)libpf.a
LIBRARY_RM     = $(LIB_DIR)librm.a
LIBRARY_IX     = $(LIB_DIR)libix.a
LIBRARY_SM     = $(LIB_DIR)libsm.a
LIBRARY_QL     = $(LIB_DIR)libql.a
LIBRARY_PARSER = $(LIB_DIR)libparser.a
#LIBRARY_GTEST  = $(LIB_DIR)libgtest.a
LIBRARIES      = $(LIBRARY_PF) $(LIBRARY_RM) $(LIBRARY_IX) \
                 $(LIBRARY_SM) $(LIBRARY_QL) $(LIBRARY_PARSER) 
#		 $(LIBRARY_GTEST)

UTILS          = $(UTILS_SOURCES:.cc=)
TESTS          = $(TESTER_SOURCES:.cc=)
EXECUTABLES    = $(UTILS) $(TESTS)

LIBS           = -lparser -lql ../lib/libsm.a -lix -lrm -lpf 

#
# Build targets
#
all: $(LIBRARIES) $(UTILS) tags 

clean: tags-clean
	rm -f $(BUILD_DIR)*.o $(BUILD_DIR)*.d y.output y.tab.h \
	parse.c	scan.c test.out testrel.* \
	$(LIBRARIES) $(EXECUTABLES) $(BUILD_DIR)*.d.*

testers: all $(TESTS)

#test: testers gtest
#	./pf_test1
#	./rm_test
#	./rm_testTA
#	./ix_test
#	./ix_testkpg_2
#	./gtest

tag tags: 
	find . -type f | grep -v "\.git" | grep -v docs | xargs ctags

tags-clean:
	rm -f TAGS

#val:
#	valgrind --leak-check=full --suppressions=valgrind.supp ./gtest --gtest_filter=*NestedB*


#
# Libraries
#
$(LIBRARY_PF): $(PF_OBJECTS)
	$(AR) $(LIBRARY_PF) $(PF_OBJECTS)
	$(RANLIB) $(LIBRARY_PF)

$(LIBRARY_RM): $(RM_OBJECTS)
	$(AR) $(LIBRARY_RM) $(RM_OBJECTS)
	$(RANLIB) $(LIBRARY_RM)

$(LIBRARY_IX): $(IX_OBJECTS)
	$(AR) $(LIBRARY_IX) $(IX_OBJECTS)
	$(RANLIB) $(LIBRARY_IX)

$(LIBRARY_SM): $(SM_OBJECTS)
	$(AR) $(LIBRARY_SM) $(SM_OBJECTS)
	$(RANLIB) $(LIBRARY_SM)

$(LIBRARY_QL): $(QL_OBJECTS)
	$(AR) $(LIBRARY_QL) $(QL_OBJECTS)
	$(RANLIB) $(LIBRARY_QL)

$(LIBRARY_PARSER): $(PARSER_OBJECTS)
	$(AR) $(LIBRARY_PARSER) $(PARSER_OBJECTS)
	$(RANLIB) $(LIBRARY_PARSER)
	
#
# Parser
#
y.tab.h: parse.c

parse.c: parse.y
	$(YACC) parse.y; mv y.tab.c parse.c

scan.c: scan.l scanhelp.c y.tab.h
	$(LEX) scan.l; mv lex.yy.c scan.c

$(BUILD_DIR)parse.o: parse.c

$(BUILD_DIR)scan.o: scan.c y.tab.h

$(BUILD_DIR)nodes.o: nodes.c

$(BUILD_DIR)interp.o: interp.c

#
# Rules
#
-include $(OBJECTS:.o=.d)

$(BUILD_DIR)%.d: %.cc
	@set -e; \
	 rm -f $@; \
	 $(CC) $(CFLAGS) -MM -MT $(@:.d=.o) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,\1.o $@: ,g' $@.$$$$ > $@; \
	 rm -f $@.$$$$

$(OBJECTS): %.o: 
	$(CC) $(CFLAGS) -c $< -o $@

$(EXECUTABLES): %: $(BUILD_DIR)%.o $(LIBRARIES)
	$(CC) $(CFLAGS) $< -o $@ -L$(LIB_DIR) $(LIBS)

