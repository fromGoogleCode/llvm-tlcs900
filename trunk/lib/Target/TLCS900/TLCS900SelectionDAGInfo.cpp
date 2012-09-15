//===-- TLCS900SelectionDAGInfo.cpp - TLCS900 SelectionDAG Info -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the TLCS900SelectionDAGInfo class.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "tlcs900-selectiondag-info"
#include "TLCS900TargetMachine.h"
using namespace llvm;

TLCS900SelectionDAGInfo::TLCS900SelectionDAGInfo(const TLCS900TargetMachine &TM)
  : TargetSelectionDAGInfo(TM) {
}

TLCS900SelectionDAGInfo::~TLCS900SelectionDAGInfo() {
}
