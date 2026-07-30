// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: behaviour/handler installer. Selects an update routine for the
// object `p` from `kind` and stores it in the function-pointer field at +0x4C.
// Dispatch is the 13-entry jr-table at 0x0026E310; kinds 0 and 4 (and any
// kind >= 13) fall through to the default handler func_001CAA00.
//   1 -> func_001CAF60    2 -> func_001CACB0    3 -> func_001CAE30
//   5 -> func_001CB360    6 -> func_001CAF70    7 -> func_001CB480
//   8 -> func_001CB060    9 -> func_001CB130   10 -> func_001CB1F0
//  11 -> func_001CB580   12 -> func_001CB2B0
//   0, 4, default -> func_001CAA00
// The case order below is the original source order (it is what fixes the
// emitted body layout); the table itself is positional 0..12.
//
// NEARMISS 99.83% — jr-table external-dispatch wall (proven s84): the original
// consolidated all jump tables into an external rodata TU, so mwcc's local @15
// table is a permanent reloc mismatch. The instruction stream is otherwise
// byte-for-byte identical to the target.

extern void func_001CAA00();
extern void func_001CACB0();
extern void func_001CAE30();
extern void func_001CAF60();
extern void func_001CAF70();
extern void func_001CB060();
extern void func_001CB130();
extern void func_001CB1F0();
extern void func_001CB2B0();
extern void func_001CB360();
extern void func_001CB480();
extern void func_001CB580();

void func_001CA5F0(char *p, unsigned int kind) {
    switch (kind) {
    case 2:
        *(void (**)())(p + 0x4C) = func_001CACB0;
        break;
    case 6:
        *(void (**)())(p + 0x4C) = func_001CAF70;
        break;
    case 5:
        *(void (**)())(p + 0x4C) = func_001CB360;
        break;
    case 7:
        *(void (**)())(p + 0x4C) = func_001CB480;
        break;
    case 1:
        *(void (**)())(p + 0x4C) = func_001CAF60;
        break;
    case 8:
        *(void (**)())(p + 0x4C) = func_001CB060;
        break;
    case 9:
        *(void (**)())(p + 0x4C) = func_001CB130;
        break;
    case 10:
        *(void (**)())(p + 0x4C) = func_001CB1F0;
        break;
    case 12:
        *(void (**)())(p + 0x4C) = func_001CB2B0;
        break;
    case 11:
        *(void (**)())(p + 0x4C) = func_001CB580;
        break;
    case 3:
        *(void (**)())(p + 0x4C) = func_001CAE30;
        break;
    default:
        *(void (**)())(p + 0x4C) = func_001CAA00;
        break;
    }
}
