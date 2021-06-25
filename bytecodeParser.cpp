#include <Windows.h>
#include "hookUtils.h"
#include "console.h"

EXTERN_C_START
#include <lua/lopcodes.h>
EXTERN_C_END

void parseOpcodes(int *opcodes) {
	auto fpout = Console::getInstance()->fpout;
	void* luaV_execute = findSignatureImpl(getWarframeExe(), "48 8B C4 89 50 10 48 89 48 08 55 53 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ?");
	fprintf(fpout, "luaV_execute %X\n", luaV_execute);
	fflush(fpout);
	void* end = findSignatureImpl((unsigned char*)luaV_execute, "49 8B E3 41 5F 41 5E 41 5D 41 5C 5F 5E 5B 5D C3");
	uint32_t* jumpTable = (uint32_t*)(((((uint64_t)end + 0x10) / 8) + 1) * 8);
	fprintf(fpout, "jumpTable %X\n", jumpTable);
	fflush(fpout);

	void* patterns[40];
	patterns[OP_TAILCALL] = findSignatureImpl((unsigned char*)luaV_execute, "C1 EB 17 8B C3 85 DB 74 0B 48 C1 E0 04 48 03 C6 49 89 43 10");
	patterns[OP_CLOSURE] = findSignatureImpl((unsigned char*)luaV_execute, "4C 8B 74 24 ? 48 8B D3 48 C1 EA 0E 49 8B 46 20 4D 8B 46 18 48 8B 48 20 48 8B 1C D1");
	patterns[OP_EQ] = findSignatureImpl((unsigned char*)luaV_execute, "4C 8B 74 24 ? 8B C3 C1 E8 17 8B C8 0F BA E0 08 73 0C 0F B6 D1 48 C1 E2 04 49 03 D6");
	patterns[OP_CONST] = findSignatureImpl((unsigned char*)luaV_execute, "81 E3 ? ? ? ? C7 46 ? ? ? ? ? 89 1E");
	patterns[OP_CONCAT] = findSignatureImpl((unsigned char*)luaV_execute, "8B C3 4D 89 53 30 C1 E8 17 49 8B CB C1 EB 0E 81 E3 ? ? ? ? 8B F8 8B D3 44 8B C3 2B D0 FF C2");
	patterns[OP_GETTABLE] = findSignatureImpl((unsigned char*)luaV_execute, "8B C3 4D 89 53 30 C1 E8 0E 8B C8 0F BA E0 08 73 0F 44 0F B6 C1 49 C1 E0 04 4C 03 44 24 ? ");
	patterns[OP_SETLIST] = findSignatureImpl((unsigned char*)luaV_execute, "8B C3 C1 EB 0E 81 E3 ? ? ? ? C1 E8 17 8B F8 8B D3 85 C0 75 19 49 8B 7B 10");
	patterns[OP_LOADK] = findSignatureImpl((unsigned char*)luaV_execute, "48 8B CB 48 C1 E9 0E 48 C1 E1 04 48 03 4C 24 ? 48 8B 01 48 89 06 8B 41 08 89 46 08");
	patterns[OP_JMP] = findSignatureImpl((unsigned char*)luaV_execute, "48 8B C3 48 C1 E8 0E 4C 8D 14 85 ? ? ? ? 4D 03 D5");
	patterns[OP_TFORLOOP] = findSignatureImpl((unsigned char*)luaV_execute, "48 8B 46 20 48 8D 56 30 48 89 42 20 49 8B CB 8B 46 28 89 42 28 48 8B 46 10 48 89 42 10 8B 46 18 89 42");
	patterns[OP_SETUPVAL] = findSignatureImpl((unsigned char*)luaV_execute, "48 8B 4C 24 ? 48 8B C3 48 C1 E8 17 48 8B 54 C1 ? 48 8B 06 48 8B 4A 10 48 89 01 8B 46 08 89 41 08 83 7E");
	patterns[OP_NOT] = findSignatureImpl((unsigned char*)luaV_execute, "48 8B C3 48 C1 E8 17 48 03 C0 8B 4C C2 08 85 C9 74 1B 83 F9 01 75 06 83 3C C2 00");
	patterns[OP_VARARG] = findSignatureImpl((unsigned char*)luaV_execute, "4D 8B 7B 28 48 8B 44 24 ? C1 EB 17 FF CB 49 8B 3F 49 2B 7F 08 48 8B 40 20 48 C1 FF 04 0F B6 48 71");
	patterns[OP_GETUPVAL] = findSignatureImpl((unsigned char*)luaV_execute, "48 8B 4C 24 ? 48 8B C3 48 C1 E8 17 48 8B 44 C1 ? 48 8B 48 10 48 8B 01 48 89 06 8B 41 08 89 46 08");
	patterns[OP_RETURN] = findSignatureImpl((unsigned char*)luaV_execute, "C1 EB 17 8B C3 85 DB 74 0F 48 C1 E0 04 48 83 C0 F0 48 03 C6 49 89 43 10");
	patterns[OP_LEN] = findSignatureImpl((unsigned char*)luaV_execute, "48 C1 EB 17 48 C1 E3 04 48 03 DA 8B 4B 08 85 C9 0F 84 ? ? ? ? 83 E9 04 74 60 83 F9 01 74 3C 4C 8B CE");
	patterns[OP_NEWTABLE] = findSignatureImpl((unsigned char*)luaV_execute, "8B D3 C1 EB 0E 81 E3 ? ? ? ? C1 EA 17 8B CB C1 E9 03 83 E1 1F 74 0A 83 E3 07 83 C3 08 FF C9 D3 E3");
	patterns[OP_TESTSET] = findSignatureImpl((unsigned char*)luaV_execute, "48 8B CB 48 C1 E9 17 48 03 C9 8B 44 CA 08 85 C0 74 0F 83 F8 01 75 06 83 3C CA 00");
	patterns[OP_SETTABLE] = findSignatureImpl((unsigned char*)luaV_execute, "48 8B 4C 24 ? 8B C3 C1 E8 17 44 8B E0 25 ? ? ? ? 89 85 ? ? ? ? 74 0D 41 0F B6 FC 48 C1 E7 04 48 03 F9");
	patterns[OP_UNM] = findSignatureImpl((unsigned char*)luaV_execute, "48 C1 EB 17 48 C1 E3 04 48 03 DA 83 7B 08 03 75 17 F3 0F 10 03 0F 57 C7 C7 46 ? ? ? ? ? F3 0F 11 06");
	patterns[OP_LT] = findSignatureImpl((unsigned char*)luaV_execute, "4C 8B 74 24 ? 8B C3 C1 E8 0E 4D 89 53 30 8B C8 0F BA E0 08 73 0D 44 0F B6 C1 49 C1 E0 04 4D 03 C6");
	patterns[OP_FORLOOP] = findSignatureImpl((unsigned char*)luaV_execute, "F3 0F 10 46 ? 33 C0 0F 2F C6 F3 0F 10 56 ? 0F 28 C8 F3 0F 58 0E 76 05 0F 2F D1");
	patterns[OP_CALL] = findSignatureImpl((unsigned char*)luaV_execute, "8B C3 C1 EB 0E 81 E3 ? ? ? ? C1 E8 17 FF CB 8B C8 85 C0 74 0D 8B C1 48 C1 E0 04 48 03 C6");
	patterns[OP_LE] = findSignatureImpl((unsigned char*)luaV_execute, "4C 8B 44 24 ? 8B C3 C1 E8 0E 4D 89 53 30 8B C8 0F BA E0 08 73 0C 0F B6 F9 48 C1 E7 04 49 03 F8");
	patterns[OP_LOADBOOL] = findSignatureImpl((unsigned char*)luaV_execute, "8B C3 C7 46 ? ? ? ? ? C1 E8 17 41 BA ? ? ? ? F7 C3 ? ? ? ? 89 06 B8 ? ? ? ? 44 0F 44 D0 4D 03 D4");
	patterns[OP_FORPREP] = findSignatureImpl((unsigned char*)luaV_execute, "4D 89 53 30 48 8D 7E 20 83 7E 08 03 74 19 48 8B D6 48 8B CE E8 ? ? ? ? 48 85 C0 75 09 48 8D 15 ? ? ? ?");
	patterns[OP_HASH] = findSignatureImpl((unsigned char*)luaV_execute, "4C 8B C3 BB ? ? ? ? 49 C1 E8 0E 49 C1 E0 04 4C 03 44 24 ? 49 8B 00 48 8B 48 10 48 8D 50 18 48 85 C9 74 1E");
	patterns[OP_TEST] = findSignatureImpl((unsigned char*)luaV_execute, "8B 46 08 85 C0 74 0E 83 F8 01 75 05 83 3E 00 74 04");
	patterns[OP_SELF] = findSignatureImpl((unsigned char*)luaV_execute, "48 8B C3 C1 EB 0E 48 C1 E8 17 48 C1 E0 04 48 03 D0 48 8B 02 48 89 46 10 8B 42 08 89 46 18 4D 89 53 30 8B C3");
	patterns[OP_MOVE] = findSignatureImpl((unsigned char*)luaV_execute, "48 8B CB 48 C1 E9 17 48 03 C9 48 8B 04 CA 48 89 06 8B 44 CA 08 89 46 08");
	patterns[OP_CLOSE] = findSignatureImpl((unsigned char*)luaV_execute, "48 8B D6 49 8B CB E8 ? ? ? ? E9");
	patterns[OP_LOADNIL] = findSignatureImpl((unsigned char*)luaV_execute, "48 8B CB 48 C1 E9 17 48 C1 E1 04 48 03 CA 66 0F 1F 44 00 ?");
	patterns[OP_DIV] = findSignatureImpl((unsigned char*)luaV_execute, "4C 8B 74 24 ? 8B C3 C1 E8 17 8B C8 0F BA E0 08 73 0C 0F B6 F9 48 C1 E7 04 49 03 FE EB 0A 48 8B F9 48 C1 E7 04 48 03 FA C1 EB 0E 0F BA E3 08 73 0C 0F B6 DB 48 C1 E3 04 49 03 DE EB 0D 81 E3 ? ? ? ? 48 C1 E3 04 48 03 DA 83 7F 08 03 75 1E");
	patterns[OP_MOD] = findSignatureImpl((unsigned char*)luaV_execute, "4C 8B 74 24 ? 8B C3 C1 E8 17 8B C8 0F BA E0 08 73 0C 0F B6 F9 48 C1 E7 04 49 03 FE EB 0A 48 8B F9 48 C1 E7 04 48 03 FA C1 EB 0E 0F BA E3 08 73 0C 0F B6 DB 48 C1 E3 04 49 03 DE EB 0D 81 E3 ? ? ? ? 48 C1 E3 04 48 03 DA 83 7F 08 03 75 57");
	patterns[OP_POW] = findSignatureImpl((unsigned char*)luaV_execute, "4C 8B 74 24 ? 8B C3 C1 E8 17 8B C8 0F BA E0 08 73 0C 0F B6 F9 48 C1 E7 04 49 03 FE EB 0A 48 8B F9 48 C1 E7 04 48 03 FA C1 EB 0E 0F BA E3 08 73 0C 0F B6 DB 48 C1 E3 04 49 03 DE EB 0D 81 E3 ? ? ? ? 48 C1 E3 04 48 03 DA 83 7F 08 03 75 23");
	patterns[OP_MUL] = findSignatureImpl((unsigned char*)luaV_execute, "4C 8B 74 24 ? 8B C3 C1 E8 17 8B C8 0F BA E0 08 73 0D 44 0F B6 C1 49 C1 E0 04 4D 03 C6 EB 0A 4C 8B C1 49 C1 E0 04 4C 03 C2 C1 EB 0E 44 8B CB 0F BA E3 08 73 0D 45 0F B6 C9 49 C1 E1 04 4D 03 CE EB 0E 41 81 E1 ? ? ? ? 49 C1 E1 04 4C 03 CA 41 83 78 ? ? 75 21 41 83 79 ? ? 75 1A F3 41 0F 10 01 F3 41 0F 59 00");
	patterns[OP_ADD] = findSignatureImpl((unsigned char*)luaV_execute, "4C 8B 74 24 ? 8B C3 C1 E8 17 8B C8 0F BA E0 08 73 0D 44 0F B6 C1 49 C1 E0 04 4D 03 C6 EB 0A 4C 8B C1 49 C1 E0 04 4C 03 C2 C1 EB 0E 44 8B CB 0F BA E3 08 73 0D 45 0F B6 C9 49 C1 E1 04 4D 03 CE EB 0E 41 81 E1 ? ? ? ? 49 C1 E1 04 4C 03 CA 41 83 78 ? ? 75 21 41 83 79 ? ? 75 1A F3 41 0F 10 01 F3 41 0F 58 00");
	patterns[OP_SUB] = findSignatureImpl((unsigned char*)luaV_execute, "4C 8B 74 24 ? 8B C3 C1 E8 17 8B C8 0F BA E0 08 73 0D 44 0F B6 C1 49 C1 E0 04 4D 03 C6 EB 0A 4C 8B C1 49 C1 E0 04 4C 03 C2 C1 EB 0E 44 8B CB 0F BA E3 08 73 0D 45 0F B6 C9 49 C1 E1 04 4D 03 CE EB 0E 41 81 E1 ? ? ? ? 49 C1 E1 04 4C 03 CA 41 83 78 ? ? 75 21 41 83 79 ? ? 75 1A F3 41 0F 10 00 F3 41 0F 5C 01");
	patterns[OP_GETGLOBAL] = findSignatureImpl((unsigned char*)luaV_execute, "48 8B 44 24 ? 48 8D 54 24 ? 4C 8B C3 C7 44 24 ? ? ? ? ? 49 C1 E8 0E 4C 8B CE 49 C1 E0 04 49 8B CB 48 8B 40 18 4C 03 44 24 ? 48 89 44 24 ? 4D 89 53 30 E8 ? ? ? ? 4C");
	patterns[OP_SETGLOBAL] = findSignatureImpl((unsigned char*)luaV_execute, "48 8B 44 24 ? 48 8D 54 24 ? 4C 8B C3 C7 44 24 ? ? ? ? ? 49 C1 E8 0E 4C 8B CE 49 C1 E0 04 49 8B CB 48 8B 40 18 4C 03 44 24 ? 48 89 44 24 ? 4D 89 53 30 E8 ? ? ? ? E9");
	fprintf(fpout, "parse all opcodes done\n");
	fflush(fpout);

	for (int i = 0; i < 40; i++) {
		uint64_t pattern = (uint64_t)patterns[i];
		pattern -= (uint64_t)getWarframeExe();
		opcodes[i] = -1;

		for (int j = 0; j < 40; j++) {
			if (pattern == (uint64_t)jumpTable[j]) {
				opcodes[i] = j;
				fprintf(fpout, "opcode OP_%s - %d\n", luaP_opnames[i], j);
				fflush(fpout);
				break;
			}
		}

		if (opcodes[i] == -1) {
			fprintf(fpout, "opcode OP_%s - cant find\n", luaP_opnames[i]);
			fflush(fpout);
		}
	}

}