// NEARMISS func_001E4CE0  (vram 0x001E4CE0, 0x910 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 83.90% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 83.90 with mwcc 2.3.3 (-O4,p -sdatathreshold 4); ~185 diff rows out of 580 instructions (991202 82.05, mwcc 2.4 81.31). Decode verified against the whole .s including both jump tables; two real decode bugs were found and fixed during verification (m2c's raw output was right, my first transcriptio...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// SEMANTICS: per-frame update of a "burst/spray" particle emitter actor,
// dispatched on the life-cycle byte arg0[4] (0 = init, 1 = run, 2/3 = free).
// arg0[0xD] is the emitter VARIANT (2..14); arg0+0x1F0 is the emitter scratch
// block: +0x00 rng seed, +0x04 frame counter, +0x08 age, +0x0C age step.
//   0 (init): pick the per-variant parameter table for arg0+0x30 (jtbl_0026E930,
//      variants 0/1/11 keep whatever is there); variants 9/12/13 also register
//      the 0x8000006E sound at arg0+0xB0. arg0[0] becomes 2 for variant 4 and 1
//      otherwise. Normalise the orientation at +0xD0 and compose it with +0xC0
//      then +0xB0, zero the frame counter, seed the rng from func_00122BB8,
//      set age = age step = 0.1, install the think-fn func_001E49F0, clear
//      arg0[0xC] / arg0[9] and advance to state 1 (falls through).
//   1 (run): a second per-variant switch (jtbl_0026E8F0) selects the mesh set
//      (s0) plus the four scratch floats at 0x70003A20..0x2C - respectively the
//      age-step divisor, the "stretch" length (0 disables the stretch path),
//      the width and the fade parameter - and the lifetime cap `limit`.
//      NOTE variants 0, 1 and 11 select NOTHING here: the mesh pointer and the
//      lifetime cap are left UNINITIALISED, exactly as in the original.
//      A texture/material handle comes from func_001CD070(arg0+0x100, 0x30);
//      for variants 6 and 7 in stage 0x10 area 0x01, an emitter standing inside
//      the box x in (275,335) and z in (170,230) gets +0x40000 added to it.
//      Unless the handle is the 0xFFFFFF "none", the emitter is drawn: the
//      scale comes from 2 * the first float of the variant table via
//      func_001CD2B0, the LOD pass is 1 (6 once D_00275C00 >= 0x101), and then
//      the stretched quad (func_001E4A00) when 0x70003A24 is non-zero, or else
//      the +0x120 particle batch once the age is past 0.2. EITHER WAY the base
//      batch and the +0x90 batch then follow, with the animation frame picked
//      out of D_00253C50 by float_to_int of width * age/1.8 (stored back into
//      0x70003A28) and written to set+0x70. Each func_001CFB50/func_001CFBE0
//      batch consumes one step of the seed*37+11 LCG for its jitter.
//      Finally the age step eases toward 0.01, the age advances by it, and at
//      age >= 1.8 the emitter goes to state 3; otherwise the frame counter ticks
//      (arg0[0] = 2 past `limit`) and the actor is re-registered with
//      func_001B1B70 for variants 5 and 14, func_001B17A0 for the rest.
//   2, 3: func_001AFC10 frees the actor.
extern void func_001EFD20(int id, void *at);
extern void func_001029C0(void *m);
extern void func_00102C58(void *d, void *s, void *r);
extern void func_00102918(void *d, void *s, void *r);
extern int func_00122BB8(void);
extern int func_001CD070(void *pos, int flags);
extern float func_001CD2B0(int kind, float w, float h, float u, float v);
extern void func_001E4A00(void *m, float age, float len);
extern void func_001CFB50(void *out, int mode, void *m, float age, float jitter, float one, float eps, float fade);
extern void func_001CFBE0(int handle, int pass, void *mesh, void *desc, int alt);
extern int float_to_int(float x);
extern void func_001B1B70(void *actor);
extern void func_001B17A0(void *actor);
extern void func_001AFC10(void *actor);
extern int func_001E49F0();

extern float D_00254230[];
extern float D_00254244[];
extern float D_00254258[];
extern float D_0025426C[];
extern float D_00254280[];
extern float D_00254294[];
extern float D_002542A8[];
extern float D_002542BC[];
extern char D_002542D0[];
extern char D_002543F0[];
extern char D_00254510[];
extern char D_00254630[];
extern char D_00254750[];
extern char D_00254900[];
extern char D_00254AB0[];
extern char D_00254BD0[];
extern char D_00254D80[];
extern char D_00253C50[];
extern int D_00275C00;
extern unsigned char D_00810700;
extern unsigned char D_00810701;

