cmake_minimum_required(VERSION 3.20)
project(MojMergefuncPass)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake from ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_library(MojMergefuncPass MODULE MojMergefuncPass.cpp)

# Pull in the flags LLVM was built with — critically this includes -fno-rtti.
target_compile_options(MojMergefuncPass PRIVATE ${LLVM_CXXFLAGS})

# Disable RTTI and exceptions explicitly as a belt-and-suspenders guard.
target_compile_options(MojMergefuncPass PRIVATE -fno-rtti -fno-exceptions)

llvm_map_components_to_libnames(LLVM_LIBS core support passes)
target_link_libraries(MojMergefuncPass PRIVATE ${LLVM_LIBS})