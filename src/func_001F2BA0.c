// NEARMISS func_001F2BA0  (vram 0x001F2BA0, 0x2E4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.43% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation permutation across the case-1 scan loop (s0-s4 relabeled end-to-end vs target for the same live ranges: miss-counter/loop-index/stride/entry-pointer) plus one branch-likely-vs-plain-branch lowering difference on the idx-=2 odd/even adjust (target: beql; mwcc233: beqz), plus ev...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// arg0 = actor/entity. arg0+4 is a mode byte: mode 0 advances arg0+0xB4 (a timer/phase
// float) by a per-arg0+0xD sub-mode delta (7.0/3.5/7.0/3.5/2.0 for sub-modes 0/1/5/6/4,
// none for others), rebuilds a transform at arg0+0xD0 from arg0+0xC0/arg0+0xB0 via
// func_001029C0/func_00102C58/func_00102918, then kicks func_001F2E90(arg0, table row
// arg0+0xD selected from D_0025A350 (row size 0x60)). Mode 1: two passes over the entity
// list D_00275B40 (arg0+0xC entries, index k=4*i): pass 1 calls func_001F3620 per entry
// whose halfword +0x80 is 0 (a "not yet resolved" flag) and, if still unresolved, fires
// func_001F3E30 on a per-entry particle/state block (D_700036A0, entry+0x40, and two
// table-row words at row+0x50/+(idx*4+0x54)/+0x5C, idx = i&1 nudged -2 when i<0); entries
// that were already resolved decrement the `miss` counter. If `miss` hits 0, arg0+4 is
// set to 3 (mode transition). Pass 2 re-scans the same list; entries with a nonzero
// dword at +0x78 are skipped, otherwise func_001F3340(entry, table row, arg0+0xD) fires
// and a second counter is capped at 4 (early return once reached). Modes 2/3 both just
// free the actor via func_001AFC10(arg0).
//
// WALL: two independent, both permuter-class artifacts. (1) In the five state-0
// "+= const" tails the target pairs the loaded arg0+0xB4 field into the ODD half of a
// 64-bit FP register pair with the constant in the EVEN half (`add.s f0,f0f,f0`); mwcc
// 2.3.3 always colors the field into f1 and the constant into f0 regardless of operand
// order in the source (`add.s f0,f0,f1`) -- confirmed non-fixable by source reordering,
// same class as func_001509A0. (2) Inside the case-1 first scan loop, the whole s0-s4
// register set is permuted end-to-end relative to the target (miss-counter / loop
// index / stride / entry-pointer get different physical registers throughout, though
// the same logical values), and the `idx -= 2` adjust lowers to a plain `beqz` in mwcc
// 2.3.3 where the target uses branch-likely `beql`. Both read as regalloc/scheduling
// permutation, not the clean-store nop; per the campaign rules this class is parked
// after idiom attempts (fp-arg-order idiom, assign-in-arg idiom, and z==0.0f-temp idiom
// were all tried and did not change the coloring).
extern void func_00102918(void *a0, void *a1, void *a2);
extern void func_001029C0(void *a0);
extern void func_00102C58(void *obj, int unused, float *v);
extern void func_001AFC10(unsigned char *handle);
extern void func_001F2E90(char *arg0, char *arg1);
extern void func_001F3340(char *a0, char *a1, unsigned char a2);
extern void func_001F3620(char *a0, unsigned char a1);
extern void func_001F3E30(char *arg0, char *arg1, int arg2, int arg3, int arg4);

extern char D_0025A350[];
extern int D_00275B40;
extern char D_700036A0[];

void func_001F2BA0(char *arg0) {
    unsigned char mode;
    unsigned char sub;
    float f;
    int i;
    int j;
    int k;
    int miss;
    char *ent;
    char *tbl;
    int idx;

    mode = *(unsigned char *)(arg0 + 4);
    switch (mode) {
    case 0:
        sub = *(unsigned char *)(arg0 + 0xD);
        switch (sub) {
        case 0:
            f = *(float *)(arg0 + 0xB4) + 7.0f;
            *(float *)(arg0 + 0xB4) = f;
            break;
        case 1:
            f = *(float *)(arg0 + 0xB4) + 3.5f;
            *(float *)(arg0 + 0xB4) = f;
            break;
        case 5:
            f = *(float *)(arg0 + 0xB4) + 7.0f;
            *(float *)(arg0 + 0xB4) = f;
            break;
        case 6:
            f = *(float *)(arg0 + 0xB4) + 3.5f;
            *(float *)(arg0 + 0xB4) = f;
            break;
        case 4:
            f = *(float *)(arg0 + 0xB4) + 2.0f;
            *(float *)(arg0 + 0xB4) = f;
            break;
        }
        func_001029C0(arg0 + 0xD0);
        func_00102C58(arg0 + 0xD0, (int)(arg0 + 0xD0), (float *)(arg0 + 0xC0));
        func_00102918(arg0 + 0xD0, arg0 + 0xD0, arg0 + 0xB0);
        func_001F2E90(arg0, D_0025A350 + (*(unsigned char *)(arg0 + 0xD) * 0x60));
        return;
    case 1:
        miss = *(unsigned char *)(arg0 + 0xC);
        i = 0;
        k = 0;
        while (i < *(unsigned char *)(arg0 + 0xC)) {
            ent = *(char **)(D_00275B40 + k);
            if (*(short *)(ent + 0x80) == 0) {
                func_001F3620(ent, *(unsigned char *)(arg0 + 0xD));
                if (*(short *)(ent + 0x80) == 0) {
                    idx = i & 1;
                    tbl = D_0025A350 + (*(unsigned char *)(arg0 + 0xD) * 0x60);
                    if (i < 0 && idx != 0) {
                        idx -= 2;
                    }
                    func_001F3E30(D_700036A0, ent + 0x40, *(int *)(tbl + 0x50), *(int *)(tbl + (idx * 4) + 0x54), *(int *)(tbl + 0x5C));
                }
            } else {
                miss -= 1;
            }
            k += 4;
            i += 1;
        }
        j = 0;
        if (miss == 0) {
            *(unsigned char *)(arg0 + 4) = 3;
            j = 0;
        }
        i = 0;
        k = 0;
        while (j < *(unsigned char *)(arg0 + 0xC)) {
            ent = *(char **)(D_00275B40 + k);
            if (*(int *)(ent + 0x78) != 0) {
                k += 4;
                j += 1;
                continue;
            }
            sub = *(unsigned char *)(arg0 + 0xD);
            func_001F3340(ent, D_0025A350 + (sub * 0x60), sub);
            i += 1;
            if (i >= 4) {
                return;
            }
            k += 4;
            j += 1;
        }
        return;
    case 2:
    case 3:
        func_001AFC10((unsigned char *)arg0);
        break;
    }
}
