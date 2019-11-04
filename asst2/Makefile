##
##	@file		Makefile
##	@brief		Makefile for Asst2: Spooky Search
##
##	@author		Gemuele Aludino
##	@date       04 Nov 2019
##	@copyright 	Copyright © 2019 Gemuele Aludino. All rights reserved.
##

## DIRECTORIES ################################################################
DIR_OBJ				= build
DIR_INC				= include
DIR_SRC				= src
DIR_TST				= tests
DIR_CLI				= client
###############################################################################

## EXECUTABLE SOURCE FILE NAMES ###############################################
SRC_CLI				= searchtest.c
SRC_TST				= test.c
###############################################################################

## EXECUTABLE NAMES ###########################################################
EXE_CLI 			= searchtest
EXE_TST				= test
###############################################################################

## COMPILER ###################################################################
CC 				    = gcc
###############################################################################

## FLAGS ######################################################################

## Flag types - right-hand side can be modified or commented out to disable ##
CSTD 				= -std=c89
DEBUG 				= -g
OPTIMIZED			= -O3
PTHREAD				= -lpthread
MATH 				= -lm
PEDANTIC			= -pedantic-errors

SANTIIZED 			= #-fsanitize=address
W_ALL 				= -Wall
W_ERR 				= -Werror

## Add new flag categories here ##############################################
CFLAGS 				= $(DEBUG) $(OPTIMIZED) $(CSTD) $(PEDANTIC) $(W_ALL) $(W_ERR) $(SANITIZED)
LIB 				= $(PTHREAD) $(MATH)
INC 				= -I $(DIR_INC)
###############################################################################

## SOURCES/ASSEMBLY/OBJECTS/EXECUTABLES #######################################
EXT_SRC := .c
EXT_INC	:= .h
EXT_OBJ := .o

