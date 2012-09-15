//==- TLCS900FrameLowering.h - Define frame lowering for TLCS900 --*- C++ -*--==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//
//===----------------------------------------------------------------------===//

#ifndef TLCS900_FRAMEINFO_H
#define TLCS900_FRAMEINFO_H

#include "TLCS900.h"
#include "TLCS900Subtarget.h"
#include "llvm/Target/TargetFrameLowering.h"

namespace llvm {
  class TLCS900Subtarget;

class TLCS900FrameLowering : public TargetFrameLowering {
protected:
  const TLCS900Subtarget &STI;

public:
  explicit TLCS900FrameLowering(const TLCS900Subtarget &sti)
    : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, 2, -2), STI(sti) {
  }

  /// emitProlog/emitEpilog - These methods insert prolog and epilog code into
  /// the function.
  void emitPrologue(MachineFunction &MF) const;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const;

  bool spillCalleeSavedRegisters(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator MI,
                                 const std::vector<CalleeSavedInfo> &CSI,
                                 const TargetRegisterInfo *TRI) const;
  bool restoreCalleeSavedRegisters(MachineBasicBlock &MBB,
                                   MachineBasicBlock::iterator MI,
                                   const std::vector<CalleeSavedInfo> &CSI,
                                   const TargetRegisterInfo *TRI) const;

  bool hasFP(const MachineFunction &MF) const;
  bool hasReservedCallFrame(const MachineFunction &MF) const;
};

} // End llvm namespace

#endif
