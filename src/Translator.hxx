/*
	Part of the R2A03 Recompiler Project
*/

#pragma once

#include <algorithm>
#include <tuple>
//#include <map>
#include "sparsepp.h"
#include <memory>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <llvm/Pass.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/Instructions.h>
#include "llvm/IR/IRBuilder.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include "llvm/IR/Verifier.h"
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/MathExtras.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/Scalar.h>
#include "Defs.hxx"

namespace RRP
{
	class Translator
	{
	public:
		//IData has the form <srcLabel, address, byte, dstLabel>
		typedef std::tuple<std::string, word_t, byte_t, std::string> IData;
		enum ExceptionCode : int
		{
			UNKNOWN=0,
			PREMATURE_EOF=1,
			INVALID_ADDR_MODE=2,
			NONEXISTANT_BRANCH=3,
			UNSUPPORTED_INSTRUCTION=4
		};
		typedef std::tuple<ExceptionCode, word_t> Exception;

		static std::array<std::string, 5> const exceptText;

		Translator(std::string const &);

		void process_file(std::ifstream &);
		void output_bitcode(std::string const &);
	private:
		typedef std::function<llvm::Value *(Translator *, std::vector<IData> &, std::vector<IData>::iterator &)> AddressModeReadCallback;
		typedef std::function<llvm::Value *(Translator *, std::vector<IData> &, std::vector<IData>::iterator &)> AddressModeWriteCallback;
		typedef std::function<void(Translator *, std::vector<IData> &, std::vector<IData>::iterator &)> PreprocessCallback;
		typedef std::tuple<AddressModeReadCallback, AddressModeWriteCallback> AddressModeCallback;
		typedef std::function<void(Translator *, std::vector<IData> &, std::vector<IData>::iterator &, AddressModeCallback &)> OperationCallback;
		typedef std::function<void(Translator *, std::vector<IData> &, std::vector<IData>::iterator &)> ImplicitModeCallback;

		enum Flag : size_t { C=0, Z=1, I=2, D=3, B=4, Q=5, V=6, S=7, MAX=8 };
		static std::array<std::array<AddressModeCallback, 8>, 4> const addrModeCb;
		static std::array<std::array<OperationCallback, 8>, 4> const opCb;
		static std::array<byte_t, 0x100> const sizeArr;
		static std::array<PreprocessCallback, 0x100> const ppCb;
		static std::array<ImplicitModeCallback, 0x100> const uuCb;

		bool _needsBr = true;

		//std::map<std::string, llvm::BasicBlock *> _bbMap;
		spp::sparse_hash_map<std::string, llvm::BasicBlock *> _bbMap;

		llvm::LLVMContext _llvmContext;
		llvm::IRBuilder<> _irBuilder;
		std::unique_ptr<llvm::Module> _mod;
		llvm::Function * _main;
		llvm::BasicBlock * _curBB;
		llvm::Value * _A;
		llvm::Value * _X;
		llvm::Value * _Y;
		llvm::Value * _S;
		llvm::Value * _op;
		llvm::Value * _memAlloc;
		std::array<llvm::Value *, Flag::MAX> _flag;
        std::vector<llvm::BasicBlock *> _indBrDests;
		llvm::Function * _memWrite;
		llvm::Function * _memRead;
		llvm::Function * _memWriteAssoc;
		llvm::Function * _memReadAssoc;
		llvm::Function * _exit;
		llvm::Function * _start;
		bool _immRead = false;
		word_t _imm = 0;

		/*
			Definitions for addressing mode callbacks and operation callbacks
		*/
	private:
		static constexpr word_t starting_addr(void);
		void memory_read(llvm::Value *&, llvm::Value *&);
		void memory_write(llvm::Value *&, llvm::Value *&);
		void memory_read_association(llvm::Value *&, llvm::Value *&);
		void memory_write_association(llvm::Value *&, llvm::Value *&);
		inline void _set_z(llvm::Value *&);
		inline void _set_s(llvm::Value *&);
		inline void _build_flags(llvm::Value *&);
		inline void _decompose_flags(llvm::Value *&);

		llvm::Value * _mode_read_indirect_zeropage_indexedX(std::vector<IData> &, std::vector<IData>::iterator &);
		llvm::Value * _mode_read_zeropage(std::vector<IData> &, std::vector<IData>::iterator &);
		llvm::Value * _mode_read_immediate(std::vector<IData> &, std::vector<IData>::iterator &);
		llvm::Value * _mode_read_accumulator(std::vector<IData> &, std::vector<IData>::iterator &);
		llvm::Value * _mode_read_absolute(std::vector<IData> &, std::vector<IData>::iterator &);
		llvm::Value * _mode_read_indirect_zeropage_indexedY(std::vector<IData> &, std::vector<IData>::iterator &);
		llvm::Value * _mode_read_zeropage_indexedX(std::vector<IData> &, std::vector<IData>::iterator &);
		llvm::Value * _mode_read_absolute_indexedX(std::vector<IData> &, std::vector<IData>::iterator &);
		llvm::Value * _mode_read_absolute_indexedY(std::vector<IData> &, std::vector<IData>::iterator &);

