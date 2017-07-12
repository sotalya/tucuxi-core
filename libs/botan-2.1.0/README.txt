#
# Botan: Crypto and TLS for C++11
# Botan (Japanese for peony) is a cryptography library written in C++11 and released under the permissive Simplified BSD license.
#
# 
# Clone repo
git clone https://github.com/randombit/botan

# Generate a cpp file the entire library code
cd botan
./configure.py --minimized-build --single-amalgamation-file --amalgamation

# you need this files :
botan_all.cpp 
botan_all.h 
botan_all_internal.h
build/include/* 

# Add custom makefile to compile lib with tucuxi

## ----------------------------------------------------------------------------
##
## TucuMath Makefile
##
## ----------------------------------------------------------------------------

NAME := botan
TYPE := LIB

SOURCES := botan_all.cpp
INCLUDES := build/include

LIBS := 
EXTLIBS := 
DEFINES :=

include $(TUCUXI_ROOT)/make/rules.mak
