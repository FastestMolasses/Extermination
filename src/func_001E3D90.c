// NEARMISS func_001E3D90  (vram 0x001E3D90, 0x870 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.69% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8) (was 74.39%, then 96.81%,
// then 97.04%, matching waves 2026-10-27/28). Fixes read from the original: the room gates are switches (case 0x100; case 0/1) with
// `D_00810702 == 5 || == 6` as one condition; the three matrix-Y bumps use per-address scratchpad
// externs (idiom-32, SPAD header) so their loads hoist above the stores; the per-variant table
// pointers are set after the bumps; the LCG steps reassign `seed`; the countdown tests `== 0` first;
// the scratchpad result vector at 0x70003A20 is one float[4] extern and the three jitter values are
// computed into locals before being stored (the original reloads [0] ahead of those stores).
// 2026-10-28: the three jitter values go to the literal scratchpad addresses 0x70003A24/28/2C
// (as the later argument loads already did), written as the sibling LCG spelling (convert, /= 65535,
// seed step, += 0.0001f), and dist is read from D_70003A20[0] before those stores.
// Remaining differences: the 0.0001f constant is set up before the first seed step in the
// original (ours after it, with a0/a1 colouring of the seed chain); the quad-count byte
// (1 or 6) is built in a temp and narrowed into a saved register (ours loads the constants straight
// into it; int/short/char/ternary/if-else/cast-at-call spellings tried); case 2 of the sound-ping
// switch sets its two float arguments in the opposite order (case reorderings, default, no-break tried).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// SPAD: 0x700036D4 0x70003714 0x70003754

// Muzzle-flash / gunfire FX driver, state machine on the effect byte at +4.
// State 0 (init): seeds three flash phases (+0x200/+0x204/+0x208 as s3+0x10..)
// from rand()/2^31, captures a rand seed (+0x1F8), normalizes the local
// transform (+0xD0) against +0xB0, points +0x30 at the per-variant record
// (D_00253CA0 + variant*8) and +0x34 at the tick callback func_001E3D20, then
// falls into state 1 (run): gated on camera mode D_008101E4 and the level/room
// pair (D_00810700:D_00810701); copies the transform into three scratchpad
// slots (D_700036A0/36E0/3720), bumps per-variant flash offsets, kicks
// rumble/shake func_0021B9A0 for specific rooms, and if a draw slot is free
// (func_001CD070 != 0xFFFFFF) emits three textured flash quads
// (func_001CFAE0 + func_001CFBE0) whose UV jitter comes from an LCG
// (seed*37+11) on the captured seed. Ends by advancing the three phases by
// per-variant deltas (wrapping at 2.0), a sound ping func_001FC3C0
// (0x411/0x412/0x413 by variant) and func_001B17A0. States 2/3: teardown via
// func_001AFC10.
extern void copy_qw4(char *dst, char *src);
extern void func_00102918(char *dst, char *a, char *b);
extern void func_001029C0(char *m);
extern int func_00122BB8(void);
extern void func_001AFC10(char *p);
extern void func_001B17A0(char *p);
extern int func_001CD070(char *a, int b);
extern float func_001CD2B0(float a0, float a1, float arg2, float arg3);
extern void func_001CFAE0(int *dst, int a, char *m, float phase, float u, float v, float w);
extern void func_001CFBE0(int slot, int n, char *rec, int *src, int a);
extern void func_001FC3C0(char *p, char *tick, int id, float a, float b);
extern void func_0021B9A0(int chan, float a, float b);
extern int func_001E3D20();
extern char D_00253CA0[];
extern char D_00253CC0[];
extern char D_00253CC8[];
extern char D_00253CD0[];
extern char D_00253CE0[];
extern char D_00253E90[];
extern char D_00254040[];
extern int D_00275C00;
extern unsigned char D_008101E4[];
extern unsigned char D_00810700[];
extern unsigned char D_00810701[];
extern unsigned char D_00810702[];
extern char D_700036A0[];
extern char D_700036E0[];
extern char D_70003720[];
extern char D_70003750[];
extern float D_700036D4[];
extern float D_70003A20[];
extern float D_70003714[];
extern float D_70003754[];

