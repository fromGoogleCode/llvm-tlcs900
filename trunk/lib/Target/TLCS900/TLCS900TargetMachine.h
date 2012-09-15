//===-- TLCS900TargetMachine.h - Define TargetMachine for TLCS900 -*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the TLCS900 specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//


#ifndef LLVM_TARGET_TLCS900_TARGETMACHINE_H
#define LLVM_TARGET_TLCS900_TARGETMACHINE_H

#include "TLCS900InstrInfo.h"
#include "TLCS900ISelLowering.h"
#include "TLCS900FrameLowering.h"
#include "TLCS900SelectionDAGInfo.h"
#include "TLCS900RegisterInfo.h"
#include "TLCS900Subtarget.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

/// TLCS900TargetMachine
///
class TLCS900TargetMachine : public LLVMTargetMachine {
  TLCS900Subtarget        Subtarget;
  const TargetData       DataLayout;       // Calculates type size & alignment
  TLCS900InstrInfo        InstrInfo;
  TLCS900TargetLowering   TLInfo;
  TLCS900SelectionDAGInfo TSInfo;
  TLCS900FrameLowering    FrameLowering;

public:
  TLCS900TargetMachine(const Target &T, StringRef TT,
                      StringRef CPU, StringRef FS, const TargetOptions &Options,
                      Reloc::Model RM, CodeModel::Model CM,
                      CodeGenOpt::Level OL);

  virtual const TargetFrameLowering *getFrameLowering() const {
    return &FrameLowering;
  }
  virtual const TLCS900InstrInfo *getInstrInfo() const  { return &InstrInfo; }
  virtual const TargetData *getTargetData() const     { return &DataLayout;}
  virtual const TLCS900Subtarget *getSubtargetImpl() const { return &Subtarget; }

  virtual const TargetRegisterInfo *getRegisterInfo() const {
    return &InstrInfo.getRegisterInfo();
  }

  virtual const TLCS900TargetLowering *getTargetLowering() const {
    return &TLInfo;
  }

  virtual const TLCS900SelectionDAGInfo* getSelectionDAGInfo() const {
    return &TSInfo;
  }

  virtual TargetPassConfig *createPassConfig(PassManagerBase &PM);
}; // TLCS900TargetMachine.

} // end namespace llvm

#endif // LLVM_TARGET_TLCS900_TARGETMACHINE_H
