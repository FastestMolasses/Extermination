// NEARMISS func_001612D0  (vram 0x001612D0, 0x3BC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 89.22% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// State-machine dispatch (switch on +6) resolves body/structure identically to target, but two genuine mwcc scheduling artifacts remain, independent of source phrasing (tried if/else vs switch, goto polarity flips, temp-var elimination): (1) the state==0 vs state==2 dispatch chain -- target leaves ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void anim_clip_arbiter(char *p, int clip, float speed, float f);
extern int func_00160220(char *p);
extern int func_001607D0(char *p);
extern void func_001749A0(char *e, int clip, int flags, float blend);
extern int func_00174AC0(char *p, int n);
extern void func_001756E0(char *p);
extern void func_00175900(char *p, int f);
extern void func_001764E0(char *p);
extern void func_00178B90(char *p, int a);
extern void func_001796C0(char *p);
extern void func_001798D0(char *p);
extern short func_0017B490(char *e, int a1, unsigned char variant, unsigned char a3);
extern void func_0017BC40(char *p);
extern void func_0017C030(char *p);
extern void func_0017C540(char *p);
extern int func_00184BA0(char *p, int a);
extern int func_001C61D0(int a, int b);
extern void func_001EFD90(int id, char *a, char *b);

extern float D_00248870[];
extern unsigned short D_00810E74;

void func_001612D0(char *arg0) {
    int st = *(unsigned char *)(arg0 + 6);

    if (st == 0x63) {
        goto case_63;
    }
    if (st == 2) {
        goto case_2;
    }
    if (st == 1) {
        goto case_1;
    }
    if (st == 0) {
        *(unsigned char *)(arg0 + 6) = (unsigned char)(st + 1);
        *(char *)(arg0 + 7) = 0;
        *(int *)(arg0 + 0x2EC) = 0;
        goto case_1;
    }
    goto block_39;

case_1:
        if (func_001607D0(arg0) == 0 && func_00160220(arg0) == 0) {
            func_00174AC0(arg0, 1);
            func_0017BC40(arg0);
            func_0017C030(arg0);
            func_00178B90(arg0, 0);
            {
                unsigned char stage = *(unsigned char *)(arg0 + 0x1F0);
                if (stage == 6 || stage == 7) {
                    *(unsigned char *)(arg0 + 6) = (unsigned char)(*(unsigned char *)(arg0 + 6) + 1);
                    *(short *)(arg0 + 0x28) = 0;
                } else if (stage == 0) {
                    *(char *)(arg0 + 5) = 0;
                    *(unsigned char *)(arg0 + 6) = 0;
                }
            }
            goto block_39;
        }
        return;

case_2:
        if (func_001607D0(arg0) == 0) {
            if (D_00810E74 & *(unsigned short *)0x70003B76) {
                if (func_00184BA0(arg0, 1) != 0) {
                    func_001798D0(arg0);
                    *(char *)(arg0 + 5) = 0x25;
                    *(unsigned char *)(arg0 + 6) = 0;
                    return;
                }
            }
            func_00174AC0(arg0, 1);
            {
                unsigned char stage = *(unsigned char *)(arg0 + 0x1F0);
                if (stage != 6 && stage != 7) {
                    if (*(float *)(arg0 + 0x240) != 0.0f) {
                        int idx = (int)*(unsigned char *)(arg0 + 0x23F) - 1;
                        int clip;
                        *(unsigned char *)(arg0 + 0x25C) = (unsigned char)idx;
                        *(float *)(arg0 + 0x38) = D_00248870[idx];
                        clip = func_0017B490(arg0, 1, *(unsigned char *)(arg0 + 0x235), *(unsigned char *)(arg0 + 0x25C));
                        if (*(unsigned char *)(arg0 + 0x1F1) == 3) {
                            func_001749A0(arg0, clip, 0, 0.0f);
                        } else {
                            *(float *)0x70003A20 = (float)func_001C61D0(*(int *)(arg0 + 0x40), clip);
                            anim_clip_arbiter(arg0, clip, 0.0f, *(float *)0x70003A20 / 2.0f);
                        }
                        *(unsigned char *)(arg0 + 6) = (unsigned char)(*(unsigned char *)(arg0 + 6) - 1);
                        *(unsigned char *)(arg0 + 0x1F0) = 1;
                        *(unsigned char *)(arg0 + 0x1F1) = 1;
                    } else {
                        *(char *)(arg0 + 5) = 0;
                        *(unsigned char *)(arg0 + 6) = 0;
                        *(unsigned char *)(arg0 + 0x1F0) = 0;
                    }
                } else {
                    if (!(*(short *)(arg0 + 0x28) & 7)) {
                        unsigned char m = *(unsigned char *)(arg0 + 0x23A);
                        if (m != 6) {
                            if (m == 5) {
                                func_001EFD90(0x80000033, arg0 + 0xB0, arg0 + 0xC0);
                            } else if (*(unsigned char *)(arg0 + 0x23C) == 0 && *(unsigned char *)(arg0 + 0x23D) == 0) {
                                func_001EFD90(0x80000012, arg0 + 0xB0, arg0 + 0xC0);
                            }
                        } else {
                            func_001EFD90(0x80000033, arg0 + 0xB0, arg0 + 0xC0);
                        }
                    }
                    *(short *)(arg0 + 0x28) = (short)(*(short *)(arg0 + 0x28) + 1);
                }
            }
            func_0017BC40(arg0);
            func_0017C030(arg0);
            func_00178B90(arg0, 0);
            goto block_39;
        }
        return;

case_63:
        func_00174AC0(arg0, 1);
        func_00178B90(arg0, 0);
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            func_0017C540(arg0);
        }

block_39:
    func_001764E0(arg0);
    if (*(unsigned char *)(arg0 + 0x23B) == 0x35) {
        *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + -0.8f;
    } else {
        *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + -0.4f;
    }
    func_00175900(arg0, 1);
    func_001756E0(arg0);
    func_001796C0(arg0);
}
