################################################################################
# Definition of the project directories.
################################################################################

INCLUDE_DIR = inc
SRC_DIR     = src
BUILD_DIR   = build
CFG_DIR     = cfg

PREFIX      = /usr/local

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
  
  #
  # The following definitions switch on asan in debug mode.
  #
  OPTION_FLAGS += -fsanitize=address,undefined -fsanitize-undefined-trap-on-error -static-libasan -fno-omit-frame-pointer
endif

################################################################################
# Ncurses has a major version and that version determines some programs and
# library names, especially the ncurses config program, which contains 
# informations for the compiler.
#
# The ncursesw6-config program is currently not part of ubuntu. The parameter
# can be used if ncurses is build from source.
################################################################################

NCURSES_MAJOR  = 5

NCURSES_CONFIG = ncursesw$(NCURSES_MAJOR)-config

################################################################################
# Definition of compiler flags. CC is defined by make and CFLAGS can be set by
# the user.
################################################################################

WARN_FLAGS  = -Wall -Wextra -Wpedantic -Werror

BUILD_FLAGS = -std=c11 -O2

FLAGS      = -DPREFIX='"$(PREFIX)"' $(BUILD_FLAGS) $(OPTION_FLAGS) $(WARN_FLAGS) -I$(INCLUDE_DIR) $(shell $(NCURSES_CONFIG) --cflags)

LIBS        = $(shell $(NCURSES_CONFIG) --libs) -lm -lmenuw

################################################################################
# The list of sources that are used to build the executable. Each of the source 
# files has a header file with the same name.
################################################################################

SRC_LIBS = \
	$(SRC_DIR)/common.c \
	$(SRC_DIR)/colors.c \
	$(SRC_DIR)/init_random_colors.c \
	$(SRC_DIR)/init_random_shapes.c \
	$(SRC_DIR)/info_area.c \
	$(SRC_DIR)/home_area.c \
	$(SRC_DIR)/bg_area.c \
	$(SRC_DIR)/game.c \
	$(SRC_DIR)/blocks.c \
	$(SRC_DIR)/s_area.c \
	$(SRC_DIR)/win_menu.c \
	$(SRC_DIR)/nz_curses.c \
	$(SRC_DIR)/score.c \
	$(SRC_DIR)/file_system.c \
	$(SRC_DIR)/s_status.c \
	$(SRC_DIR)/rules.c \
	$(SRC_DIR)/s_game_cfg.c \
	$(SRC_DIR)/ut_utils.c \
	$(SRC_DIR)/ut_common.c \
	$(SRC_DIR)/ut_s_area.c \
	$(SRC_DIR)/ut_rules.c \
	$(SRC_DIR)/ut_file_system.c \
	$(SRC_DIR)/ut_info_area.c \

OBJ_LIBS = $(subst $(SRC_DIR),$(BUILD_DIR),$(subst .c,.o,$(SRC_LIBS)))

INC_LIBS = $(subst $(SRC_DIR),$(INCLUDE_DIR),$(subst .c,.h,$(SRC_LIBS)))

################################################################################
# The main program.
################################################################################

EXEC     = nuzzle

SRC_EXEC = $(SRC_DIR)/$(EXEC).c

OBJ_EXEC = $(BUILD_DIR)/$(EXEC).o

################################################################################
# The test program.
################################################################################

UNIT_TEST     = ut_test

SRC_UNIT_TEST = $(SRC_DIR)/$(UNIT_TEST).c

OBJ_UNIT_TEST = $(BUILD_DIR)/$(UNIT_TEST).o

################################################################################
# Definition of the top-level targets. 
#
# A phony target is one that is not the name of a file. If a file 'all' exists,
# nothing will happen.
################################################################################

.PHONY: all

all: $(EXEC) tests

################################################################################
# Execute the tests.
################################################################################

.PHONY: tests

tests: $(UNIT_TEST)
	 ./$(UNIT_TEST)

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
	
$(UNIT_TEST): $(OBJ_LIBS) $(OBJ_UNIT_TEST)
	$(CC) -o $@ $^ $(FLAGS) $(LIBS)

