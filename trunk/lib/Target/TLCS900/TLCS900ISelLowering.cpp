//===-- TLCS900ISelLowering.cpp - TLCS900 DAG Lowering Implementation  ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the TLCS900TargetLowering class.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "tlcs900-lower"

#include "TLCS900ISelLowering.h"
#include "TLCS900.h"
#include "TLCS900MachineFunctionInfo.h"
#include "TLCS900TargetMachine.h"
#include "TLCS900Subtarget.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Function.h"
#include "llvm/Intrinsics.h"
#include "llvm/CallingConv.h"
#include "llvm/GlobalVariable.h"
#include "llvm/GlobalAlias.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

typedef enum {
  NoHWMult,
  HWMultIntr,
  HWMultNoIntr
} HWMultUseMode;

static cl::opt<HWMultUseMode>
HWMultMode("tlcs900-hwmult-mode",
           cl::desc("Hardware multiplier use mode"),
           cl::init(HWMultNoIntr),
           cl::values(
             clEnumValN(NoHWMult, "no",
                "Do not use hardware multiplier"),
             clEnumValN(HWMultIntr, "interrupts",
                "Assume hardware multiplier can be used inside interrupts"),
             clEnumValN(HWMultNoIntr, "use",
                "Assume hardware multiplier cannot be used inside interrupts"),
             clEnumValEnd));

TLCS900TargetLowering::TLCS900TargetLowering(TLCS900TargetMachine &tm) :
  TargetLowering(tm, new TargetLoweringObjectFileELF()),
  Subtarget(*tm.getSubtargetImpl()), TM(tm) {

  TD = getTargetData();

  // Set up the register classes.
  addRegisterClass(MVT::i8,  TLCS900::GR8RegisterClass);
  addRegisterClass(MVT::i16, TLCS900::GR16RegisterClass);

  // Compute derived properties from the register classes
  computeRegisterProperties();

  // Provide all sorts of operation actions

  // Division is expensive
  setIntDivIsCheap(false);

  setStackPointerRegisterToSaveRestore(TLCS900::SPW);
  setBooleanContents(ZeroOrOneBooleanContent);
  setBooleanVectorContents(ZeroOrOneBooleanContent); // FIXME: Is this correct?

  // We have post-incremented loads / stores.
  setIndexedLoadAction(ISD::POST_INC, MVT::i8, Legal);
  setIndexedLoadAction(ISD::POST_INC, MVT::i16, Legal);

  setLoadExtAction(ISD::EXTLOAD,  MVT::i1,  Promote);
  setLoadExtAction(ISD::SEXTLOAD, MVT::i1,  Promote);
  setLoadExtAction(ISD::ZEXTLOAD, MVT::i1,  Promote);
  setLoadExtAction(ISD::SEXTLOAD, MVT::i8,  Expand);
  setLoadExtAction(ISD::SEXTLOAD, MVT::i16, Expand);

  // We don't have any truncstores
  setTruncStoreAction(MVT::i16, MVT::i8, Expand);

  setOperationAction(ISD::SRA,              MVT::i8,    Custom);
  setOperationAction(ISD::SHL,              MVT::i8,    Custom);
  setOperationAction(ISD::SRL,              MVT::i8,    Custom);
  setOperationAction(ISD::SRA,              MVT::i16,   Custom);
  setOperationAction(ISD::SHL,              MVT::i16,   Custom);
  setOperationAction(ISD::SRL,              MVT::i16,   Custom);
  setOperationAction(ISD::ROTL,             MVT::i8,    Expand);
  setOperationAction(ISD::ROTR,             MVT::i8,    Expand);
  setOperationAction(ISD::ROTL,             MVT::i16,   Expand);
  setOperationAction(ISD::ROTR,             MVT::i16,   Expand);
  setOperationAction(ISD::GlobalAddress,    MVT::i16,   Custom);
  setOperationAction(ISD::ExternalSymbol,   MVT::i16,   Custom);
  setOperationAction(ISD::BlockAddress,     MVT::i16,   Custom);
  setOperationAction(ISD::BR_JT,            MVT::Other, Expand);
  setOperationAction(ISD::BR_CC,            MVT::i8,    Custom);
  setOperationAction(ISD::BR_CC,            MVT::i16,   Custom);
  setOperationAction(ISD::BRCOND,           MVT::Other, Expand);
  setOperationAction(ISD::SETCC,            MVT::i8,    Custom);
  setOperationAction(ISD::SETCC,            MVT::i16,   Custom);
  setOperationAction(ISD::SELECT,           MVT::i8,    Expand);
  setOperationAction(ISD::SELECT,           MVT::i16,   Expand);
  setOperationAction(ISD::SELECT_CC,        MVT::i8,    Custom);
  setOperationAction(ISD::SELECT_CC,        MVT::i16,   Custom);
  setOperationAction(ISD::SIGN_EXTEND,      MVT::i16,   Custom);
  setOperationAction(ISD::DYNAMIC_STACKALLOC, MVT::i8, Expand);
  setOperationAction(ISD::DYNAMIC_STACKALLOC, MVT::i16, Expand);

  setOperationAction(ISD::CTTZ,             MVT::i8,    Expand);
  setOperationAction(ISD::CTTZ,             MVT::i16,   Expand);
  setOperationAction(ISD::CTTZ_ZERO_UNDEF,  MVT::i8,    Expand);
  setOperationAction(ISD::CTTZ_ZERO_UNDEF,  MVT::i16,   Expand);
  setOperationAction(ISD::CTLZ,             MVT::i8,    Expand);
  setOperationAction(ISD::CTLZ,             MVT::i16,   Expand);
  setOperationAction(ISD::CTLZ_ZERO_UNDEF,  MVT::i8,    Expand);
  setOperationAction(ISD::CTLZ_ZERO_UNDEF,  MVT::i16,   Expand);
  setOperationAction(ISD::CTPOP,            MVT::i8,    Expand);
  setOperationAction(ISD::CTPOP,            MVT::i16,   Expand);

  setOperationAction(ISD::SHL_PARTS,        MVT::i8,    Expand);
  setOperationAction(ISD::SHL_PARTS,        MVT::i16,   Expand);
  setOperationAction(ISD::SRL_PARTS,        MVT::i8,    Expand);
  setOperationAction(ISD::SRL_PARTS,        MVT::i16,   Expand);
  setOperationAction(ISD::SRA_PARTS,        MVT::i8,    Expand);
  setOperationAction(ISD::SRA_PARTS,        MVT::i16,   Expand);

  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i1,   Expand);

  // FIXME: Implement efficiently multiplication by a constant
  setOperationAction(ISD::MUL,              MVT::i8,    Expand);
  setOperationAction(ISD::MULHS,            MVT::i8,    Expand);
  setOperationAction(ISD::MULHU,            MVT::i8,    Expand);
  setOperationAction(ISD::SMUL_LOHI,        MVT::i8,    Expand);
  setOperationAction(ISD::UMUL_LOHI,        MVT::i8,    Expand);
  setOperationAction(ISD::MUL,              MVT::i16,   Expand);
  setOperationAction(ISD::MULHS,            MVT::i16,   Expand);
  setOperationAction(ISD::MULHU,            MVT::i16,   Expand);
  setOperationAction(ISD::SMUL_LOHI,        MVT::i16,   Expand);
  setOperationAction(ISD::UMUL_LOHI,        MVT::i16,   Expand);

  setOperationAction(ISD::UDIV,             MVT::i8,    Expand);
  setOperationAction(ISD::UDIVREM,          MVT::i8,    Expand);
  setOperationAction(ISD::UREM,             MVT::i8,    Expand);
  setOperationAction(ISD::SDIV,             MVT::i8,    Expand);
  setOperationAction(ISD::SDIVREM,          MVT::i8,    Expand);
  setOperationAction(ISD::SREM,             MVT::i8,    Expand);
  setOperationAction(ISD::UDIV,             MVT::i16,   Expand);
  setOperationAction(ISD::UDIVREM,          MVT::i16,   Expand);
  setOperationAction(ISD::UREM,             MVT::i16,   Expand);
  setOperationAction(ISD::SDIV,             MVT::i16,   Expand);
  setOperationAction(ISD::SDIVREM,          MVT::i16,   Expand);
  setOperationAction(ISD::SREM,             MVT::i16,   Expand);

  // Libcalls names.
  if (HWMultMode == HWMultIntr) {
    setLibcallName(RTLIB::MUL_I8,  "__mulqi3hw");
    setLibcallName(RTLIB::MUL_I16, "__mulhi3hw");
  } else if (HWMultMode == HWMultNoIntr) {
    setLibcallName(RTLIB::MUL_I8,  "__mulqi3hw_noint");
    setLibcallName(RTLIB::MUL_I16, "__mulhi3hw_noint");
  }

  setMinFunctionAlignment(1);
  setPrefFunctionAlignment(2);
}

