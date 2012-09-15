//===-- TLCS900SelectionDAGInfo.h - TLCS900 SelectionDAG Info -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the TLCS900 subclass for TargetSelectionDAGInfo.
//
//===----------------------------------------------------------------------===//

#ifndef TLCS900SELECTIONDAGINFO_H
#define TLCS900SELECTIONDAGINFO_H

#include "llvm/Target/TargetSelectionDAGInfo.h"

namespace llvm {

class TLCS900TargetMachine;

class TLCS900SelectionDAGInfo : public TargetSelectionDAGInfo {
public:
  explicit TLCS900SelectionDAGInfo(const TLCS900TargetMachine &TM);
  ~TLCS900SelectionDAGInfo();
};

}

#endif
