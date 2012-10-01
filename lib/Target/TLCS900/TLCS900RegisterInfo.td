//===-- MSP430RegisterInfo.td - TLCS900 Register defs -------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the TLCS900 register file
//===----------------------------------------------------------------------===//


let Namespace = "TLCS900" in {

  // Subregister indices.
  def subreg_8bit    : SubRegIndex;
  def subreg_8bit_hi : SubRegIndex;
  def subreg_16bit   : SubRegIndex;
  def subreg_16bit_hi : SubRegIndex;

  // 8-bit registers (current bank)
  def W : Register<"w">;
  def A : Register<"a">;
  def B : Register<"b">;
  def C : Register<"c">;
  def D : Register<"d">;
  def E : Register<"e">;
  def H : Register<"h">;
  def L : Register<"l">;

  // 8-bit registers (current bank)
  // requires extension code
  let CostPerUse = 1 in {
  def QW : Register<"qw">;
  def QA : Register<"qa">;
  def QB : Register<"qb">;
  def QC : Register<"qc">;
  def QD : Register<"qd">;
  def QE : Register<"qe">;
  def QH : Register<"qh">;
  def QL : Register<"ql">;
  def IXL : Register<"ixl">;
  def IXH : Register<"ixh">;
  def QIXL : Register<"qixl">;
  def QIXH : Register<"qixh">;
  def IYL : Register<"iyl">;
  def IYH : Register<"iyh">;
  def QIYL : Register<"qiyl">;
  def QIYH : Register<"qiyh">;
  def IZL : Register<"izl">;
  def IZH : Register<"izh">;
  def QIZL : Register<"qizl">;
  def QIZH : Register<"qizh">;
  def SPL : Register<"spl">;
  def SPH : Register<"sph">;
  def QSPL : Register<"qspl">;
  def QSPH : Register<"qsph">;
  }
  
  // 16-bit registers (current bank)
  let SubRegIndices = [subreg_8bit, subreg_8bit_hi], CoveredBySubRegs = 1 in {
  def WA : RegisterWithSubRegs<"wa", [A, W]>;
  def BC : RegisterWithSubRegs<"bc", [C, B]>;
  def DE : RegisterWithSubRegs<"de", [E, D]>;
  def HL : RegisterWithSubRegs<"hl", [L, H]>;
  def IX : RegisterWithSubRegs<"ix", [IXL, IXH]>;
  def IY : RegisterWithSubRegs<"iy", [IYL, IYH]>;
  def IZ : RegisterWithSubRegs<"iz", [IZL, IZH]>;
  def SP : RegisterWithSubRegs<"sp", [SPL, SPH]>;
  }

  // 16-bit registers (current bank)
  // Upper halfs, requires extension code
  let SubRegIndices = [subreg_8bit, subreg_8bit_hi], CoveredBySubRegs = 1, CostPerUse = 1 in {
  def QWA : RegisterWithSubRegs<"qwa", [QA, QW]>;
  def QBC : RegisterWithSubRegs<"qbc", [QC, QB]>;
  def QDE : RegisterWithSubRegs<"qde", [QE, QD]>;
  def QHL : RegisterWithSubRegs<"qhl", [QL, QH]>;
  def QIX : RegisterWithSubRegs<"qix", [QIXL, QIXH]>;
  def QIY : RegisterWithSubRegs<"qiy", [QIYL, QIYH]>;
  def QIZ : RegisterWithSubRegs<"qiz", [QIZL, QIZH]>;
  def QSP : RegisterWithSubRegs<"qsp", [QSPL, QSPH]>;
  }
  
  // 32-bit registers (current bank)
  let SubRegIndices = [subreg_16bit, subreg_16bit_hi], CoveredBySubRegs = 1 in {
  def XWA : RegisterWithSubRegs<"xwa", [WA, QWA]>;
  def XBC : RegisterWithSubRegs<"xbc", [BC, QBC]>;
  def XDE : RegisterWithSubRegs<"xde", [DE, QDE]>;
  def XHL : RegisterWithSubRegs<"xhl", [HL, QHL]>;
  def XIX : RegisterWithSubRegs<"xix", [IX, QIX]>;
  def XIY : RegisterWithSubRegs<"xiy", [IY, QIY]>;
  def XIZ : RegisterWithSubRegs<"xiz", [IZ, QIZ]>;
  def XSP : RegisterWithSubRegs<"xsp", [SP, QSP]>;
  }

  // Flags register
  def F : Register<"f">;
  def F2 : Register<"f2">;	// Actually F'
  
  let SubRegIndices = [subreg_8bit] in {
  def SR : RegisterWithSubRegs<"sr", [F]>;
  }
  
  // Interrupt nesting counter
  def INTNEST : Register<"intnest">;
  
  // microDMA source address registers
  def "DMAS0" : Register<"dmas0">;
  def "DMAS1" : Register<"dmas1">;
  def "DMAS2" : Register<"dmas2">;
  def "DMAS3" : Register<"dmas3">;

  // microDMA destination address registers
  def "DMAD0" : Register<"dmad0">;
  def "DMAD1" : Register<"dmad1">;
  def "DMAD2" : Register<"dmad2">;
  def "DMAD3" : Register<"dmad3">;

  // microDMA transfer count registers
  def "DMAC0" : Register<"dmac0">;
  def "DMAC1" : Register<"dmac1">;
  def "DMAC2" : Register<"dmac2">;
  def "DMAC3" : Register<"dmac3">;  

  // microDMA transfer mode registers
  def "DMAM0" : Register<"dmam0">;
  def "DMAM1" : Register<"dmam1">;
  def "DMAM2" : Register<"dmam2">;
  def "DMAM3" : Register<"dmam3">;  
}

