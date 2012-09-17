//==-- TLCS900.h - Top-level interface for TLCS900 representation --*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in
// the LLVM TLCS900 backend.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TARGET_TLCS900_H
#define LLVM_TARGET_TLCS900_H

#include "MCTargetDesc/TLCS900MCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"

namespace TLCS900CC {
  // TLCS900 specific condition code.
  enum CondCodes {
    COND_LT = 0x1,
    COND_GE = 0x9,
    COND_LE = 0x2,
    COND_GT = 0xA,
    COND_ULE= 0x3,
    COND_UGT= 0xB,
    COND_OV = 0x4,  // aka COND_PE
    COND_NOV= 0xC,  // aka COND_PO
    COND_MI = 0x5,
    COND_PL = 0xD,
    COND_EQ = 0x6,  // aka COND_Z  
    COND_NE = 0xE,  // aka COND_NZ
    COND_C  = 0x7,  // aka COND_ULT
    COND_NC = 0xF,  // aka COND_UGE

    COND_ULT= COND_C,
    COND_UGE= COND_NC,

    COND_INVALID = -1
  };
}

namespace llvm {
  class TLCS900TargetMachine;
  class FunctionPass;
  class formatted_raw_ostream;

  FunctionPass *createTLCS900ISelDag(TLCS900TargetMachine &TM,
                                    CodeGenOpt::Level OptLevel);

  FunctionPass *createTLCS900BranchSelectionPass();

} // end namespace llvm;

#endif
