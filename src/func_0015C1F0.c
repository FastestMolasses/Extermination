// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 80.87% NEARMISS).
// Fixes read from the original instructions:
//  - Each inner selector switch lists its cases as 1, 2, 0: mwcc 2.3.3 emits the
//    compare cascade in reverse source order (the original tests 0, then 2, then 1).
//  - Selector 0 and the default share one body (`case 0: default:`); the default
//    path falls into the selector-0 store.
//  - Callee prototypes now follow their definitions in src/.
//
// Reads a mode byte at arg0+0x234 and the global selector D_00810C60, then writes
// a "kind" byte to arg0+0x2FF:
//   mode 0:  sel 1->0x3E, sel 2->0x3F, sel 0 or anything else->0x3B
//   mode 1:  sel 1->0x3E, sel 2->0x3F, sel 0 or anything else->0x40
//   other modes: 0x3D
// It then reloads that kind byte, indexes the pointer table D_0028A490 by it, and
// runs func_001CA6E0(self, table[kind]); stores func_001C6150(*(self+0x44)) as a
// byte at self+0xC; writes 0x28 to the short at self+0x96; and calls func_00200890.

extern unsigned char D_00810C60;
extern int D_0028A490[];
extern void func_001CA6E0(int a0, int a1);
extern unsigned char func_001C6150(void *a0);
extern void func_00200890(void);

void func_0015C1F0(char *arg0) {
    if (*(unsigned char *)(arg0 + 0x234) == 0) {
        switch (D_00810C60) {
        case 1:
            *(char *)(arg0 + 0x2FF) = 0x3E;
            break;
        case 2:
            *(char *)(arg0 + 0x2FF) = 0x3F;
            break;
        case 0:
        default:
            *(char *)(arg0 + 0x2FF) = 0x3B;
            break;
        }
    } else if (*(unsigned char *)(arg0 + 0x234) == 1) {
        switch (D_00810C60) {
        case 1:
            *(char *)(arg0 + 0x2FF) = 0x3E;
            break;
        case 2:
            *(char *)(arg0 + 0x2FF) = 0x3F;
            break;
        case 0:
        default:
            *(char *)(arg0 + 0x2FF) = 0x40;
            break;
        }
    } else {
        *(char *)(arg0 + 0x2FF) = 0x3D;
    }

    func_001CA6E0((int)arg0, D_0028A490[*(unsigned char *)(arg0 + 0x2FF)]);
    *(char *)(arg0 + 0xC) = func_001C6150(*(void **)(arg0 + 0x44));
    *(short *)(arg0 + 0x96) = 0x28;
    func_00200890();
}
