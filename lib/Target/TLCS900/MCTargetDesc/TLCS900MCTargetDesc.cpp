//===-- TLCS900MCTargetDesc.cpp - TLCS900 Target Descriptions ---------------===//
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

#include "TLCS900MCTargetDesc.h"
#include "TLCS900MCAsmInfo.h"
#include "InstPrinter/TLCS900InstPrinter.h"
#include "llvm/MC/MCCodeGenInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "TLCS900GenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "TLCS900GenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "TLCS900GenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *createTLCS900MCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitTLCS900MCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createTLCS900MCRegisterInfo(StringRef TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitTLCS900MCRegisterInfo(X, TLCS900::PCW);
  return X;
}

static MCSubtargetInfo *createTLCS900MCSubtargetInfo(StringRef TT, StringRef CPU,
                                                    StringRef FS) {
  MCSubtargetInfo *X = new MCSubtargetInfo();
  InitTLCS900MCSubtargetInfo(X, TT, CPU, FS);
  return X;
}

static MCCodeGenInfo *createTLCS900MCCodeGenInfo(StringRef TT, Reloc::Model RM,
                                                CodeModel::Model CM,
                                                CodeGenOpt::Level OL) {
  MCCodeGenInfo *X = new MCCodeGenInfo();
  X->InitMCCodeGenInfo(RM, CM, OL);
  return X;
}

static MCInstPrinter *createTLCS900MCInstPrinter(const Target &T,
                                                unsigned SyntaxVariant,
                                                const MCAsmInfo &MAI,
                                                const MCInstrInfo &MII,
                                                const MCRegisterInfo &MRI,
                                                const MCSubtargetInfo &STI) {
  if (SyntaxVariant == 0)
    return new TLCS900InstPrinter(MAI, MII, MRI);
  return 0;
}

extern "C" void LLVMInitializeTLCS900TargetMC() {
  // Register the MC asm info.
  RegisterMCAsmInfo<TLCS900MCAsmInfo> X(TheTLCS900Target);

  // Register the MC codegen info.
  TargetRegistry::RegisterMCCodeGenInfo(TheTLCS900Target,
                                        createTLCS900MCCodeGenInfo);

  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(TheTLCS900Target, createTLCS900MCInstrInfo);

  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(TheTLCS900Target,
                                    createTLCS900MCRegisterInfo);

  // Register the MC subtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(TheTLCS900Target,
                                          createTLCS900MCSubtargetInfo);

  // Register the MCInstPrinter.
  TargetRegistry::RegisterMCInstPrinter(TheTLCS900Target,
                                        createTLCS900MCInstPrinter);
}
