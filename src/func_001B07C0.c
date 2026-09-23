// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 1
//
// Player placement from the area spawn tables D_0024D650 (FINDINGS "SPAWN-RECORD
// FIXED CAMERAS" / opening spawn). Callers: the frame machine func_001AE040
// (0x001AE094, 0x001AE0F4) and 0x001ACD30; not a per-frame function. The spawn
// record is p = D_0024D650[area D_00810700][room D_00810701] + entry
// D_00810702*0x30 (+0/4/8 position, +0xC yaw, +0x14 walk-out byte);
// func_001B0250(area table, room*4) runs first. `base` is the player struct
// D_008102B0. When D_00275BE0 == 1 (the LOAD-GAME flag set by the title/continue
// flow) both position vectors +0xA0..+0xAC and +0xB0..+0xBC are seeded from
// D_00810710/14/18 and the rotation block +0xC0..+0xC8 from D_00810720/24/28;
// otherwise both come from the record position and the rotation is (0, record
// yaw, 0) (+0xC4 = heading). w = 1.0 each. +0xB0..+0xCC are mirrored to the
// scratchpad 0x70003B40..0x70003B5C. D_00810706 (saved from +0x235 by
// func_0015CF90) is masked &1, written back and copied to +0x235 (bit 0 =
// low-health latch, health <= 35, set by func_0015D100 and cleared by
// func_0015C700; the &1 mask keeps only that bit), D_00810707 to +0x234
// (infected latch), D_00810858 (health) to +0x220 and D_0081085C (infection)
// to +0x228. With area
// flag D_008106C8 & 4: D_00810C60 = 2/1/0 from D_00810C7E/D_00810C7D, and with
// D_008106C8 & 0x60 also func_001EFE00(0x80000018, base) whose result is kept
// at +0x304; with bit 2 clear, D_00810C60 = 0 and an existing +0x304 object
// gets its +4 byte = 2 and is dropped. Then func_0015C1F0(base), +0xE = record
// +0x14, eight floats (+0x60..+0x6C, +0x80..+0x8C) = 1.0f, +0x230 = 0. Tail:
// load-game forces +0xE = 0; otherwise, if arg0 != 0: +0xE == 1 sets +4 = 5,
// +5 = 1, +6 = 0; a non-null +0x1C gets its +4 byte = 1 unless the scratchpad
// byte 0x70003B8D is set; a nonzero pending damage +0x224 or pending infection
// +0x22C is zeroed and the event byte +0 = 1. func_001B0460(arg0) (camera
// re-init from the spawn record) runs last, unconditionally.
//
// MATCH NOTE (load-bearing, do not "clean up"): the target reads D_00275BE0 gp-relative
// (%gp_rel) but every D_008107xx/D_00810Cxx byte absolutely (%hi/%lo). mwcc picks gp-rel for an
// extern whose declared size is <= -sdatathreshold, so this file is built with
// `-sdatathreshold 1` and the byte globals that must stay ABSOLUTE are declared as
// `unsigned char X[2]` (size 2 > 1) and read through `X[0]`. Keeping the symbol name on the
// array preserves the relocation symbol. With `-sdatathreshold 0` everything is absolute
// (99.04%); with the mwcc default threshold everything is gp-relative (85.44%).

extern void func_001B0250(void *a, int b);
extern int func_001EFE00(int a, void *b);
extern void func_0015C1F0(void *a);
extern void func_001B0460(int a0);

extern unsigned char D_00810700[2];
extern unsigned char D_00810701[2];
extern unsigned char D_00810702[2];
extern unsigned char D_00275BE0;
extern void *D_0024D650[];
extern void *D_008102B0;
extern float D_00810710;
extern float D_00810714;
extern float D_00810718;
extern float D_00810720;
extern float D_00810724;
extern float D_00810728;
extern volatile unsigned char D_00810706[2];
extern unsigned char D_00810707[2];
extern float D_00810858;
extern float D_0081085C;
extern int D_008106C8;
extern unsigned char D_00810C7D[2];
extern unsigned char D_00810C7E[2];
extern unsigned char D_00810C60[2];

