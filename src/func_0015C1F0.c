// NEARMISS func_0015C1F0  (vram 0x0015C1F0, 0x118 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 80.87% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Inner 3-way select (sel 0/2/1) dispatch-shape / branch-scheduling permutation: mwcc emits the compare cascade with a slightly different branch/insert layout than the target. Body + all stored values match; not the clean-store nop. Permuter/branch-layout territory.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS (o233 80.9%, o991 76.7%): logic fully recovered; sole residual is a
// dispatch-shape / branch-scheduling artifact on the inner 3-way select.
//
// Reads a mode byte at arg0+0x234 and the global selector D_00810C60, then writes
// a "kind" byte to arg0+0x2FF:
//   mode 0:  sel 0->0x3B, sel 2->0x3F, sel 1->0x3E, else 0x3B
//   mode 1:  sel 0->0x40, sel 2->0x3F, sel 1->0x3E, else 0x40
//   mode !=0,1: 0x3D
// It then reloads that kind byte, indexes the pointer table D_0028A490 by it, and
// runs func_001CA6E0(self, table[kind]); stores func_001C6150(*(int*)(self+0x44))
// as a byte at self+0xC; writes 0x28 to the short at self+0x96; and tail-calls
// func_00200890 with the leftover a0 (= *(int*)(self+0x44), not reloaded -> declared
// no-arg so mwcc keeps the live register).
//
// WALL: the inner select is emitted as a 0/2/1 compare cascade with the result set
// in each branch delay slot. mwcc (both builds) lays the dispatch tree out with a
// slightly different branch/insert shape than the target -- a regalloc/branch-
// scheduling permutation, not the clean-store nop. Body and all stored values match.

extern unsigned char D_00810C60;
extern int D_0028A490[];
extern void func_001CA6E0(char *p, int a);
extern unsigned char func_001C6150(int a);
extern void func_00200890();

void func_0015C1F0(char *arg0) {
    unsigned char mode;
    unsigned char sel;

    mode = *(unsigned char *)(arg0 + 0x234);
    if (mode == 0) {
        sel = D_00810C60;
        switch (sel) {
        case 0:
            *(char *)(arg0 + 0x2FF) = 0x3B;
            break;
        case 2:
            *(char *)(arg0 + 0x2FF) = 0x3F;
            break;
        case 1:
            *(char *)(arg0 + 0x2FF) = 0x3E;
            break;
        default:
            *(char *)(arg0 + 0x2FF) = 0x3B;
            break;
        }
    } else if (mode == 1) {
        sel = D_00810C60;
        switch (sel) {
        case 0:
            *(char *)(arg0 + 0x2FF) = 0x40;
            break;
        case 2:
            *(char *)(arg0 + 0x2FF) = 0x3F;
            break;
        case 1:
            *(char *)(arg0 + 0x2FF) = 0x3E;
            break;
        default:
            *(char *)(arg0 + 0x2FF) = 0x40;
            break;
        }
    } else {
        *(char *)(arg0 + 0x2FF) = 0x3D;
    }

    func_001CA6E0(arg0, D_0028A490[*(unsigned char *)(arg0 + 0x2FF)]);
    *(char *)(arg0 + 0xC) = func_001C6150(*(int *)(arg0 + 0x44));
    *(short *)(arg0 + 0x96) = 0x28;
    func_00200890();
}
