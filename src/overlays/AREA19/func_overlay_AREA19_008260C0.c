// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x00826100 (splat/link name 008260C0; overlay code
//  is linked 0x40 below where it runs), 0x364 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA19; lane A13C).
// Role: sub 0 placement [18]. State 0 when func_001B0FD0 returns 0: +0 = 1,
//  state 3 when D_00810777 == 0xFF, else state 4. State 4: with the player in
//  the box 962 < x < 968, y > 293, 923.6 < z < 929.6 and D_008104A0 == 0x2A:
//  script 0x82C6E0, state 1. State 1: while z > 796.589, after 31 frames z -=
//  1 per frame, and between z 810.023 and 916.321 y -= 0.1597; when z < 808
//  and D_00810777 == 0: func_001FC580(self, 0x19D), func_001B1E20(7, 40),
//  func_001EFD90(0x8000000A / 0x80000015 / 0x8000000B) at the object,
//  D_00810777 = 0xFF; state 3 when the script ends.
typedef void (*ActorFn)(unsigned char *);
extern float D_00810350;
extern float D_00810358;
extern unsigned char D_00810777;
extern char D_overlay_AREA19_0082C6E0[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern int func_001B17A0(unsigned char *self);
extern void func_001FC580(unsigned char *self, int id);
extern void func_001B1E20(int a, int b);
extern void func_001EFD90(unsigned int msg, void *pos, void *dir);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA19_008260C0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    float x;
    float z;
    short t;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
            self[0] = 1;
            if (*(unsigned char *)0x810777 == 0xFF) {
                self[4] = 3;
            } else {
                self[4] = 4;
            }
            *(int *)(self + 0x2E8) = 0;
            *(int *)(self + 0x2EC) = 0;
        }
        break;
    case 4:
        x = D_00810350;
        if (!(x <= 962.0f) && x < 968.0f && !(*(float *)0x810354 <= 293.0f)) {
            z = *(float *)0x810358;
            if (!(z <= 923.6f) && z < 929.6f && *(unsigned char *)0x8104A0 == 0x2A) {
                func_001BA1A0(talk, D_overlay_AREA19_0082C6E0);
                self[4] = 1;
            }
        }
        if (func_001B17A0(self) != 0) {
            (*(ActorFn *)(self + 0x4C))(self);
        }
        break;
    case 1:
        z = D_00810358;
        if (!(z <= 796.589f)) {
            t = *(short *)(self + 0x28);
            if (t > 0x1E) {
                *(float *)0x810358 = z - 1.0f;
            } else {
                *(short *)(self + 0x28) = t + 1;
            }
            z = *(float *)0x810358;
            if (z < 916.321f && !(z <= 810.023f)) {
                *(float *)0x810354 -= 0.1597114f;
            }
        }
        if (D_00810358 < 808.0f && *(unsigned char *)0x810777 == 0) {
            func_001FC580(self, 0x19D);
            func_001B1E20(7, 0x28);
            func_001EFD90(0x8000000A, self + 0xB0, self + 0xC0);
            func_001EFD90(0x80000015, self + 0xB0, self + 0xC0);
            func_001EFD90(0x8000000B, self + 0xB0, self + 0xC0);
            *(unsigned char *)0x810777 = 0xFF;
        }
        if (func_001BA1F0(self) != 0) {
            self[4] = 3;
        }
        if (D_00810777 == 0 && func_001B17A0(self) != 0) {
            (*(ActorFn *)(self + 0x4C))(self);
        }
        break;
    case 3:
        func_001AFC10(self);
        break;
    }
}
