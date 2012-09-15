//==- TLCS900CallingConv.td - Calling Conventions for TLCS900 -*- tablegen -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// This describes the calling conventions for TLCS900 architecture.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// TLCS900 Return Value Calling Convention
//===----------------------------------------------------------------------===//
def RetCC_TLCS900 : CallingConv<[
  // i8 are returned in registers W, A
  CCIfType<[i8], CCAssignToReg<[W, A]>>,

  // i16 are returned in register WA
  CCIfType<[i16], CCAssignToReg<[WA]>>,

  // i32 are returned in register XWA
  CCIfType<[i32], CCAssignToReg<[XWA]>>
]>;

//===----------------------------------------------------------------------===//
// TLCS900 Argument Calling Conventions
//===----------------------------------------------------------------------===//
def CC_TLCS900 : CallingConv<[
  // Promote i8 arguments to i16.
  CCIfType<[i8], CCPromoteToType<i16>>,

  // The first 4 integer arguments of non-varargs functions are passed in
  // integer registers.
  CCIfNotVarArg<CCIfType<[i16], CCAssignToReg<[WA]>>>,
  CCIfNotVarArg<CCIfType<[i32], CCAssignToReg<[XWA]>>>,

  // Integer values get stored in stack slots that are 2 bytes in
  // size and 2-byte aligned.
  CCIfType<[i16], CCAssignToStack<2, 2>>,
  CCIfType<[i32], CCAssignToStack<4, 2>>
]>;
