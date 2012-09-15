//===-- TLCS900Subtarget.cpp - TLCS900 Subtarget Information ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the TLCS900 specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "TLCS900Subtarget.h"
#include "TLCS900.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "TLCS900GenSubtargetInfo.inc"

using namespace llvm;

void TLCS900Subtarget::anchor() { }

TLCS900Subtarget::TLCS900Subtarget(const std::string &TT,
                                 const std::string &CPU,
                                 const std::string &FS) :
  TLCS900GenSubtargetInfo(TT, CPU, FS) {
  std::string CPUName = "generic";

  // Parse features string.
  ParseSubtargetFeatures(CPUName, FS);
}
