#ifndef E_PUG_STATION_INSTRUCTION
#define E_PUG_STATION_INSTRUCTION

#include <cstdint>

namespace ePugStation
{
	struct Instruction 
	{
		Instruction() = default;
		~Instruction() = default;

		Instruction(uint32_t operation) : op(operation) {}
		Instruction(const Instruction& instruction) : op(instruction.op) {}

		union 
		{
			uint32_t op;
			struct 
			{
				union 
				{
					struct 
					{
						signed imm : 16;
						unsigned t : 5; // [20:16]
						unsigned s : 5; // [25:21]
						unsigned function : 6; // [31:26]
					};
					struct 
					{
						unsigned sub : 6;
						unsigned h : 5;
						unsigned d : 5;
						unsigned t : 5; // [20:16]
						unsigned s : 5; // [25:21]
						unsigned function : 6; // [31:26]
					}ShiftOperation;
				};
			};
		};

		uint32_t immJump()
		{
			return op & 0x03FFFFFF;
		}
	};
}
#endif