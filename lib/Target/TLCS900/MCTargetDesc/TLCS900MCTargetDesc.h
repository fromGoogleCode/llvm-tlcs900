//===-- TLCS900MCTargetDesc.h - TLCS900 Target Descriptions -------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides TLCS900 specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef TLCS900MCTARGETDESC_H
#define TLCS900MCTARGETDESC_H

namespace llvm {
class Target;

extern Target TheTLCS900Target;

} // End llvm namespace

// Defines symbolic names for TLCS900 registers.
// This defines a mapping from register name to register number.
#define GET_REGINFO_ENUM
#include "TLCS900GenRegisterInfo.inc"

// Defines symbolic names for the TLCS900 instructions.
#define GET_INSTRINFO_ENUM
#include "TLCS900GenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "TLCS900GenSubtargetInfo.inc"

#endif
