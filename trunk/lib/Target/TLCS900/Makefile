##===- lib/Target/TLCS900/Makefile --------------------------*- Makefile -*-===##
# 
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source 
# License. See LICENSE.TXT for details.
# 
##===----------------------------------------------------------------------===##

LEVEL = ../../..
LIBRARYNAME = LLVMTLCS900CodeGen
TARGET = TLCS900

# Make sure that tblgen is run, first thing.
BUILT_SOURCES = TLCS900GenRegisterInfo.inc TLCS900GenInstrInfo.inc \
		TLCS900GenAsmWriter.inc \
		TLCS900GenDAGISel.inc TLCS900GenCallingConv.inc \
		TLCS900GenSubtargetInfo.inc

DIRS = InstPrinter TargetInfo MCTargetDesc

include $(LEVEL)/Makefile.common

