// NEARMISS func_0014AFA0  (vram 0x0014AFA0, 0x808 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.87% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Clean-store idiom-13 delay-slot fill (branch-target-advance): after the bnez guarding the func_001B2F70(...)==0 store, BOTH available builds (991202 and 2.3.3) fill the delay slot with a speculated 'lui at,0x7000' copied from the taken/join block and advance the branch target by +4; the original ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Actor facing/steering check (same family as func_00140F80/func_00132FB0):
// ent = actor record (+0xB0 world pos, +0xC4 yaw, +0x52 status bits, +4/5/6 mode bytes),
// st = per-slot state block (+0x48 signed phase float, +0x4C turn float, +0x6C height,
// +0x81 result bits, +0x8D/+0x8E flags/timer). Seeds scratchpad vec D_700038A0 with
// (0,3,+-3,1), rotates by yaw (func_001B2B10) + translates (func_001028B8), then
// angle-tests via func_001B32F0/func_001B3390 (pi/4) setting st[0x81]|=1 (blocked).
// phase>0: tests 4 offset table rows D_00245BE0[1..4]; phase<-0.1: tests 4 fixed
// side vectors pairwise. If blocked, sets ent status bit 0x52|=1. phase<0 (descend):
// probes 5 table rows against a -15y vector via func_001B3440 (st[0x81]|=2, phase=0 on
// hit); ground-seed logic via func_001B2F70; func_001031E0 + func_0019B6C0 ground snap;
// on surface id 0x5B sets st[0x8D]|=1, st[0x6C]=water height, and once (st[0x8E]==0 and
// turn!=0) spawns effect 0x8000001D via func_001EFD90 with timer (rand>>9&7)+7.
// phase>0 (ascend): ceiling probe +15y/+20.002 via func_0019AB20 id 0x80000007
// (st[0x81]|=4, phase=0 on hit).
//
// NEARMISS 99.87% (mwcc233; 991202 = 93.82%) - logic fully recovered. Sole residual:
// clean-store idiom-13 delay-slot fill after the bnez guarding func_001B2F70(...)==0
// (both compilers speculate 'lui at,0x7000' into the slot and advance the branch +4;
// target leaves nop). Same documented wall as func_00140F80.

extern void func_001B2B10(void *a, void *b, void *c);
extern void func_001028B8(void *a, void *b, void *c);
extern int func_001B32F0(char *a, void *b, float c);
extern void func_001029C0(void *a);
extern void func_00102BB0(void *a, void *b, float c);
extern void func_001026A0(void *a, void *b, void *c);
extern int func_001B3390(char *a, void *b, void *c, float d);
extern int func_001B3440(char *a, void *b, void *c, float d);
extern int func_001B2F70(char *a, void *b);
extern void func_001031E0(void *a, void *b);
extern int func_0019B6C0(void *a, void *b);
extern int func_00122BB8(void);
extern void func_001EFD90(int a, void *b, void *c);
extern int func_0019AB20(char *a, void *b, void *c, int d);

extern int D_700038A0;
extern int D_700038B0;
extern int D_700038C0;
extern int D_700038D0;
extern int D_700036A0;
extern char D_00245BE0[];

