// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// SEMANTICS: Door/gate actor behaviour tick (sibling of the byte-matched
// func_001BDFC0 and of func_001BD9F0). Dispatches on the actor state byte p[4]
// (0=init, 1=opening sequence, 2=trigger wait, 3=despawn, 4=scripted):
//   state 0  - bone/anim init (func_001B0FD0), installs the behaviour table
//              &D_00275610 at p+0x30, builds the world matrix (func_001C6380),
//              then arms the actor from the global level/room pair
//              D_00810700/D_00810702 (level 13 sub 0, or level 4 sub 7 ->
//              script-run state 4 via func_001BA1A0(sub, D_0024E1A0)).
//   state 1  - 8-way jump-table sub-state machine (jtbl_0026E1E0) driving the
//              open animation: gate checks on the room mask
//              D_00810841[D_00810700], the linked actor chain
//              (+0x18 -> +0x18 -> byte +0xB), the 300-frame timer at p+0x28,
//              and func_001BC740 (player-proximity/interaction test).
//   state 2  - 3-way sub-state waiting on the hardware short at 0x70003B84
//              (== 0x208) or the player pointer D_008106C0 (health byte +4 and
//              distance float +0xB0 < 554.0).
//   state 3  - despawn (func_001AFC10).
//   state 4  - run the attached script (func_001BA1F0) then fall back to state 1.
// States 1/2/4 finish with func_001B17A0(p) and the actor's own vfunc at p+0x4C.

extern int D_00275610;
extern unsigned char D_00810700[8];
extern unsigned char D_00810702[8];
extern unsigned char D_0081076A[8];
extern unsigned char D_008107EA[8];
extern unsigned char D_0081083E[8];
extern unsigned char D_00810841[];
extern unsigned char *D_008106C0[2];
extern unsigned char D_0024E1A0[];

extern void func_001B0FD0(unsigned char *);
extern void func_001C6380(unsigned char *);
extern void func_001BA1A0(unsigned char *, unsigned char *);
extern void func_00158590(unsigned char *, int, int);
extern int func_001BC740(unsigned char *, unsigned char *);
extern int func_001BA1F0(unsigned char *);
extern void func_001B17A0(unsigned char *);
extern void func_001AFC10(unsigned char *);

