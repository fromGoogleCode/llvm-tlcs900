include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/.. )

add_llvm_library(LLVMTLCS900AsmPrinter
  TLCS900InstPrinter.cpp
  )

add_dependencies(LLVMTLCS900AsmPrinter TLCS900CommonTableGen)
