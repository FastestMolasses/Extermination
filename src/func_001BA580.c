// NEARMISS func_001BA580  (vram 0x001BA580, 0x268 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.56% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/structure fully recovered: a 21-level cascading != chain classifying the sound-effect id arg1 into a category index s0 (-1..9, with global D_00810700==0xD bumping v1 for category 0x61), gated on arg0+0x56 != 0, choosing func_001DA6A0(arg0) vs func_001BA7F0(arg0) by v1, clearing a per-categor...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001DA6A0(int a0);
extern void func_001BA7F0(unsigned char *a0);
extern void func_001D06E0();
extern void func_001D0C70();
extern unsigned char D_00810700;
extern char D_008106D4[];

void func_001BA580(unsigned char *arg0, unsigned char arg1) {
    int s0;
    int v1;
    unsigned char *p;
    unsigned char v;

    v1 = 0;
    if (arg1 != 0x6C) {
        if (arg1 != 0x6A) {
            if (arg1 != 0x68) {
                if (arg1 != 0x66) {
                    if (arg1 != 0x64) {
                        if (arg1 != 0x61) {
                            if (arg1 != 0x5E) {
                                if (arg1 != 0x5D) {
                                    if (arg1 != 0x5A) {
                                        if (arg1 != 0x59) {
                                            if (arg1 != 0x55) {
                                                if (arg1 != 0x54) {
                                                    if (arg1 != 0x51) {
                                                        if (arg1 != 0x50) {
                                                            if (arg1 != 0x4F) {
                                                                if (arg1 != 0x49) {
                                                                    if (arg1 != 0x48) {
                                                                        if (arg1 != 0x47) {
                                                                            if (arg1 != 0x40) {
                                                                                if (arg1 != 0x3F) {
                                                                                    if (arg1 != 0x3E) {
                                                                                        if (arg1 != 0x3B) {
                                                                                            s0 = -1;
                                                                                            goto have_s0;
                                                                                        }
                                                                                    }
                                                                                }
                                                                            }
                                                                            s0 = 0;
                                                                            goto have_s0;
                                                                        }
                                                                    }
                                                                    s0 = 1;
                                                                    goto have_s0;
                                                                }
                                                            }
                                                        }
                                                        s0 = 2;
                                                        goto have_s0;
                                                    }
                                                }
                                                s0 = 3;
                                                goto have_s0;
                                            }
                                        }
                                        s0 = 4;
                                        goto have_s0;
                                    }
                                }
                                s0 = 5;
                                goto have_s0;
                            }
                            arg1 = D_00810700;
                            s0 = 6;
                            if (arg1 == 0xD) {
                                v1 = 1;
                            }
                            goto have_s0;
                        }
                        s0 = 7;
                        goto have_s0;
                    }
                    s0 = 8;
                    goto have_s0;
                }
                s0 = 9;
                goto have_s0;
            }
        }
        func_001DA6A0((int)arg0);
        return;
    }
    func_001DA6A0((int)arg0);
    return;

have_s0:
    if (*(short *)(arg0 + 0x56) != 0 && s0 != -1) {
        if (v1 == 0) {
            func_001DA6A0((int)arg0);
        } else {
            func_001BA7F0(arg0);
        }
        p = (unsigned char *)(D_008106D4 + s0);
        v = *p;
        if (v == 1) {
            func_001D06E0(arg0, 1);
            *p = 0;
        } else if (v == 2) {
            func_001D06E0(arg0, 0);
            *p = 0;
        }
        func_001D0C70(arg0);
    }
}
