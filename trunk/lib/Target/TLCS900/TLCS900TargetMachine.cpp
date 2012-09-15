//===-- TLCS900TargetMachine.cpp - Define TargetMachine for TLCS900 ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Top-level implementation for the TLCS900 target.
//
//===----------------------------------------------------------------------===//

#include "TLCS900TargetMachine.h"
#include "TLCS900.h"
#include "llvm/PassManager.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

extern "C" void LLVMInitializeTLCS900Target() {
  // Register the target.
  RegisterTargetMachine<TLCS900TargetMachine> X(TheTLCS900Target);
}

TLCS900TargetMachine::TLCS900TargetMachine(const Target &T,
                                         StringRef TT,
                                         StringRef CPU,
                                         StringRef FS,
                                         const TargetOptions &Options,
                                         Reloc::Model RM, CodeModel::Model CM,
                                         CodeGenOpt::Level OL)
  : LLVMTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL),
    Subtarget(TT, CPU, FS),
    // FIXME: Check TargetData string.
    DataLayout("e-p:16:16:16-i8:8:8-i16:16:16-i32:16:32-n8:16"),
    InstrInfo(*this), TLInfo(*this), TSInfo(*this),
    FrameLowering(Subtarget) { }

namespace {
/// TLCS900 Code Generator Pass Configuration Options.
class TLCS900PassConfig : public TargetPassConfig {
public:
  TLCS900PassConfig(TLCS900TargetMachine *TM, PassManagerBase &PM)
    : TargetPassConfig(TM, PM) {}

  TLCS900TargetMachine &getTLCS900TargetMachine() const {
    return getTM<TLCS900TargetMachine>();
  }

  virtual bool addInstSelector();
  virtual bool addPreEmitPass();
};
} // namespace

TargetPassConfig *TLCS900TargetMachine::createPassConfig(PassManagerBase &PM) {
  return new TLCS900PassConfig(this, PM);
}

bool TLCS900PassConfig::addInstSelector() {
  // Install an instruction selector.
  PM->add(createTLCS900ISelDag(getTLCS900TargetMachine(), getOptLevel()));
  return false;
}

bool TLCS900PassConfig::addPreEmitPass() {
  // Must run branch selection immediately preceding the asm printer.
  PM->add(createTLCS900BranchSelectionPass());
  return false;
}
