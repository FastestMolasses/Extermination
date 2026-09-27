// NEARMISS func_0019D770  (vram 0x0019D770, 0x3D8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.13% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The body follows the original instructions;
// the residual diff is code generation only:
// Body corrected 2026-09-26 against the original instructions, as its twin func_0019CF50.c was (was 86.58% with the wrong body): halfword stride of the six bounds at 0x70003240 (both the copy and the walk pointer), word stride of the column table at 0x70003228, halfword stride of the cell list. The bound copy sits between the two 0019F1A0 call pairs (unchanged). Residual: register coloring and loop-pointer strength reduction.
// Twin of func_0019CF50: this one has no upper bound on the face kind (only 0x51..0x53 are skipped).
// Result: 1 only if at least one face was accepted by 0019ED80 during the walk and the word at
// 0x700031D0 read after the LAST accepted face is nonzero. On its success path 0019ED80 stores
// its face argument (the accepted face record's address) there, and on failure stores nothing
// to the scratchpad, so the word is the last accepted face's address: the result is 0 only if
// no face was accepted or that address is 0. On 1, 0x700031B0..B8 is restored from the last
// accepted face's point and the word is written back to 0x700031D0.
//
// NO-SPAN PATH (caller registers). The walk bounds s1/s2 and the cell-list index best_i are set
// only when some span is shorter than the 0x7000320C word. If none is, the original walks with
// whatever its caller left in $s1 (start), $s2 (end) and $s4 (cell list 0x70003210 + 4 * $s4).
// C cannot express that: here s1, s2, hit, best_i are uninitialised on that path, and they are
// declared in this order ON PURPOSE so that mwcc 2.3.3 colors them $s1, $s2, $s3, $s4 exactly as
// the original does (s1, s2, best_i, hit colored best_i to $s3 and walked the wrong list). The
// only caller is 0019A910 (call at 0x0019AA04; no other reference in the boot image or any
// overlay); there $s1 is its mode byte (argument 3 & 0xFF, bit 2 set) and $s2 its frame
// address + 0x40, a main-RAM stack address (every thread stack the game sets up lies at or
// above 0x00276890), and $s4 is inherited from 0019A910's 33 call sites, not characterized.
// So from 0019A910 the no-span walk starts at cell $s1 (<= 0xFF) and runs up to $s2: millions
// of cells. Whether the game reaches this path (it needs
// every one of the six span lengths >= the 0x7000320C word) is not established; a port must
// either prove it unreachable for its data or reproduce the caller-register walk.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int func_0019ED80(void *node, void *edata);
extern void func_0019F1A0(void *seg, int mask);
extern float D_70003190;
extern float D_700031A0;
extern short D_70003240;
extern int D_70003228;
extern int D_70003210;

int func_0019D770(void) {
    int s1, s2, hit, best_i;
    int s5, s6;
    int sp80[6];
    float spA0[3];
    int i;
    short *p;
    int *q;
    int v0;
    short v1;
    short a2, a3;
    int t0;

    hit = 0;
    if (!(*(float *)0x70003190 <= *(float *)0x700031A0)) {
        s5 = 1;
        s6 = 2;
    } else {
        s5 = 2;
        s6 = 1;
    }
    if (!(*(float *)0x70003194 <= *(float *)0x700031A4)) {
        s5 |= 4;
        s6 |= 8;
    } else {
        s5 |= 8;
        s6 |= 4;
    }
    if (!(*(float *)0x70003198 <= *(float *)0x700031A8)) {
        s5 |= 0x10;
        s6 |= 0x20;
    } else {
        s5 |= 0x20;
        s6 |= 0x10;
    }

    func_0019F1A0(&D_70003190, s6);
    func_0019F1A0(&D_700031A0, s5);

    for (i = 0; i < 6; i++) {
        sp80[i] = (&D_70003240)[i];
    }

    func_0019F1A0(&D_70003190, s5);
    func_0019F1A0(&D_700031A0, s6);

    v0 = *(int *)0x7000320C;
    p = &D_70003240;
    q = &D_70003228;
    for (i = 0; i < 6; i++) {
        if (i & 1) {
            *(short *)0x70003B86 = *p;
            a2 = *(short *)(*q + sp80[i] * 2);
        } else {
            *(short *)0x70003B86 = *(short *)(*q + sp80[i] * 2);
            *(short *)0x70003B88 = *p;
            a2 = *(short *)0x70003B88 + 1;
        }
        *(short *)0x70003B88 = a2;
        a3 = *(short *)0x70003B88;
        a2 = *(short *)0x70003B86;
        t0 = a3 - a2;
        if (t0 < v0) {
            v0 = t0;
            s1 = a2;
            s2 = a3;
            best_i = i;
        }
        p += 1;
        q += 1;
    }

    if (s1 < s2) {
        short *node = (short *)(*(&D_70003210 + best_i)) + s1;
        do {
            char *e = (char *)(*(int *)0x70003208) + (*node << 6);
            node += 1;
            if (*(short *)0x70003240 >= *(short *)(e + 0xC) &&
                *(short *)(e + 0xE) >= *(short *)0x70003242 &&
                *(short *)0x70003248 >= *(short *)(e + 0x14) &&
                *(short *)(e + 0x16) >= *(short *)0x7000324A &&
                *(short *)0x70003244 >= *(short *)(e + 0x10) &&
                *(short *)(e + 0x12) >= *(short *)0x70003246) {
                short st = *(unsigned char *)(e + 0x1A);
                *(short *)0x70003B88 = st;
                st = *(short *)0x70003B88;
                if (st < 0x51 || st >= 0x54) {
                    if (func_0019ED80(&D_70003190, e)) {
                        int k;
                        for (k = 0; k < 3; k++) {
                            *(float *)(0x70003190 + 0x10 + k * 4) = *(float *)(0x70003190 + 0x20 + k * 4);
                            spA0[k] = *(float *)(0x70003190 + 0x20 + k * 4);
                        }
                        hit = *(int *)0x700031D0;
                    }
                }
            }
            s1 += 1;
        } while (s1 < s2);
    }

    v0 = 0;
    if (hit != 0) {
        int k;
        for (k = 0; k < 3; k++) {
            *(float *)(0x70003190 + 0x20 + k * 4) = spA0[k];
        }
        v0 = 1;
        *(int *)0x700031D0 = hit;
    }
    return v0;
}
