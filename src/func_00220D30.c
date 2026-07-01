// NEARMISS func_00220D30  (vram 0x00220D30, 0x328 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 85.55% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation/scheduling permutation wall on a 3-state (0/1/2) switch dispatcher with two near-duplicate 4.5f-scaled-vec3 blocks (case 1 and case 2 bodies). Logic and structure fully recovered. The target ELF disasm shows D_00275B40 accessed via a single-instruction %gp_rel(D_00275B40)($gp)...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int func_001749A0(int a0, short a1, int a2);
extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_00102948(void *a0, void *a1);
extern void func_001028B8(void *a0, void *a1, void *a2);
extern int func_0019AFE0(char *a, void *b, void *c, int n);
extern float **D_00275B40;
extern char D_700038A0[];
extern char D_700038B0[];

void func_00220D30(char *arg0) {
    unsigned char st;
    float *p;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        func_001749A0(0x3C, 0, 0);
        func_00102948(arg0 + 0x290, arg0 + 0xB0);
        return;
    case 1:
        p = *D_00275B40;
        *(float *)0x700038A0 = p[0];
        *(float *)0x700038A4 = p[1];
        *(float *)0x700038A8 = p[2];
        *(int *)0x700038AC = 0;
        func_001026A0(D_700038B0, arg0 + 0xD0, D_700038A0);
        func_001028B8(arg0 + 0xB0, arg0 + 0x290, D_700038B0);
        func_00102948(D_700038A0, arg0 + 0x290);
        func_00102948(D_700038B0, arg0 + 0xB0);
        *(float *)0x700038B4 = 10.0f + *(float *)0x700038B4;
        *(float *)0x700038A4 = *(float *)0x700038B4;
        if (func_0019AFE0(arg0, D_700038A0, D_700038B0, 6) != 0) {
            p = *(float **)0x700031D0;
            *(float *)(arg0 + 0xB0) = *(float *)0x700031B0 + 4.5f * p[9];
            *(float *)(arg0 + 0xB8) = *(float *)0x700031B8 + 4.5f * p[0xB];
        }
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            func_001749A0((int)arg0, 0x3D, 0);
            *(float *)(arg0 + 0x2E0) = (*D_00275B40)[0];
            *(float *)(arg0 + 0x2E8) = (*D_00275B40)[2];
        }
        return;
    case 2:
        p = *D_00275B40;
        *(float *)0x700038A0 = p[0] + *(float *)(arg0 + 0x2E0);
        *(float *)0x700038A4 = p[1];
        *(float *)0x700038A8 = p[2] + *(float *)(arg0 + 0x2E8);
        *(int *)0x700038AC = 0;
        func_001026A0(D_700038B0, arg0 + 0xD0, D_700038A0);
        func_001028B8(arg0 + 0xB0, arg0 + 0x290, D_700038B0);
        func_00102948(D_700038A0, arg0 + 0x290);
        func_00102948(D_700038B0, arg0 + 0xB0);
        *(float *)0x700038B4 = 10.0f + *(float *)0x700038B4;
        *(float *)0x700038A4 = *(float *)0x700038B4;
        if (func_0019AFE0(arg0, D_700038A0, D_700038B0, 6) != 0) {
            p = *(float **)0x700031D0;
            *(float *)(arg0 + 0xB0) = *(float *)0x700031B0 + 4.5f * p[9];
            *(float *)(arg0 + 0xB8) = *(float *)0x700031B8 + 4.5f * p[0xB];
        }
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
        }
        break;
    }
}
