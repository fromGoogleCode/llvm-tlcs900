//===-- TLCS900MCAsmInfo.cpp - TLCS900 asm properties -----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the TLCS900MCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "TLCS900MCAsmInfo.h"
#include "llvm/ADT/StringRef.h"
using namespace llvm;

void TLCS900MCAsmInfo::anchor() { }

TLCS900MCAsmInfo::TLCS900MCAsmInfo(const Target &T, StringRef TT) {
  PointerSize = 2;

  PrivateGlobalPrefix = ".L";
  WeakRefDirective ="\t.weak\t";
  PCSymbol=".";
  CommentString = ";";

  AlignmentIsInBytes = false;
  AllowNameToStartWithDigit = true;
  UsesELFSectionDirectiveForBSS = true;
}