//===----------------------------------------------------------------------===//
//  Registers
//===----------------------------------------------------------------------===//


def GR8 : RegisterClass<"TLCS900", [i8], 8,
   // Volatile registers
  (add W, A, B, C, D, E, H, L,
       QW, QA, QB, QC, QD, QE, QH, QL,
       IXL, IXH, IYL, IYH, IZL, IZH, SPL, SPH,
       QIXL, QIXH, QIYL, QIYH, QIZL, QIZH, QSPL, QSPH
  )>;

def GR16 : RegisterClass<"TLCS900", [i16], 16,
   // Volatile registers
  (add WA, BC, DE, HL, IX, IY, IZ, SP,
       QWA, QBC, QDE, QHL, QIX, QIY, QIZ, QSP)> {
  
  let SubRegClasses = [(GR8 subreg_8bit, subreg_8bit_hi)];
}

def GR32 : RegisterClass<"TLCS900", [i32], 32,
   // Volatile registers
  (add XWA, XBC, XDE, XHL, XIX, XIY, XIZ, XSP)> {
  
  let SubRegClasses = [(GR8 subreg_16bit, subreg_16bit_hi)];
}


def CCR8 : RegisterClass<"TLCS900", [i8], 8,
  (add F, F2)> {
    let CopyCost = -1;  // Don't allow copying of status registers.
    let isAllocatable = 0;
}

// Status flags registers.
def CCR : RegisterClass<"TLCS900", [i16], 16,
  (add SR, INTNEST)> {
    let CopyCost = -1;  // Don't allow copying of status registers.
    let isAllocatable = 0;
    let SubRegClasses = [(CCR8 subreg_8bit)];
}


def DMA_ADDR_REG : RegisterClass<"TLCS900", [i32], 32,
  (add DMAS0, DMAS1, DMAS2, DMAS3,
       DMAD0, DMAD1, DMAD2, DMAD3)> {
    let isAllocatable = 0;
}

def DMA_COUNT_REG : RegisterClass<"TLCS900", [i16], 16,
  (add DMAC0, DMAC1, DMAC2, DMAC3)> {
    let isAllocatable = 0;
}

def DMA_MODE_REG : RegisterClass<"TLCS900", [i8], 8,
  (add DMAM0, DMAM1, DMAM2, DMAM3)> {
    let isAllocatable = 0;
}