void func_001E4CE0(unsigned char *a) {
    char desc[0x60];
    char *set;
    unsigned char *sub;
    int handle;
    int seed;
    int limit;
    int pass;
    int kind;
    float age;
    float step;
    float len;
    float scale;

    sub = a + 0x1F0;
    switch (a[4]) {
    case 0:
        switch (a[0xD]) {
        case 2:
            *(int *)(a + 0x30) = (int)D_00254230;
            break;
        case 4:
            *(int *)(a + 0x30) = (int)D_00254258;
            break;
        case 5:
            *(int *)(a + 0x30) = (int)D_002542BC;
            break;
        case 6:
            *(int *)(a + 0x30) = (int)D_00254258;
            break;
        case 7:
            *(int *)(a + 0x30) = (int)D_00254258;
            break;
        case 8:
            *(int *)(a + 0x30) = (int)D_0025426C;
            break;
        case 10:
            *(int *)(a + 0x30) = (int)D_00254294;
            break;
        case 9:
        case 12:
        case 13:
            *(int *)(a + 0x30) = (int)D_00254280;
            break;
        case 14:
            *(int *)(a + 0x30) = (int)D_002542A8;
            break;
        case 3:
            *(int *)(a + 0x30) = (int)D_00254244;
            break;
        }
        switch (a[0xD]) {
        case 9:
        case 12:
        case 13:
            func_001EFD20(0x8000006E, a + 0xB0);
            break;
        }
        if (a[0xD] != 4) {
            a[0] = 1;
        } else {
            a[0] = 2;
        }
        func_001029C0(a + 0xD0);
        func_00102C58(a + 0xD0, a + 0xD0, a + 0xC0);
        func_00102918(a + 0xD0, a + 0xD0, a + 0xB0);
        *(int *)(sub + 4) = 0;
        *(int *)(sub + 0) = func_00122BB8();
        *(int *)(sub + 8) = 0x3DCCCCCD;
        *(int *)(sub + 0xC) = 0x3DCCCCCD;
        *(int *)(a + 0x34) = (int)func_001E49F0;
        a[0xC] = 0;
        a[9] = 0;
        a[4] = 1;
        /* fallthrough */
    case 1:
        switch (a[0xD]) {
        case 10:
            set = D_002542D0;
            *(volatile int *)0x70003A20 = 0x41600000;
            *(volatile int *)0x70003A24 = 0x40C00000;
            *(volatile int *)0x70003A28 = 0x41100000;
            limit = 0x1E;
            *(volatile int *)0x70003A2C = 0x40400000;
            break;
        case 2:
            set = D_002543F0;
            *(volatile int *)0x70003A20 = 0x41600000;
            *(volatile int *)0x70003A24 = 0x41600000;
            *(volatile int *)0x70003A28 = 0x41100000;
            limit = 0x1E;
            *(volatile int *)0x70003A2C = 0x40A00000;
            break;
        case 3:
            set = D_00254510;
            *(volatile int *)0x70003A20 = 0x41600000;
            *(volatile int *)0x70003A24 = 0x41900000;
            *(volatile int *)0x70003A28 = 0x41100000;
            limit = 0x1E;
            *(volatile int *)0x70003A2C = 0x41200000;
            break;
        case 4:
            set = D_00254630;
            *(volatile int *)0x70003A20 = 0x41600000;
            *(volatile int *)0x70003A24 = 0x42200000;
            *(volatile int *)0x70003A28 = 0x41100000;
            limit = 0x1E;
            *(volatile int *)0x70003A2C = 0x41A00000;
            break;
        case 5:
            set = D_00254630;
            *(volatile int *)0x70003A20 = 0x41600000;
            *(volatile int *)0x70003A24 = 0x42200000;
            *(volatile int *)0x70003A28 = 0x41100000;
            limit = 0x3E7;
            *(volatile int *)0x70003A2C = 0x41A00000;
            break;
        case 6:
            set = D_00254750;
            *(volatile int *)0x70003A24 = 0;
            *(volatile int *)0x70003A20 = 0x41400000;
            *(volatile int *)0x70003A28 = 0x41100000;
            limit = 0x1E;
            *(volatile int *)0x70003A2C = 0x41A00000;
            break;
        case 7:
            set = D_00254AB0;
            *(volatile int *)0x70003A20 = 0x41600000;
            *(volatile int *)0x70003A24 = 0x42200000;
            *(volatile int *)0x70003A28 = 0x41100000;
            limit = 0x1E;
            *(volatile int *)0x70003A2C = 0x41A00000;
            break;
        case 8:
            set = D_00254900;
            *(volatile int *)0x70003A24 = 0;
            *(volatile int *)0x70003A20 = 0x41400000;
            *(volatile int *)0x70003A28 = 0x41100000;
            limit = 0x1E;
            *(volatile int *)0x70003A2C = 0x41A00000;
            break;
        case 9:
        case 12:
        case 13:
            set = D_00254BD0;
            *(volatile int *)0x70003A24 = 0;
            *(volatile int *)0x70003A20 = 0x41600000;
            *(volatile int *)0x70003A28 = 0x41100000;
            limit = 0x1E;
            *(volatile int *)0x70003A2C = 0x40A00000;
            break;
        case 14:
            set = D_00254D80;
            *(volatile int *)0x70003A24 = 0;
            *(volatile int *)0x70003A20 = 0x41200000;
            *(volatile int *)0x70003A28 = 0x3F800000;
            limit = 0x1E;
            *(volatile int *)0x70003A2C = 0x41F00000;
            break;
        }
        seed = *(int *)(sub + 0);
        kind = a[0xD];
        handle = func_001CD070(a + 0x100, 0x30);
        if (kind == 7 || kind == 6) {
            kind = (D_00810700 << 8) + D_00810701;
            if (kind == 0x1001) {
                if (!(*(float *)(a + 0x100) <= 275.0f) && *(float *)(a + 0x100) < 335.0f) {
                    if (!(*(float *)(a + 0x108) <= 170.0f) && *(float *)(a + 0x108) < 230.0f) {
                        handle += 0x40000;
                    }
                }
            }
        }
        if (handle != 0xFFFFFF) {
            scale = 2.0f * **(float **)(a + 0x30);
            *(volatile float *)0x70003A34 = scale;
            *(volatile float *)0x70003A30 = func_001CD2B0(kind, scale, scale, 256.0f, 256.0f);
            pass = 1;
            if (D_00275C00 >= 0x101) {
                pass = 6;
            }
            pass = pass & 0xFF;
            len = *(volatile float *)0x70003A24;
            if (len != 0.0f) {
                func_001E4A00(a + 0xD0, *(float *)(sub + 8), len);
            } else {
                age = *(float *)(sub + 8);
                if (!(age <= 0.2f)) {
                    func_001CFB50(desc, 0, a + 0xD0,
                                  age,
                                  (float)((seed >> 16) & 0xFFFF) / 65535.0f + 0.0001f,
                                  1.0f, 0.000001f, *(volatile float *)0x70003A2C);
                    seed = seed * 0x25 + 0xB;
                    func_001CFBE0(handle, pass, set + 0x120, desc, 0);
                }
            }
            *(volatile float *)0x70003A28 = *(volatile float *)0x70003A28 * (*(float *)(sub + 8) / 1.8f);
            *(long long *)(set + 0x70) = *(long long *)(D_00253C50 + float_to_int(*(volatile float *)0x70003A28) * 8);
            func_001CFB50(desc, 0, a + 0xD0,
                          *(float *)(sub + 8),
                          (float)((seed >> 16) & 0xFFFF) / 65535.0f + 0.0001f,
                          1.0f, 0.000001f, *(volatile float *)0x70003A2C);
            seed = seed * 0x25 + 0xB;
            func_001CFBE0(handle, pass, set, desc, 1);
            func_001CFB50(desc, 0, a + 0xD0,
                          *(float *)(sub + 8),
                          (float)((seed >> 16) & 0xFFFF) / 65535.0f + 0.0001f,
                          1.0f, 0.000001f, *(volatile float *)0x70003A2C);
            func_001CFBE0(handle, pass, set + 0x90, desc, 0);
        }
        step = *(float *)(sub + 0xC);
        step = step + ((0.01f - step) / *(volatile float *)0x70003A20);
        *(float *)(sub + 0xC) = step;
        if (step < 0.01f) {
            step = 0.01f;
        }
        *(float *)(sub + 0xC) = step;
        age = *(float *)(sub + 8) + step;
        *(float *)(sub + 8) = age;
        if (!(age < 1.8f)) {
            a[4] = 3;
            return;
        }
        *(int *)(sub + 4) = *(int *)(sub + 4) + 1;
        if (limit < *(int *)(sub + 4)) {
            a[0] = 2;
        }
        switch (a[0xD]) {
        case 5:
        case 14:
            func_001B1B70(a);
            return;
        default:
            func_001B17A0(a);
            return;
        }
    case 2:
    case 3:
        func_001AFC10(a);
        return;
    }
}