SDValue TLCS900TargetLowering::LowerOperation(SDValue Op,
                                             SelectionDAG &DAG) const {
  switch (Op.getOpcode()) {
  case ISD::SHL: // FALLTHROUGH
  case ISD::SRL:
  case ISD::SRA:              return LowerShifts(Op, DAG);
  case ISD::GlobalAddress:    return LowerGlobalAddress(Op, DAG);
  case ISD::BlockAddress:     return LowerBlockAddress(Op, DAG);
  case ISD::ExternalSymbol:   return LowerExternalSymbol(Op, DAG);
  case ISD::SETCC:            return LowerSETCC(Op, DAG);
  case ISD::BR_CC:            return LowerBR_CC(Op, DAG);
  case ISD::SELECT_CC:        return LowerSELECT_CC(Op, DAG);
  case ISD::SIGN_EXTEND:      return LowerSIGN_EXTEND(Op, DAG);
  case ISD::RETURNADDR:       return LowerRETURNADDR(Op, DAG);
  case ISD::FRAMEADDR:        return LowerFRAMEADDR(Op, DAG);
  default:
    llvm_unreachable("unimplemented operand");
  }
}

//===----------------------------------------------------------------------===//
//                       TLCS900 Inline Assembly Support
//===----------------------------------------------------------------------===//

/// getConstraintType - Given a constraint letter, return the type of
/// constraint it is for this target.
TargetLowering::ConstraintType
TLCS900TargetLowering::getConstraintType(const std::string &Constraint) const {
  if (Constraint.size() == 1) {
    switch (Constraint[0]) {
    case 'r':
      return C_RegisterClass;
    default:
      break;
    }
  }
  return TargetLowering::getConstraintType(Constraint);
}

std::pair<unsigned, const TargetRegisterClass*>
TLCS900TargetLowering::
getRegForInlineAsmConstraint(const std::string &Constraint,
                             EVT VT) const {
  if (Constraint.size() == 1) {
    // GCC Constraint Letters
    switch (Constraint[0]) {
    default: break;
    case 'r':   // GENERAL_REGS
      if (VT == MVT::i8)
        return std::make_pair(0U, TLCS900::GR8RegisterClass);

      return std::make_pair(0U, TLCS900::GR16RegisterClass);
    }
  }

  return TargetLowering::getRegForInlineAsmConstraint(Constraint, VT);
}

//===----------------------------------------------------------------------===//
//                      Calling Convention Implementation
//===----------------------------------------------------------------------===//

#include "TLCS900GenCallingConv.inc"

SDValue
TLCS900TargetLowering::LowerFormalArguments(SDValue Chain,
                                           CallingConv::ID CallConv,
                                           bool isVarArg,
                                           const SmallVectorImpl<ISD::InputArg>
                                             &Ins,
                                           DebugLoc dl,
                                           SelectionDAG &DAG,
                                           SmallVectorImpl<SDValue> &InVals)
                                             const {

  switch (CallConv) {
  default:
    llvm_unreachable("Unsupported calling convention");
  case CallingConv::C:
  case CallingConv::Fast:
    return LowerCCCArguments(Chain, CallConv, isVarArg, Ins, dl, DAG, InVals);
  case CallingConv::TLCS900_INTR:
    if (Ins.empty())
      return Chain;
    report_fatal_error("ISRs cannot have arguments");
  }
}

SDValue
TLCS900TargetLowering::LowerCall(SDValue Chain, SDValue Callee,
                                CallingConv::ID CallConv, bool isVarArg,
                                bool doesNotRet, bool &isTailCall,
                                const SmallVectorImpl<ISD::OutputArg> &Outs,
                                const SmallVectorImpl<SDValue> &OutVals,
                                const SmallVectorImpl<ISD::InputArg> &Ins,
                                DebugLoc dl, SelectionDAG &DAG,
                                SmallVectorImpl<SDValue> &InVals) const {
  // TLCS900 target does not yet support tail call optimization.
  isTailCall = false;

  switch (CallConv) {
  default:
    llvm_unreachable("Unsupported calling convention");
  case CallingConv::Fast:
  case CallingConv::C:
    return LowerCCCCallTo(Chain, Callee, CallConv, isVarArg, isTailCall,
                          Outs, OutVals, Ins, dl, DAG, InVals);
  case CallingConv::TLCS900_INTR:
    report_fatal_error("ISRs cannot be called directly");
  }
}

