#include "CPU.h"
#include <Utilities/OpUtilities.h>
#include <iostream>

namespace ePugStation
{
	CPU::CPU() :
		m_ip(BIOS_ROM_LOGICAL),
		m_sr(0),
		m_loadPair({ 0, 0 })
	{
		m_nextInstruction.op = 0x0;

		m_registers.fill(0xdeadbeef);
		m_outputRegisters.fill(0xdeadbeef);
		m_registers[0] = 0; // Constant R0
	}

	void CPU::runNextInstruction()
	{
		Instruction currentInstruction(m_nextInstruction);
		m_nextInstruction = Instruction(load32(m_ip));
		// Point IP to next instruction
		std::cout << "m_ip : " + std::to_string(m_ip) + "\n";
		m_ip += 4;
		setReg(m_loadPair);
		m_loadPair = { 0,0 };
		decodeAndExecute(currentInstruction.op);
		m_registers = m_outputRegisters;
	}

	uint8_t CPU::load8(uint32_t address) const
	{
		return m_interconnect.load8(address);
	}

	void CPU::store8(uint32_t address, uint8_t value)
	{
		m_interconnect.store8(address, value);
	}

	uint16_t CPU::load16(uint32_t address) const
	{
		return m_interconnect.load16(address);
	}

	void CPU::store16(uint32_t address, uint16_t value)
	{
		m_interconnect.store16(address, value);
	}

	uint32_t CPU::load32(uint32_t address) const
	{
		return m_interconnect.load32(address);
	}

	void CPU::store32(uint32_t address, uint32_t value)
	{
		m_interconnect.store32(address, value);
	}

	void CPU::decodeAndExecute(uint32_t instruction)
	{
		Instruction formatedInstruction;
		formatedInstruction.op = instruction;

		matchOp(formatedInstruction);
	}

	void CPU::matchOp(Instruction instruction)
	{
		switch (instruction.function)
		{
			// Sub Op
		case 0b000000: matchSubOp(instruction); break;

			// Jump
		case 0b000010: opJ(instruction); break;
		case 0b000011: opJAL(instruction); break;

			// Add
		case 0b001000: opADDI(instruction); break;
		case 0b001001: opADDIU(instruction); break;

			// And
		case 0b001100: opANDI(instruction); break;

			// Branch
		case 0b000100: opBEQ(instruction); break;
		case 0b000101: opBNE(instruction); break;
		case 0b000001: matchSubBranchOp(instruction); break;
		case 0b000111: opBGTZ(instruction); break;
		case 0b000110: opBLEZ(instruction); break;

			// Load
		case 0b001111: opLUI(instruction); break;
		case 0b100000: opLB(instruction); break;
		case 0b100100: opLBU(instruction); break;
		case 0b100001: opLH(instruction); break;
		case 0b100011: opLW(instruction); break;

			// OR
		case 0b001101: opORI(instruction); break;

			// Store
		case 0b101000: opSB(instruction); break;
		case 0b101001: opSH(instruction); break;
		case 0b101011: opSW(instruction); break;

			// Set
		case 0b001010: opSLTI(instruction); break;
		case 0b001011: opSLTIU(instruction); break;

			// XOR
		case 0b001110: opXORI(instruction); break;

			// Cop
		case 0b010000: opCop0(instruction); break;

		default:
			throw std::runtime_error("Instruction function not implemented with function : " + std::to_string(instruction.function));
		}
	}

	void CPU::matchSubBranchOp(Instruction instruction)
	{

		switch (instruction.t)
		{
		case 0b00001: opBGEZ(instruction); break;
		case 0b10001: opBGEZAL(instruction); break;
		case 0b00000: opBLTZ(instruction); break;
		case 0b10000: opBLTZAL(instruction); break;
		default:
			throw std::runtime_error("Unsupported t value at address : " + std::to_string(instruction.function));
		}

	}

	void CPU::matchSubOp(Instruction instruction)
	{
		switch (instruction.SubOperation.sub)
		{
			// Add
		case 0b100000: opADD(instruction); break;
		case 0b100001: opADDU(instruction); break;

			// And
		case 0b100100: opAND(instruction); break;

			// Div
		case 0b011010: opDIV(instruction); break;
		case 0b011011: opDIVU(instruction); break;

			// Jump
		case 0b001000: opJR(instruction); break;

			// Move
		case 0b010000: opMFHI(instruction); break;
		case 0b010010: opMFLO(instruction); break;

			// Mult
		case 0b011000: opMULT(instruction); break;
		case 0b011001: opMULTU(instruction); break;

			// Or
		case 0b100101: opOR(instruction); break;

			// Shift Left
		case 0b000000: opSLL(instruction);  break;
		case 0b000100: opSLLV(instruction); break;

			// Set
		case 0b101010: opSLT(instruction); break;
		case 0b101011: opSLTU(instruction);  break;

			// Shift right
		case 0b000011: opSRA(instruction);  break;
		case 0b000010: opSRL(instruction);  break;
		case 0b000110: opSRLV(instruction);  break;

			// Sub
		case 0b100010: opSUB(instruction); break;
		case 0b100011: opSUBU(instruction); break;

			// XOR
		case 0b100110: opXOR(instruction); break;

		default:
			throw std::runtime_error("SubOperation not implemented...");
		}
	}

