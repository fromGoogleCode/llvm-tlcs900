//===-- TLCS900RegisterInfo.cpp - TLCS900 Register Information --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the TLCS900 implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "msp430-reg-info"

#include "TLCS900RegisterInfo.h"
#include "TLCS900.h"
#include "TLCS900MachineFunctionInfo.h"
#include "TLCS900TargetMachine.h"
#include "llvm/Function.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/Support/ErrorHandling.h"

#define GET_REGINFO_TARGET_DESC
#include "TLCS900GenRegisterInfo.inc"

using namespace llvm;

// FIXME: Provide proper call frame setup / destroy opcodes.
TLCS900RegisterInfo::TLCS900RegisterInfo(TLCS900TargetMachine &tm,
                                       const TargetInstrInfo &tii)
  : TLCS900GenRegisterInfo(TLCS900::PCW), TM(tm), TII(tii) {
  StackAlign = TM.getFrameLowering()->getStackAlignment();
}

const uint16_t*
TLCS900RegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  const TargetFrameLowering *TFI = MF->getTarget().getFrameLowering();
  const Function* F = MF->getFunction();
  static const uint16_t CalleeSavedRegs[] = {
    TLCS900::FPW, TLCS900::R5W, TLCS900::R6W, TLCS900::R7W,
    TLCS900::R8W, TLCS900::R9W, TLCS900::R10W, TLCS900::R11W,
    0
  };
  static const uint16_t CalleeSavedRegsFP[] = {
    TLCS900::R5W, TLCS900::R6W, TLCS900::R7W,
    TLCS900::R8W, TLCS900::R9W, TLCS900::R10W, TLCS900::R11W,
    0
  };
  static const uint16_t CalleeSavedRegsIntr[] = {
    TLCS900::FPW,  TLCS900::R5W,  TLCS900::R6W,  TLCS900::R7W,
    TLCS900::R8W,  TLCS900::R9W,  TLCS900::R10W, TLCS900::R11W,
    TLCS900::R12W, TLCS900::R13W, TLCS900::R14W, TLCS900::R15W,
    0
  };
  static const uint16_t CalleeSavedRegsIntrFP[] = {
    TLCS900::R5W,  TLCS900::R6W,  TLCS900::R7W,
    TLCS900::R8W,  TLCS900::R9W,  TLCS900::R10W, TLCS900::R11W,
    TLCS900::R12W, TLCS900::R13W, TLCS900::R14W, TLCS900::R15W,
    0
  };

  if (TFI->hasFP(*MF))
    return (F->getCallingConv() == CallingConv::TLCS900_INTR ?
            CalleeSavedRegsIntrFP : CalleeSavedRegsFP);
  else
    return (F->getCallingConv() == CallingConv::TLCS900_INTR ?
            CalleeSavedRegsIntr : CalleeSavedRegs);

}

BitVector TLCS900RegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());
  const TargetFrameLowering *TFI = MF.getTarget().getFrameLowering();

  // Mark 4 special registers with subregisters as reserved.
  Reserved.set(TLCS900::PCB);
  Reserved.set(TLCS900::SPB);
  Reserved.set(TLCS900::SRB);
  Reserved.set(TLCS900::CGB);
  Reserved.set(TLCS900::PCW);
  Reserved.set(TLCS900::SPW);
  Reserved.set(TLCS900::SRW);
  Reserved.set(TLCS900::CGW);

  // Mark frame pointer as reserved if needed.
  if (TFI->hasFP(MF))
    Reserved.set(TLCS900::FPW);

  return Reserved;
}

const TargetRegisterClass *
TLCS900RegisterInfo::getPointerRegClass(unsigned Kind) const {
  return &TLCS900::GR16RegClass;
}

