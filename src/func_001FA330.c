// NEARMISS func_001FA330  (vram 0x001FA330, 0x234 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.84% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// regalloc
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS (objdiff 95.8% vs func_001FA330.o under mwcc 2.3.3). Body fully
// recovered; sole residual is register-allocation order (loop index colored
// $s2 here vs $s0 in target) plus one dead cap-select tail mwcc schedules
// differently. Permuter-class. 991202 build scores 90.5%.
//
// Per-channel meter update over 3 entries (D_00282150[].active flag at +4,
// D_00281FD0[] state block stride 0x60: current value +0x58, delta +0x54,
// callback flag +0x5C). Channel 0 is gated on the global D_0028215B (when
// set, drives two outputs via 1<<D_00281FD4 / 1<<D_002820F4); channels 1..2
// drive func_0011A608(*(ll*)(blk+8), val, val). cap = 0x3FFF, or 0x3000 for
// channel 0 while D_0028215B clear.
extern int float_to_int(float);
extern void func_0011A608(long long, int, int);
extern void func_001FAAC0(int);
extern char D_00281FD0;
extern int D_00281FD4;
extern int D_002820F4;
extern char D_00282150;
extern unsigned char D_0028215B;

void func_001FA330(void) {
    char *s1;
    char *s2;
    int i;
    int cap;
    float v;
    float f20;
    int iv;

    i = 0;
    s1 = &D_00282150;
    s2 = &D_00281FD0;
    do {
        if (i != 0) {
            cap = 0x3FFF;
        } else {
            cap = 0x3000;
            if (D_0028215B == 0) {
                cap = 0x3FFF;
            }
        }
        if (*(char *)(s1 + 4) != 0) {
            v = *(float *)(s2 + 0x54);
            if (v != 0.0f) {
                *(float *)(s2 + 0x58) = *(float *)(s2 + 0x58) + v;
                if (*(float *)(s2 + 0x54) > 0.0f) {
                    if (!(*(float *)(s2 + 0x58) < (float)cap)) {
                        *(int *)(s2 + 0x54) = 0;
                        *(float *)(s2 + 0x58) = (float)cap;
                    }
                } else if (*(float *)(s2 + 0x58) <= 0.0f) {
                    *(int *)(s2 + 0x54) = 0;
                    *(int *)(s2 + 0x58) = 0;
                    if (*(char *)(s2 + 0x5C) != 0) {
                        func_001FAAC0(i);
                    }
                }
                if (i != 0) {
                    f20 = *(float *)(s2 + 0x58);
                    iv = float_to_int(f20);
                    func_0011A608(*(long long *)(s2 + 8), iv, float_to_int(f20));
                } else if (D_0028215B == 0) {
                    func_0011A608((long long)1 << D_00281FD4, 0, float_to_int(*(float *)(s2 + 0x58)));
                    func_0011A608((long long)1 << D_002820F4, float_to_int(*(float *)(s2 + 0x58)), 0);
                } else {
                    f20 = *(float *)(s2 + 0x58);
                    iv = float_to_int(f20);
                    func_0011A608((long long)1 << D_00281FD4, iv, float_to_int(f20));
                    f20 = *(float *)(s2 + 0x58);
                    iv = float_to_int(f20);
                    func_0011A608((long long)1 << D_002820F4, iv, float_to_int(f20));
                }
            }
        }
        i += 1;
        s1 += 1;
        s2 += 0x60;
    } while (i < 3);
}