	void CPU::setReg(uint32_t index, uint32_t value)
	{
		m_outputRegisters[index] = value;
		m_outputRegisters[0] = 0;
	}

	void CPU::setReg(std::pair<uint32_t, uint32_t> setRegPair)
	{
		m_outputRegisters[setRegPair.first] = setRegPair.second;
		m_outputRegisters[0] = 0;
	}

	void CPU::branch(uint32_t offset)
	{
		m_ip -= 4;
		m_ip += (offset << 2);
	}

	void CPU::opCop0(Instruction instruction)
	{
		auto regValue = m_registers[instruction.t];
		switch (instruction.s)
		{
		case 0b00100:
			// To be used when Cop0 registers are implemented
			//auto copRegIndex = instruction.SubOperation.d;
			switch (instruction.SubOperation.d)
			{
			case 3:
			case 5:
			case 6:
			case 7:
			case 9:
			case 11:
				if (regValue != 0)
					throw std::runtime_error("Unhandled write to cop0r");
				break;
			case 12:
				m_sr = regValue;
				break;
			case 13:
				if (regValue != 0)
					throw std::runtime_error("Unhandled write to CAUSE register!");
				break;
			default:
				throw std::runtime_error("Unhandled cop Reg value...");
			}
			break;
		default:
			throw std::runtime_error("Unhandled cop0 instruction...");
		}
	}

	void CPU::opSLT(Instruction instruction)
	{
		setReg(instruction.SubOperation.d, m_registers[instruction.s] < m_registers[instruction.t]);
	}

	void CPU::opSLTU(Instruction instruction)
	{
		setReg(instruction.SubOperation.d, m_registers[instruction.s] < m_registers[instruction.t]);
	}

	void CPU::opSLTI(Instruction instruction)
	{
		setReg(instruction.t, m_registers[instruction.s] < instruction.imm);
	}

	void CPU::opSLTIU(Instruction instruction)
	{
		setReg(instruction.t, m_registers[instruction.s] < instruction.imm);
	}

	void CPU::opSRA(Instruction instruction)
	{
		// For arithmetic shift : Signed integer
		int32_t signedValue = static_cast<int32_t>(m_registers[instruction.t]);

		int32_t test = -10; // TODO: Remove this
		test >> 10;

		// Here need to keep signed bit
		setReg(instruction.SubOperation.d, signedValue >> instruction.SubOperation.h);
	}

	void CPU::opSRL(Instruction instruction)
	{
		// For logical shift : Unsigned integer
		setReg(instruction.SubOperation.d, m_registers[instruction.t] >> instruction.SubOperation.h);
	}

	void CPU::opSRLV(Instruction instruction)
	{
		setReg(instruction.SubOperation.d, m_registers[instruction.t] >> m_registers[instruction.s]);
	}

	void CPU::opSUB(Instruction instruction)
	{
		setReg(instruction.SubOperation.d, m_registers[instruction.s] - m_registers[instruction.t]);
	}

	void CPU::opSUBU(Instruction instruction)
	{
		setReg(instruction.SubOperation.d, m_registers[instruction.s] - m_registers[instruction.t]);
	}

	void CPU::opXOR(Instruction instruction)
	{
		setReg(instruction.SubOperation.d, m_registers[instruction.s] ^ m_registers[instruction.t]);
	}

	void CPU::opXORI(Instruction instruction)
	{
		setReg(instruction.t, m_registers[instruction.s] ^ m_registers[instruction.imm]);
	}

	void CPU::opSYSCALL(Instruction instruction)
	{
		// Do nothing...
	}

