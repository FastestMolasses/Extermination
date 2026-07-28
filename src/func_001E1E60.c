// NEARMISS func_001E1E60  (vram 0x001E1E60, 0x400 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.43% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// PARK/NEARMISS — mwcc 2.3.3 saved-register allocation-ORDER permutation (the documented '2.3.3 does NOT fix regalloc permutation' class). Under 2.3.3 every one of the 27 residual diff rows is register-NAME-only: 2.3.3 demotes the across-calls param copy (idx) to s4 where the target has s1, rotatin...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Render-pass/VIF packet builder for pass `idx` (D_00275670 render-context family,
// same family as func_001D49D0). Saves the pass's current packet write node
// (ctx + idx*4 + 0x10) as the return value, then: func_001D1F80(idx,0,7) +
// func_001D1FF0(idx,0) state setup; func_001D6F60(idx, *(64-bit)(ctx+0x1D0), 0x80);
// packs an RGBA word from four float channels at ctx+0x1C0..0x1CC (each *128 ->
// int, A<<24|B<<16|G<<8|R) into func_001D7080(idx, rgba, 1.0f); copies matrix
// ctx+0x2380 into D_00253570 (func_00102798), doubles the scale vec4 at
// D_00253580..8C (volatile = keeps the 4 read-modify-writes serialized like the
// target), multiplies D_00253570 by an X/Y-swap matrix (func_001026D0), latches
// D_002535B8 = ctx+0x2468; uploads via func_001D7100: hdr D_00253560 at VU addrs
// 0/0x81/0x102 (0x10 bytes) and matrix D_00253570 at 0x200 (0x80 bytes); appends
// ref tag D_0023C990 and func_001D71A0(idx,0). If debug flag 0x23: picks sound
// params snd[4]+vol[4] by flag 0x24 (ids 0x4B4E4514/0xCDC3D411, level 16+16*rand
// or 32+32*rand in [0,1)), stops the previous handle D_0027569C (func_001D73A0)
// if any, starts a new one (func_001D72D0), func_001D1F80(idx,2,2), submits
// func_001D75E0(idx, snd, 0x2007780621322A00 GS-reg qword, 0x80808080, 384.0f,
// 384.0f), and if the old handle reported 0, func_001CEFD0(snd, vol). Finally
// func_001D2040(idx,1) and recycles the packet node: byte+3=0x60, word+4=0,
// half+0=0, advance by 0x10; returns the node pointer saved at entry.
// (`off = arg1; i = off; off *= 4;` is the match shape for off=idx*4, i=idx.)

extern void func_001D1F80(int, int, int);
extern void func_001D1FF0(int, int);
extern void func_001D6F60(int, long, int);
extern int func_00128250(float);
extern void func_001D7080(int, int, float);
extern void func_00102798(void *, void *);
extern void func_001026D0(void *, void *, void *);
extern void func_001D7100();
extern void vif_append_ref_tag();
extern void func_001D71A0();
extern int func_001D2910();
extern int func_00122BB8(void);
extern int func_001D73A0(int);
extern int func_001D72D0(int *);
extern void func_001D75E0(int, int *, long, int, float, float);
extern void func_001CEFD0(int *, float *);
extern void func_001D2040(int, int);

extern char *D_00275670;
extern int D_0027569C;
extern float D_00253560[4];
extern float D_00253570[16];
extern volatile float D_00253580[2];
extern volatile float D_00253584[2];
extern volatile float D_00253588[2];
extern volatile float D_0025358C[2];
extern float D_002535B8[2];
extern int D_0023C990[2];

int func_001E1E60(int arg0, int arg1) {
    int off;
    int i;
    int color;
    int ret;
    char *g;
    float m[4][4];
    int snd[4];
    float vol[4];
    int old;
    float f;
    char *p;
    int n;

    off = arg1;
    i = off;
    off *= 4;
    ret = *(int *)((off + (int)D_00275670) + 0x10);
    n = 7;
    func_001D1F80(i, 0, n);
    func_001D1FF0(i, 0);
    n = 0x80;
    func_001D6F60(i, *(long *)(D_00275670 + 0x1D0), n);
    g = D_00275670;
    color = func_00128250(128.0f * *(float *)(g + 0x1CC)) << 24;
    color |= func_00128250(128.0f * *(float *)(g + 0x1C8)) << 16;
    n = func_00128250(128.0f * *(float *)(g + 0x1C4)) << 8;
    color = n | color;
    n = func_00128250(128.0f * *(float *)(g + 0x1C0));
    func_001D7080(i, n | color, 1.0f);
    func_00102798(D_00253570, D_00275670 + 0x2380);
    D_00253580[0] *= 2.0f;
    m[0][0] = 0.0f;
    m[1][0] = 1.0f;
    m[2][0] = 0.0f;
    m[3][0] = 0.0f;
    m[0][1] = 1.0f;
    D_00253584[0] *= 2.0f;
    m[1][1] = 0.0f;
    m[2][1] = 0.0f;
    m[3][1] = 0.0f;
    m[0][2] = 0.0f;
    m[1][2] = 0.0f;
    m[2][2] = 1.0f;
    m[3][2] = 0.0f;
    m[0][3] = 0.0f;
    D_00253588[0] *= 2.0f;
    m[1][3] = 0.0f;
    m[2][3] = 0.0f;
    m[3][3] = 1.0f;
    D_0025358C[0] *= 2.0f;
    func_001026D0(D_00253570, m, D_00253570);
    n = 0x10;
    D_002535B8[0] = *(float *)(D_00275670 + 0x2468);
    func_001D7100(i, 0, D_00253560, n);
    n = 0x10;
    func_001D7100(i, 0x81, D_00253560, n);
    n = 0x10;
    func_001D7100(i, 0x102, D_00253560, n);
    n = 0x80;
    func_001D7100(i, 0x200, D_00253570, n);
    vif_append_ref_tag(i, D_0023C990);
    func_001D71A0(i, 0);
    if (func_001D2910(0x23)) {
        if (func_001D2910(0x24)) {
            f = 16.0f + 16.0f * (4.656613e-10f * (float)func_00122BB8());
            snd[0] = 0x4B4E4514;
            snd[1] = 0xC1200000;
            snd[2] = 0x4E1D842D;
            snd[3] = 0;
            vol[3] = f;
            vol[2] = f;
            vol[1] = f;
            vol[0] = f;
        } else {
            f = 32.0f + 32.0f * (4.656613e-10f * (float)func_00122BB8());
            snd[0] = 0xCDC3D411;
            snd[1] = 0xC1200000;
            snd[2] = 0x4E1D842D;
            snd[3] = 0;
            vol[3] = f;
            vol[2] = f;
            vol[1] = f;
            vol[0] = f;
        }
        old = (D_0027569C != -1) ? func_001D73A0(D_0027569C) : -1;
        D_0027569C = func_001D72D0(snd);
        func_001D1F80(i, 2, 2);
        func_001D75E0(i, snd, 0x2007780621322A00L, 0x80808080, 384.0f, 384.0f);
        if (old == 0) {
            func_001CEFD0(snd, vol);
        }
    }
    func_001D2040(i, 1);
    p = D_00275670 + off;
    *(char *)(*(char **)(p + 0x10) + 3) = 0x60;
    *(int *)(*(char **)(p + 0x10) + 4) = 0;
    *(short *)(*(char **)(p + 0x10) + 0) = 0;
    *(char **)(p + 0x10) = *(char **)(p + 0x10) + 0x10;
    return ret;
}
