/*
	Part of the R2A03 Recompiler Project

	This file contains the static data used by the Translator object.
*/

#include "Translator.hxx"

namespace RRP {

	/*
		exceptText defines string variables describing the possible exceptions
	*/
	std::array<std::string, 5> const Translator::exceptText =
	{ {
			"Exception::UNKNOWN",
			"Exception::PREMATURE_EOF",
			"Exception::INVALID_ADDR_MODE",
			"Exception::NONEXISTANT_BRANCH",
			"Exception::UNSUPPORTED_INSTRUCTION"
	} };

	/*
		addrModeCb defines the addressing mode callback object pairs used during translation.
	*/
	std::array<std::array<Translator::AddressModeCallback, 8>, 4> const Translator::addrModeCb = 
	{ {
		{ {
			AddressModeCallback( &Translator::_mode_read_immediate, &Translator::_mode_write_immediate ),
			AddressModeCallback( &Translator::_mode_read_zeropage, &Translator::_mode_write_zeropage ),
			AddressModeCallback( nullptr, nullptr ),
			AddressModeCallback( &Translator::_mode_read_absolute, &Translator::_mode_write_absolute ),
			AddressModeCallback( nullptr, nullptr ),
			AddressModeCallback( &Translator::_mode_read_zeropage_indexedX, &Translator::_mode_write_zeropage_indexedX ),
			AddressModeCallback( nullptr, nullptr ),
			AddressModeCallback( &Translator::_mode_read_absolute_indexedX, &Translator::_mode_write_absolute_indexedX )
		} },
		{ {
			AddressModeCallback( &Translator::_mode_read_indirect_zeropage_indexedX, &Translator::_mode_write_indirect_zeropage_indexedX ),
			AddressModeCallback( &Translator::_mode_read_zeropage, &Translator::_mode_write_zeropage ),
			AddressModeCallback( &Translator::_mode_read_immediate, &Translator::_mode_write_immediate ),
			AddressModeCallback( &Translator::_mode_read_absolute, &Translator::_mode_write_absolute ),
			AddressModeCallback( &Translator::_mode_read_indirect_zeropage_indexedY, &Translator::_mode_write_indirect_zeropage_indexedY ),
			AddressModeCallback( &Translator::_mode_read_zeropage_indexedX, &Translator::_mode_write_zeropage_indexedX ),
			AddressModeCallback( &Translator::_mode_read_absolute_indexedY, &Translator::_mode_write_absolute_indexedY ),
			AddressModeCallback( &Translator::_mode_read_absolute_indexedX, &Translator::_mode_write_absolute_indexedX )
			
		} },
		{ {
			AddressModeCallback( &Translator::_mode_read_immediate, &Translator::_mode_write_immediate ),
			AddressModeCallback( &Translator::_mode_read_zeropage, &Translator::_mode_write_zeropage ),
			AddressModeCallback( &Translator::_mode_read_accumulator, &Translator::_mode_write_accumulator ),
			AddressModeCallback( &Translator::_mode_read_absolute, &Translator::_mode_write_absolute ),
			AddressModeCallback( nullptr, nullptr ),
			AddressModeCallback( &Translator::_mode_read_zeropage_indexedX, &Translator::_mode_write_zeropage_indexedX ),
			AddressModeCallback( nullptr, nullptr ),
			AddressModeCallback( &Translator::_mode_read_absolute_indexedX, &Translator::_mode_write_absolute_indexedX )
		} },
		{ {
			AddressModeCallback( nullptr, nullptr ),
			AddressModeCallback( nullptr, nullptr ),
			AddressModeCallback( nullptr, nullptr ),
			AddressModeCallback( nullptr, nullptr ),
			AddressModeCallback( nullptr, nullptr ),
			AddressModeCallback( nullptr, nullptr ),
			AddressModeCallback( nullptr, nullptr ),
			AddressModeCallback( nullptr, nullptr )
		} }
	} };