/// LowerCCCArguments - transform physical registers into virtual registers and
/// generate load operations for arguments places on the stack.
// FIXME: struct return stuff
// FIXME: varargs
SDValue
TLCS900TargetLowering::LowerCCCArguments(SDValue Chain,
                                        CallingConv::ID CallConv,
                                        bool isVarArg,
                                        const SmallVectorImpl<ISD::InputArg>
                                          &Ins,
                                        DebugLoc dl,
                                        SelectionDAG &DAG,
                                        SmallVectorImpl<SDValue> &InVals)
                                          const {
  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo *MFI = MF.getFrameInfo();
  MachineRegisterInfo &RegInfo = MF.getRegInfo();

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
		 getTargetMachine(), ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeFormalArguments(Ins, CC_TLCS900);

  assert(!isVarArg && "Varargs not supported yet");

  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    if (VA.isRegLoc()) {
      // Arguments passed in registers
      EVT RegVT = VA.getLocVT();
      switch (RegVT.getSimpleVT().SimpleTy) {
      default:
        {
#ifndef NDEBUG
          errs() << "LowerFormalArguments Unhandled argument type: "
               << RegVT.getSimpleVT().SimpleTy << "\n";
#endif
          llvm_unreachable(0);
        }
      case MVT::i16:
        unsigned VReg =
          RegInfo.createVirtualRegister(TLCS900::GR16RegisterClass);
        RegInfo.addLiveIn(VA.getLocReg(), VReg);
        SDValue ArgValue = DAG.getCopyFromReg(Chain, dl, VReg, RegVT);

        // If this is an 8-bit value, it is really passed promoted to 16
        // bits. Insert an assert[sz]ext to capture this, then truncate to the
        // right size.
        if (VA.getLocInfo() == CCValAssign::SExt)
          ArgValue = DAG.getNode(ISD::AssertSext, dl, RegVT, ArgValue,
                                 DAG.getValueType(VA.getValVT()));
        else if (VA.getLocInfo() == CCValAssign::ZExt)
          ArgValue = DAG.getNode(ISD::AssertZext, dl, RegVT, ArgValue,
                                 DAG.getValueType(VA.getValVT()));

        if (VA.getLocInfo() != CCValAssign::Full)
          ArgValue = DAG.getNode(ISD::TRUNCATE, dl, VA.getValVT(), ArgValue);

        InVals.push_back(ArgValue);
      }
    } else {
      // Sanity check
      assert(VA.isMemLoc());
      // Load the argument to a virtual register
      unsigned ObjSize = VA.getLocVT().getSizeInBits()/8;
      if (ObjSize > 2) {
        errs() << "LowerFormalArguments Unhandled argument type: "
             << EVT(VA.getLocVT()).getEVTString()
             << "\n";
      }
      // Create the frame index object for this incoming parameter...
      int FI = MFI->CreateFixedObject(ObjSize, VA.getLocMemOffset(), true);

      // Create the SelectionDAG nodes corresponding to a load
      //from this parameter
      SDValue FIN = DAG.getFrameIndex(FI, MVT::i16);
      InVals.push_back(DAG.getLoad(VA.getLocVT(), dl, Chain, FIN,
                                   MachinePointerInfo::getFixedStack(FI),
                                   false, false, false, 0));
    }
  }

  return Chain;
}

SDValue
TLCS900TargetLowering::LowerReturn(SDValue Chain,
                                  CallingConv::ID CallConv, bool isVarArg,
                                  const SmallVectorImpl<ISD::OutputArg> &Outs,
                                  const SmallVectorImpl<SDValue> &OutVals,
                                  DebugLoc dl, SelectionDAG &DAG) const {

  // CCValAssign - represent the assignment of the return value to a location
  SmallVector<CCValAssign, 16> RVLocs;

  // ISRs cannot return any value.
  if (CallConv == CallingConv::TLCS900_INTR && !Outs.empty())
    report_fatal_error("ISRs cannot return any value");

  // CCState - Info about the registers and stack slot.
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
		 getTargetMachine(), RVLocs, *DAG.getContext());

  // Analize return values.
  CCInfo.AnalyzeReturn(Outs, RetCC_TLCS900);

  // If this is the first return lowered for this function, add the regs to the
  // liveout set for the function.
  if (DAG.getMachineFunction().getRegInfo().liveout_empty()) {
    for (unsigned i = 0; i != RVLocs.size(); ++i)
      if (RVLocs[i].isRegLoc())
        DAG.getMachineFunction().getRegInfo().addLiveOut(RVLocs[i].getLocReg());
  }

  SDValue Flag;

  // Copy the result values into the output registers.
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    Chain = DAG.getCopyToReg(Chain, dl, VA.getLocReg(),
                             OutVals[i], Flag);

    // Guarantee that all emitted copies are stuck together,
    // avoiding something bad.
    Flag = Chain.getValue(1);
  }

  unsigned Opc = (CallConv == CallingConv::TLCS900_INTR ?
                  TLCS900ISD::RETI_FLAG : TLCS900ISD::RET_FLAG);

  if (Flag.getNode())
    return DAG.getNode(Opc, dl, MVT::Other, Chain, Flag);

  // Return Void
  return DAG.getNode(Opc, dl, MVT::Other, Chain);
}

