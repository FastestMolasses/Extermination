// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS:
//   Command dispatcher for a 7-opcode command block. arg2 points at the command
//   record: +0x8 = opcode (unsigned), +0x18 = handle/id, +0x20 = float parameter.
//   arg0 is a context value passed straight through to func_001FBD50; arg1 is
//   unused. Always returns 1.
//     op 0 -> func_001FBD50(ctx, id, 0, 300.0f)      (fixed 300.0 parameter)
//     op 1 -> func_001FBD50(ctx, id, 0, rec[+0x20])
//     op 3 -> func_001FBC50()
//     op 4 -> func_001FA790(0, id)
//     op 5 -> func_001FABB0()
//     op 6 -> func_001FAE70(id)
//     op 2 and every op >= 7 take the default:
//             func_001FB9F0(id, 0x1000, 0x1000, 0x1000)
//   Case bodies are written in the original's block-layout order (6,3,4,5,1,0)
//   because mwcc emits jump-table case blocks in source order; that ordering
//   reproduces the target's block layout exactly.
//   ABI note: floats are passed in f12.. INDEPENDENTLY of the integer a0..a3
//   slots (a float argument does not consume an integer register), so
//   func_001FBD50's float parameter is declared last while arg0 still occupies
//   a0 -- confirmed empirically (the "float first" O32 reading scores worse).

extern void func_001FA790(int a, int b);
extern void func_001FABB0(void);
extern void func_001FAE70(int a);
extern void func_001FB9F0(int a, int b, int c, int d);
extern void func_001FBC50(void);
extern void func_001FBD50(int a, int b, int c, float f);

int func_001B6D70(int arg0, int arg1, char *arg2)
{
    switch (*(unsigned int *)(arg2 + 8)) {
    case 6:
        func_001FAE70(*(int *)(arg2 + 0x18));
        break;
    case 3:
        func_001FBC50();
        break;
    case 4:
        func_001FA790(0, *(int *)(arg2 + 0x18));
        break;
    case 5:
        func_001FABB0();
        break;
    case 1:
        func_001FBD50(arg0, *(int *)(arg2 + 0x18), 0, ((float *)arg2)[8]);
        break;
    case 0:
        func_001FBD50(arg0, *(int *)(arg2 + 0x18), 0, 300.0f);
        break;
    default:
        func_001FB9F0(*(int *)(arg2 + 0x18), 0x1000, 0x1000, 0x1000);
        break;
    }
    return 1;
}
