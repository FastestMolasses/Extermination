// NEARMISS func_001370C0  (vram 0x001370C0, 0x2E8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.88% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Paired float-register scheduling/coloring artifact (FPU-coloring class): after the func_001B15D0 call (whose result occupies the $f0/$f1 pair), the target immediately reloads *(float*)(rec+0xB4) directly into the ODD HALF of that same register pair ($f1 / "fv0f") for the subsequent store-back and...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Spawns/initializes an actor-preset record via func_001AFA90(2). arg0
// selects a row in the table D_00275B40 (each row's field+0x90 feeds the
// bone-copy); arg1 (masked to bit 1) selects between a fixed preset
// (230,340,1) written straight into the scratch block 0x700038D0..DC, or a
// copy of the current default preset via func_00102948(D_700038D0,
// D_00810360). That quadword is then split into the record's type field
// (+0x1F0) and float triple (p = record+0x1F0; +4/+8/+0xC). Builds a second
// scratch vector at 0x700038A0..AC (17.0, 0, 0, 1), copies the row's +0x90
// bone via copy_qw4, transforms it with func_001026A0, zeroes the working
// vector D_700036D0..D8, and copies the transformed vector into the record's
// +0xB0..B8. Computes func_001B15D0 between the current and the (possibly
// preset) target, derives a clamped-positive delta float, and picks p+0x24
// (heading) from func_0011E620/func_001B1470 of the record's D_700036A0
// bone axes. If the masked bit is set, p+0x28 is forced to -0.06981317f;
// otherwise it is derived the same way from the delta/derived-heading pair,
// clamped to 0.7853982f. Finally derives 3 scaled trig fields (+0xC0/C4/C8)
// from p+0x24/+0x28 via func_0011E2A8/func_0011DE90 scaled by 2.2.
extern void copy_qw4(void *a0, void *a1);
extern int func_001026A0(void *a0, void *a1, void *a2);
extern void func_00102948(void *a0, void *a1);
extern float func_0011DE90(float a);
extern float func_0011E2A8(float a);
extern float func_0011E620(float a, float b);
extern char *func_001AFA90(int a);
extern float func_001B1470(float a);
extern float func_001B15D0(void *a0, void *a1);
extern int D_00275B40;
extern unsigned char D_00810360[16];
extern unsigned char D_700036A0[16];
extern unsigned char D_700038A0[16];
extern unsigned char D_700038B0[16];
extern unsigned char D_700038D0[16];
extern void func_001373B0();

void func_001370C0(int arg0, unsigned char arg1) {
    char *rec;
    char *p;
    int mask;
    float t;
    float ang;
    float d;

    rec = func_001AFA90(2);
    if (rec == 0) {
        return;
    }
    *(char *)(rec + 3) = 3;
    mask = (arg1 & 0xFF) & 2;
    *(char *)(rec + 0xD) = arg1;
    p = rec + 0x1F0;
    *(int *)(rec + 0x10) = (int)&func_001373B0;
    if (mask != 0) {
        *(int *)0x700038D0 = 0x428C0000;
        *(float *)0x700038D4 = 230.0f;
        *(int *)0x700038D8 = 0x43870000;
        *(int *)0x700038DC = 0x3F800000;
    } else {
        func_00102948(D_700038D0, D_00810360);
    }
    *(float *)p = *(float *)0x700038D0;
    *(float *)(p + 4) = *(float *)0x700038D4;
    *(float *)(p + 8) = *(float *)0x700038D8;
    *(float *)(p + 0xC) = *(float *)0x700038DC;
    copy_qw4(D_700036A0, *(char **)(D_00275B40 + arg0 * 4) + 0x90);
    *(int *)0x700038A0 = 0x41100000;
    *(float *)0x700038A4 = 0.0f;
    *(int *)0x700038A8 = 0;
    *(int *)0x700038AC = 0x3F800000;
    func_001026A0(D_700038A0, D_700036A0, D_700038A0);
    *(int *)0x700036D8 = 0;
    *(int *)0x700036D4 = 0;
    *(int *)0x700036D0 = 0;
    *(float *)(rec + 0xB0) = *(float *)0x700038A0;
    *(float *)(rec + 0xB4) = *(float *)0x700038A4;
    *(float *)(rec + 0xB8) = *(float *)0x700038A8;
    func_00102948(D_700038B0, D_700038D0);
    *(int *)0x700038B4 = 0;
    *(int *)0x700038A4 = 0;
    t = func_001B15D0(D_700038A0, D_700038B0);
    d = (20.0f + *(float *)0x700038D4) - *(float *)(rec + 0xB4);
    *(float *)0x700038A4 = *(float *)(rec + 0xB4);
    if (d < 0.0f) {
        d = 0.0f;
    }
    *(float *)(p + 0x24) = func_001B1470(func_0011E620(*(float *)0x700036A0, *(float *)0x700036A8));
    if (mask != 0) {
        *(float *)(p + 0x28) = -0.06981317f;
    } else {
        ang = func_001B1470(func_0011E620(d, t));
        *(float *)(p + 0x28) = ang;
        if (ang > 0.7853982f) {
            *(float *)(p + 0x28) = 0.7853982f;
        }
    }
    *(float *)(rec + 0xC0) = 2.2f * func_0011E2A8(*(float *)(p + 0x24));
    *(float *)(rec + 0xC4) = 2.2f * func_0011E2A8(*(float *)(p + 0x28));
    *(float *)(rec + 0xC8) = 2.2f * func_0011DE90(*(float *)(p + 0x24));
}
