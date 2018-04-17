# define the C compiler to use
CC = emcc

# define any compile-time flags
CFLAGS = --std=c++11 -s ALLOW_MEMORY_GROWTH=1 --no-heap-copy

# define any directories containing header files other than /usr/include
#
INCLUDES = -I../include

# define any libraries to link into executable:
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname 
#   option, something like (this will link in libmylib.so and libm.so:
LIBS = ../lib/libgmp.a ../lib/libntl.a

# define the C source files
SRCS = main.cpp aes.cpp ArithmeticCircuit.cpp CommBF.cpp Common.cpp ConfigFile.cpp GF2_8LookupTable.cpp MPCCommunicationBF.cpp Prg.cpp Protocol.cpp SecurityLevel.cpp TemplateField.cpp

# define the C object files 
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .bc suffix
#
OBJS = $(SRCS:.cpp=.bc)

# define the executable file 
MAIN = protocol.html

#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

.PHONY: depend clean

all: $(MAIN)
	@echo  Simple compiler named mycc has been compiled

$(MAIN): $(OBJS) 
	rm -f ./.depend
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LIBS) --embed-file assets/inputs333.txt --embed-file assets/1000000G_1000000MG_333In_50Out_20D_OutputOne3P.txt --embed-file assets/parties.conf


# this is a suffix replacement rule for building .bc's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .bc file) 
# (see the gnu make manual section about automatic variables)
%.bc: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	-rm -rf *.bc $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
