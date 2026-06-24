// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Dispatch/trigger helper. Selects an index either directly (0x18 when the
// global state byte D_0081078F == 1) or by mapping the actor's mode byte at
// arg0+0x2FF: 0x3B->0x18, 0x3E->0x19, 0x3F->0x1A, 0x40->0x1B, anything else
// -> -1 (no-op). For a valid index it fetches the word D_0028A490[index] and
// calls func_001CA700(actor, that_word, 7); on a nonzero result it calls
// func_001D06D0(actor, 1) and stamps the scratchpad/hardware guard byte at
// 0x70003B8F = 2. Returns void.
//
// mwcc 2.3.3 byte-matches; the pinned 991202 build caps at 69.0% (it lowers the
// beql switch chain and clean-store delay slots differently). The byte switch
// emits the exact beql/beq comparison cascade with addiu-in-delay-slot index
// loads. Verified objdiff 100.0% vs build/expected.
extern unsigned char D_0081078F;
extern int D_0028A490[];
extern int func_001CA700(int p, int arg, int n);
extern void func_001D06D0(int p, int flag);

void func_001B81D0(int arg0) {
    int idx;

    if (D_0081078F == 1) {
        idx = 0x18;
    } else {
        switch (*(unsigned char *)(arg0 + 0x2FF)) {
        case 0x3B: idx = 0x18; break;
        case 0x3E: idx = 0x19; break;
        case 0x3F: idx = 0x1A; break;
        case 0x40: idx = 0x1B; break;
        default:   idx = -1;   break;
        }
    }

    if (idx != -1) {
        if (func_001CA700(arg0, D_0028A490[idx], 7) != 0) {
            func_001D06D0(arg0, 1);
            *(char *)0x70003B8F = 2;
        }
    }
}
