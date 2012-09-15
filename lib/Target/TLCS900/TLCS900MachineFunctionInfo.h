//===- TLCS900MachineFuctionInfo.h - TLCS900 machine function info -*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares TLCS900-specific per-machine-function information.
//
//===----------------------------------------------------------------------===//

#ifndef TLCS900MACHINEFUNCTIONINFO_H
#define TLCS900MACHINEFUNCTIONINFO_H

#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {

/// TLCS900MachineFunctionInfo - This class is derived from MachineFunction and
/// contains private TLCS900 target-specific information for each MachineFunction.
class TLCS900MachineFunctionInfo : public MachineFunctionInfo {
  virtual void anchor();

  /// CalleeSavedFrameSize - Size of the callee-saved register portion of the
  /// stack frame in bytes.
  unsigned CalleeSavedFrameSize;

  /// ReturnAddrIndex - FrameIndex for return slot.
  int ReturnAddrIndex;

public:
  TLCS900MachineFunctionInfo() : CalleeSavedFrameSize(0) {}

  explicit TLCS900MachineFunctionInfo(MachineFunction &MF)
    : CalleeSavedFrameSize(0), ReturnAddrIndex(0) {}

  unsigned getCalleeSavedFrameSize() const { return CalleeSavedFrameSize; }
  void setCalleeSavedFrameSize(unsigned bytes) { CalleeSavedFrameSize = bytes; }

  int getRAIndex() const { return ReturnAddrIndex; }
  void setRAIndex(int Index) { ReturnAddrIndex = Index; }
};

} // End llvm namespace

#endif