/// LowerCCCCallTo - functions arguments are copied from virtual regs to
/// (physical regs)/(stack frame), CALLSEQ_START and CALLSEQ_END are emitted.
/// TODO: sret.
SDValue
TLCS900TargetLowering::LowerCCCCallTo(SDValue Chain, SDValue Callee,
                                     CallingConv::ID CallConv, bool isVarArg,
                                     bool isTailCall,
                                     const SmallVectorImpl<ISD::OutputArg>
                                       &Outs,
                                     const SmallVectorImpl<SDValue> &OutVals,
                                     const SmallVectorImpl<ISD::InputArg> &Ins,
                                     DebugLoc dl, SelectionDAG &DAG,
                                     SmallVectorImpl<SDValue> &InVals) const {
  // Analyze operands of the call, assigning locations to each operand.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
		 getTargetMachine(), ArgLocs, *DAG.getContext());

  CCInfo.AnalyzeCallOperands(Outs, CC_TLCS900);

  // Get a count of how many bytes are to be pushed on the stack.
  unsigned NumBytes = CCInfo.getNextStackOffset();

  Chain = DAG.getCALLSEQ_START(Chain ,DAG.getConstant(NumBytes,
                                                      getPointerTy(), true));

  SmallVector<std::pair<unsigned, SDValue>, 4> RegsToPass;
  SmallVector<SDValue, 12> MemOpChains;
  SDValue StackPtr;

  // Walk the register/memloc assignments, inserting copies/loads.
  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];

    SDValue Arg = OutVals[i];

    // Promote the value if needed.
    switch (VA.getLocInfo()) {
      default: llvm_unreachable("Unknown loc info!");
      case CCValAssign::Full: break;
      case CCValAssign::SExt:
        Arg = DAG.getNode(ISD::SIGN_EXTEND, dl, VA.getLocVT(), Arg);
        break;
      case CCValAssign::ZExt:
        Arg = DAG.getNode(ISD::ZERO_EXTEND, dl, VA.getLocVT(), Arg);
        break;
      case CCValAssign::AExt:
        Arg = DAG.getNode(ISD::ANY_EXTEND, dl, VA.getLocVT(), Arg);
        break;
    }

    // Arguments that can be passed on register must be kept at RegsToPass
    // vector
    if (VA.isRegLoc()) {
      RegsToPass.push_back(std::make_pair(VA.getLocReg(), Arg));
    } else {
      assert(VA.isMemLoc());

      if (StackPtr.getNode() == 0)
        StackPtr = DAG.getCopyFromReg(Chain, dl, TLCS900::SPW, getPointerTy());

      SDValue PtrOff = DAG.getNode(ISD::ADD, dl, getPointerTy(),
                                   StackPtr,
                                   DAG.getIntPtrConstant(VA.getLocMemOffset()));


      MemOpChains.push_back(DAG.getStore(Chain, dl, Arg, PtrOff,
                                         MachinePointerInfo(),false, false, 0));
    }
  }

  // Transform all store nodes into one single node because all store nodes are
  // independent of each other.
  if (!MemOpChains.empty())
    Chain = DAG.getNode(ISD::TokenFactor, dl, MVT::Other,
                        &MemOpChains[0], MemOpChains.size());

  // Build a sequence of copy-to-reg nodes chained together with token chain and
  // flag operands which copy the outgoing args into registers.  The InFlag in
  // necessary since all emitted instructions must be stuck together.
  SDValue InFlag;
  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i) {
    Chain = DAG.getCopyToReg(Chain, dl, RegsToPass[i].first,
                             RegsToPass[i].second, InFlag);
    InFlag = Chain.getValue(1);
  }

  // If the callee is a GlobalAddress node (quite common, every direct call is)
  // turn it into a TargetGlobalAddress node so that legalize doesn't hack it.
  // Likewise ExternalSymbol -> TargetExternalSymbol.
  if (GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee))
    Callee = DAG.getTargetGlobalAddress(G->getGlobal(), dl, MVT::i16);
  else if (ExternalSymbolSDNode *E = dyn_cast<ExternalSymbolSDNode>(Callee))
    Callee = DAG.getTargetExternalSymbol(E->getSymbol(), MVT::i16);

  // Returns a chain & a flag for retval copy to use.
  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);
  SmallVector<SDValue, 8> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);

  // Add argument registers to the end of the list so that they are
  // known live into the call.
  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i)
    Ops.push_back(DAG.getRegister(RegsToPass[i].first,
                                  RegsToPass[i].second.getValueType()));

  if (InFlag.getNode())
    Ops.push_back(InFlag);

  Chain = DAG.getNode(TLCS900ISD::CALL, dl, NodeTys, &Ops[0], Ops.size());
  InFlag = Chain.getValue(1);

  // Create the CALLSEQ_END node.
  Chain = DAG.getCALLSEQ_END(Chain,
                             DAG.getConstant(NumBytes, getPointerTy(), true),
                             DAG.getConstant(0, getPointerTy(), true),
                             InFlag);
  InFlag = Chain.getValue(1);

  // Handle result values, copying them out of physregs into vregs that we
  // return.
  return LowerCallResult(Chain, InFlag, CallConv, isVarArg, Ins, dl,
                         DAG, InVals);
}

/// LowerCallResult - Lower the result values of a call into the
/// appropriate copies out of appropriate physical registers.
///
SDValue
TLCS900TargetLowering::LowerCallResult(SDValue Chain, SDValue InFlag,
                                      CallingConv::ID CallConv, bool isVarArg,
                                      const SmallVectorImpl<ISD::InputArg> &Ins,
                                      DebugLoc dl, SelectionDAG &DAG,
                                      SmallVectorImpl<SDValue> &InVals) const {

  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
		 getTargetMachine(), RVLocs, *DAG.getContext());

  CCInfo.AnalyzeCallResult(Ins, RetCC_TLCS900);

  // Copy all of the result registers out of their specified physreg.
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    Chain = DAG.getCopyFromReg(Chain, dl, RVLocs[i].getLocReg(),
                               RVLocs[i].getValVT(), InFlag).getValue(1);
    InFlag = Chain.getValue(2);
    InVals.push_back(Chain.getValue(0));
  }

  return Chain;
}

SDValue TLCS900TargetLowering::LowerShifts(SDValue Op,
                                          SelectionDAG &DAG) const {
  unsigned Opc = Op.getOpcode();
  SDNode* N = Op.getNode();
  EVT VT = Op.getValueType();
  DebugLoc dl = N->getDebugLoc();

  // Expand non-constant shifts to loops:
  if (!isa<ConstantSDNode>(N->getOperand(1)))
    switch (Opc) {
    default: llvm_unreachable("Invalid shift opcode!");
    case ISD::SHL:
      return DAG.getNode(TLCS900ISD::SHL, dl,
                         VT, N->getOperand(0), N->getOperand(1));
    case ISD::SRA:
      return DAG.getNode(TLCS900ISD::SRA, dl,
                         VT, N->getOperand(0), N->getOperand(1));
    case ISD::SRL:
      return DAG.getNode(TLCS900ISD::SRL, dl,
                         VT, N->getOperand(0), N->getOperand(1));
    }

  uint64_t ShiftAmount = cast<ConstantSDNode>(N->getOperand(1))->getZExtValue();

  // Expand the stuff into sequence of shifts.
  // FIXME: for some shift amounts this might be done better!
  // E.g.: foo >> (8 + N) => sxt(swpb(foo)) >> N
  SDValue Victim = N->getOperand(0);

  if (Opc == ISD::SRL && ShiftAmount) {
    // Emit a special goodness here:
    // srl A, 1 => clrc; rrc A
    Victim = DAG.getNode(TLCS900ISD::RRC, dl, VT, Victim);
    ShiftAmount -= 1;
  }

  while (ShiftAmount--)
    Victim = DAG.getNode((Opc == ISD::SHL ? TLCS900ISD::RLA : TLCS900ISD::RRA),
                         dl, VT, Victim);

  return Victim;
}

