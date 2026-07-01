// NEARMISS func_001814E0  (vram 0x001814E0, 0x248 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.35% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 97.35% mwcc233 near-miss (logic fully recovered). Residual = arg-setup register order for func_001026A0 (a1 arg0+0xD0 vs a2 &D_700038A0 emitted swapped) + one entry-block nop / return-0 delay-slot fill placement. Register/scheduling permutation, not source-controllable. 991 build = 89.6%. Permute...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 97.35% (mwcc233). Logic fully recovered. Sole residuals are compiler
// scheduling artifacts: (1) the func_001026A0 call sets up its a1 (arg0+0xD0) and
// a2 (&D_700038A0) argument registers in the opposite order from the target, and
// (2) one entry-block store nop / return-0 zero-fill delay-slot placement swap.
// Not source-controllable (tried arg temp, symbol/array/literal address forms).
// Builds a rotation scratch at D_700038A0 (angle picked by arg1==0 / actor byte
// 0x23F==3 / else), applies it via func_001026A0+func_001031E0, advances the
// D_700038C4 float by 25.0, and if func_0019A570 succeeds copies actor byte 0xD
// from *(0x700031D0)+0x1A, returning 1 (or 2 in the second, 23F==3 pass) when
// that byte is 0x34 or 0x1E; else re-tries once for the 23F==3 case; else 0.
extern int func_001026A0(float *, char *, float *);
extern int func_001031E0(float *, float *);
extern int func_0019A570(float *, float *, int, int);
extern char D_700038A0;
extern char D_700038B0;
extern char D_700038C0;

int func_001814E0(char *arg0, int arg1) {
    unsigned char v;

    if (arg1 == 0) {
        *(int *)0x700038A0 = 0;
        *(int *)0x700038A4 = 0;
        *(int *)0x700038A8 = 0x408CCCCD;
        *(int *)0x700038AC = 0x3F800000;
    } else if (*(unsigned char *)(arg0 + 0x23F) == 3) {
        *(int *)0x700038A0 = 0;
        *(int *)0x700038A4 = 0;
        *(int *)0x700038A8 = 0x41326666;
        *(int *)0x700038AC = 0x3F800000;
    } else {
        *(int *)0x700038A0 = 0;
        *(int *)0x700038A4 = 0;
        *(int *)0x700038A8 = 0x40D4CCCD;
        *(int *)0x700038AC = 0x3F800000;
    }
    func_001026A0((float *)&D_700038B0, arg0 + 0xD0, (float *)&D_700038A0);
    func_001031E0((float *)&D_700038C0, (float *)&D_700038B0);
    { float _c = 25.0f; *(float *)0x700038C4 = *(float *)0x700038C4 + _c; }
    if (func_0019A570((float *)&D_700038B0, (float *)&D_700038C0, 4, 0) != 0) {
        *(unsigned char *)(arg0 + 0xD) = *(unsigned char *)(*(char **)0x700031D0 + 0x1A);
        v = *(unsigned char *)(arg0 + 0xD);
        if (v == 0x34 || v == 0x1E) {
            return 1;
        }
    } else if (*(unsigned char *)(arg0 + 0x23F) == 3) {
        *(int *)0x700038A0 = 0;
        *(int *)0x700038A4 = 0;
        *(int *)0x700038A8 = 0x40D4CCCD;
        *(int *)0x700038AC = 0x3F800000;
        func_001026A0((float *)&D_700038B0, arg0 + 0xD0, (float *)&D_700038A0);
        func_001031E0((float *)&D_700038C0, (float *)&D_700038B0);
        { float _c = 25.0f; *(float *)0x700038C4 = *(float *)0x700038C4 + _c; }
        if (func_0019A570((float *)&D_700038B0, (float *)&D_700038C0, 4, 0) != 0) {
            *(unsigned char *)(arg0 + 0xD) = *(unsigned char *)(*(char **)0x700031D0 + 0x1A);
            v = *(unsigned char *)(arg0 + 0xD);
            if (v == 0x34 || v == 0x1E) {
                return 2;
            }
        }
    }
    return 0;
}
