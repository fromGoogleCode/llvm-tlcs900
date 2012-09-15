include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/.. )

add_llvm_library(LLVMTLCS900Info
  TLCS900TargetInfo.cpp
  )

add_dependencies(LLVMTLCS900Info TLCS900CommonTableGen)
