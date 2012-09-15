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
    COND_E  = 0,  // aka COND_Z
    COND_NE = 1,  // aka COND_NZ
    COND_HS = 2,  // aka COND_C
    COND_LO = 3,  // aka COND_NC
    COND_GE = 4,
    COND_L  = 5,

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
