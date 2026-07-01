// NEARMISS func_0015A2C0  (vram 0x0015A2C0, 0x48C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.63% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring / instruction-scheduling residual only (same documented family as src/func_0012B410.c). Top-level state dispatch: target colors the switch selector into $a2 with a beqz-to-body + early partial-epilogue (jumps to a mid-restore label with lq $ra in its own delay slot); mine colors...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int func_00122BB8(void);
extern int func_0015A200(unsigned char *parent, int kind, int pairIdx);
extern void func_0015A750(unsigned char *self);
extern void func_001AFC10(unsigned char *self, int state);
extern void func_001B17A0(unsigned char *self);
extern void func_001E9580(unsigned char *self, unsigned short a1, void *a2);
extern void func_001E9E60(unsigned short a0, int a1);
extern void func_001FBD50(unsigned char *self, int a1, int a2, float a3);
extern float D_00248120[];
extern unsigned char D_002481B0[];
extern unsigned char D_002481D0[];
extern float D_002481F0[];
extern unsigned char D_008106EC;
extern unsigned char D_008106ED;
extern float D_700038A0;
extern float D_700038A8;

void func_0015A2C0(unsigned char *self) {
    int state;
    float *tbl;
    int sub;
    float f;
    int row;

    state = *(unsigned char *)(self + 4);
    if (state == 3) {
        goto state23;
    }
    if (state == 2) {
        goto state23;
    }
    if (state == 1) {
        goto state1;
    }
    if (state != 0) {
        return;
    }
    {
        tbl = D_00248120 + (*(short *)(self + 0x54)) * 5;
        *(float **)(self + 0x30) = tbl;
        *(self + 0xC) = 0;
        *(self + 9) = 0;
        *(self + 0) = 1;
        *(self + 4) = 1;
        *(int *)(self + 0x1F0) = 0;
        *(int *)(self + 0x1F4) = func_00122BB8();
        *(float *)0x700038A0 = tbl[0] * 2.0f;
        *(float *)0x700038A8 = tbl[2] * 2.0f;
        func_001E9580(self, *(unsigned short *)(self + 0xE), (void *)&D_700038A0);
        *(self + 0xA) = 0;
        *(unsigned short *)(self + 0x2E) = 0;
        *(float *)(self + 0x20) = 0.0f;
        switch (*(short *)(self + 0x56)) {
        case 0:
            break;
        case 1:
            row = *(int *)0x70003B68 & 3;
            *(short *)(self + 0x56) = D_002481B0[(row << 3) + (D_008106EC & 7)];
            D_008106EC += 1;
            if (*(short *)(self + 0x56) == 1) {
                func_0015A200(self, 0xE, 0);
                func_0015A200(self, 0xE, 1);
            }
            break;
        case 2:
            row = *(int *)0x70003B68 & 3;
            *(short *)(self + 0x56) = D_002481D0[(row << 3) + (D_008106ED & 7)];
            D_008106ED += 1;
            if (*(short *)(self + 0x56) == 1) {
                func_0015A200(self, 0xE, 0);
                func_0015A200(self, 0xE, 1);
            }
            break;
        }
        *(float *)(self + 0x80) = 0.0f;
        *(self + 0xB) = 0;
        return;
    }
state1:
    {
        func_001E9E60(*(unsigned short *)(self + 0xE), state);
        if (*(short *)(self + 0x56) == 1) {
            sub = *(unsigned char *)(self + 5);
            switch (sub) {
            case 0:
                *(float *)(self + 0x80) = *(float *)(self + 0x20) / 100.0f;
                if (*(self + 0xA) != 0) {
                    f = *(float *)(self + 0x20) + 1.0f;
                    *(float *)(self + 0x20) = f;
                    if (!(f < 100.0f)) {
                        *(self + 5) = *(unsigned char *)(self + 5) + 1;
                        *(float *)(self + 0x20) = 60.0f;
                        *(float *)(self + 0x80) = 1.0f;
                        *(self + 0xB) = 1;
                    }
                } else {
                    *(float *)(self + 0x20) = 0.0f;
                }
                break;
            case 1:
                if (!(*(int *)0x70003B64 & 0x7F)) {
                    func_001FBD50(self, 0x42F, 0, 300.0f);
                }
                *(float *)(self + 0x80) = *(float *)(self + 0x20) / 60.0f;
                func_0015A750(self);
                if (*(self + 0xA) == 0) {
                    f = *(float *)(self + 0x20) - 1.0f;
                    *(float *)(self + 0x20) = f;
                    if (f <= 0.0f) {
                        *(float *)(self + 0x80) = 0.0f;
                        *(self + 5) = 0;
                        *(self + 0xB) = 0;
                        *(int *)(self + 0x1F0) = 0;
                    }
                } else {
                    *(float *)(self + 0x20) = 60.0f;
                }
                break;
            }
        } else if (*(short *)(self + 0x56) == 2) {
            sub = *(unsigned char *)(self + 5);
            if (sub != 2) {
                switch (sub) {
                case 0:
                    if (*(self + 0xA) != 0) {
                        f = *(float *)(self + 0x20) + 1.0f;
                        *(float *)(self + 0x20) = f;
                        if (!(f <= 120.0f)) {
                            if (func_0015A200(self, 0xD, 0) != 0) {
                                *(unsigned short *)(self + 0x2E) = *(unsigned short *)(self + 0x2E) + 1;
                            }
                            if ((int)*(unsigned short *)(self + 0x2E) >= 4) {
                                *(self + 5) = 2;
                            } else {
                                *(self + 5) = 1;
                                row = (func_00122BB8() >> 0x10) * 3;
                                *(float *)(self + 0x20) = D_002481F0[row >> 0xF];
                            }
                        }
                    } else {
                        *(float *)(self + 0x20) = 0.0f;
                    }
                    break;
                case 1:
                    f = *(float *)(self + 0x20) - 1.0f;
                    *(float *)(self + 0x20) = f;
                    if (f <= 0.0f) {
                        *(self + 5) = 0;
                    }
                    break;
                }
            }
        }
        func_001B17A0(self);
        *(self + 0xA) = 0;
        return;
    }
state23:
    func_001AFC10(self, state);
}
