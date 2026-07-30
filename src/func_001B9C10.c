// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS:
//   11-opcode setter for a 3-component float vector plus its "dirty/weight"
//   trailing float, applied either to an instance (arg0) or to a global slot.
//   arg2 is the command record: +0x8 = opcode (unsigned), +0x20..+0x28 = the
//   x/y/z payload. arg1 is unused. Always returns 1, including for op >= 11.
//     op 0/1/2  -> instance component [op]: arg0[0xC0 + op*4] = rec[0x20 + op*4],
//                  then arg0[0xCC] = 1.0f
//     op 3      -> instance whole vector: arg0[0xC0..0xC8] = rec[0x20..0x28],
//                  then arg0[0xCC] = 1.0f
//     op 4/5/6  -> no-op (present in the table, empty bodies)
//     op 7/8/9  -> global component [op-7]: D_00810370..78 = rec[0x20..0x28],
//                  then D_0081037C = 1.0f
//     op 10     -> global whole vector: D_00810370/74/78 = rec[0x20/24/28],
//                  then D_0081037C = 1.0f
//   D_00810354[] is the SAME storage as D_00810370: the global vector's x/y/z
//   are elements [7],[8],[9] and the trailing float is [10]. The original wrote
//   the op 7/8/9 arm as an indexed store off the biased base, which is why the
//   target relocates against D_00810354 (= &D_00810370 - 0x1C) there and against
//   the individual element symbols in the op 10 arm. Both forms are kept so the
//   emitted relocations match the target's.
//   The `(char *)(op * 4) + (int)ptr` spelling is deliberate: it is byte-offset
//   arithmetic with the scaled index as the ADDU's first operand, which is the
//   operand order the target emits (`addu v0, v1, a2`). Writing it as
//   `ptr + op * 4` makes mwcc canonicalize the pointer into the first operand.

extern float D_00810354[];
extern float D_00810370;
extern float D_00810374;
extern float D_00810378;
extern float D_0081037C;

int func_001B9C10(char *arg0, int arg1, char *arg2)
{
    unsigned int op;

    op = *(unsigned int *)(arg2 + 8);
    switch (op) {
    case 0:
    case 1:
    case 2:
        *(float *)((char *)(op * 4) + (int)arg0 + 0xC0) = *(float *)((char *)(op * 4) + (int)arg2 + 0x20);
        *(float *)(arg0 + 0xCC) = 1.0f;
        break;
    case 3:
        *(float *)(arg0 + 0xC0) = *(float *)(arg2 + 0x20);
        *(float *)(arg0 + 0xC4) = *(float *)(arg2 + 0x24);
        *(float *)(arg0 + 0xC8) = *(float *)(arg2 + 0x28);
        *(float *)(arg0 + 0xCC) = 1.0f;
        break;
    case 4:
    case 5:
    case 6:
        break;
    case 7:
    case 8:
    case 9:
        D_00810354[op] = *(float *)((char *)(op * 4) + (int)arg2 + 4);
        D_0081037C = 1.0f;
        break;
    case 10:
        D_00810370 = *(float *)(arg2 + 0x20);
        D_00810374 = *(float *)(arg2 + 0x24);
        D_00810378 = *(float *)(arg2 + 0x28);
        D_0081037C = 1.0f;
        break;
    }
    return 1;
}