################################################################################
# The cleanup goal deletes the executable, the test programs, all object files
# and some editing remains.
################################################################################

.PHONY: clean

clean:
	rm -f $(BUILD_DIR)/*.o
	rm -f $(BUILD_DIR)/*.gz
	rm -f $(BUILD_DIR)/*.deb
	rm -rf $(BUILD_DIR)/nuzzle_*_amd64/
	rm -f $(SRC_DIR)/*.c~
	rm -f $(INCLUDE_DIR)/*.h~
	rm -f $(EXEC) $(UNIT_TEST)
	
################################################################################
# Goals to install and uninstall the executable.
# --owner=root --group=root 
################################################################################

BINDIR = $(PREFIX)/games

DATDIR = $(PREFIX)/share/games/$(EXEC)

DOCDIR = $(PREFIX)/share/doc/$(EXEC)

MANDIR = $(PREFIX)/share/man/man6

MANPAGE = nuzzle.6

.PHONY: install uninstall


#
# lintian says that: "install --strip" does not remove .comment and .note 
# informations. 
# Use: "strip -s --remove-section=.comment --remove-section=.note"
#
install: $(EXEC)
	gzip -9n -c man/$(MANPAGE) > $(BUILD_DIR)/$(MANPAGE).gz
	install -D --mode=644 $(BUILD_DIR)/$(MANPAGE).gz --target-directory=$(MANDIR)
	cp $(EXEC) $(BUILD_DIR)/$(EXEC)
	strip -s --remove-section=.comment --remove-section=.note $(BUILD_DIR)/$(EXEC)
	install -D --mode=755 $(BUILD_DIR)/$(EXEC) --target-directory=$(BINDIR)
	install -D --mode=644 LICENSE $(DOCDIR)/copyright
	cp changelog $(BUILD_DIR)/changelog
	if [ -n "${MAIL}" ]; then sed -i "s/<MAIL>/<${MAIL}>/g" $(BUILD_DIR)/changelog ; fi
	gzip -9n -c $(BUILD_DIR)/changelog > $(BUILD_DIR)/changelog.gz
	install -D --mode=644 $(BUILD_DIR)/changelog.gz   $(DOCDIR)/changelog.gz
	install -D --mode=644 $(CFG_DIR)/5-shapes.cfg     $(DATDIR)/5-shapes.cfg
	install -D --mode=644 $(CFG_DIR)/shapes-lines.cfg $(DATDIR)/shapes-lines.cfg
	install -D --mode=644 $(CFG_DIR)/nuzzle.cfg       $(DATDIR)/nuzzle.cfg
	install -D --mode=644 $(CFG_DIR)/color.cfg       $(DATDIR)/color.cfg

uninstall:
	rm -f $(DOCDIR)/copyright
	rm -f $(DOCDIR)/changelog.gz
	if [ -d "$(DOCDIR)" ]; then rmdir $(DOCDIR); fi
	rm -f $(BINDIR)/$(EXEC)
	rm -f $(MANDIR)/$(MANPAGE).gz
	rm -f $(DATDIR)/*.cfg
	if [ -d "$(DATDIR)" ]; then rmdir $(DATDIR); fi

################################################################################
# The goal prints a help message the the nuzzle specific options for the build.
################################################################################

.PHONY: help

help:
	@echo "Targets:"
	@echo ""
	@echo "  make | make all               : Triggers the build of the executable."
	@echo "  make clean                    : Removes executables and temporary files from the build."
	@echo "  make install | make uninstall : Installs / uninstalles the program."
	@echo "  make help                     : Prints this message."
	@echo ""
	@echo "Parameter:"
	@echo ""
	@echo "  DEBUG=[true|false]            : A debug flag for the application. (default: false)"
	@echo "  NCURSES_MAJOR=[5|6]           : The major verion of ncurses. (default: 5)"
	@echo "  PREFIX=<PATH>                 : The path prefix for the build, install and uninstall."
	@echo "                                  (default: $(PREFIX))"