	/*
		opCb defines the operation callbacks used during translation.
	*/
	std::array<std::array<Translator::OperationCallback, 8>, 4> const Translator::opCb =
	{ {
		{ {
			nullptr,
			&Translator::_op_BIT,
			nullptr,
			nullptr,
			&Translator::_op_STY,
			&Translator::_op_LDY,
			&Translator::_op_CPY,
			&Translator::_op_CPX
		} },
		{ {
			&Translator::_op_ORA,
			&Translator::_op_AND,
			&Translator::_op_EOR,
			&Translator::_op_ADC,
			&Translator::_op_STA,
			&Translator::_op_LDA,
			&Translator::_op_CMP,
			&Translator::_op_SBC
		} },
		{ {
			&Translator::_op_ASL,
			&Translator::_op_ROL,
			&Translator::_op_LSR,
			&Translator::_op_ROR,
			&Translator::_op_STX,
			&Translator::_op_LDX,
			&Translator::_op_DEC,
			&Translator::_op_INC
		} }
	} };


	/*
		sizeArr defines the size of each of the possible 256 instructions, in bytes, including
		opcode plus possible immediate, address, and offset.
	*/
	std::array<byte_t, 0x100> const Translator::sizeArr =
	{ {
			//	00				01				02				03				04				05				06				07
			1,					1,				1,				1,				2,				2,				2,				1,		
			//	08				09				0A				0B				0C				0D				0E				0F
			1,					2,				1,				1,				3,				3,				3,				1,		
			//	10				11				12				13				14				15				16				17
			2,					1,				1,				1,				2,				2,				2,				1,		
			//	18				19				1A				1B				1C				1D				1E				1F
			1,					3,				1,				1,				3,				3,				3,				1,		
			//	20				21				22				23				24				25				26				27
			3,					1,				1,				1,				2,				2,				2,				1,		
			//	28				29				2A				2B				2C				2D				2E				2F
			1,					2,				1,				1,				3,				3,				3,				1,		
			//	30				31				32				33				34				35				36				37
			2,					1,				1,				1,				2,				2,				2,				1,		
			//	38				39				3A				3B				3C				3D				3E				3F
			1,					3,				1,				1,				3,				3,				3,				1,		
			//	40				41				42				43				44				45				46				47
			1,					1,				1,				1,				2,				2,				2,				1,		
			//	48				49				4A				4B				4C				4D				4E				4F
			1,					2,				1,				1,				3,				3,				3,				1,		
			//	50				51				52				53				54				55				56				57
			2,					1,				1,				1,				2,				2,				2,				1,		
			//	58				59				5A				5B				5C				5D				5E				5F
			1,					3,				1,				1,				3,				3,				3,				1,		
			//	60				61				62				63				64				65				66				67
			1,					1,				1,				1,				2,				2,				2,				1,		
			//	68				69				6A				6B				6C				6D				6E				6F
			1,					2,				1,				1,				3,				3,				3,				1,		
			//	70				71				72				73				74				75				76				77
			2,					1,				1,				1,				2,				2,				2,				1,		
			//	78				79				7A				7B				7C				7D				7E				7F
			1,					3,				1,				1,				3,				3,				3,				1,		
			//	80				81				82				83				84				85				86				87
			1,					1,				1,				1,				2,				2,				2,				1,		
			//	88				89				8A				8B				8C				8D				8E				8F
			1,					1,				1,				1,				3,				3,				3,				1,		
			//	90				91				92				93				94				95				96				97
			2,					1,				1,				1,				2,				2,				2,				1,		
			//	98				99				9A				9B				9C				9D				9E				9F
			1,					3,				1,				1,				3,				3,				3,				1,		
			//	A0				A1				A2				A3				A4				A5				A6				A7
			2,					1,				2,				1,				2,				2,				2,				1,		
			//	A8				A9				AA				AB				AC				AD				AE				AF
			1,					2,				1,				1,				3,				3,				3,				1,		
			//	B0				B1				B2				B3				B4				B5				B6				B7
			2,					1,				1,				1,				2,				2,				2,				1,		
			//	B8				B9				BA				BB				BC				BD				BE				BF
			1,					3,				1,				1,				3,				3,				3,				1,		
			//	C0				C1				C2				C3				C4				C5				C6				C7
			2,					1,				1,				1,				2,				2,				2,				1,		
			//	C8				C9				CA				CB				CC				CD				CE				CF
			1,					2,				1,				1,				3,				3,				3,				1,		
			//	D0				D1				D2				D3				D4				D5				D6				D7
			2,					1,				1,				1,				2,				2,				2,				1,		
			//	D8				D9				DA				DB				DC				DD				DE				DF
			1,					3,				1,				1,				3,				3,				3,				1,		
			//	E0				E1				E2				E3				E4				E5				E6				E7
			1,					1,				1,				1,				2,				2,				2,				1,		
			//	E8				E9				EA				EB				EC				ED				EE				EF
			1,					2,				1,				1,				3,				3,				3,				1,		
			//	F0				F1				F2				F3				F4				F5				F6				F7
			2,					1,				1,				1,				2,				2,				2,				1,		
			//	F8				F9				FA				FB				FC				FD				FE				FF
			1,					3,				1,				1,				3,				3,				3,				1
		} };

