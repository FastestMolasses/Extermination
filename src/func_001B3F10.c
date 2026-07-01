// NEARMISS func_001B3F10  (vram 0x001B3F10, 0x2DC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.97% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// NEARMISS 97.97% (mwcc233; 991202 82.34%). Logic and structure fully recovered and body-identical instruction stream to the target; a real semantic bug was found and fixed in the process (m2c had mis-signed the func_0019A6F0==0 case as return-0, but ground truth is return-1 -- fixed via goto). Sol...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Aim/turn-lock test: computes the yaw error between arg1 (candidate bearing)
// and the angle from the world reference point (D_00810360,D_00810368) to
// self+0xB0, wrapped through func_001B1470/fabsf. Picks one of two 4-entry
// float tables (D_0024D720.. for the "aimed" case, D_0024D730.. for the
// "not aimed" case) keyed on self's difficulty/type byte (self+3: 1/7/6/5/
// default), then indexes it by D_0081050C&3 to get a per-difficulty radius
// passed to func_001B13F0 as a magnitude test against the world reference.
// On success, copies self's position (0xB0..0xBC) and the world reference
// into the EE scratchpad (0x70003600.. / 0x70003610..), checks the y-delta
// is within arg2, nudges both y values by +15.0f, then re-runs
// func_0019A6F0 (mode 7, arg4=0x40). If func_0019A6F0 returns 0 the function
// still returns 1 (the "no obstruction found" case counts as a pass); only
// when it returns nonzero AND the resulting hit record's flag word
// (*(short*)(*0x700031D0 + 0x1A)) has bit 0x2000 set does the function reject
// with 0.
extern float func_0011DF78(float x);
extern float func_001B1240(unsigned char *origin, float x, float z);
extern float func_001B1470(float a);
extern int func_001B13F0(void *a, void *b, float r);
extern int func_0019A6F0(unsigned char *a, float *b, float *c, int d, int e);

extern float D_0024D720[4];
extern float D_0024D730[4];
extern float D_0024D740[4];
extern float D_0024D750[4];
extern float D_0024D760[4];
extern float D_0024D770[4];
extern float D_0024D780[4];
extern float D_0024D790[4];
extern float D_0024D7A0[4];
extern float D_0024D7B0[4];

extern unsigned char D_008102B4;
extern float D_00810350;
extern float D_00810354;
extern float D_00810358;
extern float D_0081035C;
extern float D_00810360;
extern float D_00810368;
extern unsigned char D_0081050C;
extern float D_70003600;
extern float D_70003610;

int func_001B3F10(unsigned char *arg0, float arg1, float arg2) {
    float *tbl;

    if (func_0011DF78(func_001B1470(func_001B1240(arg0 + 0xB0, D_00810360, D_00810368) - arg1)) <= 0.7853982f) {
        switch (arg0[3]) {
        case 1:
            tbl = D_0024D720;
            break;
        case 7:
            tbl = D_0024D780;
            break;
        case 6:
            tbl = D_0024D760;
            break;
        case 5:
            tbl = D_0024D740;
            break;
        default:
            tbl = D_0024D7A0;
            break;
        }
    } else {
        switch (arg0[3]) {
        case 1:
            tbl = D_0024D730;
            break;
        case 7:
            tbl = D_0024D790;
            break;
        case 6:
            tbl = D_0024D770;
            break;
        case 5:
            tbl = D_0024D750;
            break;
        default:
            tbl = D_0024D7B0;
            break;
        }
    }

    if (D_008102B4 != 1) {
        return 0;
    }

    if (func_001B13F0(&D_00810360, arg0 + 0xB0, tbl[D_0081050C & 3]) == 0) {
        return 0;
    }

    {
        float t0, t1, t2, t3, p0;
        t0 = D_00810350;
        p0 = *(float *)(arg0 + 0xB0);
        t1 = D_00810354;
        t2 = D_00810358;
        t3 = D_0081035C;
        *(float *)0x70003600 = p0;
        *(float *)0x70003604 = *(float *)(arg0 + 0xB4);
        *(float *)0x70003608 = *(float *)(arg0 + 0xB8);
        *(float *)0x7000360C = *(float *)(arg0 + 0xBC);
        *(float *)0x70003610 = t0;
        *(float *)0x70003614 = t1;
        *(float *)0x70003618 = t2;
        *(float *)0x7000361C = t3;
    }

    if (!(func_0011DF78(*(float *)0x70003604 - *(float *)0x70003614) < arg2)) {
        return 0;
    }

    {
        float a, b;
        a = *(float *)0x70003604 + 15.0f;
        b = *(float *)0x70003614 + 15.0f;
        *(float *)0x70003604 = a;
        if (func_0019A6F0(arg0, &D_70003600, &D_70003610, 7, (*(float *)0x70003614 = b, 0x40)) == 0) goto one;
    }

    if (!(*(short *)(*(char **)0x700031D0 + 0x1A) & 0x2000)) goto one;
    return 0;
one:
    return 1;
}