void func_001B07C0(int arg0) {
    unsigned char *base;
    unsigned char *p;
    unsigned char flags;

    base = (unsigned char *)&D_008102B0;
    p = (unsigned char *)((void **)D_0024D650[D_00810700[0]])[D_00810701[0]];
    p += D_00810702[0] * 0x30;

    func_001B0250((void *)D_0024D650[D_00810700[0]], D_00810701[0] * 4);

    if (D_00275BE0 == 1) {
        *(float *)(base + 0xA0) = D_00810710;
        *(float *)(base + 0xA4) = D_00810714;
        *(float *)(base + 0xA8) = D_00810718;
        *(int *)(base + 0xAC) = 0x3F800000;
        *(float *)(base + 0xB0) = D_00810710;
        *(float *)(base + 0xB4) = D_00810714;
        *(float *)(base + 0xB8) = D_00810718;
        *(int *)(base + 0xBC) = 0x3F800000;
        *(float *)(base + 0xC0) = D_00810720;
        *(float *)(base + 0xC4) = D_00810724;
        *(float *)(base + 0xC8) = D_00810728;
        *(int *)(base + 0xCC) = 0x3F800000;
    } else {
        *(float *)(base + 0xA0) = *(float *)(p + 0);
        *(float *)(base + 0xA4) = *(float *)(p + 4);
        *(float *)(base + 0xA8) = *(float *)(p + 8);
        *(int *)(base + 0xAC) = 0x3F800000;
        *(float *)(base + 0xB0) = *(float *)(p + 0);
        *(float *)(base + 0xB4) = *(float *)(p + 4);
        *(float *)(base + 0xB8) = *(float *)(p + 8);
        *(int *)(base + 0xBC) = 0x3F800000;
        *(int *)(base + 0xC0) = 0;
        *(float *)(base + 0xC4) = *(float *)(p + 0xC);
        *(int *)(base + 0xC8) = 0;
        *(int *)(base + 0xCC) = 0x3F800000;
    }

    flags = D_00810706[0];
    *(volatile float *)0x70003B40 = *(float *)(base + 0xB0);
    *(volatile float *)0x70003B44 = *(float *)(base + 0xB4);
    *(volatile float *)0x70003B48 = *(float *)(base + 0xB8);
    flags &= 1;
    *(volatile float *)0x70003B4C = *(float *)(base + 0xBC);
    *(volatile float *)0x70003B50 = *(float *)(base + 0xC0);
    *(volatile float *)0x70003B54 = *(float *)(base + 0xC4);
    *(volatile float *)0x70003B58 = *(float *)(base + 0xC8);
    *(volatile float *)0x70003B5C = *(float *)(base + 0xCC);

    D_00810706[0] = flags;
    *(base + 0x235) = D_00810706[0];
    *(base + 0x234) = D_00810707[0];
    *(float *)(base + 0x220) = D_00810858;
    *(float *)(base + 0x228) = D_0081085C;

    if (D_008106C8 & 4) {
        if (D_00810C7E[0] != 0) {
            if (D_00810C7D[0] != 0) {
                D_00810C60[0] = 2;
            } else {
                D_00810C60[0] = 1;
            }
        } else {
            D_00810C60[0] = 0;
        }
        if (D_008106C8 & 0x60) {
            *(int *)(base + 0x304) = func_001EFE00(0x80000018, base);
        }
    } else {
        D_00810C60[0] = 0;
        if (*(int *)(base + 0x304) != 0) {
            *(*(unsigned char **)(base + 0x304) + 4) = 2;
            *(int *)(base + 0x304) = 0;
        }
    }

    func_0015C1F0(base);

    *(base + 0xE) = *(p + 0x14);
    *(int *)(base + 0x60) = 0x3F800000;
    *(int *)(base + 0x64) = 0x3F800000;
    *(int *)(base + 0x68) = 0x3F800000;
    *(int *)(base + 0x6C) = 0x3F800000;
    *(int *)(base + 0x80) = 0x3F800000;
    *(int *)(base + 0x84) = 0x3F800000;
    *(int *)(base + 0x88) = 0x3F800000;
    *(int *)(base + 0x8C) = 0x3F800000;
    *(int *)(base + 0x230) = 0;

    if (D_00275BE0 == 1) {
        *(base + 0xE) = 0;
    } else if (arg0 != 0) {
        if (*(base + 0xE) == 1) {
            *(base + 4) = 5;
            *(base + 5) = 1;
            *(base + 6) = 0;
        }
        if (*(int *)(base + 0x1C) != 0 && *(volatile unsigned char *)0x70003B8D == 0) {
            *(*(unsigned char **)(base + 0x1C) + 4) = 1;
        }
        {
            float zero = 0.0f;
            if (*(float *)(base + 0x224) != zero || *(float *)(base + 0x22C) != zero) {
                *(int *)(base + 0x224) = 0;
                *(int *)(base + 0x22C) = 0;
                *(base + 0) = 1;
            }
        }
    }

    func_001B0460(arg0);
}
