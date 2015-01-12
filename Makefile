################################################################################
# makefile for test util
################################################################################

MAKE_INCLUDE_PATH=mk.inc

# ------------------------------------------------------------------------------
# Compiler and BIT
# ------------------------------------------------------------------------------
CC=gcc
BIT = 64

# ------------------------------------------------------------------------------
# debugger
# ------------------------------------------------------------------------------
DBGOPT = -g

# ------------------------------------------------------------------------------
# sources
# ------------------------------------------------------------------------------
SOURCES = worker.c alias.c

LSOWN = msgcat mqutil

MQINST = /opt/mqm/750

# ------------------------------------------------------------------------------
# main source
# ------------------------------------------------------------------------------
MAIN = main.c

# ------------------------------------------------------------------------------
# BINARY
# ------------------------------------------------------------------------------
BINARY = dumpmq

# ------------------------------------------------------------------------------
# libraries dynamic & static
# ------------------------------------------------------------------------------
LIBRARY = 

ARCHIVE  = 

# ------------------------------------------------------------------------------
# rollout includes
# ------------------------------------------------------------------------------
ROLLOUT_INC = 

# ------------------------------------------------------------------------------
# general includes
# ------------------------------------------------------------------------------
include $(MAKE_INCLUDE_PATH)/general.modules.mk

# ------------------------------------------------------------------------------
# clean local
# ------------------------------------------------------------------------------
cleanlocal :
	$(RM) var/log/*.log
	$(RM) core.*



# ------------------------------------------------------------------------------
# tests
# ------------------------------------------------------------------------------
#TEST = t_file_000 t_string_000 t_fork_000
include $(MAKE_INCLUDE_PATH)/test.mk

