/*
	Part of the R2A03 Recompiler Project

	This file is the "meat" of RRP. It contains all the implementations of addressing mode callbacks and
	operation callbacks.
*/

#include <iostream>
#include "Translator.hxx"

namespace RRP
{
	//implemented
	llvm::Value * Translator::_mode_read_indirect_zeropage_indexedX(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		if (!_immRead)
		{
			if (++i == buf.end())
			{
				throw Exception(ExceptionCode::PREMATURE_EOF, std::get<1>(*i));
				//throw Translator::Exception::PREMATURE_EOF;
			}
			_imm = std::get<2>(*i);
			_immRead = true;
		}
		llvm::Value * fetchaddr = _irBuilder.CreateAdd(_irBuilder.CreateLoad(_X), llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), llvm::APInt(8, _imm)));
		llvm::Value * fetchaddrplus1 = _irBuilder.CreateAdd(fetchaddr, llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), llvm::APInt(8, 1)));

		llvm::Value * zextFetchAddr = _irBuilder.CreateZExt(fetchaddr, llvm::IntegerType::getInt16Ty(_llvmContext));
		llvm::Value * zextFetchAddrPlus1 = _irBuilder.CreateZExt(fetchaddrplus1, llvm::IntegerType::getInt16Ty(_llvmContext));
		llvm::Value * argsReadLo[] = { zextFetchAddr },
			*argsReadHi[] = { zextFetchAddrPlus1 };
		llvm::Value * addrLo = _irBuilder.CreateZExt(_irBuilder.CreateCall(_memRead, llvm::ArrayRef<llvm::Value *>(argsReadLo, 1)), llvm::IntegerType::getInt16Ty(_llvmContext));
		llvm::Value * addrHi = _irBuilder.CreateZExt(_irBuilder.CreateCall(_memRead, llvm::ArrayRef<llvm::Value *>(argsReadHi, 1)), llvm::IntegerType::getInt16Ty(_llvmContext));
		llvm::Value * addr = _irBuilder.CreateOr(_irBuilder.CreateShl(addrHi, 8), addrLo);
		memory_read(addr, _op);
		return _op;
	}

	//implemented
	llvm::Value * Translator::_mode_read_zeropage(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		if (!_immRead)
		{
			if (++i == buf.end())
			{
				throw Exception(ExceptionCode::PREMATURE_EOF, std::get<1>(*i));
			}
			_imm = std::get<2>(*i);
			_immRead = true;
		}
		llvm::Value * addr = llvm::ConstantInt::get(llvm::Type::getInt16Ty(_llvmContext), llvm::APInt(16, _imm));
		memory_read(addr, _op);
		return _op;
	}

	//implemented
	llvm::Value * Translator::_mode_read_immediate(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		if (!_immRead)
		{
			if (++i == buf.end())
			{
				throw Exception(ExceptionCode::PREMATURE_EOF, std::get<1>(*i));
			}
			_immRead = true;
		}
		byte_t argument = std::get<2>(*i);
		return llvm::ConstantInt::get(llvm::Type::getInt8Ty(_llvmContext), llvm::APInt(8, argument));
	}

	//implemented
	llvm::Value * Translator::_mode_read_accumulator(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		return _irBuilder.CreateLoad(_A);
	}

	//implemented
	llvm::Value * Translator::_mode_read_absolute(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		if (!_immRead)
		{
			if (++i == buf.end())
			{
				throw Exception(ExceptionCode::PREMATURE_EOF, std::get<1>(*i));
			}
			byte_t argument_lo = std::get<2>(*i);
			if (++i == buf.end())
			{
				throw Exception(ExceptionCode::PREMATURE_EOF, std::get<1>(*i));
			}
			byte_t argument_hi = std::get<2>(*i);
			_imm = makeword(argument_lo, argument_hi);
			_immRead = true;
		}
		llvm::Value * addr = llvm::ConstantInt::get(llvm::Type::getInt16Ty(_llvmContext), llvm::APInt(16, _imm));
		memory_read(addr, _op);
		return _op;
	}

	//implemented
	llvm::Value * Translator::_mode_read_indirect_zeropage_indexedY(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		if (!_immRead)
		{
			if (++i == buf.end())
			{
				throw Exception(ExceptionCode::PREMATURE_EOF, std::get<1>(*i));
			}
			_imm = std::get<2>(*i);
			_immRead = true;
		}
		llvm::Value * fetchaddr = llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), llvm::APInt(8, _imm));
		llvm::Value * fetchaddrplus1 = llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), llvm::APInt(8, _imm + 1));

		llvm::Value * zextFetchAddr = _irBuilder.CreateZExt(fetchaddr, llvm::IntegerType::getInt16Ty(_llvmContext));
		llvm::Value * zextFetchAddrPlus1 = _irBuilder.CreateZExt(fetchaddrplus1, llvm::IntegerType::getInt16Ty(_llvmContext));
		llvm::Value * argsReadLo[] = { zextFetchAddr },
			*argsReadHi[] = { zextFetchAddrPlus1 };
		llvm::Value * addrLo = _irBuilder.CreateZExt(_irBuilder.CreateCall(_memRead, llvm::ArrayRef<llvm::Value *>(argsReadLo, 1)), llvm::IntegerType::getInt16Ty(_llvmContext));
		llvm::Value * addrHi = _irBuilder.CreateZExt(_irBuilder.CreateCall(_memRead, llvm::ArrayRef<llvm::Value *>(argsReadHi, 1)), llvm::IntegerType::getInt16Ty(_llvmContext));
		llvm::Value * addr = _irBuilder.CreateAdd(_irBuilder.CreateOr(_irBuilder.CreateShl(addrHi, 8), addrLo), _irBuilder.CreateLoad(_Y));
		memory_read(addr, _op);
		return _op;
	}

	//implemented
	llvm::Value * Translator::_mode_read_zeropage_indexedX(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		if (!_immRead)
		{
			if (++i == buf.end())
			{
				throw Exception(ExceptionCode::PREMATURE_EOF, std::get<1>(*i));
			}
			_imm = std::get<2>(*i);
			_immRead = true;
		}
		llvm::Value * addr = _irBuilder.CreateAdd(_irBuilder.CreateLoad(_X), llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), llvm::APInt(8, _imm)));
		addr = _irBuilder.CreateZExt(addr, llvm::IntegerType::getInt16Ty(_llvmContext));
		memory_read(addr, _op);
		return _op;
	}

	//implemented
	llvm::Value * Translator::_mode_read_absolute_indexedX(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		if (!_immRead)
		{
			if (++i == buf.end())
			{
				throw Exception(ExceptionCode::PREMATURE_EOF, std::get<1>(*i));
			}
			byte_t argument_lo = std::get<2>(*i);
			if (++i == buf.end())
			{
				throw Exception(ExceptionCode::PREMATURE_EOF, std::get<1>(*i));
			}
			byte_t argument_hi = std::get<2>(*i);
			_imm = makeword(argument_lo, argument_hi);
			_immRead = true;
		}
		llvm::Value * addr = llvm::ConstantInt::get(llvm::Type::getInt16Ty(_llvmContext), llvm::APInt(16, _imm));
		addr = _irBuilder.CreateAdd(addr, _irBuilder.CreateZExt(_irBuilder.CreateLoad(_X), llvm::Type::getInt16Ty(_llvmContext)));
		memory_read(addr, _op);
		return _op;
	}

	//implemented
	llvm::Value * Translator::_mode_read_absolute_indexedY(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		if (!_immRead)
		{
			if (++i == buf.end())
			{
				throw Exception(ExceptionCode::PREMATURE_EOF, std::get<1>(*i));
			}
			byte_t argument_lo = std::get<2>(*i);
			if (++i == buf.end())
			{
				throw Exception(ExceptionCode::PREMATURE_EOF, std::get<1>(*i));
			}
			byte_t argument_hi = std::get<2>(*i);
			_imm = makeword(argument_lo, argument_hi);
			_immRead = true;
		}
		llvm::Value * addr = llvm::ConstantInt::get(llvm::Type::getInt16Ty(_llvmContext), llvm::APInt(16, _imm));
		addr = _irBuilder.CreateAdd(addr, _irBuilder.CreateZExt(_irBuilder.CreateLoad(_Y), llvm::Type::getInt16Ty(_llvmContext)));
		memory_read(addr, _op);
		return _op;
	}

	//implemented
	llvm::Value * Translator::_mode_write_indirect_zeropage_indexedX(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		if (!_immRead)
		{
			if (++i == buf.end())
			{
				throw Exception(ExceptionCode::PREMATURE_EOF, std::get<1>(*i));
			}
			_imm = std::get<2>(*i);
			_immRead = true;
		}
		llvm::Value * fetchaddr = _irBuilder.CreateAdd(_irBuilder.CreateLoad(_X), llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), llvm::APInt(8, _imm)));
		llvm::Value * fetchaddrplus1 = _irBuilder.CreateAdd(fetchaddr, llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), llvm::APInt(8, 1)));

		llvm::Value * zextFetchAddr = _irBuilder.CreateZExt(fetchaddr, llvm::IntegerType::getInt16Ty(_llvmContext));
		llvm::Value * zextFetchAddrPlus1 = _irBuilder.CreateZExt(fetchaddrplus1, llvm::IntegerType::getInt16Ty(_llvmContext));
		llvm::Value * argsReadLo[] = { zextFetchAddr },
			*argsReadHi[] = { zextFetchAddrPlus1 };
		llvm::Value * addrLo = _irBuilder.CreateZExt(_irBuilder.CreateCall(_memRead, llvm::ArrayRef<llvm::Value *>(argsReadLo, 1)), llvm::IntegerType::getInt16Ty(_llvmContext));
		llvm::Value * addrHi = _irBuilder.CreateZExt(_irBuilder.CreateCall(_memRead, llvm::ArrayRef<llvm::Value *>(argsReadHi, 1)), llvm::IntegerType::getInt16Ty(_llvmContext));
		llvm::Value * addr = _irBuilder.CreateOr(_irBuilder.CreateShl(addrHi, 8), addrLo);
		memory_write(addr, _op);
		return nullptr;
	}

	//implemented
	llvm::Value * Translator::_mode_write_zeropage(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		if (!_immRead)
		{
			if (++i == buf.end())
			{
				throw Exception(ExceptionCode::PREMATURE_EOF, std::get<1>(*i));
			}
			_imm = std::get<2>(*i);
			_immRead = true;
		}
		llvm::Value * addr = llvm::ConstantInt::get(llvm::Type::getInt16Ty(_llvmContext), llvm::APInt(16, _imm));
		memory_write(addr, _op);
		return nullptr;
	}

	//meaningless
	llvm::Value * Translator::_mode_write_immediate(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		return nullptr;
	}

	//implemented
	llvm::Value * Translator::_mode_write_accumulator(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		_irBuilder.CreateStore(_op, _A);
		return nullptr;
	}

	//implemented
	llvm::Value * Translator::_mode_write_absolute(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//std::cout << "write_abs" << std::endl;
		if (!_immRead)
		{
			if (++i == buf.end())
			{
				throw Exception(ExceptionCode::PREMATURE_EOF, std::get<1>(*i));
			}
			byte_t argument_lo = std::get<2>(*i);
			if (++i == buf.end())
			{
				throw Exception(ExceptionCode::PREMATURE_EOF, std::get<1>(*i));
			}
			byte_t argument_hi = std::get<2>(*i);
			_imm = makeword(argument_lo, argument_hi);
			_immRead = true;
		}
		llvm::Value * addr = llvm::ConstantInt::get(llvm::Type::getInt16Ty(_llvmContext), llvm::APInt(16, _imm));
		memory_write(addr, _op);
		return nullptr;
	}

	//implemented
	llvm::Value * Translator::_mode_write_indirect_zeropage_indexedY(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		if (!_immRead)
		{
			if (++i == buf.end())
			{
				throw Exception(ExceptionCode::PREMATURE_EOF, std::get<1>(*i));
			}
			_imm = std::get<2>(*i);
			_immRead = true;
		}
		llvm::Value * fetchaddr = llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), llvm::APInt(8, _imm));
		llvm::Value * fetchaddrplus1 = llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), llvm::APInt(8, _imm+1));

		llvm::Value * zextFetchAddr = _irBuilder.CreateZExt(fetchaddr, llvm::IntegerType::getInt16Ty(_llvmContext));
		llvm::Value * zextFetchAddrPlus1 = _irBuilder.CreateZExt(fetchaddrplus1, llvm::IntegerType::getInt16Ty(_llvmContext));
		llvm::Value * argsReadLo[] = { zextFetchAddr },
			*argsReadHi[] = { zextFetchAddrPlus1 };
		llvm::Value * addrLo = _irBuilder.CreateZExt(_irBuilder.CreateCall(_memRead, llvm::ArrayRef<llvm::Value *>(argsReadLo, 1)), llvm::IntegerType::getInt16Ty(_llvmContext));
		llvm::Value * addrHi = _irBuilder.CreateZExt(_irBuilder.CreateCall(_memRead, llvm::ArrayRef<llvm::Value *>(argsReadHi, 1)), llvm::IntegerType::getInt16Ty(_llvmContext));
		llvm::Value * addr = _irBuilder.CreateAdd(_irBuilder.CreateOr(_irBuilder.CreateShl(addrHi, 8), addrLo), _irBuilder.CreateLoad(_Y));
		memory_write(addr, _op);
		return nullptr;
	}

	//implemented
	llvm::Value * Translator::_mode_write_zeropage_indexedX(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		if (!_immRead)
		{
			if (++i == buf.end())
			{
				throw Exception(ExceptionCode::PREMATURE_EOF, std::get<1>(*i));
			}
			_imm = std::get<2>(*i);
			_immRead = true;
		}
		llvm::Value * addr = _irBuilder.CreateAdd(_irBuilder.CreateLoad(_X), llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), llvm::APInt(8, _imm)));
		addr = _irBuilder.CreateZExt(addr, llvm::IntegerType::getInt16Ty(_llvmContext));
		memory_write(addr, _op);
		return nullptr;
	}

	//implemented
	llvm::Value * Translator::_mode_write_absolute_indexedX(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//std::cout << "write_abs_indxxmode" << std::endl;
		if (!_immRead)
		{
			if (++i == buf.end())
			{
				throw Exception(ExceptionCode::PREMATURE_EOF, std::get<1>(*i));
			}
			byte_t argument_lo = std::get<2>(*i);
			if (++i == buf.end())
			{
				throw Exception(ExceptionCode::PREMATURE_EOF, std::get<1>(*i));
			}
			byte_t argument_hi = std::get<2>(*i);
			_imm = makeword(argument_lo, argument_hi);
			_immRead = true;
		}
		llvm::Value * addr = llvm::ConstantInt::get(llvm::Type::getInt16Ty(_llvmContext), llvm::APInt(16, _imm));
		addr = _irBuilder.CreateAdd(addr, _irBuilder.CreateZExt(_irBuilder.CreateLoad(_X), llvm::Type::getInt16Ty(_llvmContext)));
		memory_write(addr, _op);
		return nullptr;
	}

	//implemented
	llvm::Value * Translator::_mode_write_absolute_indexedY(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//std::cout << "abs_indxymode" << std::endl;
		if (!_immRead)
		{
			if (++i == buf.end())
			{
				throw Exception(ExceptionCode::PREMATURE_EOF, std::get<1>(*i));
			}
			byte_t argument_lo = std::get<2>(*i);
			if (++i == buf.end())
			{
				throw Exception(ExceptionCode::PREMATURE_EOF, std::get<1>(*i));
			}
			byte_t argument_hi = std::get<2>(*i);
			_imm = makeword(argument_lo, argument_hi);
			_immRead = true;
		}
		llvm::Value * addr = llvm::ConstantInt::get(llvm::Type::getInt16Ty(_llvmContext), llvm::APInt(16, _imm));
		addr = _irBuilder.CreateAdd(addr, _irBuilder.CreateZExt(_irBuilder.CreateLoad(_Y), llvm::Type::getInt16Ty(_llvmContext)));
		memory_write(addr, _op);
		return nullptr;
	}

	//Requires 8-bit value input
	inline void Translator::_build_flags(llvm::Value *&val)
	{
		val = llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), 0);
		for(size_t i=0;i<Flag::MAX;++i)
		{
			llvm::Value * current = _irBuilder.CreateLoad(_flag[i]);
			llvm::Value * current_zext = _irBuilder.CreateZExt(current, llvm::IntegerType::getInt8Ty(_llvmContext));
			llvm::Value * current_shl = _irBuilder.CreateShl(current_zext, i);
			val = _irBuilder.CreateOr(val, current_shl);
		}
	}

	//Requires 8-bit value input
	inline void Translator::_decompose_flags(llvm::Value *&val)
	{
		for(size_t i=0;i<Flag::MAX;++i)
		{
			llvm::Value * current_ashr = _irBuilder.CreateAShr(val, i);
			llvm::Value * current_trunc = _irBuilder.CreateTrunc(current_ashr, llvm::IntegerType::getInt1Ty(_llvmContext));
			_irBuilder.CreateStore(current_trunc, _flag[i]);
		}
	}

	inline void Translator::_set_z(llvm::Value *&val)
	{
		llvm::Value * zero = llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), 0);
		_irBuilder.CreateStore(_irBuilder.CreateICmpEQ(val, zero), _flag[Flag::Z]);
	}

	inline void Translator::_set_s(llvm::Value *&val)
	{
		llvm::Value * neg = llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), 0x80);
		_irBuilder.CreateStore(_irBuilder.CreateICmpEQ(neg, _irBuilder.CreateAnd(val, neg)), _flag[Flag::S]);
	}

	//implemented
	void Translator::_op_ORA(std::vector<IData> &buf, std::vector<IData>::iterator &i, AddressModeCallback &cb)
	{
		//std::cout << "ORA" << std::endl;
		_op = _irBuilder.CreateOr((std::get<0>(cb))(this, buf, i), _irBuilder.CreateLoad(_A));
		_set_z(_op);
		_set_s(_op);
		_irBuilder.CreateStore(_op, _A);
	}

	//implemented
	void Translator::_op_AND(std::vector<IData> &buf, std::vector<IData>::iterator &i, AddressModeCallback &cb)
	{
		//std::cout << "AND" << std::endl;
		_op = _irBuilder.CreateAnd((std::get<0>(cb))(this, buf, i), _irBuilder.CreateLoad(_A));
		_set_z(_op);
		_set_s(_op);
		_irBuilder.CreateStore(_op, _A);
	}

	//implemented
	void Translator::_op_EOR(std::vector<IData> &buf, std::vector<IData>::iterator &i, AddressModeCallback &cb)
	{
		//std::cout << "EOR" << std::endl;
		_op = _irBuilder.CreateXor((std::get<0>(cb))(this, buf, i), _irBuilder.CreateLoad(_A));
		_set_z(_op);
		_set_s(_op);
		_irBuilder.CreateStore(_op, _A);
	}

	//implemented
	void Translator::_op_ADC(std::vector<IData> &buf, std::vector<IData>::iterator &i, AddressModeCallback &cb)
	{
		//std::cout << "ADC" << std::endl;
		auto byte = (std::get<0>(cb))(this, buf, i);
		auto accum = _irBuilder.CreateLoad(_A);
		_op = _irBuilder.CreateZExt(byte, llvm::IntegerType::getInt16Ty(_llvmContext));
		_op = _irBuilder.CreateAdd(_op, _irBuilder.CreateZExt(accum, llvm::IntegerType::getInt16Ty(_llvmContext)));
		_op = _irBuilder.CreateAdd(_op, _irBuilder.CreateZExt(_irBuilder.CreateLoad(_flag[Flag::C]), llvm::IntegerType::getInt16Ty(_llvmContext)));
		auto u = _irBuilder.CreateICmpSGT(_op, llvm::ConstantInt::get(llvm::Type::getInt16Ty(_llvmContext), llvm::APInt(16, 0xFF)));
		_irBuilder.CreateStore(u, _flag[Flag::C]);
		_op = _irBuilder.CreateTrunc(_op, llvm::IntegerType::getInt8Ty(_llvmContext));
		auto neg = llvm::ConstantInt::get(llvm::Type::getInt8Ty(_llvmContext), llvm::APInt(8, 0x80));
		auto a = _irBuilder.CreateAnd(_irBuilder.CreateXor(accum, byte), neg);
		auto b = _irBuilder.CreateAnd(_irBuilder.CreateXor(accum, _op), neg);
		a = _irBuilder.CreateICmpNE(a, neg);
		b = _irBuilder.CreateICmpEQ(b, neg);
		_irBuilder.CreateStore(_irBuilder.CreateAnd(a, b), _flag[Flag::V]);
		_set_z(_op);
		_set_s(_op);
		_irBuilder.CreateStore(_op, _A);
	}

	//implemented
	void Translator::_op_STA(std::vector<IData> &buf, std::vector<IData>::iterator &i, AddressModeCallback &cb)
	{
		//std::cout << "STA" << std::endl;
		_op = _irBuilder.CreateLoad(_A, "");
		(std::get<1>(cb))(this, buf, i);	//execute write callback
	}

	//implemented
	void Translator::_op_LDA(std::vector<IData> &buf, std::vector<IData>::iterator &i, AddressModeCallback &cb)
	{
		//std::cout << "LDA" << std::endl;
		_op = (std::get<0>(cb))(this, buf, i);	//execute read callback
		_set_z(_op);
		_set_s(_op);
		_irBuilder.CreateStore(_op, _A);
	}

	//implemented
	void Translator::_op_CMP(std::vector<IData> &buf, std::vector<IData>::iterator &i, AddressModeCallback &cb)
	{
		//std::cout << "CMP" << std::endl;
		auto byte = (std::get<0>(cb))(this, buf, i);
		auto accum = _irBuilder.CreateLoad(_A);
		_op = _irBuilder.CreateZExt(byte, llvm::IntegerType::getInt16Ty(_llvmContext));
		_op = _irBuilder.CreateSub(_irBuilder.CreateZExt(accum, llvm::IntegerType::getInt16Ty(_llvmContext)), _op);
		//_op = _irBuilder.CreateSub(_op, _irBuilder.CreateZExt(_irBuilder.CreateNot(_irBuilder.CreateLoad(_flag[Flag::C])), llvm::IntegerType::getInt16Ty(_llvmContext)));
		auto u = _irBuilder.CreateICmpULT(_op, llvm::ConstantInt::get(llvm::Type::getInt16Ty(_llvmContext), llvm::APInt(16, 0x100)));
		_irBuilder.CreateStore(u, _flag[Flag::C]);
		_op = _irBuilder.CreateTrunc(_op, llvm::IntegerType::getInt8Ty(_llvmContext));
		auto neg = llvm::ConstantInt::get(llvm::Type::getInt8Ty(_llvmContext), llvm::APInt(8, 0x80));
		auto a = _irBuilder.CreateAnd(_irBuilder.CreateXor(accum, _op), neg);
		auto b = _irBuilder.CreateAnd(_irBuilder.CreateXor(accum, byte), neg);
		a = _irBuilder.CreateICmpEQ(a, neg);
		b = _irBuilder.CreateICmpEQ(b, neg);
		_irBuilder.CreateStore(_irBuilder.CreateAnd(a, b), _flag[Flag::V]);
		_set_z(_op);
		_set_s(_op);
	}

	//implemented
	void Translator::_op_SBC(std::vector<IData> &buf, std::vector<IData>::iterator &i, AddressModeCallback &cb)
	{
		//std::cout << "SBC" << std::endl;
		auto byte = (std::get<0>(cb))(this, buf, i);
		auto accum = _irBuilder.CreateLoad(_A);
		_op = _irBuilder.CreateZExt(byte, llvm::IntegerType::getInt16Ty(_llvmContext));
		_op = _irBuilder.CreateSub(_irBuilder.CreateZExt(accum, llvm::IntegerType::getInt16Ty(_llvmContext)), _op);
		_op = _irBuilder.CreateSub(_op, _irBuilder.CreateZExt(_irBuilder.CreateNot(_irBuilder.CreateLoad(_flag[Flag::C])), llvm::IntegerType::getInt16Ty(_llvmContext)));
		auto u = _irBuilder.CreateICmpULT(_op, llvm::ConstantInt::get(llvm::Type::getInt16Ty(_llvmContext), llvm::APInt(16, 0x100)));
		_irBuilder.CreateStore(u, _flag[Flag::C]);
		_op = _irBuilder.CreateTrunc(_op, llvm::IntegerType::getInt8Ty(_llvmContext));
		auto neg = llvm::ConstantInt::get(llvm::Type::getInt8Ty(_llvmContext), llvm::APInt(8, 0x80));
		auto a = _irBuilder.CreateAnd(_irBuilder.CreateXor(accum, _op), neg);
		auto b = _irBuilder.CreateAnd(_irBuilder.CreateXor(accum, byte), neg);
		a = _irBuilder.CreateICmpEQ(a, neg);
		b = _irBuilder.CreateICmpEQ(b, neg);
		_irBuilder.CreateStore(_irBuilder.CreateAnd(a, b), _flag[Flag::V]);
		_set_z(_op);
		_set_s(_op);
		_irBuilder.CreateStore(_op, _A);
	}

	//implemented
	void Translator::_op_ASL(std::vector<IData> &buf, std::vector<IData>::iterator &i, AddressModeCallback &cb)
	{
		//std::cout << "ASL" << std::endl;
		_op = (std::get<0>(cb))(this, buf, i);
		llvm::Value * neg = llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), 0x80);
		auto k = _irBuilder.CreateICmpEQ(neg, _irBuilder.CreateAnd(_op, neg));
		_op = _irBuilder.CreateShl(_op, 1);
		_irBuilder.CreateStore(k, _flag[Flag::C]);
		_set_z(_op);
		_set_s(_op);
		(std::get<1>(cb))(this, buf, i);
	}

	//implemented
	void Translator::_op_ROL(std::vector<IData> &buf, std::vector<IData>::iterator &i, AddressModeCallback &cb)
	{
		//std::cout << "ROL" << std::endl;
		_op = (std::get<0>(cb))(this, buf, i);
		llvm::Value * neg = llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), 0x80);
		auto k = _irBuilder.CreateICmpEQ(neg, _irBuilder.CreateAnd(_op, neg));
		_op = _irBuilder.CreateShl(_op, 1);
		_op = _irBuilder.CreateOr(_op, _irBuilder.CreateZExt(_irBuilder.CreateLoad(_flag[Flag::C]), llvm::IntegerType::getInt8Ty(_llvmContext)));
		_irBuilder.CreateStore(k, _flag[Flag::C]);
		_set_z(_op);
		_set_s(_op);
		(std::get<1>(cb))(this, buf, i);
	}

	//implemented
	void Translator::_op_LSR(std::vector<IData> &buf, std::vector<IData>::iterator &i, AddressModeCallback &cb)
	{
		//std::cout << "LSR" << std::endl;
		_op = (std::get<0>(cb))(this, buf, i);
		llvm::Value * neg = llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), 1);
		auto k = _irBuilder.CreateICmpEQ(neg, _irBuilder.CreateAnd(_op, neg));
		_op = _irBuilder.CreateAShr(_op, 1);
		_irBuilder.CreateStore(k, _flag[Flag::C]);
		_set_z(_op);
		_set_s(_op);
		(std::get<1>(cb))(this, buf, i);
	}

	//implemented
	void Translator::_op_ROR(std::vector<IData> &buf, std::vector<IData>::iterator &i, AddressModeCallback &cb)
	{
		//std::cout << "ROR" << std::endl;
		_op = (std::get<0>(cb))(this, buf, i);
		llvm::Value * neg = llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), 1);
		auto k = _irBuilder.CreateICmpEQ(neg, _irBuilder.CreateAnd(_op, neg));
		_op = _irBuilder.CreateAShr(_op, 1);
		auto u = _irBuilder.CreateZExt(_irBuilder.CreateLoad(_flag[Flag::C]), llvm::IntegerType::getInt8Ty(_llvmContext));
		_op = _irBuilder.CreateOr(_op, _irBuilder.CreateShl(u, 7));
		_irBuilder.CreateStore(k, _flag[Flag::C]);
		_set_z(_op);
		_set_s(_op);
		(std::get<1>(cb))(this, buf, i);
	}

	//implemented
	void Translator::_op_STX(std::vector<IData> &buf, std::vector<IData>::iterator &i, AddressModeCallback &cb)
	{
		//std::cout << "STX" << std::endl;
		_op = _irBuilder.CreateLoad(_X, "");
		(std::get<1>(cb))(this, buf, i);	//execute write callback
	}

	//implemented
	void Translator::_op_LDX(std::vector<IData> &buf, std::vector<IData>::iterator &i, AddressModeCallback &cb)
	{
		//std::cout << "LDX" << std::endl;
		_op = (std::get<0>(cb))(this, buf, i);	//execute read callback
		_set_z(_op);
		_set_s(_op);
		_irBuilder.CreateStore(_op, _X);
	}

	//implemented
	void Translator::_op_DEC(std::vector<IData> &buf, std::vector<IData>::iterator &i, AddressModeCallback &cb)
	{
		//std::cout << "DEC" << std::endl;
		_op = (std::get<0>(cb))(this, buf, i);	//execute read callback
		_op = _irBuilder.CreateSub(_op, llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), 1));
		_set_z(_op);
		_set_s(_op);
		(std::get<1>(cb))(this, buf, i);		//execute write callback
	}

	//implemented
	void Translator::_op_INC(std::vector<IData> &buf, std::vector<IData>::iterator &i, AddressModeCallback &cb)
	{
		//std::cout << "INC" << std::endl;
		_op = (std::get<0>(cb))(this, buf, i);	//execute read callback
		_op = _irBuilder.CreateAdd(_op, llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), 1));
		_set_z(_op);
		_set_s(_op);
		(std::get<1>(cb))(this, buf, i);		//execute write callback
	}

	//implemented
	void Translator::_op_BIT(std::vector<IData> &buf, std::vector<IData>::iterator &i, AddressModeCallback &cb)
	{
		//std::cout << "BIT" << std::endl;
		_op = _irBuilder.CreateAnd((std::get<0>(cb))(this, buf, i), _irBuilder.CreateLoad(_A));
		_set_z(_op);
		_set_s(_op);
	}

	//implemented
	void Translator::_op_STY(std::vector<IData> &buf, std::vector<IData>::iterator &i, AddressModeCallback &cb)
	{
		//std::cout << "STY" << std::endl;
		_op = _irBuilder.CreateLoad(_Y, "");
		(std::get<1>(cb))(this, buf, i);	//execute write callback
	}

	//implemented
	void Translator::_op_LDY(std::vector<IData> &buf, std::vector<IData>::iterator &i, AddressModeCallback &cb)
	{
		//std::cout << "LDY" << std::endl;
		_op = (std::get<0>(cb))(this, buf, i);	//execute read callback
		_set_z(_op);
		_set_s(_op);
		_irBuilder.CreateStore(_op, _Y);
	}

	//implemented
	void Translator::_op_CPY(std::vector<IData> &buf, std::vector<IData>::iterator &i, AddressModeCallback &cb)
	{
		//std::cout << "CPY" << std::endl;
		auto byte = (std::get<0>(cb))(this, buf, i);
		auto accum = _irBuilder.CreateLoad(_Y);
		_op = _irBuilder.CreateZExt(byte, llvm::IntegerType::getInt16Ty(_llvmContext));
		_op = _irBuilder.CreateSub(_irBuilder.CreateZExt(accum, llvm::IntegerType::getInt16Ty(_llvmContext)), _op);
		//_op = _irBuilder.CreateSub(_op, _irBuilder.CreateZExt(_irBuilder.CreateNot(_irBuilder.CreateLoad(_flag[Flag::C])), llvm::IntegerType::getInt16Ty(_llvmContext)));
		auto u = _irBuilder.CreateICmpULT(_op, llvm::ConstantInt::get(llvm::Type::getInt16Ty(_llvmContext), llvm::APInt(16, 0x100)));
		_irBuilder.CreateStore(u, _flag[Flag::C]);
		_op = _irBuilder.CreateTrunc(_op, llvm::IntegerType::getInt8Ty(_llvmContext));
		auto neg = llvm::ConstantInt::get(llvm::Type::getInt8Ty(_llvmContext), llvm::APInt(8, 0x80));
		auto a = _irBuilder.CreateAnd(_irBuilder.CreateXor(accum, _op), neg);
		auto b = _irBuilder.CreateAnd(_irBuilder.CreateXor(accum, byte), neg);
		a = _irBuilder.CreateICmpEQ(a, neg);
		b = _irBuilder.CreateICmpEQ(b, neg);
		_irBuilder.CreateStore(_irBuilder.CreateAnd(a, b), _flag[Flag::V]);
		_set_z(_op);
		_set_s(_op);
	}

	//implemented
	void Translator::_op_CPX(std::vector<IData> &buf, std::vector<IData>::iterator &i, AddressModeCallback &cb)
	{
		//std::cout << "CPX" << std::endl;
		auto byte = (std::get<0>(cb))(this, buf, i);
		auto accum = _irBuilder.CreateLoad(_X);
		_op = _irBuilder.CreateZExt(byte, llvm::IntegerType::getInt16Ty(_llvmContext));
		_op = _irBuilder.CreateSub(_irBuilder.CreateZExt(accum, llvm::IntegerType::getInt16Ty(_llvmContext)), _op);
		//_op = _irBuilder.CreateSub(_op, _irBuilder.CreateZExt(_irBuilder.CreateNot(_irBuilder.CreateLoad(_flag[Flag::C])), llvm::IntegerType::getInt16Ty(_llvmContext)));
		auto u = _irBuilder.CreateICmpULT(_op, llvm::ConstantInt::get(llvm::Type::getInt16Ty(_llvmContext), llvm::APInt(16, 0x100)));
		_irBuilder.CreateStore(u, _flag[Flag::C]);
		_op = _irBuilder.CreateTrunc(_op, llvm::IntegerType::getInt8Ty(_llvmContext));
		auto neg = llvm::ConstantInt::get(llvm::Type::getInt8Ty(_llvmContext), llvm::APInt(8, 0x80));
		auto a = _irBuilder.CreateAnd(_irBuilder.CreateXor(accum, _op), neg);
		auto b = _irBuilder.CreateAnd(_irBuilder.CreateXor(accum, byte), neg);
		a = _irBuilder.CreateICmpEQ(a, neg);
		b = _irBuilder.CreateICmpEQ(b, neg);
		_irBuilder.CreateStore(_irBuilder.CreateAnd(a, b), _flag[Flag::V]);
		_set_z(_op);
		_set_s(_op);
	}

	//implemented
	void Translator::_pp_mklb1(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//q is at addr of next function, k is a copy of q, begin has the orig
		std::vector<IData>::iterator q(i + 2), k(q), begin(i);
		offset_t byte = std::get<2>(*(++i));
		k += byte;

		//k now has addr of the destination
		std::stringstream toname("");
		toname << std::hex << (int)std::get<1>(*k);
		std::string tostr = "A" + toname.str();
		

		std::stringstream nextname("");
		nextname << std::hex << (int)std::get<1>(*q);
		std::string nextstr = "A" + nextname.str();

		std::get<0>(*k) = tostr;
		std::get<0>(*q) = nextstr;
		std::get<3>(*begin) = tostr;

		if(_bbMap.find(tostr) == _bbMap.end())
		{
			_bbMap[tostr] = llvm::BasicBlock::Create(_llvmContext, tostr);
		}
		if (_bbMap.find(nextstr) == _bbMap.end())
		{
			_bbMap[nextstr] = llvm::BasicBlock::Create(_llvmContext, nextstr);
		}
	}

	void Translator::_pp_mklb2(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		std::vector<IData>::iterator q(i + 3), begin(i);
		byte_t argument_lo = std::get<2>(*(++i));
		byte_t argument_hi = std::get<2>(*(++i));
		word_t addr = makeword(argument_lo, argument_hi);

		auto k = std::find_if(buf.begin(), buf.end(), [&addr](auto const &item)
		{
			return std::get<1>(item) == addr;
		});

		//k now has addr of the destination
		std::stringstream toname("");
		toname << std::hex << (int)std::get<1>(*k);
		std::string tostr = "A" + toname.str();


		std::stringstream nextname("");
		nextname << std::hex << (int)std::get<1>(*q);
		std::string nextstr = "A" + nextname.str();

		std::get<0>(*k) = tostr;
		std::get<0>(*q) = nextstr;
		std::get<3>(*begin) = tostr;

		if (_bbMap.find(tostr) == _bbMap.end())
		{
			_bbMap[tostr] = llvm::BasicBlock::Create(_llvmContext, tostr);
		}
		if (_bbMap.find(nextstr) == _bbMap.end())
		{
			_bbMap[nextstr] = llvm::BasicBlock::Create(_llvmContext, nextstr);
		}
	}

	void Translator::_uu_TXA(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//std::cout << "TXA" << std::endl;
		llvm::Value * src = _irBuilder.CreateLoad(_X);
		_set_s(src);
		_set_z(src);
		_irBuilder.CreateStore(src, _A);
	}

	void Translator::_uu_TAX(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//std::cout << "TAX" << std::endl;
		llvm::Value * src = _irBuilder.CreateLoad(_A);
		_set_s(src);
		_set_z(src);
		_irBuilder.CreateStore(src, _X);
	}

	void Translator::_uu_TYA(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//std::cout << "TYA" << std::endl;
		llvm::Value * src = _irBuilder.CreateLoad(_Y);
		_set_s(src);
		_set_z(src);
		_irBuilder.CreateStore(src, _A);
	}

	void Translator::_uu_TAY(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//std::cout << "TAY" << std::endl;
		llvm::Value * src = _irBuilder.CreateLoad(_A);
		_set_s(src);
		_set_z(src);
		_irBuilder.CreateStore(src, _Y);
	}

	void Translator::_uu_TXS(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//needs testing
		//std::cout << "TXS" << std::endl;
		llvm::Value * src = _irBuilder.CreateLoad(_X);
		_irBuilder.CreateStore(src, _S);
	}

	void Translator::_uu_TSX(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//needs testing
		//std::cout << "TSX" << std::endl;
		llvm::Value * src = _irBuilder.CreateLoad(_S);
		_set_s(src);
		_set_z(src);
		_irBuilder.CreateStore(src, _X);
	}

	void Translator::_uu_NOP(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//dummy instruction
		//std::cout << "NOP" << std::endl;
	}

	void Translator::_uu_PHP(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//needs testing
		//std::cout << "PHP" << std::endl;
		llvm::Value * accum;
		_build_flags(accum);
		llvm::Value * stack = _irBuilder.CreateLoad(_S);
		llvm::Value * stack_start = llvm::ConstantInt::get(llvm::IntegerType::getInt16Ty(_llvmContext), 0x100);
		llvm::Value * stack_offset_zext = _irBuilder.CreateZExt(stack, llvm::IntegerType::getInt16Ty(_llvmContext));
		llvm::Value * ptr = _irBuilder.CreateAdd(stack_start, stack_offset_zext);
		memory_write(accum, ptr);
		llvm::Value * decremented_sp = _irBuilder.CreateSub(stack, llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), 0x1));
		_irBuilder.CreateStore(decremented_sp, _S);
	}

	void Translator::_uu_PLP(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//needs testing
		//std::cout << "PLP" << std::endl;
		llvm::Value * stack = _irBuilder.CreateLoad(_S);
		llvm::Value * incremented_sp = _irBuilder.CreateAdd(stack, llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), 0x1));
		_irBuilder.CreateStore(incremented_sp, _S);
		llvm::Value * stack_start = llvm::ConstantInt::get(llvm::IntegerType::getInt16Ty(_llvmContext), 0x100);
		llvm::Value * stack_offset_zext = _irBuilder.CreateZExt(incremented_sp, llvm::IntegerType::getInt16Ty(_llvmContext));
		llvm::Value * ptr = _irBuilder.CreateAdd(stack_start, stack_offset_zext);
		llvm::Value * accum;
		memory_read(ptr, accum);
		_decompose_flags(accum);
	}

	void Translator::_uu_PHA(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//needs testing
		//std::cout << "PHA" << std::endl;
		llvm::Value * accum = _irBuilder.CreateLoad(_A);
		llvm::Value * stack = _irBuilder.CreateLoad(_S);
		llvm::Value * stack_start = llvm::ConstantInt::get(llvm::IntegerType::getInt16Ty(_llvmContext), 0x100);
		llvm::Value * stack_offset_zext = _irBuilder.CreateZExt(stack, llvm::IntegerType::getInt16Ty(_llvmContext));
		llvm::Value * ptr = _irBuilder.CreateAdd(stack_start, stack_offset_zext);
		memory_write(accum, ptr);
		llvm::Value * decremented_sp = _irBuilder.CreateSub(stack, llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), 0x1));
		_irBuilder.CreateStore(decremented_sp, _S);
	}

	void Translator::_uu_PLA(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//needs testing
		//std::cout << "PLA" << std::endl;
		llvm::Value * stack = _irBuilder.CreateLoad(_S);
		llvm::Value * incremented_sp = _irBuilder.CreateAdd(stack, llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), 0x1));
		_irBuilder.CreateStore(incremented_sp, _S);
		llvm::Value * stack_start = llvm::ConstantInt::get(llvm::IntegerType::getInt16Ty(_llvmContext), 0x100);
		llvm::Value * stack_offset_zext = _irBuilder.CreateZExt(incremented_sp, llvm::IntegerType::getInt16Ty(_llvmContext));
		llvm::Value * ptr = _irBuilder.CreateAdd(stack_start, stack_offset_zext);
		llvm::Value * accum;
		memory_read(ptr, accum);
		_set_s(accum);
		_set_z(accum);
		_irBuilder.CreateStore(accum, _A);
	}

	void Translator::_uu_INX(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//std::cout << "INX" << std::endl;
		llvm::Value * src = _irBuilder.CreateLoad(_X);
		src = _irBuilder.CreateAdd(src, llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), 1));
		_set_s(src);
		_set_z(src);
		_irBuilder.CreateStore(src, _X);
	}

	void Translator::_uu_DEX(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//std::cout << "DEX" << std::endl;
		llvm::Value * src = _irBuilder.CreateLoad(_X);
		src = _irBuilder.CreateSub(src, llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), 1));
		_set_s(src);
		_set_z(src);
		_irBuilder.CreateStore(src, _X);
	}

	void Translator::_uu_INY(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//std::cout << "INY" << std::endl;
		llvm::Value * src = _irBuilder.CreateLoad(_Y);
		src = _irBuilder.CreateAdd(src, llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), 1));
		_set_s(src);
		_set_z(src);
		_irBuilder.CreateStore(src, _Y);
	}

	void Translator::_uu_DEY(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//std::cout << "DEY" << std::endl;
		llvm::Value * src = _irBuilder.CreateLoad(_Y);
		src = _irBuilder.CreateSub(src, llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(_llvmContext), 1));
		_set_s(src);
		_set_z(src);
		_irBuilder.CreateStore(src, _Y);
	}

	void Translator::_uu_SEC(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//std::cout << "SEC" << std::endl;
		_irBuilder.CreateStore(llvm::ConstantInt::get(_llvmContext, llvm::APInt(1, 1)), _flag[Flag::C]);
	}

	void Translator::_uu_CLC(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//std::cout << "CLC" << std::endl;
		_irBuilder.CreateStore(llvm::ConstantInt::get(_llvmContext, llvm::APInt(1, 0)), _flag[Flag::C]);
	}

	void Translator::_uu_SED(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//std::cout << "SED" << std::endl;
		_irBuilder.CreateStore(llvm::ConstantInt::get(_llvmContext, llvm::APInt(1, 1)), _flag[Flag::D]);
	}

	void Translator::_uu_CLD(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//std::cout << "CLD" << std::endl;
		_irBuilder.CreateStore(llvm::ConstantInt::get(_llvmContext, llvm::APInt(1, 0)), _flag[Flag::D]);
	}

	void Translator::_uu_SEV(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//std::cout << "SEV" << std::endl;
		_irBuilder.CreateStore(llvm::ConstantInt::get(_llvmContext, llvm::APInt(1, 1)), _flag[Flag::V]);
	}

	void Translator::_uu_CLV(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//std::cout << "CLV" << std::endl;
		_irBuilder.CreateStore(llvm::ConstantInt::get(_llvmContext, llvm::APInt(1, 0)), _flag[Flag::V]);
	}

	void Translator::_uu_SEI(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//std::cout << "SEI" << std::endl;
		_irBuilder.CreateStore(llvm::ConstantInt::get(_llvmContext, llvm::APInt(1, 1)), _flag[Flag::I]);
	}

	void Translator::_uu_CLI(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//std::cout << "CLI" << std::endl;
		_irBuilder.CreateStore(llvm::ConstantInt::get(_llvmContext, llvm::APInt(1, 0)), _flag[Flag::I]);
	}

	void Translator::_uu_IJMP(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//std::cout << "IJMP" << std::endl;
		throw Exception(ExceptionCode::UNSUPPORTED_INSTRUCTION, std::get<1>(*i));
	}

	void Translator::_uu_JSR(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//std::cout << "JSR" << std::endl;
		throw Exception(ExceptionCode::UNSUPPORTED_INSTRUCTION, std::get<1>(*i));
	}

	void Translator::_uu_RTS(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//std::cout << "RTS" << std::endl;
		throw Exception(ExceptionCode::UNSUPPORTED_INSTRUCTION, std::get<1>(*i));
	}

	void Translator::_uu_RTI(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//std::cout << "RTI" << std::endl;
		throw Exception(ExceptionCode::UNSUPPORTED_INSTRUCTION, std::get<1>(*i));
	}

	void Translator::_uu_bra(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//std::cout << "branch: ";
		std::vector<IData>::iterator q(i);
		byte_t op = std::get<2>(*(i++));
		byte_t bit = (op >> 5) & 1;
		byte_t flag = (op >> 6) & 3;
		llvm::Value * toTest;
		switch (flag)
		{
		case 0:
			toTest = _flag[Flag::S];
			//std::cout << "on S=" << (int)bit << std::endl;
			break;
		case 1:
			toTest = _flag[Flag::V];
			//std::cout << "on V=" << (int)bit << std::endl;
			break;
		case 2:
			toTest = _flag[Flag::C];
			//std::cout << "on C=" << (int)bit << std::endl;
			break;
		case 3:
			toTest = _flag[Flag::Z];
			//std::cout << "on Z=" << (int)bit << std::endl;
			break;
		}
		llvm::Value * cond = _irBuilder.CreateICmpEQ(_irBuilder.CreateLoad(toTest), llvm::ConstantInt::get(_llvmContext, llvm::APInt(1, bit)));
		std::map<std::string, llvm::BasicBlock *>::iterator destBB = _bbMap.find(std::get<3>(*q));
		std::map<std::string, llvm::BasicBlock *>::iterator nextBB = _bbMap.find(std::get<0>(*(q+2)));
		if(destBB != _bbMap.end() && nextBB != _bbMap.end())
		{
			_main->getBasicBlockList().push_back(_curBB);
			_irBuilder.CreateCondBr(cond, destBB->second, nextBB->second);
			_curBB = nextBB->second;
			_irBuilder.SetInsertPoint(_curBB);
			_needsBr = false;
		}
		else
		{
			throw Exception(ExceptionCode::NONEXISTANT_BRANCH, std::get<1>(*i));
		}
	}

	void Translator::_uu_jump(std::vector<IData> &buf, std::vector<IData>::iterator &i)
	{
		//std::cout << "JMP" << std::endl;
		std::map<std::string, llvm::BasicBlock *>::iterator destBB = _bbMap.find(std::get<3>(*i));
		std::map<std::string, llvm::BasicBlock *>::iterator nextBB = _bbMap.find(std::get<0>(*(i + 3)));
		++++i;
		if (destBB != _bbMap.end() && nextBB != _bbMap.end())
		{
			_main->getBasicBlockList().push_back(_curBB);
			_irBuilder.CreateBr(destBB->second);
			_curBB = nextBB->second;
			_irBuilder.SetInsertPoint(_curBB);
			_needsBr = false;
		}
		else
		{
			throw Exception(ExceptionCode::NONEXISTANT_BRANCH, std::get<1>(*i));
		}
	}
}
