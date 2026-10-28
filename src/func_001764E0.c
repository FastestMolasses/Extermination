// NEARMISS func_001764E0  (vram 0x001764E0, 0x6F4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.08% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// Corrected body (decomp2 lane round 5: the overhead-lane height clause and crawl test, the ankle pass entity branch). Left: saved-register choice of the j counter and the func_0019AFE0 result.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// CORRECTED (decomp lane round 5, 2026-10-20): func_00176C80 takes arg0 (the
// original calls it with its own a0 untouched; the old C passed nothing).
//
// CORRECTED (decomp2 lane round 5, 2026-10-21), read from the original
// instructions; 83.54% -> 97.08%:
//  - Overhead lane (j-loop, +0x236 == 0): the old C inverted the height
//    clause. The original probes again (func_0019AD00 at the lane end + 0.1)
//    when j >= 3, the player mode +0x1F0 is 0x37 / 0x36 / 0x3E, the speed
//    +0x38 is negative, or the overhang is LESS than 13.8 above the player;
//    otherwise it probes when the crawl result s2 is 0 and lowers the player
//    (+0x236 = 1) when it is not. The two probes are separate call sites in
//    the target.
//  - Ankle pass, contact class 0x2000: with an entity at 0x700031D4 only its
//    flag / type test decides; the slope-angle test runs only when there is
//    no entity (the old C also ran it when the entity test failed).
//  - flagBit: the original reads $s1 before its first write in this function
//    and keeps bit 2 (`andi` of the caller's $s1, then $s1 becomes the ankle
//    counter). C has no spelling for a caller's register; `i & 4` on the not
//    yet initialised counter compiles to the same instruction. The port
//    models it as the caller's $s1 (em_player_floor.c inherited_s1).
//  - No zero store to 0x70003A20 before the 13.8 height in the low branch.
//  - The selector for the last ray tests +0x236 == 0 first (D_002488E0).
//  - -sdatathreshold 4 (D_00275B00 is gp-relative); 0x70003A20 is the
//    relocated extern D_70003A20 (idiom-32); `|=` for the +0x314 bits.
// Left: the j counter's saved register ($s4 in the target, shared with
// flagBit; $s1 here) and the func_0019AFE0 result kept in $s1; 1200
// declaration orders, merging flagBit into j and the result into i tried.

extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_00102918(char *, char *, char *);
extern void func_00102948(char *, char *);
extern void func_001029C0(char *);
extern void func_00102BB0(char *, char *, float);
extern void func_001031E0(char *, char *);
extern float func_0011DF78(float);
extern int func_001760C0(char *, char *, int, float);
extern void func_00176390(char *, int, char *, int);
extern int func_00176C80(char *);
extern void func_0019A310(char *, char *);
extern int func_0019AD00(char *, char *, int);
extern int func_0019AFE0(char *, char *, char *, int);
extern float func_001B1470(float);

extern int D_002488C0[];
extern float D_002488C4[];
extern int D_002488D0[];
extern int D_002488E0[];
extern float D_00248950[];
extern unsigned char D_00275B00;
extern char D_700031B0[];
extern char D_700036A0[];
extern char D_700036D0[];
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038C0[];
extern float D_70003A20[];

