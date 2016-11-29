cmake_minimum_required(VERSION 3.4.3)
project(RRP)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(rrpc src/main.cxx src/Translator.cxx src/TranslatorData.cxx src/TranslatorCallbacks.cxx)

llvm_map_components_to_libnames(llvm_libs support core irreader analysis bitwriter scalaropts linker vectorize)

target_link_libraries(rrpc ${llvm_libs})