SDValue TLCS900TargetLowering::LowerGlobalAddress(SDValue Op,
                                                 SelectionDAG &DAG) const {
  const GlobalValue *GV = cast<GlobalAddressSDNode>(Op)->getGlobal();
  int64_t Offset = cast<GlobalAddressSDNode>(Op)->getOffset();

  // Create the TargetGlobalAddress node, folding in the constant offset.
  SDValue Result = DAG.getTargetGlobalAddress(GV, Op.getDebugLoc(),
                                              getPointerTy(), Offset);
  return DAG.getNode(TLCS900ISD::Wrapper, Op.getDebugLoc(),
                     getPointerTy(), Result);
}

SDValue TLCS900TargetLowering::LowerExternalSymbol(SDValue Op,
                                                  SelectionDAG &DAG) const {
  DebugLoc dl = Op.getDebugLoc();
  const char *Sym = cast<ExternalSymbolSDNode>(Op)->getSymbol();
  SDValue Result = DAG.getTargetExternalSymbol(Sym, getPointerTy());

  return DAG.getNode(TLCS900ISD::Wrapper, dl, getPointerTy(), Result);
}

SDValue TLCS900TargetLowering::LowerBlockAddress(SDValue Op,
                                                SelectionDAG &DAG) const {
  DebugLoc dl = Op.getDebugLoc();
  const BlockAddress *BA = cast<BlockAddressSDNode>(Op)->getBlockAddress();
  SDValue Result = DAG.getBlockAddress(BA, getPointerTy(), /*isTarget=*/true);

  return DAG.getNode(TLCS900ISD::Wrapper, dl, getPointerTy(), Result);
}

static SDValue EmitCMP(SDValue &LHS, SDValue &RHS, SDValue &TargetCC,
                       ISD::CondCode CC,
                       DebugLoc dl, SelectionDAG &DAG) {
  // FIXME: Handle bittests someday
  assert(!LHS.getValueType().isFloatingPoint() && "We don't handle FP yet");

  // FIXME: Handle jump negative someday
  TLCS900CC::CondCodes TCC = TLCS900CC::COND_INVALID;
  switch (CC) {
  default: llvm_unreachable("Invalid integer condition!");
  case ISD::SETEQ:
    TCC = TLCS900CC::COND_EQ;     // aka COND_Z
    // Minor optimization: if LHS is a constant, swap operands, then the
    // constant can be folded into comparison.
    if (LHS.getOpcode() == ISD::Constant)
      std::swap(LHS, RHS);
    break;
  case ISD::SETNE:
    TCC = TLCS900CC::COND_NE;    // aka COND_NZ
    // Minor optimization: if LHS is a constant, swap operands, then the
    // constant can be folded into comparison.
    if (LHS.getOpcode() == ISD::Constant)
      std::swap(LHS, RHS);
    break;
  case ISD::SETULE:
    std::swap(LHS, RHS);        // FALLTHROUGH
  case ISD::SETUGE:
    // Turn lhs u>= rhs with lhs constant into rhs u< lhs+1, this allows us to
    // fold constant into instruction.
    if (const ConstantSDNode * C = dyn_cast<ConstantSDNode>(LHS)) {
      LHS = RHS;
      RHS = DAG.getConstant(C->getSExtValue() + 1, C->getValueType(0));
      TCC = TLCS900CC::COND_ULT;
      break;
    }
    TCC = TLCS900CC::COND_UGE;    // aka COND_C
    break;
  case ISD::SETUGT:
    std::swap(LHS, RHS);        // FALLTHROUGH
  case ISD::SETULT:
    // Turn lhs u< rhs with lhs constant into rhs u>= lhs+1, this allows us to
    // fold constant into instruction.
    if (const ConstantSDNode * C = dyn_cast<ConstantSDNode>(LHS)) {
      LHS = RHS;
      RHS = DAG.getConstant(C->getSExtValue() + 1, C->getValueType(0));
      TCC = TLCS900CC::COND_UGE;
      break;
    }
    TCC = TLCS900CC::COND_ULT;    // aka COND_NC
    break;
  case ISD::SETLE:
    std::swap(LHS, RHS);        // FALLTHROUGH
  case ISD::SETGE:
    // Turn lhs >= rhs with lhs constant into rhs < lhs+1, this allows us to
    // fold constant into instruction.
    if (const ConstantSDNode * C = dyn_cast<ConstantSDNode>(LHS)) {
      LHS = RHS;
      RHS = DAG.getConstant(C->getSExtValue() + 1, C->getValueType(0));
      TCC = TLCS900CC::COND_LT;
      break;
    }
    TCC = TLCS900CC::COND_GE;
    break;
  case ISD::SETGT:
    std::swap(LHS, RHS);        // FALLTHROUGH
  case ISD::SETLT:
    // Turn lhs < rhs with lhs constant into rhs >= lhs+1, this allows us to
    // fold constant into instruction.
    if (const ConstantSDNode * C = dyn_cast<ConstantSDNode>(LHS)) {
      LHS = RHS;
      RHS = DAG.getConstant(C->getSExtValue() + 1, C->getValueType(0));
      TCC = TLCS900CC::COND_GE;
      break;
    }
    TCC = TLCS900CC::COND_LT;
    break;
  }

  TargetCC = DAG.getConstant(TCC, MVT::i8);
  return DAG.getNode(TLCS900ISD::CMP, dl, MVT::Glue, LHS, RHS);
}


SDValue TLCS900TargetLowering::LowerBR_CC(SDValue Op, SelectionDAG &DAG) const {
  SDValue Chain = Op.getOperand(0);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(1))->get();
  SDValue LHS   = Op.getOperand(2);
  SDValue RHS   = Op.getOperand(3);
  SDValue Dest  = Op.getOperand(4);
  DebugLoc dl   = Op.getDebugLoc();

  SDValue TargetCC;
  SDValue Flag = EmitCMP(LHS, RHS, TargetCC, CC, dl, DAG);

  return DAG.getNode(TLCS900ISD::BR_CC, dl, Op.getValueType(),
                     Chain, Dest, TargetCC, Flag);
}

