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
			int16_t imm_se : 16;
			uint16_t imm : 16;
			uint32_t immJump : 26;

			struct
			{
				uint32_t sub : 6;
				uint32_t h : 5;
				uint32_t d : 5;
				uint32_t t : 5; // [20:16]
				uint32_t s : 5; // [25:21]
				uint32_t function : 6; // [31:26]
			};
		};
	};
}
#endif