## Used for directives below (disabled for Asst2)
SOURCES	:= $(wildcard $(DIR_SRC)/*$(EXT_SRC))
OBJECTS := $(patsubst $(DIR_SRC)/%$(EXT_SRC), $(DIR_OBJ)/%$(EXT_OBJ), $(SOURCES))

ALL_EXE	= $(EXE_CLI) $(EXE_TST)
###############################################################################

## CUSTOM VARIABLES FOR ASST2 #################################################
PROC 				= proc
THREAD 				= thread
DIR_SRC_CNC			= $(DIR_SRC)/concurrent
DIR_OBJ_CNC			= $(DIR_OBJ)/concurrent

MULTITEST_PROC 		= multitest_$(PROC)
MULTITEST_THREAD 	= multitest_$(THREAD)

SRC_PROC			= $(MULTITEST_PROC)$(EXT_SRC)
SRC_THREAD			= $(MULTITEST_THREAD)$(EXT_SRC)

OBJ_PROC			= $(MULTITEST_PROC)$(EXT_OBJ)
OBJ_THREAD			= $(MULTITEST_THREAD)$(EXT_OBJ)

SOURCES_PROC		= $(DIR_SRC_CNC)/$(SRC_PROC)
SOURCES_THREAD		= $(DIR_SRC_CNC)/$(SRC_THREAD)

OBJECTS_PROC		= $(DIR_OBJ_CNC)/$(OBJ_PROC)
OBJECTS_THREAD		= $(DIR_OBJ_CNC)/$(OBJ_THREAD)
###############################################################################

## DIRECTIVES #################################################################

##
## $(CC) flags
##
## Preprocessing	-E
##	preprocessor directives in the input source code are expanded;
## 	comments are omitted
##
## Compilation		-S
##	post preprocessing, input source code is compiled (translated)
##	to the target system architecture's assembly language
##
## Assembly			-c
## 	post compilation, assembly language code is translated to object code,
## 	which consists of instructions to be executed by the target system architecture
##	(although the resulting object files are of binary format, these files are not
##	 executable programs)
##
## Linkage			-o
##	post assembly, object code contained within the object files are linked with
##	other object code in other object files to form the final binary executable;
##	the result is a program that will run on the target system architecture
##
## This Makefile includes instructions for producing object code from
## plaintext source code, as well as instructions for producing executables from
## object files -- the intermediary steps have been skipped for brevity.
##

all:
	@echo
	@echo "USAGE:"
	@echo "------"
	@echo "At the prompt,"
	@echo "type 'make proc' for process-based functionality."
	@echo "type 'make thread' for thread-based functionality."
	@echo ""
	@echo "--- examples ---"
	@echo ""
	@echo "your-machine:"$(notdir $(shell pwd))" your_username$$ make proc"
	@echo "   (for process-based functionality)"
	@echo ""
	@echo "your-machine:"$(notdir $(shell pwd))" your_username$$ make thread"
	@echo "   (for thread-based functionality)"
	@echo ""
	@echo "Please try again."
	@echo ""

sources: $(OBJECTS)

## Source code is preprocessed, compiled, and assembled - .o object files produced
$(DIR_OBJ)/%$(EXT_OBJ): $(DIR_SRC)/%$(EXT_SRC)
	$(CC) -c $< -o $@ $(CFLAGS) $(INC)

## Links .o object files - binary executable produced for processes
$(PROC): $(DIR_INC)/*$(EXT_INC) $(OBJECTS) $(OBJECTS_PROC) $(DIR_CLI)/$(SRC_CLI)
	@echo;
	@echo "Linking $(EXE_CLI) for process-based functionality..."
	@echo;

	$(CC) -o $(EXE_CLI) $(DIR_CLI)/$(SRC_CLI) $(OBJECTS) $(OBJECTS_PROC) $(CFLAGS) $(LIB) $(INC)

	@echo;
	@echo "Linking complete."
	@echo;

## Links .o object files - binary executable produced for threads
$(THREAD): $(DIR_INC)/*$(EXT_INC) $(OBJECTS) $(OBJECTS_THREAD) $(DIR_CLI)/$(SRC_CLI)
	@echo;
	@echo "Linking $(EXE_CLI) for thread-based functionality..."
	@echo;

	$(CC) -o $(EXE_CLI) $(DIR_CLI)/$(SRC_CLI) $(OBJECTS) $(OBJECTS_THREAD) $(CFLAGS) $(LIB) $(INC)

	@echo;
	@echo "Linking complete."
	@echo;

## Links .o object files - binary executable produced
# DISABLED FOR ASST2
#$(EXE_CLI): $(DIR_INC)/*$(EXT_INC) $(OBJECTS) $(DIR_CLI)/$(SRC_CLI)
#	@echo;
#	@echo "Linking $(EXE_CLI)..."
#	@echo;
#
#	$(CC) -o $(EXE_CLI) $(DIR_CLI)/$(SRC_CLI) $(OBJECTS) $(CFLAGS) $(LIB) $(INC)
#
#	@echo;
#	@echo "Linking complete."
#	@echo;

## Links .o object files - binary executable produced
$(EXE_TST): $(DIR_INC)/*$(EXT_INC) $(OBJECTS) $(DIR_TST)/$(SRC_TST)
	@echo;
	@echo "Linking $(EXE_TST)..."
	@echo;

	$(CC) -o $(EXE_TST) $(DIR_TST)/$(SRC_TST) $(OBJECTS) $(CFLAGS) $(LIB) $(INC)

	@echo;
	@echo "Linking complete."
	@echo;

clean:
	@echo;
	@echo "Cleaning..."
	@echo;

	@echo;
	@echo "Removing object files..."
	@echo;

	rm -rf $(OBJECTS) $(OBJECTS_PROC) $(OBJECTS_THREAD) || true

	@echo;
	@echo "Removed all object files."
	@echo;

	@echo;
	@echo "Removing executables..."
	@echo;

	rm -rf $(ALL_EXE) || true

	@echo;
	@echo "Removed all executables."
	@echo;

	@echo;
	@echo "Removing packages..."
	@echo;

	rm -rf *.dSYM || true

	@echo;
	@echo "Packages removed."
	@echo;

	@echo;
	@echo "Clean complete."
	@echo;

###############################################################################
