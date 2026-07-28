// NEARMISS func_0016AE40  (vram 0x0016AE40, 0x948 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.98% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Single register-coloring permutation (2 instructions): in case 1 the target loads the pad mask D_00810E74 into $a1 ('lhu a1' + 'and v1,a1,v1'); mwcc 2.3.3 picks the equally-dead $a0. Everything else byte-identical. 10+ shapes tried across this and the prior wave (operand temps on either side, hit...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// Boss/large-enemy behaviour state machine, dispatched on the state byte at
// e+6 (cases 0,1,10,11,20,30,40,41,42,43,50), with nested sub-state machines
// on the phase byte e+7 inside cases 20, 30 and 50. Case 0 initialises
// (mode 0x22, clears the phase/sub-index bytes, kicks the clip picked by
// func_001885B0 at 16.0f and snapshots the position e+0xB0 into e+0x290);
// case 1 polls the pad mask at D_00810E74 against the scratchpad mask
// 0x70003B76 and branches into the 10/20/30 runs. The 20/30/50 runs share a
// phase pattern: phase 0 aims via func_001B1470/func_001B12B0 and starts
// clip 0xBC/0xC3, phase 1 waits on contact flag 0x8000, phase 2/3 wait on
// flag 0x1000, play the impact sound (func_00179B90()+0x112, 300-radius) and
// either advance the phase or track the camera height rate (D_00275B40) via
// func_00182100/func_00181E20/func_00178B90. Case 40 chains on the object
// handle at e+0x24C; 43 flips the facing angle by pi and re-inits.
//
// NEARMISS residual (mwcc 2.3.3, 99.98%): a single register-coloring choice
// in case 1 — the target loads D_00810E74 into $a1, mwcc picks the equally
// dead $a0 (2 instructions: lhu + and operand). Everything else is
// byte-identical. 10+ source shapes tried (operand temps/swaps, volatile,
// int casts, guard-chain reshape) — pure regalloc permutation. Permuter
// territory.
extern int func_001885B0(unsigned char *e);
extern void func_001749A0(unsigned char *e, int id, int z, float f);
extern void func_00102948(void *dst, void *src);
extern int func_00181D70(unsigned char *e);
extern int func_0016ADE0(unsigned char *e);
extern int func_00175390();
extern int func_00181E20(unsigned char *e, int a, float f);
extern int func_00181F60(unsigned char *e);
extern void func_00179880(unsigned char *e, void *p);
extern int func_00175900(unsigned char *e, int f);
extern float func_001B1470(float a);
extern void func_00182090(unsigned char *e);
extern void build_trs_matrix(void *m, void *pos, void *rot, void *scale);
extern int func_00179B90();
extern void func_001FBD50(unsigned char *e, int a, int b, float f);
extern float func_00182100();
extern void func_001811F0(unsigned char *e);
extern void func_00178B90(unsigned char *e, int a);
extern void func_00181430(unsigned char *e);
extern float func_001B12B0(float goal, float cur, float rate);
extern void func_001751A0(unsigned char *e);
extern void func_0016A4B0(unsigned char *e);
extern void func_00181950(unsigned char *e);
extern int D_00275B40;
extern unsigned short D_00810E74[];

