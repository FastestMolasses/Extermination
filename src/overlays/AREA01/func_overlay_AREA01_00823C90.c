// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA01 overlay, runtime 0x00823CD0 (splat/link name 00823C90; overlay code is
// linked 0x40 below where it runs), 0x40C bytes. Byte-identical C, linked from
// its compiled object: the text and both jump tables (runtime 0x0082CBA0 and
// 0x0082CBD0) match the original. The compiled .rodata is placed by
// tools/overlay/jt_pin.py at the table's original address, with the table
// address and its case-label entries resolved at runtime addresses (link +
// 0x40), which is what the original stores.
// Role: O3 owner. +4 (jump table, 6 entries): 0 = set-up (func_001BA1C0 id 7,
//  then func_00128AB0 / func_00129780), 1 = run: +5 (jump table, 10 entries)
//  dispatches to 0x8240E0, 0x824340, 0x824770 and 0x824D50, then the common
//  func_001C64F0 / func_00102958 / func_001C69A0 tail and the +0x4C callback;
//  2 and 3 call func_001AFC10; 4 falls into 5, which waits for +0x3C to reach
//  509.0 (+4 = 3) and, at 510.0, calls func_001EFD90(0x80000009, ...).
extern char *D_00275B40;
extern unsigned char D_002758C8[4];
extern int D_0028A6E8[2];
extern unsigned char D_008107DF[8];
extern float D_70003000[12];
extern float D_70003400[16];
extern float D_700038A0[4];
extern float D_700038B0[4];
extern int func_001BA1C0(unsigned char *self, int id);
extern int func_00128AB0(unsigned char *self, unsigned char *talk);
extern int func_00129780(unsigned char *self, unsigned char *talk, int arg);
extern void func_001C63E0(unsigned char *self, int arg);
extern void func_001029C0(void *m);
extern void func_001B17A0(unsigned char *self);
extern void func_overlay_AREA01_008240E0(unsigned char *self, unsigned char *talk);
extern void func_overlay_AREA01_00824340(unsigned char *self, unsigned char *talk);
extern void func_overlay_AREA01_00824770(unsigned char *self, unsigned char *talk);
extern void func_overlay_AREA01_00824D50(unsigned char *self, unsigned char *talk);
extern short func_001C64F0(unsigned char *self, float scale);
extern void func_00102958(void *dst, void *src);
extern void func_001C69A0(unsigned char *self);
extern void func_001288D0(unsigned char *self, unsigned char *talk);
extern void func_001AFC10(unsigned char *self);
extern void func_001CA6F0(unsigned char *self, int arg);
extern void func_001C6960(unsigned char *self);
extern void func_001026A0(void *dst, void *a, void *b);
extern void func_001FB9F0(int id, int a1, int a2, int a3);
extern void func_001EFD90(int id, void *a, void *b);

void func_overlay_AREA01_00823C90(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;

    switch (self[4]) {
    case 0:
        switch (self[5]) {
        case 0:
            if (func_001BA1C0(self, 7)) {
                self[4] = 3;
                break;
            }
            talk[0xE2] = self[0xD] >> 4;
            self[0xD] &= 0xF;
            talk[0xE3] = D_002758C8[talk[0xE2]];
            if (func_00128AB0(self, talk)) {
                self[5]++;
                *(short *)(talk + 0xF8) = 1;
                func_001C63E0(self, 1);
            }
            break;
        case 1:
            if (func_00129780(self, talk, self[0xD])) {
                self[0] = 2;
            }
            break;
        }
        break;
    case 1:
        func_001029C0(D_70003000);
        talk[0xFA] = 0;
        func_001B17A0(self);
        switch (self[5]) {
        case 0:
            self[5]++;
            self[6] = 0;
            self[7] = 0;
            break;
        case 1:
            talk[0xFA] = 1;
            func_overlay_AREA01_008240E0(self, talk);
            if (D_008107DF[0] == 2) {
                if (talk[0xE3] == 0) {
                    self[5] = 5;
                    self[6] = 0;
                    self[7] = 0;
                } else {
                    talk[0xE3]--;
                }
            }
            break;
        case 2:
        case 3:
        case 4:
        case 5:
            talk[0xFA] = 1;
            func_overlay_AREA01_00824340(self, talk);
            break;
        case 6:
            talk[0xFA] = 1;
            func_overlay_AREA01_00824770(self, talk);
            break;
        case 7:
            func_overlay_AREA01_00824D50(self, talk);
            break;
        case 8:
        case 9:
            break;
        }
        if (*(float *)(talk + 0xD8)) {
            *(float *)(talk + 0xEC) = 2.0f;
        } else {
            *(float *)(talk + 0xEC) = 1.0f;
        }
        *(short *)(talk + 0xF4) = func_001C64F0(self, *(float *)(talk + 0xEC));
        if (self[5] != 7) {
            func_00102958(D_70003400, D_70003000);
            func_001C69A0(self);
        }
        if (self[1]) {
            if (*(signed char *)(talk + 0xFA)) {
                func_001288D0(self, talk);
            }
            (*(void (**)(unsigned char *))(self + 0x4C))(self);
        }
        self[0xB] = 0;
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    case 4:
        func_001CA6F0(self, 1);
        self[4]++;
        *(int *)(self + 0x40) = D_0028A6E8[0];
        *(short *)(talk + 0xF8) = 1;
        func_001C63E0(self, 1);
    case 5:
        func_001288D0(self, talk);
        *(short *)(talk + 0xF4) = func_001C64F0(self, 1.0f);
        if (*(float *)(self + 0x3C) == 509.0f) {
            self[4] = 3;
        }
        func_001C6960(self);
        if (*(float *)(self + 0x3C) == 510.0f) {
            *(float *)0x700038A0 = 0.0f;
            *(float *)0x700038B0 = 0.0f;
            *(float *)0x700038B4 = 0.0f;
            *(float *)0x700038B8 = 0.0f;
            *(float *)0x700038A4 = 1.0f;
            *(float *)0x700038A8 = 1.0f;
            *(float *)0x700038AC = 1.0f;
            *(float *)0x700038BC = 1.0f;
            func_001026A0(D_700038A0, *(char **)(D_00275B40 + 0xC) + 0x90, D_700038A0);
            func_001FB9F0(0x1B2, 0x1000, 0x1000, 0x1000);
            func_001EFD90(0x80000009, D_700038A0, D_700038B0);
        }
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    }
}
