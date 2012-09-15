set(LLVM_TARGET_DEFINITIONS TLCS900.td)

tablegen(LLVM TLCS900GenRegisterInfo.inc -gen-register-info)
tablegen(LLVM TLCS900GenInstrInfo.inc -gen-instr-info)
tablegen(LLVM TLCS900GenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM TLCS900GenDAGISel.inc -gen-dag-isel)
tablegen(LLVM TLCS900GenCallingConv.inc -gen-callingconv)
tablegen(LLVM TLCS900GenSubtargetInfo.inc -gen-subtarget)
add_public_tablegen_target(TLCS900CommonTableGen)

add_llvm_target(TLCS900CodeGen
  TLCS900BranchSelector.cpp
  TLCS900ISelDAGToDAG.cpp
  TLCS900ISelLowering.cpp
  TLCS900InstrInfo.cpp
  TLCS900FrameLowering.cpp
  TLCS900MachineFunctionInfo.cpp
  TLCS900RegisterInfo.cpp
  TLCS900Subtarget.cpp
  TLCS900TargetMachine.cpp
  TLCS900SelectionDAGInfo.cpp
  TLCS900AsmPrinter.cpp
  TLCS900MCInstLower.cpp
  )

add_subdirectory(InstPrinter)
add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
