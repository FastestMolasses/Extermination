// NEARMISS func_00206BF0  (vram 0x00206BF0, 0xC8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.40% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Single-store addressing-mode/scheduling artifact: the CW target materializes &v34 into a register (addiu v1,sp,0x34) as the first of the five pre-call address setups and stores D_00275850 indirectly (sw v0,0(v1)) in the func_00206B00 delay slot; mwcc 2.3.3 instead emits the equivalent direct sw v...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// NEARMISS 97.4% (mwcc233). Body/logic fully recovered; sole residual is a single
// store addressing-mode/scheduling artifact (see wall).
// Reads gp-rel global D_00275850 into local v34, calls func_00206B00 to fill four
// out-params (v2C,v38,v30,v3C), early-returns 0 if v38+v3C<4. Builds two GIT/DMA-
// tag words via the (x<<36)>>36 | 0x20000000 dsll32/dsrl32 mask idiom and calls
// func_00207150(tag(v2C), v38, tag(v30), v3C, &v34, 4, 0, 0); feeds its result
// into func_00206B10(D_002DF740, r); calls func_00204B80(a0+0x48); then
// if (*(int*)(a0+0xA8)==0) *(int*)(a0+0xA8)=2; returns 1.
extern int func_00206B00(void *a0, int *a1, int *a2, int *a3, int *a4);
extern int func_00207150();
extern int func_00206B10();
extern void func_00204B80(void *a0);

extern int D_00275850;
extern char D_002DF740[];

int func_00206BF0(unsigned char *a0) {
    int v2C, v30, v34, v38, v3C;
    int r;

    v34 = D_00275850;
    func_00206B00(a0, &v2C, &v38, &v30, &v3C);
    if (v38 + v3C < 4) {
        return 0;
    }
    r = func_00207150((unsigned long long)v2C << 36 >> 36 | 0x20000000,
                      v38,
                      (unsigned long long)v30 << 36 >> 36 | 0x20000000,
                      v3C, &v34, 4, 0, 0);
    func_00206B10(D_002DF740, r);
    func_00204B80((char *)a0 + 0x48);
    if (*(int *)(a0 + 0xA8) == 0) {
        *(int *)(a0 + 0xA8) = 2;
    }
    return 1;
}