void TLCS900RegisterInfo::
eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
                              MachineBasicBlock::iterator I) const {
  const TargetFrameLowering *TFI = MF.getTarget().getFrameLowering();

  if (!TFI->hasReservedCallFrame(MF)) {
    // If the stack pointer can be changed after prologue, turn the
    // adjcallstackup instruction into a 'sub SPW, <amt>' and the
    // adjcallstackdown instruction into 'add SPW, <amt>'
    // TODO: consider using push / pop instead of sub + store / add
    MachineInstr *Old = I;
    uint64_t Amount = Old->getOperand(0).getImm();
    if (Amount != 0) {
      // We need to keep the stack aligned properly.  To do this, we round the
      // amount of space needed for the outgoing arguments up to the next
      // alignment boundary.
      Amount = (Amount+StackAlign-1)/StackAlign*StackAlign;

      MachineInstr *New = 0;
      if (Old->getOpcode() == TII.getCallFrameSetupOpcode()) {
        New = BuildMI(MF, Old->getDebugLoc(),
                      TII.get(TLCS900::SUB16ri), TLCS900::SPW)
          .addReg(TLCS900::SPW).addImm(Amount);
      } else {
        assert(Old->getOpcode() == TII.getCallFrameDestroyOpcode());
        // factor out the amount the callee already popped.
        uint64_t CalleeAmt = Old->getOperand(1).getImm();
        Amount -= CalleeAmt;
        if (Amount)
          New = BuildMI(MF, Old->getDebugLoc(),
                        TII.get(TLCS900::ADD16ri), TLCS900::SPW)
            .addReg(TLCS900::SPW).addImm(Amount);
      }

      if (New) {
        // The SRW implicit def is dead.
        New->getOperand(3).setIsDead();

        // Replace the pseudo instruction with a new instruction...
        MBB.insert(I, New);
      }
    }
  } else if (I->getOpcode() == TII.getCallFrameDestroyOpcode()) {
    // If we are performing frame pointer elimination and if the callee pops
    // something off the stack pointer, add it back.
    if (uint64_t CalleeAmt = I->getOperand(1).getImm()) {
      MachineInstr *Old = I;
      MachineInstr *New =
        BuildMI(MF, Old->getDebugLoc(), TII.get(TLCS900::SUB16ri),
                TLCS900::SPW).addReg(TLCS900::SPW).addImm(CalleeAmt);
      // The SRW implicit def is dead.
      New->getOperand(3).setIsDead();

      MBB.insert(I, New);
    }
  }

  MBB.erase(I);
}

void
TLCS900RegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                        int SPAdj, RegScavenger *RS) const {
  assert(SPAdj == 0 && "Unexpected");

  unsigned i = 0;
  MachineInstr &MI = *II;
  MachineBasicBlock &MBB = *MI.getParent();
  MachineFunction &MF = *MBB.getParent();
  const TargetFrameLowering *TFI = MF.getTarget().getFrameLowering();
  DebugLoc dl = MI.getDebugLoc();
  while (!MI.getOperand(i).isFI()) {
    ++i;
    assert(i < MI.getNumOperands() && "Instr doesn't have FrameIndex operand!");
  }

  int FrameIndex = MI.getOperand(i).getIndex();

  unsigned BasePtr = (TFI->hasFP(MF) ? TLCS900::FPW : TLCS900::SPW);
  int Offset = MF.getFrameInfo()->getObjectOffset(FrameIndex);

  // Skip the saved PC
  Offset += 2;

  if (!TFI->hasFP(MF))
    Offset += MF.getFrameInfo()->getStackSize();
  else
    Offset += 2; // Skip the saved FPW

  // Fold imm into offset
  Offset += MI.getOperand(i+1).getImm();

  if (MI.getOpcode() == TLCS900::ADD16ri) {
    // This is actually "load effective address" of the stack slot
    // instruction. We have only two-address instructions, thus we need to
    // expand it into mov + add

    MI.setDesc(TII.get(TLCS900::MOV16rr));
    MI.getOperand(i).ChangeToRegister(BasePtr, false);

    if (Offset == 0)
      return;

    // We need to materialize the offset via add instruction.
    unsigned DstReg = MI.getOperand(0).getReg();
    if (Offset < 0)
      BuildMI(MBB, llvm::next(II), dl, TII.get(TLCS900::SUB16ri), DstReg)
        .addReg(DstReg).addImm(-Offset);
    else
      BuildMI(MBB, llvm::next(II), dl, TII.get(TLCS900::ADD16ri), DstReg)
        .addReg(DstReg).addImm(Offset);

    return;
  }

  MI.getOperand(i).ChangeToRegister(BasePtr, false);
  MI.getOperand(i+1).ChangeToImmediate(Offset);
}

void
TLCS900RegisterInfo::processFunctionBeforeFrameFinalized(MachineFunction &MF)
                                                                         const {
  const TargetFrameLowering *TFI = MF.getTarget().getFrameLowering();

  // Create a frame entry for the FPW register that must be saved.
  if (TFI->hasFP(MF)) {
    int FrameIdx = MF.getFrameInfo()->CreateFixedObject(2, -4, true);
    (void)FrameIdx;
    assert(FrameIdx == MF.getFrameInfo()->getObjectIndexBegin() &&
           "Slot for FPW register must be last in order to be found!");
  }
}

unsigned TLCS900RegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  const TargetFrameLowering *TFI = MF.getTarget().getFrameLowering();

  return TFI->hasFP(MF) ? TLCS900::FPW : TLCS900::SPW;
}
