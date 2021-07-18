#PROGRAMS
CXX = g++
CC = gcc
BIN = treeview
RM = rm -v

#DIRECTORIES
OBJDIR = obj
SRCDIR = src
INCDIR = src
BINDIR = bin

#PARSE SOURCES FILES
#use notdir to remove directory
SRC  = $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/*.cpp)
OBJ  = $(filter %.o,$(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o))
OBJ += $(filter %.o,$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC)))
INC  = $(wildcard $(INCDIR)/*.h) $(wildcard $(INCDIR)/*.hpp)

#FLAGS
LDFLAGS = $(shell fltk-config --ldflags) -Wl,--gc-sections -s
#CFLAGS =  -g -O0
CFLAGS = -Os
CFLAGS += $(patsubst %,-I%, $(INCDIR)) $(shell fltk-config --cxxflags)
CFLAGS += -Wall -Wextra -ffunction-sections -fdata-sections
CFLAGS += -fdiagnostics-color -fno-diagnostics-show-caret

#.PHONY: rule and not file name
.PHONY: clean all mrproper
#.SILENT:

#COMPILATIONS RULES
all: .depend $(BINDIR)/$(BIN)

$(BINDIR)/$(BIN):	$(OBJ)
		$(CXX) -o $@ $^ $(LDFLAGS)

#(INC) : if we modified an include, we recompile all
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
		$(CXX) -o $@ -c $< $(CFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
		$(CC) -o $@ -c $< $(CFLAGS)

clean:
		$(RM) $(OBJ)

mrproper: clean
		@$(RM) $(BINDIR)/$(BIN)
		@$(RM) .depend

#build dependency file
.depend:
	$(CC) -MM -I$(INCDIR) $(SRC) > $@
	sed -i 's/\(.*\.o:\)/$(OBJDIR)\/\1/g' $@

#including dependencies
-include .depend