void func_001E3D90(char *p) {
    int seed;
    int id;
    char *s3;
    char *tbl;
    char *tbl2;
    unsigned char flag;
    int n;
    float fadeA;
    float fadeB;
    float fadeC;
    float dist;
    float t24;
    float t28;
    float t2c;
    float fv;
    int buf[24];

    s3 = p + 0x1F0;
    switch (*(unsigned char *)(p + 4)) {
    case 0:
        D_70003A20[0] = (float)func_00122BB8() / 2147483648.0f;
        *(float *)(s3 + 0x10) = D_70003A20[0];
        *(float *)(s3 + 0x14) = D_70003A20[0];
        *(float *)(s3 + 0x18) = D_70003A20[0];
        *(int *)(s3 + 0) = 0;
        *(int *)(s3 + 4) = 0;
        *(int *)(s3 + 8) = func_00122BB8();
        *(int *)(s3 + 0xC) = -1;
        func_001029C0(p + 0xD0);
        func_00102918(p + 0xD0, p + 0xD0, p + 0xB0);
        *(char **)(p + 0x30) = D_00253CA0 + *(unsigned char *)(p + 0xD) * 8;
        *(int *)(p + 0x34) = (int)func_001E3D20;
        *(char *)(p + 0xC) = 0;
        *(char *)(p + 9) = 0;
        *(char *)(p + 0) = 1;
        *(unsigned char *)(p + 4) = 1;
        /* fallthrough */
    case 1:
        if (D_008101E4[0] == 3) {
            switch ((D_00810700[0] << 8) + D_00810701[0]) {
            case 0x100:
                if (*(float *)(p + 0xB8) < -700.0f) {
                    return;
                }
                break;
            }
        } else {
            switch ((D_00810700[0] << 8) + D_00810701[0]) {
            case 0:
            case 1:
                if (D_00810702[0] == 5 || D_00810702[0] == 6) {
                    return;
                }
                break;
            }
        }
        seed = *(int *)(s3 + 8);
        copy_qw4(D_700036A0, p + 0xD0);
        copy_qw4(D_700036E0, p + 0xD0);
        copy_qw4(D_70003720, p + 0xD0);
        switch (*(unsigned char *)(p + 0xD)) {
        case 0:
            fadeA = 0.005f;
            fadeB = 0.0175f;
            fadeC = 0.0125f;
            D_700036D4[0] += 6.0f;
            D_70003714[0] += 1.5f;
            D_70003754[0] += 2.0f;
            tbl = D_00253CE0;
            tbl2 = D_00253CC0;
            break;
        case 1:
            fadeA = 0.005f;
            fadeB = 0.0175f;
            fadeC = 0.0125f;
            D_700036D4[0] += 12.0f;
            D_70003714[0] += 4.0f;
            D_70003754[0] += 4.0f;
            tbl = D_00253E90;
            tbl2 = D_00253CC8;
            break;
        case 2:
            fadeA = 0.005f;
            fadeB = 0.0175f;
            fadeC = 0.0125f;
            D_700036D4[0] += 18.0f;
            D_70003714[0] += 6.0f;
            D_70003754[0] += 6.0f;
            tbl = D_00254040;
            tbl2 = D_00253CD0;
            break;
        }
        switch ((D_00810700[0] << 8) + D_00810701[0]) {
        case 0x200:
        case 0x202:
        case 0xE00:
        case 0x1100:
            func_0021B9A0(2, 1.0f, 50.0f);
            func_0021B9A0(3, 1.0f, 50.0f);
            break;
        case 0x1301:
            func_0021B9A0(2, 1.0f, 150.0f);
            func_0021B9A0(3, 1.0f, 150.0f);
            break;
        }
        id = func_001CD070(D_70003750, 0x30);
        if (id != 0xFFFFFF) {
            D_70003A20[0] = func_001CD2B0(*(float *)tbl2, *(float *)(tbl2 + 4), 320.0f, 320.0f);
            t24 = (float)((seed >> 16) & 0xFFFF);
            t24 /= 65535.0f;
            seed = seed * 37 + 11;
            t24 += 0.0001f;
            t28 = (float)((seed >> 16) & 0xFFFF);
            t28 /= 65535.0f;
            seed = seed * 37 + 11;
            t28 += 0.0001f;
            t2c = (float)((seed >> 16) & 0xFFFF);
            t2c /= 65535.0f;
            t2c += 0.0001f;
            dist = D_70003A20[0];
            *(float *)0x70003A24 = t24;
            *(float *)0x70003A28 = t28;
            *(float *)0x70003A2C = t2c;
            if (dist != 0.0f) {
                func_001CFAE0(buf, 0, D_700036A0, *(float *)(s3 + 0x10), t24, dist, 0.4f);
                func_001CFBE0(id - 0x2000, 1, tbl, buf, 0);
                func_001CFAE0(buf, 0, D_700036E0, *(float *)(s3 + 0x14), *(float *)0x70003A28, D_70003A20[0], 0.2f);
                func_001CFBE0(id, 1, tbl + 0x90, buf, 0);
            }
            n = 1;
            if (D_00275C00 > 0x100) {
                n = 6;
            }
            flag = n;
            func_001CFAE0(buf, 0, D_70003720, *(float *)(s3 + 0x18), *(float *)0x70003A2C, 1.0f, 0.2f);
            func_001CFBE0(id, flag, tbl + 0x120, buf, 0);
        }
        switch ((D_00810700[0] << 8) + D_00810701[0]) {
        case 0x200:
        case 0x202:
        case 0xE00:
        case 0x1100:
        case 0x1301:
            func_0021B9A0(1, 0.0f, 0.0f);
            break;
        }
        if (*(int *)(s3 + 0) == 0) {
            *(char *)(p + 0) = 1;
        } else {
            *(int *)(s3 + 0) -= 1;
            *(char *)(p + 0) = 2;
        }
        switch (*(unsigned char *)(p + 0xD)) {
        case 0:
            func_001FC3C0(p, s3 + 0xC, 0x411, 100.0f, 4096.0f);
            break;
        case 1:
            func_001FC3C0(p, s3 + 0xC, 0x412, 100.0f, 4096.0f);
            break;
        case 2:
            func_001FC3C0(p, s3 + 0xC, 0x413, 100.0f, 4096.0f);
            break;
        }
        *(int *)(s3 + 4) += 1;
        *(float *)(s3 + 0x10) += fadeA;
        *(float *)(s3 + 0x14) += fadeB;
        *(float *)(s3 + 0x18) += fadeC;
        fv = *(float *)(s3 + 0x10);
        if (!(fv <= 2.0f)) {
            *(float *)(s3 + 0x10) = fv - 1.0f;
        }
        fv = *(float *)(s3 + 0x14);
        if (!(fv <= 2.0f)) {
            *(float *)(s3 + 0x14) = fv - 1.0f;
        }
        fv = *(float *)(s3 + 0x18);
        if (!(fv <= 2.0f)) {
            *(float *)(s3 + 0x18) = fv - 1.0f;
        }
        func_001B17A0(p);
        break;
    case 2:
    case 3:
        func_001AFC10(p);
        break;
    }
}