SDValue TLCS900TargetLowering::LowerSETCC(SDValue Op, SelectionDAG &DAG) const {
  SDValue LHS   = Op.getOperand(0);
  SDValue RHS   = Op.getOperand(1);
  DebugLoc dl   = Op.getDebugLoc();

  // If we are doing an AND and testing against zero, then the CMP
  // will not be generated.  The AND (or BIT) will generate the condition codes,
  // but they are different from CMP.
  // FIXME: since we're doing a post-processing, use a pseudoinstr here, so
  // lowering & isel wouldn't diverge.
  bool andCC = false;
  if (ConstantSDNode *RHSC = dyn_cast<ConstantSDNode>(RHS)) {
    if (RHSC->isNullValue() && LHS.hasOneUse() &&
        (LHS.getOpcode() == ISD::AND ||
         (LHS.getOpcode() == ISD::TRUNCATE &&
          LHS.getOperand(0).getOpcode() == ISD::AND))) {
      andCC = true;
    }
  }
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(2))->get();
  SDValue TargetCC;
  SDValue Flag = EmitCMP(LHS, RHS, TargetCC, CC, dl, DAG);

  // Get the condition codes directly from the status register, if its easy.
  // Otherwise a branch will be generated.  Note that the AND and BIT
  // instructions generate different flags than CMP, the carry bit can be used
  // for NE/EQ.
  bool Invert = false;
  bool Shift = false;
  bool Convert = true;
  switch (cast<ConstantSDNode>(TargetCC)->getZExtValue()) {
   default:
    Convert = false;
    break;
   case TLCS900CC::COND_UGE:
     // Res = SRW & 1, no processing is required
     break;
   case TLCS900CC::COND_ULT:
     // Res = ~(SRW & 1)
     Invert = true;
     break;
   case TLCS900CC::COND_NE:
     if (andCC) {
       // C = ~Z, thus Res = SRW & 1, no processing is required
     } else {
       // Res = ~((SRW >> 1) & 1)
       Shift = true;
       Invert = true;
     }
     break;
   case TLCS900CC::COND_EQ:
     Shift = true;
     // C = ~Z for AND instruction, thus we can put Res = ~(SRW & 1), however,
     // Res = (SRW >> 1) & 1 is 1 word shorter.
     break;
  }
  EVT VT = Op.getValueType();
  SDValue One  = DAG.getConstant(1, VT);
  if (Convert) {
    SDValue SR = DAG.getCopyFromReg(DAG.getEntryNode(), dl, TLCS900::SRW,
                                    MVT::i16, Flag);
    if (Shift)
      // FIXME: somewhere this is turned into a SRL, lower it MSP specific?
      SR = DAG.getNode(ISD::SRA, dl, MVT::i16, SR, One);
    SR = DAG.getNode(ISD::AND, dl, MVT::i16, SR, One);
    if (Invert)
      SR = DAG.getNode(ISD::XOR, dl, MVT::i16, SR, One);
    return SR;
  } else {
    SDValue Zero = DAG.getConstant(0, VT);
    SDVTList VTs = DAG.getVTList(Op.getValueType(), MVT::Glue);
    SmallVector<SDValue, 4> Ops;
    Ops.push_back(One);
    Ops.push_back(Zero);
    Ops.push_back(TargetCC);
    Ops.push_back(Flag);
    return DAG.getNode(TLCS900ISD::SELECT_CC, dl, VTs, &Ops[0], Ops.size());
  }
}

SDValue TLCS900TargetLowering::LowerSELECT_CC(SDValue Op,
                                             SelectionDAG &DAG) const {
  SDValue LHS    = Op.getOperand(0);
  SDValue RHS    = Op.getOperand(1);
  SDValue TrueV  = Op.getOperand(2);
  SDValue FalseV = Op.getOperand(3);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(4))->get();
  DebugLoc dl    = Op.getDebugLoc();

  SDValue TargetCC;
  SDValue Flag = EmitCMP(LHS, RHS, TargetCC, CC, dl, DAG);

  SDVTList VTs = DAG.getVTList(Op.getValueType(), MVT::Glue);
  SmallVector<SDValue, 4> Ops;
  Ops.push_back(TrueV);
  Ops.push_back(FalseV);
  Ops.push_back(TargetCC);
  Ops.push_back(Flag);

  return DAG.getNode(TLCS900ISD::SELECT_CC, dl, VTs, &Ops[0], Ops.size());
}

SDValue TLCS900TargetLowering::LowerSIGN_EXTEND(SDValue Op,
                                               SelectionDAG &DAG) const {
  SDValue Val = Op.getOperand(0);
  EVT VT      = Op.getValueType();
  DebugLoc dl = Op.getDebugLoc();

  assert(VT == MVT::i16 && "Only support i16 for now!");

  return DAG.getNode(ISD::SIGN_EXTEND_INREG, dl, VT,
                     DAG.getNode(ISD::ANY_EXTEND, dl, VT, Val),
                     DAG.getValueType(Val.getValueType()));
}

SDValue
TLCS900TargetLowering::getReturnAddressFrameIndex(SelectionDAG &DAG) const {
  MachineFunction &MF = DAG.getMachineFunction();
  TLCS900MachineFunctionInfo *FuncInfo = MF.getInfo<TLCS900MachineFunctionInfo>();
  int ReturnAddrIndex = FuncInfo->getRAIndex();

  if (ReturnAddrIndex == 0) {
    // Set up a frame object for the return address.
    uint64_t SlotSize = TD->getPointerSize();
    ReturnAddrIndex = MF.getFrameInfo()->CreateFixedObject(SlotSize, -SlotSize,
                                                           true);
    FuncInfo->setRAIndex(ReturnAddrIndex);
  }

  return DAG.getFrameIndex(ReturnAddrIndex, getPointerTy());
}

SDValue TLCS900TargetLowering::LowerRETURNADDR(SDValue Op,
                                              SelectionDAG &DAG) const {
  MachineFrameInfo *MFI = DAG.getMachineFunction().getFrameInfo();
  MFI->setReturnAddressIsTaken(true);

  unsigned Depth = cast<ConstantSDNode>(Op.getOperand(0))->getZExtValue();
  DebugLoc dl = Op.getDebugLoc();

  if (Depth > 0) {
    SDValue FrameAddr = LowerFRAMEADDR(Op, DAG);
    SDValue Offset =
      DAG.getConstant(TD->getPointerSize(), MVT::i16);
    return DAG.getLoad(getPointerTy(), dl, DAG.getEntryNode(),
                       DAG.getNode(ISD::ADD, dl, getPointerTy(),
                                   FrameAddr, Offset),
                       MachinePointerInfo(), false, false, false, 0);
  }

  // Just load the return address.
  SDValue RetAddrFI = getReturnAddressFrameIndex(DAG);
  return DAG.getLoad(getPointerTy(), dl, DAG.getEntryNode(),
                     RetAddrFI, MachinePointerInfo(), false, false, false, 0);
}

