add_llvm_library( LLVMMojMergeReturnPass MODULE
  MojMergeReturnPass.cpp

  PLUGIN_TOOL
  opt
  )
