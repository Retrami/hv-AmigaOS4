#
# Makefile generated by:
# codebench 0.55
#
# Project: hv
#
# Created on: 24-07-2022 16:03:57
#
#

###################################################################
##
##////  Objects
##
###################################################################

hv_OBJ := \
	 hv.o


###################################################################
##
##////  Variables and Environment
##
###################################################################

CC := gcc:bin/gcc

INCPATH := -I.

CFLAGS := $(INCPATH) -Wall -Werror -Wwrite-strings


###################################################################
##
##////  General rules
##
###################################################################

.PHONY: all all-before all-after clean clean-custom realclean

all: all-before hv all-after

all-before:
#	You can add rules here to execute before the project is built

all-after:
#	You can add rules here to execute after the project is built

clean: clean-custom
	@echo "Cleaning compiler objects..."
	@rm -f  $(hv_OBJ)

realclean:
	@echo "Cleaning compiler objects and targets..."
	@rm -f  $(hv_OBJ) hv


###################################################################
##
##////  Targets
##
###################################################################

hv: $(hv_OBJ)
	@echo "Linking hv"
	@gcc:bin/gcc -o hv $(hv_OBJ) -lauto
	@echo "Removing stale debug target: hv"
	@rm -f hv.debug


###################################################################
##
##////  Standard rules
##
###################################################################

# A default rule to make all the objects listed below
# because we are hiding compiler commands from the output

.c.o:
	@echo "Compiling $<"
	@$(CC) -c $< -o $*.o $(CFLAGS)

hv.o: hv.c

