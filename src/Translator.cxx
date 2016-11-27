/*
	Part of the R2A03 Recompiler Project
*/

#include <iostream>
#include "Translator.hxx"

namespace RRP
{

	/*
		Translator class constructor
	*/
	Translator::Translator(std::string const &name) : _llvmContext(),
		_irBuilder(_llvmContext),
		_mod(std::make_unique<llvm::Module>(name, _llvmContext)),
		_main(llvm::cast<llvm::Function>(_mod.get()->getOrInsertFunction("main",
			llvm::Type::getInt32Ty(_llvmContext),
			nullptr))),
		_curBB(llvm::BasicBlock::Create(_llvmContext, "ep")),
		_bbMap()
	{
		
		_irBuilder.SetInsertPoint(_curBB);

		
		_A = _irBuilder.CreateAlloca(llvm::IntegerType::getInt8Ty(_llvmContext), nullptr, "A");
		_X = _irBuilder.CreateAlloca(llvm::IntegerType::getInt8Ty(_llvmContext), nullptr, "X");
		_Y = _irBuilder.CreateAlloca(llvm::IntegerType::getInt8Ty(_llvmContext), nullptr, "Y");
		_S = _irBuilder.CreateAlloca(llvm::IntegerType::getInt8Ty(_llvmContext), nullptr, "S");
		_op = nullptr;

		llvm::Type * argsWrite[] = { llvm::IntegerType::getInt16Ty(_llvmContext), llvm::IntegerType::getInt8Ty(_llvmContext) };
		llvm::Type * argsRead[] = { llvm::IntegerType::getInt16Ty(_llvmContext) };
		llvm::Type * argsExit[] = { llvm::IntegerType::getInt32Ty(_llvmContext) };

		auto writeType = llvm::FunctionType::get(llvm::FunctionType::getVoidTy(_llvmContext), llvm::ArrayRef<llvm::Type *>(argsWrite, 2), false);
		auto readType = llvm::FunctionType::get(llvm::FunctionType::getInt8Ty(_llvmContext), llvm::ArrayRef<llvm::Type *>(argsRead, 1), false);
		auto exitType = llvm::FunctionType::get(llvm::FunctionType::getVoidTy(_llvmContext), llvm::ArrayRef<llvm::Type *>(argsExit, 1), false);

		_memWrite = llvm::Function::Create(writeType, llvm::Function::ExternalLinkage, "rrpMemWrite", _mod.get());
		_memRead = llvm::Function::Create(readType, llvm::Function::ExternalLinkage, "rrpMemRead", _mod.get());
		_exit = llvm::Function::Create(exitType, llvm::Function::ExternalLinkage, "rrpExit", _mod.get());
		
		_memWrite->setCallingConv(llvm::CallingConv::C);
		_memRead->setCallingConv(llvm::CallingConv::C);
		_exit->setCallingConv(llvm::CallingConv::C);

		for(llvm::Value *&v : _flag)
		{
			v = _irBuilder.CreateAlloca(llvm::Type::getInt1Ty(_llvmContext), nullptr, "");
		}

		//initialize the stack pointer
		_irBuilder.CreateStore(llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), 0xFF), _S);

		_bbMap["A8000"] = _curBB;
	}

	constexpr word_t Translator::starting_addr(void)
	{
		return 0x8000;
	}

	void Translator::memory_read(llvm::Value *&addr, llvm::Value *&val)
	{
		llvm::Value * argsRead[] = { addr };
		val = _irBuilder.CreateCall(_memRead, llvm::ArrayRef<llvm::Value *>(argsRead, 1));
	}

	void Translator::memory_write(llvm::Value *&addr, llvm::Value *&val)
	{
		llvm::Value * argsWrite[] = { addr, val };
		_irBuilder.CreateCall(_memWrite, llvm::ArrayRef<llvm::Value *>(argsWrite, 2), "");
	}

	void Translator::output_bitcode(std::string const &name)
	{
		llvm::legacy::FunctionPassManager fpm(_mod.get());
		fpm.add(llvm::createPromoteMemoryToRegisterPass());
		fpm.add(llvm::createDeadCodeEliminationPass());
		fpm.run(*_main);

		//llvm::verifyFunction(*_main);

		//_mod.get()->dump();

		std::error_code ec;
		llvm::raw_fd_ostream os(name, ec, llvm::sys::fs::OpenFlags::F_None);
		llvm::WriteBitcodeToFile(_mod.get(), os);
		os.flush();
	}

	void Translator::process_file(std::ifstream &s)
	{
		std::vector<IData> buffer;
		{
			word_t addr = starting_addr();
			std::for_each((std::istreambuf_iterator<char>(s)), (std::istreambuf_iterator<char>()), [&buffer, &addr](byte_t c)
			{
				buffer.push_back(IData("", addr, c, ""));
				++addr;
			});
		}

		buffer.push_back(IData("", starting_addr()+buffer.size()+1, 0xEA, ""));

		//first pass - preprocessing
		//basically, create the labels that branches will use
		std::vector<IData>::iterator i = buffer.begin();
		while (i != buffer.end())
		{
			byte_t byte = std::get<2>(*i);
			Translator::PreprocessCallback pp = Translator::ppCb[byte];
			if(pp != nullptr)
			{
				pp(this, buffer, i);
				++i;
			}
			else
			{
				i += Translator::sizeArr[byte];
			}
		}

		//second pass - generate the LLVM
		i = buffer.begin();
		while(i != buffer.end())
		{
			//std::cout << "Beginning instruction" << std::endl;
			if(_needsBr && std::get<0>(*i) != "")
			{
				std::string bbname = std::get<0>(*i);
				std::map<std::string, llvm::BasicBlock *>::iterator v = _bbMap.find(bbname);
				if(v == _bbMap.end())
				{
					throw Exception(ExceptionCode::UNKNOWN, std::get<1>(*i));
				}
				_irBuilder.CreateBr(v->second);
				_main->getBasicBlockList().push_back(_curBB);
				_curBB = v->second;
				_irBuilder.SetInsertPoint(_curBB);
			}
			_needsBr = true;
			byte_t instruction = std::get<2>(*i);
			if (uuCb[instruction] == nullptr)
			{
				byte_t mode = instruction & 3;
				byte_t opcode = instruction >> 5;
				byte_t addrmode = (instruction >> 2) & 7;
				Translator::AddressModeCallback addrModeFunc = Translator::addrModeCb[mode][addrmode];
				Translator::OperationCallback opFunc = Translator::opCb[mode][opcode];
				_immRead = false;
				opFunc(this, buffer, i, addrModeFunc);
				++i;
			}
			else
			{
				uuCb[instruction](this, buffer, i);
				++i;
			}
		}
		llvm::Value * argsExit[] = { llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(_llvmContext), llvm::APInt(32, 0)) };
		_main->getBasicBlockList().push_back(_curBB);
		_irBuilder.CreateCall(_exit, argsExit);
		_irBuilder.CreateRet(llvm::ConstantInt::get(_llvmContext, llvm::APInt(32, 0)));
	}
}
