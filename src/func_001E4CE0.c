// NEARMISS func_001E4CE0  (vram 0x001E4CE0, 0x910 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.59% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4) (was 83.90%; matching wave 2026-10-27).
// Fixes read from the original: func_001CD2B0 takes four floats (no variant argument); the
// area check reads D_00810700/1 as non-gp arrays; the variant-6/7 test, the area test and the
// a[0] choice are switches; the state cases end in break; limit is set after the last
// scratchpad store; the jitter is staged in a float local (cvt, /=, +=) before the seed step;
// the actor arrives as an int and is cast into a local (gives the original's saved registers);
// 0x70003A30 is the relocated scratchpad extern D_70003A30 (idiom-32).
// Remaining differences: one scheduling choice - the third func_001CFB50 call materialises
// its 1.0f/1e-6 constants in a different order (~8 rows of 580; idiom-24 int-to-float
// locals and further SPAD externs for 0x70003A20..34 did not help).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// SPAD: 0x70003A30

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
//      func_001CD2B0, the LOD pass is 1 (6 once D_00275C00 > 0x100), and then
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
extern float func_001CD2B0(float w, float h, float u, float v);
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
extern float D_70003A30[];
extern unsigned char D_00810700[];
extern unsigned char D_00810701[];

typedef struct {
    int seed;
    int frame;
    float age;
    float step;
} Burst;