	/*
		ppCb defines callbacks to be invoked for each instruction by the preprocessor.
		Jump and branch instructions require the creation of labels.
	*/
	std::array<Translator::PreprocessCallback, 0x100> const Translator::ppCb =
	{ {
		//	00							01				02				03				04							05				06				07
			nullptr,					nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	08							09				0A				0B				0C							0D				0E				0F
			nullptr,					nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	10							11				12				13				14							15				16				17
			&Translator::_pp_mklb1,		nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	18							19				1A				1B				1C							1D				1E				1F
			nullptr,					nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	20							21				22				23				24							25				26				27
			&Translator::_pp_mklb2,		nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	28							29				2A				2B				2C							2D				2E				2F
			nullptr,					nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	30							31				32				33				34							35				36				37
			&Translator::_pp_mklb1,		nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	38							39				3A				3B				3C							3D				3E				3F
			nullptr,					nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	40							41				42				43				44							45				46				47
			nullptr,					nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	48							49				4A				4B				4C							4D				4E				4F
			nullptr,					nullptr,		nullptr,		nullptr,		&Translator::_pp_mklb2,		nullptr,		nullptr,		nullptr,
		//	50							51				52				53				54							55				56				57
			&Translator::_pp_mklb1,		nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	58							59				5A				5B				5C							5D				5E				5F
			nullptr,					nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	60							61				62				63				64							65				66				67
			&Translator::_pp_mklb0,		nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	68							69				6A				6B				6C							6D				6E				6F
			nullptr,					nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	70							71				72				73				74							75				76				77
			&Translator::_pp_mklb1,		nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	78							79				7A				7B				7C							7D				7E				7F
			nullptr,					nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	80							81				82				83				84							85				86				87
			nullptr,					nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	88							89				8A				8B				8C							8D				8E				8F
			nullptr,					nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	90							91				92				93				94							95				96				97
			&Translator::_pp_mklb1,		nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	98							99				9A				9B				9C							9D				9E				9F
			nullptr,					nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	A0							A1				A2				A3				A4							A5				A6				A7
			nullptr,					nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	A8							A9				AA				AB				AC							AD				AE				AF
			nullptr,					nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	B0							B1				B2				B3				B4							B5				B6				B7
			&Translator::_pp_mklb1,		nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	B8							B9				BA				BB				BC							BD				BE				BF
			nullptr,					nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	C0							C1				C2				C3				C4							C5				C6				C7
			nullptr,					nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	C8							C9				CA				CB				CC							CD				CE				CF
			nullptr,					nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	D0							D1				D2				D3				D4							D5				D6				D7
			&Translator::_pp_mklb1,		nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	D8							D9				DA				DB				DC							DD				DE				DF
			nullptr,					nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	E0							E1				E2				E3				E4							E5				E6				E7
			nullptr,					nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	E8							E9				EA				EB				EC							ED				EE				EF
			nullptr,					nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	F0							F1				F2				F3				F4							F5				F6				F7
			&Translator::_pp_mklb1,		nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
		//	F8							F9				FA				FB				FC							FD				FE				FF
			nullptr,					nullptr,		nullptr,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr
	} };

