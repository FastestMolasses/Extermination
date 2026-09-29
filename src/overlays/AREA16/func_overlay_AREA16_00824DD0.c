// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA16 overlay, runtime 0x00824E10 (splat/link name 00824DD0; overlay code is
// linked 0x40 below where it runs), 0x44C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA16; lane A15C).
// Role: object with +0x20 (record) and +0x24 (switch owner). State 0 (after
//  func_001BAD40): state 1, attachments 0xB / 0xD / 0xC through func_001C5570,
//  0x8256E0(self, 1) (falls through). State 1: +0x28++, state 2 when the
//  owner's +0x2E mask has bit +0x2E; otherwise (record +4 not 0x270C/0x270D)
//  by record +0xA: 5 func_001C5C90 and return, 4 animate, 3 nothing, 0
//  func_001BA580 / func_001C64F0(record +0xC) / animate, other 0x825780(self,
//  1) and the same. Then keeps the three attachments on the +0x12C matrix (the
//  0xC attachment is retired at +0x28 == 0x2120 through 0x825730) and draws
//  (-22.9961, 30.6261, 10.9414) with func_001F4E20. State 2 advances to 3 and
//  calls func_001BA540 when the record allows. State 3 retires the attachments
//  and func_001AFC10.
extern float D_700038A0[];
extern int D_700038B0[];
extern int func_001BAD40(unsigned char *self, unsigned char *a);
extern unsigned char *func_001C5570(unsigned char *self, float *pos, int kind, int a3);
extern void func_overlay_AREA16_008256E0(unsigned char *self, int idx);
extern void func_overlay_AREA16_00825780(unsigned char *self, int idx);
extern void func_overlay_AREA16_00825730(unsigned char *self);
extern void func_001C5C90(unsigned char *self);
extern void func_001C68C0(unsigned char *self);
extern void func_001BA580(unsigned char *self, int a1);
extern void func_001C64F0(unsigned char *self, float step);
extern void func_00102958(void *dst, void *src);
extern void func_001026A0(void *dst, void *m, void *v);
extern void func_001F4E20(void *pos, void *col, float f12);
extern void func_001BA540(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA16_00824DD0(unsigned char *self) {
    unsigned char *a = *(unsigned char **)(self + 0x20);
    unsigned char *b = *(unsigned char **)(self + 0x24);
    unsigned char st = self[4];
    unsigned char *c;
    unsigned char *c2; /* a separate variable fixes the a0/a1 choice */
    int *p;
    switch (st) {
    case 0:
        if (func_001BAD40(self, a) != 0) {
            break;
        }
        self[4] = 1;
        D_700038A0[0] = 1.0f;
        D_700038A0[1] = 1.0f;
        D_700038A0[2] = 1.0f;
        D_700038A0[3] = 0.25f;
        *(unsigned char **)(self + 0x2EC) = func_001C5570(self, D_700038A0, 0xB, 0);
        *(unsigned char **)(self + 0x2E4) = func_001C5570(self, D_700038A0, 0xD, 0);
        D_700038A0[0] = 0.0f;
        D_700038A0[1] = 1.0f;
        D_700038A0[2] = 0.0f;
        D_700038A0[3] = 0.25f;
        *(unsigned char **)(self + 0x2E8) = func_001C5570(self, D_700038A0, 0xC, 0);
        func_overlay_AREA16_008256E0(self, 1);
    case 1:
        (*(short *)(self + 0x28))++;
        if (*(unsigned short *)(b + 0x2E) & (1 << *(unsigned short *)(self + 0x2E))) {
            self[4] = 2;
        } else {
            if (*(short *)(a + 4) == 0x270D || *(short *)(a + 4) == 0x270C) {
                break;
            }
            switch (*(short *)(a + 0xA)) {
            case 5:
                func_001C5C90(self);
                return;
            case 4:
                func_001C68C0(self);
                (*(void (**)(unsigned char *))(self + 0x4C))(self);
                break;
            case 3:
                break;
            case 0:
                func_001BA580(self, *(short *)(a + 4));
                func_001C64F0(self, *(float *)(a + 0xC));
                func_001C68C0(self);
                self[1] = 1;
                (*(void (**)(unsigned char *))(self + 0x4C))(self);
                break;
            default:
                func_overlay_AREA16_00825780(self, 1);
                func_001C64F0(self, *(float *)(a + 0xC));
                func_001C68C0(self);
                self[1] = 1;
                (*(void (**)(unsigned char *))(self + 0x4C))(self);
                break;
            }
        }
        c = *(unsigned char **)(self + 0x2EC);
        if (c != 0 && c[4] == 1) {
            func_00102958(*(unsigned char **)(c + 0x12C) + 0x90, *(unsigned char **)(self + 0x12C) + 0x90);
        }
        c2 = *(unsigned char **)(self + 0x2E8);
        p = (int *)(self + 0x1F0) + 0x3E;
        if (c2 != 0 && *(short *)(self + 0x28) < 0x2121) {
            if (*(short *)(self + 0x28) == 0x2120) {
                c2[4] = 3;
                *p = 0;
                func_overlay_AREA16_00825730(self);
            } else if (c2[4] == 1) {
                func_00102958(*(unsigned char **)(c2 + 0x12C) + 0x90, *(unsigned char **)(self + 0x12C) + 0x90);
            }
        }
        c = *(unsigned char **)(self + 0x2E4);
        if (c != 0 && c[4] == 1) {
            func_00102958(*(unsigned char **)(c + 0x12C) + 0x90, *(unsigned char **)(self + 0x12C) + 0x90);
        }
        D_700038A0[0] = -22.9961f;
        D_700038A0[1] = 30.6261f;
        D_700038A0[2] = 10.9414f;
        D_700038A0[3] = 1.0f;
        func_001026A0(D_700038A0, *(unsigned char **)(self + 0x12C) + 0x90, D_700038A0);
        D_700038B0[0] = 0x80;
        D_700038B0[1] = 0x80;
        D_700038B0[2] = 0x66;
        D_700038B0[3] = 0x80;
        func_001F4E20(D_700038A0, D_700038B0, 8.0f);
        break;
    case 2:
        self[4] = st + 1;
        if (*(short *)(a + 4) != 0x270D && *(short *)(a + 0xA) == 0) {
            func_001BA540(self);
        }
        break;
    case 3:
        if (*(unsigned char **)(self + 0x2EC) != 0) {
            (*(unsigned char **)(self + 0x2EC))[4] = 3;
        }
        if (*(unsigned char **)(self + 0x2E8) != 0) {
            (*(unsigned char **)(self + 0x2E8))[4] = 3;
        }
        if (*(unsigned char **)(self + 0x2E4) != 0) {
            (*(unsigned char **)(self + 0x2E4))[4] = 3;
        }
        func_001AFC10(self);
        break;
    }
}