SDValue TLCS900TargetLowering::LowerFRAMEADDR(SDValue Op,
                                             SelectionDAG &DAG) const {
  MachineFrameInfo *MFI = DAG.getMachineFunction().getFrameInfo();
  MFI->setFrameAddressIsTaken(true);

  EVT VT = Op.getValueType();
  DebugLoc dl = Op.getDebugLoc();  // FIXME probably not meaningful
  unsigned Depth = cast<ConstantSDNode>(Op.getOperand(0))->getZExtValue();
  SDValue FrameAddr = DAG.getCopyFromReg(DAG.getEntryNode(), dl,
                                         TLCS900::FPW, VT);
  while (Depth--)
    FrameAddr = DAG.getLoad(VT, dl, DAG.getEntryNode(), FrameAddr,
                            MachinePointerInfo(),
                            false, false, false, 0);
  return FrameAddr;
}

/// getPostIndexedAddressParts - returns true by value, base pointer and
/// offset pointer and addressing mode by reference if this node can be
/// combined with a load / store to form a post-indexed load / store.
bool TLCS900TargetLowering::getPostIndexedAddressParts(SDNode *N, SDNode *Op,
                                                      SDValue &Base,
                                                      SDValue &Offset,
                                                      ISD::MemIndexedMode &AM,
                                                      SelectionDAG &DAG) const {

  LoadSDNode *LD = cast<LoadSDNode>(N);
  if (LD->getExtensionType() != ISD::NON_EXTLOAD)
    return false;

  EVT VT = LD->getMemoryVT();
  if (VT != MVT::i8 && VT != MVT::i16)
    return false;

  if (Op->getOpcode() != ISD::ADD)
    return false;

  if (ConstantSDNode *RHS = dyn_cast<ConstantSDNode>(Op->getOperand(1))) {
    uint64_t RHSC = RHS->getZExtValue();
    if ((VT == MVT::i16 && RHSC != 2) ||
        (VT == MVT::i8 && RHSC != 1))
      return false;

    Base = Op->getOperand(0);
    Offset = DAG.getConstant(RHSC, VT);
    AM = ISD::POST_INC;
    return true;
  }

  return false;
}


const char *TLCS900TargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch (Opcode) {
  default: return NULL;
  case TLCS900ISD::RET_FLAG:           return "TLCS900ISD::RET_FLAG";
  case TLCS900ISD::RETI_FLAG:          return "TLCS900ISD::RETI_FLAG";
  case TLCS900ISD::RRA:                return "TLCS900ISD::RRA";
  case TLCS900ISD::RLA:                return "TLCS900ISD::RLA";
  case TLCS900ISD::RRC:                return "TLCS900ISD::RRC";
  case TLCS900ISD::CALL:               return "TLCS900ISD::CALL";
  case TLCS900ISD::Wrapper:            return "TLCS900ISD::Wrapper";
  case TLCS900ISD::BR_CC:              return "TLCS900ISD::BR_CC";
  case TLCS900ISD::CMP:                return "TLCS900ISD::CMP";
  case TLCS900ISD::SELECT_CC:          return "TLCS900ISD::SELECT_CC";
  case TLCS900ISD::SHL:                return "TLCS900ISD::SHL";
  case TLCS900ISD::SRA:                return "TLCS900ISD::SRA";
  }
}

bool TLCS900TargetLowering::isTruncateFree(Type *Ty1,
                                          Type *Ty2) const {
  if (!Ty1->isIntegerTy() || !Ty2->isIntegerTy())
    return false;

  return (Ty1->getPrimitiveSizeInBits() > Ty2->getPrimitiveSizeInBits());
}

bool TLCS900TargetLowering::isTruncateFree(EVT VT1, EVT VT2) const {
  if (!VT1.isInteger() || !VT2.isInteger())
    return false;

  return (VT1.getSizeInBits() > VT2.getSizeInBits());
}

/*bool TLCS900TargetLowering::isZExtFree(Type *Ty1, Type *Ty2) const {
  // TLCS900 implicitly zero-extends 8-bit results in 16-bit registers.
  return 0 && Ty1->isIntegerTy(8) && Ty2->isIntegerTy(16);
}

bool TLCS900TargetLowering::isZExtFree(EVT VT1, EVT VT2) const {
  // TLCS900 implicitly zero-extends 8-bit results in 16-bit registers.
  return 0 && VT1 == MVT::i8 && VT2 == MVT::i16;
}*/

//===----------------------------------------------------------------------===//
//  Other Lowering Code
//===----------------------------------------------------------------------===//

