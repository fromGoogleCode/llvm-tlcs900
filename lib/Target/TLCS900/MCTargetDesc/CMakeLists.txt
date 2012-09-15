add_llvm_library(LLVMTLCS900Desc
  TLCS900MCTargetDesc.cpp
  TLCS900MCAsmInfo.cpp
  )

add_dependencies(LLVMTLCS900Desc TLCS900CommonTableGen)
