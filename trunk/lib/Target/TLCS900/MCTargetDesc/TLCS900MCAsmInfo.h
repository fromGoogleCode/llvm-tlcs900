//===-- TLCS900MCAsmInfo.h - TLCS900 asm properties --------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the TLCS900MCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef TLCS900TARGETASMINFO_H
#define TLCS900TARGETASMINFO_H

#include "llvm/MC/MCAsmInfo.h"

namespace llvm {
  class StringRef;
  class Target;

  class TLCS900MCAsmInfo : public MCAsmInfo {
    virtual void anchor();
  public:
    explicit TLCS900MCAsmInfo(const Target &T, StringRef TT);
  };

} // namespace llvm

#endif