	/*
		"Unusual" instruction operation callbacks. These callbacks are invoked for opcodes which don't follow
		the standard form. (i.e., they don't have a corresponding entry in opCb)
	*/
	std::array<Translator::ImplicitModeCallback, 0x100> const Translator::uuCb =
	{ {
			//	00						01				02							03				04							05				06				07
			nullptr,					nullptr,		nullptr,					nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	08						09				0A							0B				0C							0D				0E				0F
			&Translator::_uu_PHP,		nullptr,		nullptr,					nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	10						11				12							13				14							15				16				17
			&Translator::_uu_bra,		nullptr,		nullptr,					nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	18						19				1A							1B				1C							1D				1E				1F
			&Translator::_uu_CLC,		nullptr,		nullptr,					nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	20						21				22							23				24							25				26				27
			&Translator::_uu_JSR,		nullptr,		nullptr,					nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	28						29				2A							2B				2C							2D				2E				2F
			&Translator::_uu_PLP,		nullptr,		nullptr,					nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	30						31				32							33				34							35				36				37
			&Translator::_uu_bra,		nullptr,		nullptr,					nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	38						39				3A							3B				3C							3D				3E				3F
			&Translator::_uu_SEC,		nullptr,		nullptr,					nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	40						41				42							43				44							45				46				47
			&Translator::_uu_RTI,		nullptr,		nullptr,					nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	48						49				4A							4B				4C							4D				4E				4F
			&Translator::_uu_PHA,		nullptr,		nullptr,					nullptr,		&Translator::_uu_jump,		nullptr,		nullptr,		nullptr,
			//	50						51				52							53				54							55				56				57
			&Translator::_uu_bra,		nullptr,		nullptr,					nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	58						59				5A							5B				5C							5D				5E				5F
			&Translator::_uu_CLI,		nullptr,		nullptr,					nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	60						61				62							63				64							65				66				67
			&Translator::_uu_RTS,		nullptr,		nullptr,					nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	68						69				6A							6B				6C							6D				6E				6F
			&Translator::_uu_PLA,		nullptr,		nullptr,					nullptr,		&Translator::_uu_IJMP,		nullptr,		nullptr,		nullptr,
			//	70						71				72							73				74							75				76				77
			&Translator::_uu_bra,		nullptr,		nullptr,					nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	78						79				7A							7B				7C							7D				7E				7F
			&Translator::_uu_SEI,		nullptr,		nullptr,					nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	80						81				82							83				84							85				86				87
			nullptr,					nullptr,		nullptr,					nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	88						89				8A							8B				8C							8D				8E				8F
			&Translator::_uu_DEY,		nullptr,		&Translator::_uu_TXA,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	90						91				92							93				94							95				96				97
			&Translator::_uu_bra,		nullptr,		nullptr,					nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	98						99				9A							9B				9C							9D				9E				9F
			&Translator::_uu_TYA,		nullptr,		&Translator::_uu_TXS,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	A0						A1				A2							A3				A4							A5				A6				A7
			nullptr,					nullptr,		nullptr,					nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	A8						A9				AA							AB				AC							AD				AE				AF
			&Translator::_uu_TAY,		nullptr,		&Translator::_uu_TAX,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	B0						B1				B2							B3				B4							B5				B6				B7
			&Translator::_uu_bra,		nullptr,		nullptr,					nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	B8						B9				BA							BB				BC							BD				BE				BF
			&Translator::_uu_CLV,		nullptr,		&Translator::_uu_TSX,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	C0						C1				C2							C3				C4							C5				C6				C7
			nullptr,					nullptr,		nullptr,					nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	C8						C9				CA							CB				CC							CD				CE				CF
			&Translator::_uu_INY,		nullptr,		&Translator::_uu_DEX,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	D0						D1				D2							D3				D4							D5				D6				D7
			&Translator::_uu_bra,		nullptr,		nullptr,					nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	D8						D9				DA							DB				DC							DD				DE				DF
			&Translator::_uu_CLD,		nullptr,		nullptr,					nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	E0						E1				E2							E3				E4							E5				E6				E7
			nullptr,					nullptr,		nullptr,					nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	E8						E9				EA							EB				EC							ED				EE				EF
			&Translator::_uu_INX,		nullptr,		&Translator::_uu_NOP,		nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	F0						F1				F2							F3				F4							F5				F6				F7
			&Translator::_uu_bra,		nullptr,		nullptr,					nullptr,		nullptr,					nullptr,		nullptr,		nullptr,
			//	F8						F9				FA							FB				FC							FD				FE				FF
			&Translator::_uu_SED,		nullptr,		nullptr,					nullptr,		nullptr,					nullptr,		nullptr,		nullptr
		} };
}