void func_001BC960(unsigned char *p) {
    unsigned char *e;
    unsigned char *sub;
    unsigned char *q;
    short t;
    int n;

    e = *(unsigned char **)(p + 0x18);
    sub = p + 0x1F0;
    switch (p[4]) {
    case 0:
        func_001B0FD0(p);
        *(int *)(p + 0x30) = (int)&D_00275610;
        func_001C6380(p);
        if (*(unsigned short *)(p + 0x2E) == 0xFF) {
            if (p[3] == 1) {
                if ((D_00810700[0] == 0xD && D_00810702[0] == 0) ||
                    (D_00810700[0] == 4 && D_00810702[0] == 7)) {
                    func_001BA1A0(sub, D_0024E1A0);
                    p[4] = 4;
                } else {
                    p[0] = 1;
                }
            } else {
                p[0] = 1;
            }
        } else if (D_00810700[0] == 4) {
            if (D_00810702[0] == 7) {
                if (p[3] == 1) {
                    func_001BA1A0(sub, D_0024E1A0);
                    p[4] = 4;
                } else {
                    p[0] = 1;
                }
            } else {
                if (p[3] == 0) {
                    if (D_0081076A[0] == 0) {
                        p[4] = 2;
                        break;
                    }
                }
                if (p[3] == 1 && D_0081076A[0] != 0xFF) {
                    p[4] = 2;
                    p[5] = 1;
                }
            }
        }
        break;
    case 1:
        switch (p[5]) {
        case 0:
            if (*(unsigned short *)(p + 0x2E) == 0xFF) {
                if (p[3] == 0) {
                    if ((*(unsigned char **)(e + 0x18))[0xB] == 0) {
                        p[0] = 1;
                        func_00158590(p, 1, 0);
                        if (func_001BC740(p, sub)) {
                            p[5] = p[5] + 1;
                        }
                    } else {
                        func_00158590(p, 0, 0);
                        p[0] = 2;
                    }
                } else {
                    if ((*(unsigned char **)(*(unsigned char **)(e + 0x18) + 0x18))[0xB] == 3) {
                        p[0] = 1;
                        func_00158590(p, 1, 0);
                        if (func_001BC740(p, sub)) {
                            p[5] = p[5] + 1;
                        }
                    } else {
                        func_00158590(p, 0, 0);
                        p[0] = 2;
                    }
                }
            } else if (D_00810841[D_00810700[0]] & (1 << *(unsigned short *)(p + 0x2E))) {
                if (p[3] == 0) {
                    if ((*(unsigned char **)(e + 0x18))[0xB] == 0) {
                        p[0] = 1;
                        func_00158590(p, 1, 0);
                        if (func_001BC740(p, sub)) {
                            p[5] = p[5] + 1;
                        }
                    } else {
                        func_00158590(p, 0, 0);
                        p[0] = 2;
                    }
                } else {
                    if ((*(unsigned char **)(*(unsigned char **)(e + 0x18) + 0x18))[0xB] == 3) {
                        p[0] = 1;
                        func_00158590(p, 1, 0);
                        if (func_001BC740(p, sub)) {
                            p[5] = p[5] + 1;
                        }
                    } else {
                        func_00158590(p, 0, 0);
                        p[0] = 2;
                    }
                }
            } else {
                p[0] = 2;
                func_00158590(p, 0, 0);
            }
            break;
        case 1:
            func_00158590(p, 1, 0);
            if (func_001BA1F0(p)) {
                if (*(unsigned short *)(p + 0x2E) == 0xFF) {
                    if (p[3] == 0) {
                        p[5] = 2;
                    } else {
                        p[5] = 6;
                    }
                } else {
                    if (p[3] == 0) {
                        p[5] = 2;
                    } else {
                        p[5] = 6;
                    }
                }
            }
            break;
        case 2:
            func_00158590(p, 0, 0);
            if (*(unsigned short *)(p + 0x2E) == 0xFF) {
                if (D_0081083E[0] != 0) {
                    p[5] = 4;
                } else {
                    *(short *)(p + 0x28) = 300;
                    p[5] = 3;
                }
            } else {
                if (D_0081083E[0] == 0) {
                    p[5] = 4;
                } else {
                    *(short *)(p + 0x28) = 300;
                    p[5] = 3;
                }
            }
            break;
        case 3:
            t = *(short *)(p + 0x28) - 1;
            *(short *)(p + 0x28) = t;
            if (t == 0) {
                p[5] = p[5] + 1;
            }
            func_00158590(p, 0, 0);
            break;
        case 4:
            q = *(unsigned char **)(e + 0x18);
            if (q[0xB] == 0) {
                q[0xB] = 2;
                p[5] = p[5] + 1;
            }
            /* fallthrough */
        case 5:
            func_00158590(p, 0, 0);
            break;
        case 6:
            q = *(unsigned char **)(*(unsigned char **)(e + 0x18) + 0x18);
            if (q[0xB] == 3) {
                q[0xB] = 4;
            }
            p[5] = 7;
            /* fallthrough */
        case 7:
            func_00158590(p, 0, 0);
            break;
        }
        func_001B17A0(p);
        (*(void (**)(unsigned char *))(p + 0x4C))(p);
        break;
    case 2:
        switch (p[5]) {
        case 0:
            if (D_00810841[D_00810700[0]] & (1U << *(unsigned short *)(p + 0x2E))) {
                func_00158590(p, 0, 0);
                if (*(unsigned short *)0x70003B84 == 0x208) {
                    q = *(unsigned char **)(p + 0x18);
                    q = *(unsigned char **)(q + 0x18);
                    q[0xB] = 2;
                    p[4] = 1;
                    p[5] = 5;
                }
            }
            break;
        case 1:
            if (D_00810841[D_00810700[0]] & (1U << *(unsigned short *)(p + 0x2E))) {
                if (D_0081076A[0] == 0xFF) {
                    p[5] = p[5] + 1;
                } else if (D_008107EA[0] == 0x10) {
                    if (D_008106C0[0] == 0) {
                        p[0] = 1;
                        func_00158590(p, 1, 0);
                        if (func_001BC740(p, sub)) {
                            p[4] = 1;
                            p[5] = 1;
                        }
                    } else {
                        n = D_008106C0[0][4];
                        if (n >= 2) {
                            p[5] = p[5] + 1;
                        } else {
                            if (*(float *)(D_008106C0[0] + 0xB0) < 554.0f) {
                                p[0] = 1;
                                func_00158590(p, 1, 0);
                                if (func_001BC740(p, sub)) {
                                    p[4] = 1;
                                    p[5] = 1;
                                }
                            } else {
                                p[0] = 2;
                                func_00158590(p, 0, 0);
                            }
                        }
                    }
                }
            }
            break;
        case 2:
            p[0] = 1;
            func_00158590(p, 1, 0);
            if (func_001BC740(p, sub)) {
                p[4] = 1;
                p[5] = 1;
            }
            break;
        }
        func_001B17A0(p);
        (*(void (**)(unsigned char *))(p + 0x4C))(p);
        break;
    case 3:
        func_001AFC10(p);
        break;
    case 4:
        func_00158590(p, 0, 0);
        if (func_001BA1F0(p)) {
            p[4] = 1;
            p[0] = 1;
        }
        func_001B17A0(p);
        (*(void (**)(unsigned char *))(p + 0x4C))(p);
        break;
    }
}
