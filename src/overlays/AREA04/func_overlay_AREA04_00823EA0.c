// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA04 overlay, runtime 0x00823EE0 (splat/link name 00823EA0;
// overlay code is linked 0x40 below where it runs), 0x220 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA04; lane A04C).
// Role: model kind +0xD from the 8-byte table 0x827530 indexed by
// D_00810701 (second kind at +4, 0x827534). State 0 func_001B0FD0 and
// func_001CA5E0(self, +0x44, 1); D_00810764 == 0xFF -> state 2. State 1:
// sub 0 waits for D_008107E4 == 2 (0xFF flag -> state 2); sub 1 when
// D_70003B84 >= 0x55D, or the flag is 0xFF with the first kind still set,
// switches to the second kind (func_001C6120(D_0028A59C, kind),
// func_001CA6E0, func_001C62C0, func_001CA5E0) and state 2. State 2 copies
// +0x7C of the object at link->link->+0x110 into *D_00275B40 + 0x7C. States
// 1/2 end with func_001C6380 and the +0x4C method; state 3 func_001AFC10.
extern unsigned char D_00810764[];
extern unsigned char D_00810701[];
extern unsigned char D_008107E4[];
extern unsigned short D_70003B84[];
extern int D_0028A59C[];
extern char **D_00275B40;
extern unsigned char D_overlay_AREA04_00827530[];
extern unsigned char D_overlay_AREA04_00827534[];
extern void func_001B0FD0(unsigned char *self);
extern void func_001CA5E0(unsigned char *self, int a1, int a2);
extern char *func_001C6120(int a0, int a1);
extern void func_001CA6E0(unsigned char *self, char *model);
extern void func_001C62C0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA04_00823EA0(unsigned char *self) {
    char *link = *(char **)(*(char **)(self + 0x18) + 0x18);
    switch (self[4]) {
    case 0:
        if (D_00810764[0] != 0xFF) {
            self[0xD] = D_overlay_AREA04_00827530[D_00810701[0] * 8];
        }
        func_001B0FD0(self);
        func_001CA5E0(self, *(int *)(self + 0x44), 1);
        if (D_00810764[0] == 0xFF) {
            self[4] = 2;
        }
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (D_00810764[0] == 0xFF) {
                self[4] = 2;
            } else if (D_008107E4[0] == 2) {
                self[5] = self[5] + 1;
            }
            break;
        case 1:
            if (D_70003B84[0] >= 0x55D ||
                (D_00810764[0] == 0xFF && self[0xD] == D_overlay_AREA04_00827530[D_00810701[0] * 8])) {
                self[4] = 2;
                self[0xD] = D_overlay_AREA04_00827534[D_00810701[0] * 8];
                func_001CA6E0(self, func_001C6120(D_0028A59C[0], self[0xD]));
                func_001C62C0(self);
                func_001CA5E0(self, *(int *)(self + 0x44), 1);
            }
            break;
        }
        func_001C6380(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 2:
        *(float *)(D_00275B40[0] + 0x7C) = *(float *)(*(char **)(link + 0x110) + 0x7C);
        func_001C6380(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 3:
        func_001AFC10(self);
        break;
    }
}