	void CPU::opDIV(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opDIVU(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opOR(Instruction instruction)
	{
		setReg(instruction.SubOperation.d, m_registers[instruction.s] | m_registers[instruction.SubOperation.t]);
	}

	void CPU::opORI(Instruction instruction)
	{
		// TODO: Understand why imm is 32bit instead of 16 !!
		// Might be worth to make functions with types for now to be less error prone...
		setReg(instruction.t, m_registers[instruction.s] | (0x0000ffff & instruction.imm));
	}

	void CPU::opJ(Instruction instruction)
	{
		m_ip = (m_ip & 0xF0000000) | (instruction.immJump() << 2);
	}

	void CPU::opJAL(Instruction instruction)
	{
		setReg(31, m_ip);
		opJ(instruction);
	}

	void CPU::opJR(Instruction instruction)
	{
		m_ip = m_registers[instruction.s];
	}

	void CPU::opADDU(Instruction instruction)
	{
		setReg(instruction.SubOperation.d, m_registers[instruction.s] + m_registers[instruction.t]);
	}

	void CPU::opAND(Instruction instruction)
	{
		setReg(instruction.SubOperation.d, m_registers[instruction.s] & m_registers[instruction.t]);
	}

	void CPU::opANDI(Instruction instruction)
	{
		setReg(instruction.t, m_registers[instruction.s] & instruction.imm);
	}

	void CPU::opBNE(Instruction instruction)
	{
		if (m_registers[instruction.s] != m_registers[instruction.t])
		{
			branch(instruction.imm);
		}
	}

	void CPU::opBEQ(Instruction instruction)
	{
		if (m_registers[instruction.s] == m_registers[instruction.t])
		{
			branch(instruction.imm);
		}
	}

	void CPU::opBGEZ(Instruction instruction)
	{
		if (m_registers[instruction.s] >= 0)
		{
			branch(instruction.imm);
		}
	}

	void CPU::opBGEZAL(Instruction instruction)
	{
		if (m_registers[instruction.s] >= 0)
		{
			setReg(31, m_ip);
			branch(instruction.imm);
		}
	}

	void CPU::opBGTZ(Instruction instruction)
	{
		if (m_registers[instruction.s] > 0)
		{
			branch(instruction.imm);
		}
	}

	void CPU::opBLEZ(Instruction instruction)
	{
		if (m_registers[instruction.s] <= 0)
		{
			branch(instruction.imm);
		}
	}

	void CPU::opBLTZ(Instruction instruction)
	{
		if (m_registers[instruction.s] < 0)
		{
			branch(instruction.imm);
		}
	}

	void CPU::opBLTZAL(Instruction instruction)
	{
		if (m_registers[instruction.s] < 0)
		{
			setReg(31, m_ip);
			branch(instruction.imm);
		}
	}

	void CPU::opADDIU(Instruction instruction)
	{
		setReg(instruction.t, m_registers[instruction.s] + instruction.imm);
	}

	void CPU::opADD(Instruction instruction)
	{
		setReg(instruction.SubOperation.d, m_registers[instruction.s] + m_registers[instruction.t]);
	}

	void CPU::opADDI(Instruction instruction)
	{
		uint32_t result = 0;
		if (!safeAdd(m_registers[instruction.s], instruction.imm, result))
		{
			throw std::runtime_error("NOT IMPLEMENTED");
		}
		else
		{
			setReg(instruction.t, result);
		}
	}

	void CPU::opSLL(Instruction instruction)
	{
		setReg(instruction.SubOperation.d, m_registers[instruction.t] << instruction.SubOperation.h);
	}

	void CPU::opSLLV(Instruction instruction)
	{
		setReg(instruction.SubOperation.d, m_registers[instruction.t] << m_registers[instruction.s]);
	}

	void CPU::opMFHI(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opMFLO(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opMULT(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opMULTU(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opSH(Instruction instruction)
	{
		if ((m_sr & 0x10000) != 0)
		{
			std::cout << "Ignoring store while cache is isolated...\n";
			return;
		}
		uint32_t address = m_registers[instruction.s] + instruction.imm;
		store16(address, m_registers[instruction.t]);
	}

	void CPU::opSB(Instruction instruction)
	{
		if ((m_sr & 0x10000) != 0)
		{
			std::cout << "Ignoring store while cache is isolated...\n";
			return;
		}
		uint32_t address = m_registers[instruction.s] + instruction.imm;
		store8(address, 0xff & instruction.t);
	}

	void CPU::opSW(Instruction instruction)
	{
		if ((m_sr & 0x10000) != 0)
		{
			std::cout << "Ignoring store while cache is isolated...\n";
			return;
		}
		uint32_t address = m_registers[instruction.s] + instruction.imm;
		store32(address, m_registers[instruction.t]);
	}

	void CPU::opLW(Instruction instruction)
	{
		if ((m_sr & 0x10000) != 0)
		{
			std::cout << "Ignoring load while cache is isolated...\n";
			return;
		}
		uint32_t address = m_registers[instruction.s] + instruction.imm;
		m_loadPair = { instruction.t, load32(address) };
	}

	void CPU::opLB(Instruction instruction)
	{
		if ((m_sr & 0x10000) != 0)
		{
			std::cout << "Ignoring load while cache is isolated...\n";
			return;
		}
		uint32_t address = m_registers[instruction.s] + instruction.imm;
		int8_t loadValue = load8(address);
		m_loadPair = { instruction.t, static_cast<uint32_t>(loadValue) };
	}

	void CPU::opLBU(Instruction instruction)
	{
		// TBD if this is correct
		if ((m_sr & 0x10000) != 0)
		{
			std::cout << "Ignoring load while cache is isolated...\n";
			return;
		}
		uint32_t address = m_registers[instruction.s] + instruction.imm;
		m_loadPair = { instruction.t, load8(address) };
	}

	void CPU::opLH(Instruction instruction)
	{
		// TBD if this is correct
		if ((m_sr & 0x10000) != 0)
		{
			std::cout << "Ignoring load while cache is isolated...\n";
			return;
		}
		uint32_t address = m_registers[instruction.s] + instruction.imm;
		m_loadPair = { instruction.t, load16(address) };
	}

	void CPU::opLUI(Instruction instruction)
	{
		setReg(instruction.t, (instruction.imm << 16));
	}
}