void func_0016AE40(unsigned char *e)
{
    unsigned char st;

    st = e[6];
    switch (st) {
    case 0:
        e[6] = st + 1;
        e[7] = 0;
        e[0x1F0] = 0x22;
        e[0x1F1] = 0;
        e[0x25C] = 0;
        e[0x2F1] = 0;
        func_001749A0(e, func_001885B0(e), 0, 16.0f);
        func_00102948(e + 0x290, e + 0xB0);
        break;
    case 1:
        if (func_00181D70(e) != 0) {
            break;
        }
        if (func_0016ADE0(e) != 0) {
            break;
        }
        if (D_00810E74[0] & *(unsigned short *)0x70003B76) {
            e[6] = 0xA;
            e[0x1F0] = 0x23;
            break;
        }
        if (func_00175390() == 0) {
            break;
        }
        if (func_00181E20(e, 0, 4.5f) != 0) {
            e[6] = 0x14;
            e[7] = 0;
            e[0x1F1] = 1;
            break;
        }
        if (func_00181F60(e) != 0) {
            e[6] = 0x1E;
            e[7] = 0;
        }
        break;
    case 10:
        e[6] = st + 1;
        func_001749A0(e, 0xD6, 0, 1.0f);
        *(float *)(e + 0x2F4) = *(float *)(e + 0xB4);
        *(float *)(e + 0x2EC) = 0.0f;
        break;
    case 11:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[5] = 7;
            e[6] = 0;
            e[0x1F0] = 0xD;
        }
        func_00179880(e, e + 0x2EC);
        func_00175900(e, 1);
        break;
    case 20:
        if (func_00181D70(e) != 0) {
            break;
        }
        func_00175390(e);
        {
            unsigned char ph = e[7];
            switch (ph) {
            case 0:
                e[7] = ph + 1;
                *(float *)0x70003A20 = func_001B1470(*(float *)0x700031E4 - *(float *)(e + 0xC4));
                if (*(float *)0x70003A20 >= 0.0f) {
                    e[0x2F1] = 1;
                    func_001749A0(e, 0xBC, 0, 8.0f);
                } else {
                    e[0x2F1] = 2;
                    func_001749A0(e, 0xC3, 0, 8.0f);
                }
                *(float *)(e + 0x26C) = 1.0f;
                e[0x25C] = 2;
                *(float *)(e + 0x38) = 0.0f;
                *(float *)(e + 0x21C) = 0.0f;
                break;
            case 1:
                if (!(*(int *)(e + 0x200) & 0x8000)) {
                    e[7] = ph + 1;
                }
                break;
            case 2:
                func_00182090(e);
                build_trs_matrix(e + 0xD0, e + 0xB0, e + 0xC0, e + 0x60);
                if (*(int *)(e + 0x200) & 0x1000) {
                    func_001FBD50(e, func_00179B90() + 0x112, 0, 300.0f);
                    if (func_0016ADE0(e) != 0) {
                        return;
                    }
                    if (e[0x23F] != 0) {
                        if (func_00181E20(e, 1, 2.25f + func_00182100()) != 0) {
                            func_001811F0(e);
                            e[0x25C] = e[0x23F];
                            *(float *)(e + 0x38) = 0.0f;
                            *(float *)(e + 0x21C) = 0.0f;
                        } else {
                            e[7] = e[7] + 1;
                        }
                    } else {
                        e[7] = e[7] + 1;
                    }
                } else {
                    *(float *)(e + 0x204) = *(float *)(e + 0x26C);
                    *(float *)(e + 0x38) = *(float *)(*(int *)D_00275B40 + 8) - *(float *)(e + 0x21C);
                    *(float *)(e + 0x21C) = *(float *)(*(int *)D_00275B40 + 8);
                    if (func_00181E20(e, 1, *(float *)(e + 0x38) + func_00182100(e)) != 0) {
                        func_00178B90(e, 0);
                    }
                }
                break;
            case 3:
                e[7] = ph + 1;
                func_00181430(e);
                *(float *)(e + 0x38) = 0.0f;
                *(float *)(e + 0x21C) = 0.0f;
                break;
            case 4:
                if (*(int *)(e + 0x200) & 0x1000) {
                    func_001FBD50(e, func_00179B90() + 0x112, 0, 300.0f);
                    e[6] = 0;
                } else {
                    *(float *)(e + 0x38) = *(float *)(*(int *)D_00275B40 + 8) - *(float *)(e + 0x21C);
                    *(float *)(e + 0x21C) = *(float *)(*(int *)D_00275B40 + 8);
                    if (func_00181E20(e, 1, *(float *)(e + 0x38) + func_00182100(e)) != 0) {
                        func_00178B90(e, 0);
                    }
                }
                break;
            }
        }
        func_00181950(e);
        func_00102948(e + 0x290, e + 0xB0);
        break;
    case 30:
        *(float *)(e + 0xC4) = func_001B12B0(*(float *)(e + 0x218), *(float *)(e + 0xC4), 0.069813170f);
        {
            unsigned char ph = e[7];
            switch (ph) {
            case 0:
                e[7] = ph + 1;
                e[0x2F1] = 1;
                func_001749A0(e, 0xBC, 0, 8.0f);
                *(float *)(e + 0x26C) = 1.0f;
                e[0x25C] = 2;
                *(float *)(e + 0x38) = 0.0f;
                *(float *)(e + 0x21C) = 0.0f;
                break;
            case 1:
                if (!(*(int *)(e + 0x200) & 0x8000)) {
                    e[7] = ph + 1;
                }
                break;
            case 2:
                if (*(int *)(e + 0x200) & 0x1000) {
                    func_001FBD50(e, func_00179B90() + 0x112, 0, 300.0f);
                    e[7] = e[7] + 1;
                    func_00181430(e);
                    *(float *)(e + 0x38) = 0.0f;
                    *(float *)(e + 0x21C) = 0.0f;
                } else {
                    *(float *)(e + 0x204) = *(float *)(e + 0x26C);
                    *(float *)(e + 0x38) = *(float *)(*(int *)D_00275B40 + 8) - *(float *)(e + 0x21C);
                    *(float *)(e + 0x21C) = *(float *)(*(int *)D_00275B40 + 8);
                    func_00178B90(e, 0);
                }
                break;
            case 3:
                if (*(int *)(e + 0x200) & 0x1000) {
                    func_001FBD50(e, func_00179B90() + 0x112, 0, 300.0f);
                    e[6] = 0x28;
                    e[7] = 0;
                    *(float *)(e + 0xB0) = *(float *)(e + 0x290);
                    *(float *)(e + 0xB8) = *(float *)(e + 0x298);
                } else {
                    *(float *)(e + 0x38) = *(float *)(*(int *)D_00275B40 + 8) - *(float *)(e + 0x21C);
                    *(float *)(e + 0x21C) = *(float *)(*(int *)D_00275B40 + 8);
                    func_00178B90(e, 0);
                }
                break;
            }
        }
        break;
    case 40:
        if (func_00181D70(e) != 0) {
            break;
        }
        func_001751A0(e);
        {
            int n = *(int *)(e + 0x24C);
            if (n == 0) {
                e[6] = e[6] + 1;
                e[7] = 0;
                e[0x1F0] = 0x28;
            } else if (n == 1) {
                e[6] = e[6] + 2;
                e[7] = 0;
            }
        }
        break;
    case 41:
        func_0016A4B0(e);
        break;
    case 42:
        e[6] = st + 1;
        func_001749A0(e, 0xD7, 0, 1.0f);
        break;
    case 43:
        if (*(int *)(e + 0x200) & 0x1000) {
            *(float *)(e + 0xC4) = func_001B1470(3.1415927f + *(float *)(e + 0xC4));
            func_001749A0(e, func_001885B0(e), 0, 0.0f);
            e[6] = 0x32;
            e[7] = 0;
        }
        break;
    case 50:
        {
            unsigned char ph = e[7];
            switch (ph) {
            case 0:
                e[7] = ph + 1;
                e[0x2F1] = 1;
                func_001749A0(e, 0xBC, 0, 0.0f);
                *(float *)(e + 0x26C) = 1.0f;
                e[0x25C] = 2;
                *(float *)(e + 0x38) = 0.0f;
                *(float *)(e + 0x21C) = 0.0f;
                break;
            case 1:
                if (!(*(int *)(e + 0x200) & 0x8000)) {
                    e[7] = ph + 1;
                }
                break;
            case 2:
                if (*(int *)(e + 0x200) & 0x1000) {
                    func_001FBD50(e, func_00179B90() + 0x112, 0, 300.0f);
                    e[7] = e[7] + 1;
                    func_00181430(e);
                    *(float *)(e + 0x38) = 0.0f;
                    *(float *)(e + 0x21C) = 0.0f;
                } else {
                    *(float *)(e + 0x204) = *(float *)(e + 0x26C);
                    *(float *)(e + 0x38) = *(float *)(*(int *)D_00275B40 + 8) - *(float *)(e + 0x21C);
                    *(float *)(e + 0x21C) = *(float *)(*(int *)D_00275B40 + 8);
                    func_00178B90(e, 0);
                }
                break;
            case 3:
                if (*(int *)(e + 0x200) & 0x1000) {
                    func_001FBD50(e, func_00179B90() + 0x112, 0, 300.0f);
                    e[6] = 0;
                    e[0x1F0] = 0x22;
                }
                break;
            }
        }
        break;
    }
}
