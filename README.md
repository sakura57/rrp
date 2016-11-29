# R2A03 Recompiler Project


- BUILDING -

In the root directory, run:
cmake .
make

CMake will detect LLVM and create the makefiles accordingly. make will build rrpc in the 
parent directory.

- USAGE -

Currently, RRP's syntax is as follows:
rrpc <input 6502 binary file>

This will produce a file, rrpout.bc, in the parent directory. Currently, RRP assumes an 
origin address of 8000h. Support for different origin addresses, and also text and data 
segments, is not currently implemented, but will be added eventually.

Additionally, the compile.sh script can be used to recompile and link a complete binary 
with one command. 
If you do not have a 64-bit system, and/or the link step fails, you probably need to 
modify compile.sh to point to the correct libs.

- TIPS -

Be sure to compile a memory mapper module to link to. See the MemoryMapperDebug folder for 
an example memory mapper module. RRP's output will invoke the following functions:

rrpInit - invoked on program start, use to malloc() needed resources
rrpExit - invoked on program end, use to free() resources
rrpMemWrite - invoked on memory write
rrpMemRead - invoked on memory read
rrpMemWriteAssoc - invoked when a host CPU return address should be associated with a 
guest CPU memory address
rrpMemReadAssoc - invoked when the program needs to know the return address for RTS

These functions must be implemented by the memory mapper module.