		llvm::Value * _mode_write_indirect_zeropage_indexedX(std::vector<IData> &, std::vector<IData>::iterator &);
		llvm::Value * _mode_write_zeropage(std::vector<IData> &, std::vector<IData>::iterator &);
		llvm::Value * _mode_write_immediate(std::vector<IData> &, std::vector<IData>::iterator &);
		llvm::Value * _mode_write_accumulator(std::vector<IData> &, std::vector<IData>::iterator &);
		llvm::Value * _mode_write_absolute(std::vector<IData> &, std::vector<IData>::iterator &);
		llvm::Value * _mode_write_indirect_zeropage_indexedY(std::vector<IData> &, std::vector<IData>::iterator &);
		llvm::Value * _mode_write_zeropage_indexedX(std::vector<IData> &, std::vector<IData>::iterator &);
		llvm::Value * _mode_write_absolute_indexedX(std::vector<IData> &, std::vector<IData>::iterator &);
		llvm::Value * _mode_write_absolute_indexedY(std::vector<IData> &, std::vector<IData>::iterator &);

		void _op_ORA(std::vector<IData> &, std::vector<IData>::iterator &, AddressModeCallback &);
		void _op_AND(std::vector<IData> &, std::vector<IData>::iterator &, AddressModeCallback &);
		void _op_EOR(std::vector<IData> &, std::vector<IData>::iterator &, AddressModeCallback &);
		void _op_ADC(std::vector<IData> &, std::vector<IData>::iterator &, AddressModeCallback &);
		void _op_STA(std::vector<IData> &, std::vector<IData>::iterator &, AddressModeCallback &);
		void _op_LDA(std::vector<IData> &, std::vector<IData>::iterator &, AddressModeCallback &);
		void _op_CMP(std::vector<IData> &, std::vector<IData>::iterator &, AddressModeCallback &);
		void _op_SBC(std::vector<IData> &, std::vector<IData>::iterator &, AddressModeCallback &);

		void _op_ASL(std::vector<IData> &, std::vector<IData>::iterator &, AddressModeCallback &);
		void _op_ROL(std::vector<IData> &, std::vector<IData>::iterator &, AddressModeCallback &);
		void _op_LSR(std::vector<IData> &, std::vector<IData>::iterator &, AddressModeCallback &);
		void _op_ROR(std::vector<IData> &, std::vector<IData>::iterator &, AddressModeCallback &);
		void _op_STX(std::vector<IData> &, std::vector<IData>::iterator &, AddressModeCallback &);
		void _op_LDX(std::vector<IData> &, std::vector<IData>::iterator &, AddressModeCallback &);
		void _op_DEC(std::vector<IData> &, std::vector<IData>::iterator &, AddressModeCallback &);
		void _op_INC(std::vector<IData> &, std::vector<IData>::iterator &, AddressModeCallback &);

		void _op_BIT(std::vector<IData> &, std::vector<IData>::iterator &, AddressModeCallback &);
		void _op_STY(std::vector<IData> &, std::vector<IData>::iterator &, AddressModeCallback &);
		void _op_LDY(std::vector<IData> &, std::vector<IData>::iterator &, AddressModeCallback &);
		void _op_CPY(std::vector<IData> &, std::vector<IData>::iterator &, AddressModeCallback &);
		void _op_CPX(std::vector<IData> &, std::vector<IData>::iterator &, AddressModeCallback &);

		void _pp_mklb0(std::vector<IData> &, std::vector<IData>::iterator &);
		void _pp_mklb1(std::vector<IData> &, std::vector<IData>::iterator &);
		void _pp_mklb2(std::vector<IData> &, std::vector<IData>::iterator &);

		void _uu_TXA(std::vector<IData> &, std::vector<IData>::iterator &);
		void _uu_TAX(std::vector<IData> &, std::vector<IData>::iterator &);
		void _uu_TYA(std::vector<IData> &, std::vector<IData>::iterator &);
		void _uu_TAY(std::vector<IData> &, std::vector<IData>::iterator &);
		void _uu_TXS(std::vector<IData> &, std::vector<IData>::iterator &);
		void _uu_TSX(std::vector<IData> &, std::vector<IData>::iterator &);
		void _uu_NOP(std::vector<IData> &, std::vector<IData>::iterator &);
		void _uu_PHP(std::vector<IData> &, std::vector<IData>::iterator &);
		void _uu_PLP(std::vector<IData> &, std::vector<IData>::iterator &);
		void _uu_PHA(std::vector<IData> &, std::vector<IData>::iterator &);
		void _uu_PLA(std::vector<IData> &, std::vector<IData>::iterator &);

		void _uu_INX(std::vector<IData> &, std::vector<IData>::iterator &);
		void _uu_DEX(std::vector<IData> &, std::vector<IData>::iterator &);
		void _uu_INY(std::vector<IData> &, std::vector<IData>::iterator &);
		void _uu_DEY(std::vector<IData> &, std::vector<IData>::iterator &);

		void _uu_SEC(std::vector<IData> &, std::vector<IData>::iterator &);
		void _uu_CLC(std::vector<IData> &, std::vector<IData>::iterator &);
		void _uu_SED(std::vector<IData> &, std::vector<IData>::iterator &);
		void _uu_CLD(std::vector<IData> &, std::vector<IData>::iterator &);
		void _uu_SEV(std::vector<IData> &, std::vector<IData>::iterator &);
		void _uu_CLV(std::vector<IData> &, std::vector<IData>::iterator &);
		void _uu_SEI(std::vector<IData> &, std::vector<IData>::iterator &);
		void _uu_CLI(std::vector<IData> &, std::vector<IData>::iterator &);

		void _uu_IJMP(std::vector<IData> &, std::vector<IData>::iterator &);
		void _uu_JSR(std::vector<IData> &, std::vector<IData>::iterator &);

		void _uu_RTS(std::vector<IData> &, std::vector<IData>::iterator &);
		void _uu_RTI(std::vector<IData> &, std::vector<IData>::iterator &);

		void _uu_bra(std::vector<IData> &, std::vector<IData>::iterator &);
		void _uu_jump(std::vector<IData> &, std::vector<IData>::iterator &);
	};
}
