################################################################################
# Definition of the project directories.
################################################################################

INCLUDE_DIR = inc
SRC_DIR = src
BUILD_DIR = build

################################################################################
# A variable that collects the optional flags.
################################################################################

OPTION_FLAGS=

################################################################################
# A debug flag for the application. If set to 'true' the application has to pipe
# stderr to a file.
################################################################################

DEBUG = false

ifeq ($(DEBUG),true)
  OPTION_FLAGS += -DDEBUG -g
endif

################################################################################
# Ncurses has a major version and that version determines some programs and
# library names, especially the ncurses config program, which contains 
# informations for the compiler.
################################################################################

NCURSES_MAJOR  = 5

NCURSES_CONFIG = ncursesw$(NCURSES_MAJOR)-config

################################################################################
# Definition of compiler flags. CC is defined by make and CFLAGS can be set by
# the user.
################################################################################

WARN_FLAGS  = -Wall -Wextra -Wpedantic -Werror

BUILD_FLAGS = -std=c11 -O2

FLAGS      = $(BUILD_FLAGS) $(OPTION_FLAGS) $(WARN_FLAGS) -I$(INCLUDE_DIR) $(shell $(NCURSES_CONFIG) --cflags)

LIBS        = $(shell $(NCURSES_CONFIG) --libs) -lm

################################################################################
# The list of sources that are used to build the executable. Each of the source 
# files has a header file with the same name.
################################################################################

SRC_LIBS = \
	$(SRC_DIR)/common.c \

OBJ_LIBS = $(subst $(SRC_DIR),$(BUILD_DIR),$(subst .c,.o,$(SRC_LIBS)))

INC_LIBS = $(subst $(SRC_DIR),$(INCLUDE_DIR),$(subst .c,.h,$(SRC_LIBS)))

################################################################################
# The main program.
################################################################################

EXEC     = nuzzle

SRC_EXEC = $(SRC_DIR)/nuzzle.c

OBJ_EXEC = $(subst $(SRC_DIR),$(BUILD_DIR),$(subst .c,.o,$(SRC_EXEC)))

################################################################################
# Definition of the top-level targets. 
#
# A phony target is one that is not the name of a file. If a file 'all' exists,
# nothing will happen.
################################################################################

.PHONY: all

all: $(EXEC)

################################################################################
# A static pattern, that builds an object file from its source. The automatic
# variable $@ is the target and $< is the first prerequisite, which is the
# corrosponding source file. Example:
#
#   build/common.o: src/common.c src/common.h src/ncv_ncurses.h...
#	  gcc -c -o build/common.o src/common.c ...
#
# The complete list of header files is not necessary as a prerequisite, but it
# does not hurt.
################################################################################

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(INC_LIBS)
	$(CC) -c -o $@ $< $(FLAGS) $(LIBS)

################################################################################
# The goal compiles the executable from the object files. The automatic $^ is 
# the list of all prerequisites, which are the object files in this case.
################################################################################

$(EXEC): $(OBJ_LIBS) $(OBJ_EXEC)
	$(CC) -o $@ $^ $(FLAGS) $(LIBS)

################################################################################
# The cleanup goal deletes the executable, the test programs, all object files
# and some editing remains.
################################################################################

.PHONY: clean

clean:
	rm -f $(BUILD_DIR)/*.o
	rm -f $(SRC_DIR)/*.c~
	rm -f $(INCLUDE_DIR)/*.h~
	rm -f $(EXEC)

################################################################################
# The goal prints a help message the the nuzzle specific options for the build.
################################################################################

.PHONY: help

help:
	@echo "Targets:"
	@echo ""
	@echo "  make | make all              : Triggers the build of the executable."
	@echo "  make clean                   : Removes executables and temporary files from the build."
	@echo "  make help                    : Prints this message."
	@echo ""
	@echo "Parameter:"
	@echo ""
	@echo "  DEBUG=[true|false]           : A debug flag for the application. (default: false)"
	@echo "  NCURSES_MAJOR=[5|6]          : The major verion of ncurses. (default: 5)"
