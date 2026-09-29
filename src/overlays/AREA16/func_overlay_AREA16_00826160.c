// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA16 overlay, runtime 0x008261A0 (splat/link name 00826160; overlay code is
// linked 0x40 below where it runs), 0x224 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA16; lane A15C).
// Role: state 0: with bit 1 of D_0081080B sets +0xD = 0x10, +0xE from the
//  D_0024D7C0 record table and state 2, else +0 = 1 and state 1 (each after
//  func_001B0FD0 / func_001C6380). State 1: while +0x36 and the +0x18 parent
//  is not in state 0xC9, when *D_008102C8 == 1 swaps the model
//  (func_001C6120(D_0028A59C, 0x10), func_001CA6E0), func_001EFD20(2, +0xB0),
//  +0xE from the table, func_001C62C0, func_001C6380, sets bit 1 of D_0081080B
//  and state 2; otherwise clears +0x36. States 1/2 func_001B17A0 and +0x4C.
//  State 3 func_001AFC10.
/* 0x28-byte records; the halfword read is at record (+0x9A) * 0x28 + 0x2E */
typedef struct { char pad[6]; unsigned short id; char pad2[0x20]; } Ent;
extern Ent **D_0024D7C0[];
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_0081080B;
extern unsigned char *D_008102C8;
extern int D_0028A59C;
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern int func_001C6120(int a0, int a1);
extern void func_001CA6E0(unsigned char *self, int a1);
extern void func_001EFD20(int a0, void *pos);
extern void func_001C62C0(unsigned char *self);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA16_00826160(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (D_0081080B & 2) {
            self[0xD] = 0x10;
            *(unsigned short *)(self + 0xE) =
                D_0024D7C0[D_00810700][D_00810701][self[0x9A] + 1].id;
            if (func_001B0FD0(self) == 0) {
                func_001C6380(self);
                self[4] = 2;
            }
        } else {
            if (func_001B0FD0(self) == 0) {
                func_001C6380(self);
                self[0] = 1;
                self[4] = 1;
            }
        }
        break;
    case 1:
        if (*(short *)(self + 0x36) != 0) {
            if ((*(unsigned char **)(self + 0x18))[4] != 0xC9) {
                if (*D_008102C8 == 1) {
                    func_001CA6E0(self, func_001C6120(D_0028A59C, 0x10));
                    func_001EFD20(2, self + 0xB0);
                    *(unsigned short *)(self + 0xE) =
                        D_0024D7C0[D_00810700][D_00810701][self[0x9A] + 1].id;
                    func_001C62C0(self);
                    func_001C6380(self);
                    D_0081080B |= 2;
                    self[4] = 2;
                } else {
                    *(short *)(self + 0x36) = 0;
                }
            }
        }
        func_001B17A0(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 2:
        func_001B17A0(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 3:
        func_001AFC10(self);
        break;
    }
}
