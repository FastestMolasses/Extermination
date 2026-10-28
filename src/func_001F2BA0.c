// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 94.43% NEARMISS).
// Fixes read from the original instructions:
//  - the five sub-mode timer advances are compound `field += const` (fixes the add
//    operand order the old header called an FP pairing wall);
//  - the table-row word index is `i % 2` (the signed remainder produces the
//    branch-likely odd/negative adjust), reading the row as an int array;
//  - both scans are `for` loops with the stride/counter bumped at the loop end; the
//    second scan has its own stride local and counts calls with `++cnt >= 4`;
//  - a `(void *)` cast on the first argument of func_00102C58 restores the
//    argument-setup order;
//  - locals are declared second-scan counters first (cnt, m, j), which gives the
//    original's saved-register assignment;
//  - callee prototypes now agree with their definitions in src/.
//
// arg0 = actor/entity. arg0+4 is a mode byte: mode 0 advances arg0+0xB4 (a timer/phase
// float) by a per-arg0+0xD sub-mode delta (7.0/3.5/7.0/3.5/2.0 for sub-modes 0/1/5/6/4,
// none for others), rebuilds a transform at arg0+0xD0 from arg0+0xC0/arg0+0xB0 via
// func_001029C0/func_00102C58/func_00102918, then kicks func_001F2E90(arg0, table row
// arg0+0xD selected from D_0025A350 (row size 0x60)). Mode 1: two passes over the entity
// list D_00275B40 (arg0+0xC entries). Pass 1 calls func_001F3620 per entry whose
// halfword +0x80 is 0 (a "not yet resolved" flag) and, if still unresolved, fires
// func_001F3E30 on a per-entry particle/state block (D_700036A0, entry+0x40, and the
// table-row words 20, 21 + i%2 and 23); entries that were already resolved decrement the
// `miss` counter. If `miss` hits 0, arg0+4 is set to 3 (mode transition). Pass 2
// re-scans the same list; entries with a nonzero word at +0x78 are skipped, otherwise
// func_001F3340(entry, table row, arg0+0xD) fires, and the scan returns after 4 calls.
// Modes 2/3 both free the actor via func_001AFC10(arg0).
extern void func_00102918(void *a0, void *a1, void *a2);
/* func_001029C0 is an asm body declared (void) in src/, but the caller passes the
   matrix pointer in the first argument register. */
extern void func_001029C0(void *a0);
extern void func_00102C58(void *obj, int unused, float *v);
extern void func_001AFC10(unsigned char *handle);
extern int func_001F2E90(char *arg0, char *arg1);
extern void func_001F3340(char *arg0, char *arg1, int arg2);
extern void func_001F3620(char *p, int t);
extern void func_001F3E30(char *arg0, char *arg1, int arg2, int arg3, int arg4);

extern char D_0025A350[];
extern int D_00275B40;
extern char D_700036A0[];

void func_001F2BA0(char *arg0) {
    int cnt;
    int m;
    int j;
    int i;
    int miss;
    char *ent;
    int k;
    int *row;

    switch (*(unsigned char *)(arg0 + 4)) {
    case 0:
        switch (*(unsigned char *)(arg0 + 0xD)) {
        case 0:
            *(float *)(arg0 + 0xB4) += 7.0f;
            break;
        case 1:
            *(float *)(arg0 + 0xB4) += 3.5f;
            break;
        case 5:
            *(float *)(arg0 + 0xB4) += 7.0f;
            break;
        case 6:
            *(float *)(arg0 + 0xB4) += 3.5f;
            break;
        case 4:
            *(float *)(arg0 + 0xB4) += 2.0f;
            break;
        }
        func_001029C0(arg0 + 0xD0);
        func_00102C58((void *)(arg0 + 0xD0), (int)(arg0 + 0xD0), (float *)(arg0 + 0xC0));
        func_00102918(arg0 + 0xD0, arg0 + 0xD0, arg0 + 0xB0);
        func_001F2E90(arg0, D_0025A350 + (*(unsigned char *)(arg0 + 0xD) * 0x60));
        return;
    case 1:
        miss = *(unsigned char *)(arg0 + 0xC);
        for (i = 0, k = 0; i < *(unsigned char *)(arg0 + 0xC); i++) {
            ent = *(char **)(D_00275B40 + k);
            if (*(short *)(ent + 0x80) == 0) {
                func_001F3620(ent, *(unsigned char *)(arg0 + 0xD));
                if (*(short *)(ent + 0x80) == 0) {
                    row = (int *)(D_0025A350 + (*(unsigned char *)(arg0 + 0xD) * 0x60));
                    func_001F3E30(D_700036A0, ent + 0x40, row[20], row[21 + i % 2], row[23]);
                }
            } else {
                miss--;
            }
            k += 4;
        }
        if (miss == 0) {
            *(unsigned char *)(arg0 + 4) = 3;
        }
        for (j = 0, cnt = 0, m = 0; j < *(unsigned char *)(arg0 + 0xC); j++) {
            ent = *(char **)(D_00275B40 + m);
            if (*(int *)(ent + 0x78) == 0) {
                func_001F3340(ent, D_0025A350 + (*(unsigned char *)(arg0 + 0xD) * 0x60), *(unsigned char *)(arg0 + 0xD));
                if (++cnt >= 4) {
                    return;
                }
            }
            m += 4;
        }
        return;
    case 2:
    case 3:
        func_001AFC10((unsigned char *)arg0);
        break;
    }
}
