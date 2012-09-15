//===- llvm-build generated file --------------------------------*- C++ -*-===//
//
// Component Library Depenedency Table
//
// Automatically generated file, do not edit!
//
//===----------------------------------------------------------------------===//

struct AvailableComponent {
  /// The name of the component.
  const char *Name;

  /// The name of the library for this component (or NULL).
  const char *Library;

  /// Whether the component is installed.
  bool IsInstalled;

  /// The list of libraries required when linking this component.
  const char *RequiredLibraries[16];
} AvailableComponents[42] = {
  { "all", 0, 1, { "mcjit", "engine", "bitwriter", "debuginfo", "linker", "ipo", "nativecodegen", "tablegen", "gtest_main", "native", "instrumentation", "mcdisassembler", "asmparser", "jit", "all-targets" } },
  { "all-targets", 0, 1, { "tlcs900" } },
  { "analysis", "libLLVMAnalysis.a", 1, { "core", "support", "target" } },
  { "archive", "libLLVMArchive.a", 1, { "bitreader", "core", "support" } },
  { "asmparser", "libLLVMAsmParser.a", 1, { "core", "support" } },
  { "asmprinter", "libLLVMAsmPrinter.a", 1, { "analysis", "codegen", "core", "mc", "mcparser", "support", "target" } },
  { "bitreader", "libLLVMBitReader.a", 1, { "core", "support" } },
  { "bitwriter", "libLLVMBitWriter.a", 1, { "core", "support" } },
  { "codegen", "libLLVMCodeGen.a", 1, { "analysis", "core", "mc", "scalaropts", "support", "target", "transformutils" } },
  { "core", "libLLVMCore.a", 1, { "support" } },
  { "debuginfo", "libLLVMDebugInfo.a", 1, { "support" } },
  { "engine", 0, 1, { "interpreter" } },
  { "executionengine", "libLLVMExecutionEngine.a", 1, { "core", "mc", "support", "target" } },
  { "gtest", "libgtest.a", 0, { "support" } },
  { "gtest_main", "libgtest_main.a", 0, { "gtest" } },
  { "instcombine", "libLLVMInstCombine.a", 1, { "analysis", "core", "support", "target", "transformutils" } },
  { "instrumentation", "libLLVMInstrumentation.a", 1, { "analysis", "core", "support", "transformutils" } },
  { "interpreter", "libLLVMInterpreter.a", 1, { "codegen", "core", "executionengine", "support", "target" } },
  { "ipa", "libLLVMipa.a", 1, { "analysis", "core", "support" } },
  { "ipo", "libLLVMipo.a", 1, { "analysis", "core", "ipa", "instcombine", "scalaropts", "vectorize", "support", "target", "transformutils" } },
  { "jit", "libLLVMJIT.a", 1, { "codegen", "core", "executionengine", "mc", "runtimedyld", "support", "target" } },
  { "linker", "libLLVMLinker.a", 1, { "archive", "bitreader", "core", "support", "transformutils" } },
  { "mc", "libLLVMMC.a", 1, { "object", "support" } },
  { "mcdisassembler", "libLLVMMCDisassembler.a", 1, { "mc", "mcparser", "support" } },
  { "mcjit", "libLLVMMCJIT.a", 1, { "core", "executionengine", "runtimedyld", "support", "target" } },
  { "mcparser", "libLLVMMCParser.a", 1, { "mc", "support" } },
  { "native", 0, 1, {  } },
  { "nativecodegen", 0, 1, {  } },
  { "object", "libLLVMObject.a", 1, { "support" } },
  { "runtimedyld", "libLLVMRuntimeDyld.a", 1, { "object", "support" } },
  { "scalaropts", "libLLVMScalarOpts.a", 1, { "analysis", "core", "instcombine", "support", "target", "transformutils" } },
  { "selectiondag", "libLLVMSelectionDAG.a", 1, { "analysis", "codegen", "core", "mc", "support", "target", "transformutils" } },
  { "support", "libLLVMSupport.a", 1, {  } },
  { "tablegen", "libLLVMTableGen.a", 1, { "support" } },
  { "target", "libLLVMTarget.a", 1, { "core", "mc", "support" } },
  { "tlcs900", 0, 1, { "tlcs900asmprinter", "tlcs900info", "tlcs900desc", "tlcs900codegen" } },
  { "tlcs900asmprinter", "libLLVMTLCS900AsmPrinter.a", 1, { "mc", "support" } },
  { "tlcs900codegen", "libLLVMTLCS900CodeGen.a", 1, { "asmprinter", "codegen", "core", "mc", "tlcs900asmprinter", "tlcs900desc", "tlcs900info", "selectiondag", "support", "target" } },
  { "tlcs900desc", "libLLVMTLCS900Desc.a", 1, { "mc", "tlcs900asmprinter", "tlcs900info", "support", "target" } },
  { "tlcs900info", "libLLVMTLCS900Info.a", 1, { "mc", "support", "target" } },
  { "transformutils", "libLLVMTransformUtils.a", 1, { "analysis", "core", "ipa", "support", "target" } },
  { "vectorize", "libLLVMVectorize.a", 1, { "analysis", "core", "instcombine", "support", "target", "transformutils" } },
};