void func_001E4CE0(int arg0) {
    char desc[0x60];
    char *set;
    Burst *sub;
    unsigned char *a;
    int handle;
    int seed;
    int limit;
    int pass;
    int lod;
    float age;
    float step;
    float len;
    float scale;
    float jit;

    a = (unsigned char *)arg0;
    sub = (Burst *)(a + 0x1F0);
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
        switch (a[0xD]) {
        case 4:
            a[0] = 2;
            break;
        default:
            a[0] = 1;
            break;
        }
        func_001029C0(a + 0xD0);
        func_00102C58(a + 0xD0, a + 0xD0, a + 0xC0);
        func_00102918(a + 0xD0, a + 0xD0, a + 0xB0);
        sub->frame = 0;
        sub->seed = func_00122BB8();
        sub->age = 0.1f;
        sub->step = 0.1f;
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
            *(volatile int *)0x70003A2C = 0x40400000;
            limit = 0x1E;
            break;
        case 2:
            set = D_002543F0;
            *(volatile int *)0x70003A20 = 0x41600000;
            *(volatile int *)0x70003A24 = 0x41600000;
            *(volatile int *)0x70003A28 = 0x41100000;
            *(volatile int *)0x70003A2C = 0x40A00000;
            limit = 0x1E;
            break;
        case 3:
            set = D_00254510;
            *(volatile int *)0x70003A20 = 0x41600000;
            *(volatile int *)0x70003A24 = 0x41900000;
            *(volatile int *)0x70003A28 = 0x41100000;
            *(volatile int *)0x70003A2C = 0x41200000;
            limit = 0x1E;
            break;
        case 4:
            set = D_00254630;
            *(volatile int *)0x70003A20 = 0x41600000;
            *(volatile int *)0x70003A24 = 0x42200000;
            *(volatile int *)0x70003A28 = 0x41100000;
            *(volatile int *)0x70003A2C = 0x41A00000;
            limit = 0x1E;
            break;
        case 5:
            set = D_00254630;
            *(volatile int *)0x70003A20 = 0x41600000;
            *(volatile int *)0x70003A24 = 0x42200000;
            *(volatile int *)0x70003A28 = 0x41100000;
            *(volatile int *)0x70003A2C = 0x41A00000;
            limit = 0x3E7;
            break;
        case 6:
            set = D_00254750;
            *(volatile int *)0x70003A24 = 0;
            *(volatile int *)0x70003A20 = 0x41400000;
            *(volatile int *)0x70003A28 = 0x41100000;
            *(volatile int *)0x70003A2C = 0x41A00000;
            limit = 0x1E;
            break;
        case 7:
            set = D_00254AB0;
            *(volatile int *)0x70003A20 = 0x41600000;
            *(volatile int *)0x70003A24 = 0x42200000;
            *(volatile int *)0x70003A28 = 0x41100000;
            *(volatile int *)0x70003A2C = 0x41A00000;
            limit = 0x1E;
            break;
        case 8:
            set = D_00254900;
            *(volatile int *)0x70003A24 = 0;
            *(volatile int *)0x70003A20 = 0x41400000;
            *(volatile int *)0x70003A28 = 0x41100000;
            *(volatile int *)0x70003A2C = 0x41A00000;
            limit = 0x1E;
            break;
        case 9:
        case 12:
        case 13:
            set = D_00254BD0;
            *(volatile int *)0x70003A24 = 0;
            *(volatile int *)0x70003A20 = 0x41600000;
            *(volatile int *)0x70003A28 = 0x41100000;
            *(volatile int *)0x70003A2C = 0x40A00000;
            limit = 0x1E;
            break;
        case 14:
            set = D_00254D80;
            *(volatile int *)0x70003A24 = 0;
            *(volatile int *)0x70003A20 = 0x41200000;
            *(volatile int *)0x70003A28 = 0x3F800000;
            *(volatile int *)0x70003A2C = 0x41F00000;
            limit = 0x1E;
            break;
        }
        seed = sub->seed;
        handle = func_001CD070(a + 0x100, 0x30);
        switch (a[0xD]) {
        case 6:
        case 7:
            switch ((D_00810700[0] << 8) + D_00810701[0]) {
            case 0x1001:
                if (!(*(float *)(a + 0x100) <= 275.0f) && *(float *)(a + 0x100) < 335.0f) {
                    if (!(*(float *)(a + 0x108) <= 170.0f) && *(float *)(a + 0x108) < 230.0f) {
                        handle += 0x40000;
                    }
                }
                break;
            }
            break;
        }
        if (handle != 0xFFFFFF) {
            scale = 2.0f * **(float **)(a + 0x30);
            *(volatile float *)0x70003A34 = scale;
            D_70003A30[0] = func_001CD2B0(scale, scale, 256.0f, 256.0f);
            if (D_00275C00 > 0x100) {
                lod = 6;
            } else {
                lod = 1;
            }
            pass = lod & 0xFF;
            len = *(volatile float *)0x70003A24;
            if (len != 0.0f) {
                func_001E4A00(a + 0xD0, sub->age, len);
            } else {
                age = sub->age;
                if (!(age <= 0.2f)) {
                    jit = (float)((seed >> 16) & 0xFFFF);
                    jit /= 65535.0f;
                    jit += 0.0001f;
                    seed = seed * 0x25 + 0xB;
                    func_001CFB50(desc, 0, a + 0xD0, age, jit,
                                  1.0f, 0.000001f, *(volatile float *)0x70003A2C);
                    func_001CFBE0(handle, pass, set + 0x120, desc, 0);
                }
            }
            *(volatile float *)0x70003A28 = *(volatile float *)0x70003A28 * (sub->age / 1.8f);
            *(long long *)(set + 0x70) = *(long long *)(D_00253C50 + float_to_int(*(volatile float *)0x70003A28) * 8);
            jit = (float)((seed >> 16) & 0xFFFF);
            jit /= 65535.0f;
            jit += 0.0001f;
            seed = seed * 0x25 + 0xB;
            age = sub->age;
            func_001CFB50(desc, 0, a + 0xD0, age, jit,
                          1.0f, 0.000001f, *(volatile float *)0x70003A2C);
            func_001CFBE0(handle, pass, set, desc, 1);
            jit = (float)((seed >> 16) & 0xFFFF);
            jit /= 65535.0f;
            jit += 0.0001f;
            func_001CFB50(desc, 0, a + 0xD0, sub->age, jit,
                          1.0f, 0.000001f, *(volatile float *)0x70003A2C);
            func_001CFBE0(handle, pass, set + 0x90, desc, 0);
        }
        step = sub->step;
        step = step + ((0.01f - step) / *(volatile float *)0x70003A20);
        sub->step = step;
        if (step < 0.01f) {
            step = 0.01f;
        }
        sub->step = step;
        age = sub->age + step;
        sub->age = age;
        if (!(age < 1.8f)) {
            a[4] = 3;
            break;
        }
        sub->frame = sub->frame + 1;
        if (limit < sub->frame) {
            a[0] = 2;
        }
        switch (a[0xD]) {
        case 14:
        case 5:
            func_001B1B70(a);
            break;
        default:
            func_001B17A0(a);
            break;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(a);
        break;
    }
}