void func_001764E0(char *arg0) {
    float sp60[4];
    int j;
    int i;
    int s2;
    float *pos;
    int flagBit;
    unsigned char m;
    int v0;
    int sbuf[4];
    int v1;

    s2 = 0;
    if (*(unsigned char *)(arg0 + 0x236) == 0) {
        s2 = func_00176C80(arg0);
    }
    if (*(unsigned char *)(arg0 + 4) == 1 && *(unsigned char *)(arg0 + 5) == 1) {
        sbuf[0] = 0;
        sbuf[1] = 0x3D4CCCCD;
        sbuf[2] = 0x40900000;
        sbuf[3] = 0x3F800000;
        pos = D_00248950;
        flagBit = i & 4;
        i = 0;
        do {
            func_001029C0(D_700036A0);
            func_00102BB0(D_700036A0, D_700036A0, func_001B1470(*(float *)(arg0 + 0xC4) + pos[0]));
            func_00102918(D_700036A0, D_700036A0, arg0 + 0xB0);
            func_001026A0(D_700038A0, D_700036A0, sbuf);
            if (func_0019AD00(arg0, D_700038A0, 6) != 0) {
                v1 = *(short *)(*(int *)0x700031D0 + 0x1A) & 0xFF00;
                if (v1 == 0x1000 || v1 == 0x800) {
                    *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + *(float *)0x700031C0;
                    *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + *(float *)0x700031C4;
                    *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + *(float *)0x700031C8;
                } else if (v1 == 0x2000) {
                    v0 = *(int *)0x700031D4;
                    if (v0 != 0) {
                        if (((*(unsigned char *)(v0 + 2) & ~0xE0) == 4) && (*(unsigned char *)(v0 + 3) == 2)) {
                            *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + *(float *)0x700031C0;
                            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + *(float *)0x700031C4;
                            *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + *(float *)0x700031C8;
                        }
                    } else if (flagBit) {
                        func_0019A310((char *)D_70003A20, (char *)v0);
                        D_70003A20[0] = func_0011DF78(D_70003A20[0]);
                        if (!(D_70003A20[0] < 1.2217306f) && D_70003A20[0] <= 1.9198622f) {
                            /* skip */
                        } else {
                            *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + *(float *)0x700031C0;
                            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + *(float *)0x700031C4;
                            *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + *(float *)0x700031C8;
                        }
                    }
                }
            }
            i += 1;
            pos += 1;
        } while (i < 5);
    }

    j = 0;
    pos = D_00248950;
    (&D_00275B00)[4] = *(unsigned char *)(arg0 + 0x314);
    *(unsigned char *)(arg0 + 0x314) = 0;
    do {
        func_001029C0(D_700036A0);
        func_00102BB0(D_700036A0, D_700036A0, func_001B1470(*(float *)(arg0 + 0xC4) + pos[0]));
        func_00102918(D_700036A0, D_700036A0, arg0 + 0xB0);
        func_001026A0(D_700038A0, D_700036A0, D_002488C0);
        *(int *)0x700038B0 = 0;
        *(float *)0x700038B4 = 4.01f;
        *(int *)0x700038B8 = 0;
        *(float *)0x700038BC = 1.0f;
        func_001026A0(D_700038C0, D_700036A0, D_700038B0);
        v0 = func_0019AFE0(arg0, D_700038C0, D_700038A0, 7);
        if (v0 != 0) {
            *(unsigned char *)(arg0 + 0x314) |= 1 << j;
            func_00176390(arg0, v0, D_700038A0, j);
        } else if (*(unsigned char *)(arg0 + 0x236) == 0) {
            D_70003A20[0] = 18.0f - D_002488C4[0];
            if (func_001760C0(arg0, D_700038A0, 1, D_70003A20[0]) != 0) {
                func_00102948((char *)sp60, D_700031B0);
                sp60[1] += 0.1f;
                m = *(unsigned char *)(arg0 + 0x1F0);
                if (j >= 3 || m == 0x37 || m == 0x36 || m == 0x3E
                    || *(float *)(arg0 + 0x38) < 0.0f
                    || *(float *)0x700031B4 - *(float *)(arg0 + 0xB4) < 13.8f) {
                    v0 = func_0019AD00(arg0, (char *)sp60, 7);
                    if (v0 != 0) {
                        *(unsigned char *)(arg0 + 0x314) |= 1 << j;
                        func_00176390(arg0, v0, (char *)sp60, j);
                    }
                } else if (s2 != 0) {
                    *(unsigned char *)(arg0 + 0x236) = 1;
                } else {
                    v0 = func_0019AD00(arg0, (char *)sp60, 7);
                    if (v0 != 0) {
                        *(unsigned char *)(arg0 + 0x314) |= 1 << j;
                        func_00176390(arg0, v0, (char *)sp60, j);
                    }
                }
            }
        } else {
            D_70003A20[0] = 13.8f - D_002488C4[0];
            if (func_001760C0(arg0, D_700038A0, 1, D_70003A20[0]) != 0) {
                func_00102948(D_700038B0, D_700031B0);
                *(float *)0x700038B4 = *(float *)0x700038B4 + 0.1f;
                v0 = func_0019AD00(arg0, D_700038B0, 7);
                if (v0 != 0) {
                    *(unsigned char *)(arg0 + 0x314) |= 1 << j;
                    func_00176390(arg0, v0, D_700038B0, j);
                }
            }
        }
        func_001031E0(D_700036D0, arg0 + 0xB0);
        if (*(unsigned char *)(arg0 + 0x236) == 0) {
            func_001026A0(D_700038A0, D_700036A0, D_002488E0);
        } else {
            func_001026A0(D_700038A0, D_700036A0, D_002488D0);
        }
        v0 = func_0019AD00(arg0, D_700038A0, 7);
        if (v0 != 0) {
            *(unsigned char *)(arg0 + 0x314) |= 1 << j;
            func_00176390(arg0, v0, D_700038A0, j);
        }
        j += 1;
        pos += 1;
    } while (j < 8);
}