void func_0014AFA0(char *ent, char *st) {
    int i;
    char *p;

    st[0x81] = 0;
    *(float *)0x700038A0 = 0.0f;
    *(float *)0x700038A4 = 3.0f;
    *(float *)0x700038A8 = 3.0f;
    *(float *)0x700038AC = 1.0f;
    if (*(float *)(st + 0x4C) < 0.0f) {
        *(float *)0x700038A8 = -3.0f;
    }
    func_001B2B10(ent, &D_700038A0, &D_700038A0);
    func_001028B8(&D_700038A0, ent + 0xB0, &D_700038A0);
    if (func_001B32F0(ent, &D_700038A0, 0.7853981852531433f) != 0) {
        st[0x81] |= 1;
    } else {
        float d = *(float *)(st + 0x48);
        if (d > 0.0f) {
            func_001029C0(&D_700036A0);
            func_00102BB0(&D_700036A0, &D_700036A0, *(float *)(ent + 0xC4));
            i = 0;
            do {
                func_001026A0(&D_700038A0, &D_700036A0, &D_00245BE0[(i + 1) * 16]);
                func_001028B8(&D_700038A0, ent + 0xB0, &D_700038A0);
                if (func_001B32F0(ent, &D_700038A0, 0.7853981852531433f) != 0) {
                    st[0x81] |= 1;
                    break;
                }
                i++;
            } while (i < 4);
        } else if (d < -0.1f) {
            *(float *)0x700038A0 = 0.0f;
            *(float *)0x700038A4 = 0.0f;
            *(float *)0x700038B0 = 0.0f;
            *(float *)0x700038B4 = 0.0f;
            *(float *)0x700038A8 = -3.0f;
            *(float *)0x700038C0 = -3.0f;
            *(float *)0x700038C4 = 0.0f;
            *(float *)0x700038C8 = 0.0f;
            *(float *)0x700038AC = 1.0f;
            *(float *)0x700038CC = 1.0f;
            *(float *)0x700038B8 = 3.0f;
            *(float *)0x700038D0 = 3.0f;
            *(float *)0x700038BC = 1.0f;
            *(float *)0x700038D4 = 0.0f;
            *(float *)0x700038D8 = 0.0f;
            *(float *)0x700038DC = 1.0f;
            func_001029C0(&D_700036A0);
            func_00102BB0(&D_700036A0, &D_700036A0, *(float *)(ent + 0xC4));
            func_001026A0(&D_700038A0, &D_700036A0, &D_700038A0);
            func_001026A0(&D_700038B0, &D_700036A0, &D_700038B0);
            func_001026A0(&D_700038C0, &D_700036A0, &D_700038C0);
            func_001026A0(&D_700038D0, &D_700036A0, &D_700038D0);
            func_001028B8(&D_700038A0, ent + 0xB0, &D_700038A0);
            func_001028B8(&D_700038B0, ent + 0xB0, &D_700038B0);
            func_001028B8(&D_700038C0, ent + 0xB0, &D_700038C0);
            func_001028B8(&D_700038D0, ent + 0xB0, &D_700038D0);
            if (func_001B3390(ent, &D_700038A0, &D_700038B0, 0.7853981852531433f) != 0) {
                st[0x81] |= 1;
            } else if (func_001B3390(ent, &D_700038B0, &D_700038A0, 0.7853981852531433f) != 0) {
                st[0x81] |= 1;
            }
            if (st[0x81] == 0) {
                if (func_001B3390(ent, &D_700038C0, &D_700038D0, 0.7853981852531433f) != 0) {
                    st[0x81] |= 1;
                } else if (func_001B3390(ent, &D_700038D0, &D_700038C0, 0.7853981852531433f) != 0) {
                    st[0x81] |= 1;
                }
            }
        }
    }
    if (st[0x81] & 1) {
        *(unsigned short *)(ent + 0x52) |= 1;
    }
    {
        float d = *(float *)(st + 0x48);
        if (d < 0.0f) {
            func_001029C0(&D_700036A0);
            func_00102BB0(&D_700036A0, &D_700036A0, *(float *)(ent + 0xC4));
            *(float *)0x700038B0 = 0.0f;
            *(float *)0x700038B4 = -15.0f;
            *(float *)0x700038B8 = 0.0f;
            *(float *)0x700038BC = 1.0f;
            p = D_00245BE0;
            i = 0;
            do {
                func_001026A0(&D_700038A0, &D_700036A0, p);
                func_001028B8(&D_700038A0, ent + 0xB0, &D_700038A0);
                if (func_001B3440(ent, &D_700038A0, &D_700038B0, 0.7853981852531433f) != 0) {
                    *(float *)(st + 0x48) = 0.0f;
                    st[0x81] |= 2;
                    break;
                }
                i++;
                p += 0x10;
            } while (i < 5);
            if (!(st[0x81] & 2) && *(unsigned char *)(ent + 4) == 1) {
                unsigned char c = *(unsigned char *)(ent + 5);
                if (c != 3 && c != 4) {
                    if (func_001B2F70(ent + 0xB0, &D_700038A0) == 0) {
                        *(float *)0x700038A0 = *(float *)(ent + 0xB4) - 10.0f;
                    }
                    if (*(float *)(ent + 0xB4) - *(float *)0x700038A0 >= 10.0f &&
                        *(float *)(st + 0x48) < -0.5f) {
                        ent[5] = 4;
                        ent[6] = 0;
                    }
                }
            }
            func_001031E0(&D_700038A0, ent + 0xB0);
            *(float *)0x700038A4 += 18.0f;
            st[0x8D] &= ~1;
            if (func_0019B6C0(&D_700038A0, ent + 0xB0) != 0) {
                if (*(unsigned char *)(*(char **)0x700031D0 + 0x1A) == 0x5B) {
                    st[0x8D] |= 1;
                    *(float *)(st + 0x6C) = *(float *)0x700031B4;
                    if (st[0x8E] == 0) {
                        float c = *(float *)(st + 0x4C);
                        float zero = 0.0f;
                        if (c != zero) {
                            st[0x8E] = ((func_00122BB8() >> 9) & 7) + 7;
                            *(float *)0x700038A0 = *(float *)(ent + 0xB0);
                            *(float *)0x700038A4 = *(float *)(ent + 0xB4);
                            *(float *)0x700038A8 = *(float *)(ent + 0xB8);
                            *(int *)0x700038AC = 0x3F800000;
                            *(float *)0x700038A4 = *(float *)(st + 0x6C);
                            func_001EFD90(0x8000001D, &D_700038A0, ent + 0xC0);
                        }
                    }
                }
            }
        } else if (d > 0.0f) {
            *(float *)0x700038A0 = *(float *)(ent + 0xB0);
            *(float *)0x700038A4 = *(float *)(ent + 0xB4);
            *(float *)0x700038A8 = *(float *)(ent + 0xB8);
            *(float *)0x700038AC = *(float *)(ent + 0xBC);
            *(float *)0x700038B0 = 0.0f;
            *(float *)0x700038B4 = 15.0f;
            *(float *)0x700038B8 = 0.0f;
            *(float *)0x700038BC = 1.0f;
            *(float *)0x700038A4 += 20.002f;
            if (func_0019AB20(ent, &D_700038A0, &D_700038B0, 0x80000007) != 0) {
                *(float *)(st + 0x48) = 0.0f;
                st[0x81] |= 4;
            }
        }
    }
}