MachineBasicBlock*
TLCS900TargetLowering::EmitShiftInstr(MachineInstr *MI,
                                     MachineBasicBlock *BB) const {
  MachineFunction *F = BB->getParent();
  MachineRegisterInfo &RI = F->getRegInfo();
  DebugLoc dl = MI->getDebugLoc();
  const TargetInstrInfo &TII = *getTargetMachine().getInstrInfo();

  unsigned Opc;
  const TargetRegisterClass * RC;
  switch (MI->getOpcode()) {
  default: llvm_unreachable("Invalid shift opcode!");
  case TLCS900::Shl8:
   Opc = TLCS900::SHL8r1;
   RC = TLCS900::GR8RegisterClass;
   break;
  case TLCS900::Shl16:
   Opc = TLCS900::SHL16r1;
   RC = TLCS900::GR16RegisterClass;
   break;
  case TLCS900::Sra8:
   Opc = TLCS900::SAR8r1;
   RC = TLCS900::GR8RegisterClass;
   break;
  case TLCS900::Sra16:
   Opc = TLCS900::SAR16r1;
   RC = TLCS900::GR16RegisterClass;
   break;
  case TLCS900::Srl8:
   Opc = TLCS900::SAR8r1c;
   RC = TLCS900::GR8RegisterClass;
   break;
  case TLCS900::Srl16:
   Opc = TLCS900::SAR16r1c;
   RC = TLCS900::GR16RegisterClass;
   break;
  }

  const BasicBlock *LLVM_BB = BB->getBasicBlock();
  MachineFunction::iterator I = BB;
  ++I;

  // Create loop block
  MachineBasicBlock *LoopBB = F->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *RemBB  = F->CreateMachineBasicBlock(LLVM_BB);

  F->insert(I, LoopBB);
  F->insert(I, RemBB);

  // Update machine-CFG edges by transferring all successors of the current
  // block to the block containing instructions after shift.
  RemBB->splice(RemBB->begin(), BB,
                llvm::next(MachineBasicBlock::iterator(MI)),
                BB->end());
  RemBB->transferSuccessorsAndUpdatePHIs(BB);

  // Add adges BB => LoopBB => RemBB, BB => RemBB, LoopBB => LoopBB
  BB->addSuccessor(LoopBB);
  BB->addSuccessor(RemBB);
  LoopBB->addSuccessor(RemBB);
  LoopBB->addSuccessor(LoopBB);

  unsigned ShiftAmtReg = RI.createVirtualRegister(TLCS900::GR8RegisterClass);
  unsigned ShiftAmtReg2 = RI.createVirtualRegister(TLCS900::GR8RegisterClass);
  unsigned ShiftReg = RI.createVirtualRegister(RC);
  unsigned ShiftReg2 = RI.createVirtualRegister(RC);
  unsigned ShiftAmtSrcReg = MI->getOperand(2).getReg();
  unsigned SrcReg = MI->getOperand(1).getReg();
  unsigned DstReg = MI->getOperand(0).getReg();

  // BB:
  // cmp 0, N
  // je RemBB
  BuildMI(BB, dl, TII.get(TLCS900::CMP8ri))
    .addReg(ShiftAmtSrcReg).addImm(0);
  BuildMI(BB, dl, TII.get(TLCS900::JCC))
    .addMBB(RemBB)
    .addImm(TLCS900CC::COND_EQ);

  // LoopBB:
  // ShiftReg = phi [%SrcReg, BB], [%ShiftReg2, LoopBB]
  // ShiftAmt = phi [%N, BB],      [%ShiftAmt2, LoopBB]
  // ShiftReg2 = shift ShiftReg
  // ShiftAmt2 = ShiftAmt - 1;
  BuildMI(LoopBB, dl, TII.get(TLCS900::PHI), ShiftReg)
    .addReg(SrcReg).addMBB(BB)
    .addReg(ShiftReg2).addMBB(LoopBB);
  BuildMI(LoopBB, dl, TII.get(TLCS900::PHI), ShiftAmtReg)
    .addReg(ShiftAmtSrcReg).addMBB(BB)
    .addReg(ShiftAmtReg2).addMBB(LoopBB);
  BuildMI(LoopBB, dl, TII.get(Opc), ShiftReg2)
    .addReg(ShiftReg);
  BuildMI(LoopBB, dl, TII.get(TLCS900::SUB8ri), ShiftAmtReg2)
    .addReg(ShiftAmtReg).addImm(1);
  BuildMI(LoopBB, dl, TII.get(TLCS900::JCC))
    .addMBB(LoopBB)
    .addImm(TLCS900CC::COND_NE);

  // RemBB:
  // DestReg = phi [%SrcReg, BB], [%ShiftReg, LoopBB]
  BuildMI(*RemBB, RemBB->begin(), dl, TII.get(TLCS900::PHI), DstReg)
    .addReg(SrcReg).addMBB(BB)
    .addReg(ShiftReg2).addMBB(LoopBB);

  MI->eraseFromParent();   // The pseudo instruction is gone now.
  return RemBB;
}

MachineBasicBlock*
TLCS900TargetLowering::EmitInstrWithCustomInserter(MachineInstr *MI,
                                                  MachineBasicBlock *BB) const {
  unsigned Opc = MI->getOpcode();

  if (Opc == TLCS900::Shl8 || Opc == TLCS900::Shl16 ||
      Opc == TLCS900::Sra8 || Opc == TLCS900::Sra16 ||
      Opc == TLCS900::Srl8 || Opc == TLCS900::Srl16)
    return EmitShiftInstr(MI, BB);

  const TargetInstrInfo &TII = *getTargetMachine().getInstrInfo();
  DebugLoc dl = MI->getDebugLoc();

  assert((Opc == TLCS900::Select16 || Opc == TLCS900::Select8) &&
         "Unexpected instr type to insert");

  // To "insert" a SELECT instruction, we actually have to insert the diamond
  // control-flow pattern.  The incoming instruction knows the destination vreg
  // to set, the condition code register to branch on, the true/false values to
  // select between, and a branch opcode to use.
  const BasicBlock *LLVM_BB = BB->getBasicBlock();
  MachineFunction::iterator I = BB;
  ++I;

  //  thisMBB:
  //  ...
  //   TrueVal = ...
  //   cmpTY ccX, r1, r2
  //   jCC copy1MBB
  //   fallthrough --> copy0MBB
  MachineBasicBlock *thisMBB = BB;
  MachineFunction *F = BB->getParent();
  MachineBasicBlock *copy0MBB = F->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *copy1MBB = F->CreateMachineBasicBlock(LLVM_BB);
  F->insert(I, copy0MBB);
  F->insert(I, copy1MBB);
  // Update machine-CFG edges by transferring all successors of the current
  // block to the new block which will contain the Phi node for the select.
  copy1MBB->splice(copy1MBB->begin(), BB,
                   llvm::next(MachineBasicBlock::iterator(MI)),
                   BB->end());
  copy1MBB->transferSuccessorsAndUpdatePHIs(BB);
  // Next, add the true and fallthrough blocks as its successors.
  BB->addSuccessor(copy0MBB);
  BB->addSuccessor(copy1MBB);

  BuildMI(BB, dl, TII.get(TLCS900::JCC))
    .addMBB(copy1MBB)
    .addImm(MI->getOperand(3).getImm());

  //  copy0MBB:
  //   %FalseValue = ...
  //   # fallthrough to copy1MBB
  BB = copy0MBB;

  // Update machine-CFG edges
  BB->addSuccessor(copy1MBB);

  //  copy1MBB:
  //   %Result = phi [ %FalseValue, copy0MBB ], [ %TrueValue, thisMBB ]
  //  ...
  BB = copy1MBB;
  BuildMI(*BB, BB->begin(), dl, TII.get(TLCS900::PHI),
          MI->getOperand(0).getReg())
    .addReg(MI->getOperand(2).getReg()).addMBB(copy0MBB)
    .addReg(MI->getOperand(1).getReg()).addMBB(thisMBB);

  MI->eraseFromParent();   // The pseudo instruction is gone now.
  return BB;
}
