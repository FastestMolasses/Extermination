// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 91.09% NEARMISS).
// Fixes read from the original instructions:
//  - Each case of the type switch writes all three fields (0x68, 0x64, 0x60)
//    itself and breaks; the 2.0f case for 0x6D shares one body with the
//    0x40..0x6C group (case labels written in ascending order, 0x6D last).
//  - The 2.0f case is written before the 1.5f case (body layout order).
//  - In the trailing mode switch, case 0 with type != 0x34 falls into the
//    default arm (func_001F1110(self, 0)) instead of a duplicated call.
//
// Weapon/entity setup dispatched on the type byte at arg0+0xD. Selects a float
// written to arg0+0x60/0x64/0x68: 1.5f for type 0x5B, 2.0f for 0x6D and the
// {0x40,0x41,0x42,0x45,0x4D,0x4E,0x4F,0x55,0x56,0x57,0x59,0x6C} group, 1.0f
// otherwise. Then, on the low nibble of arg0+3: if ==1, writes 4.0f to
// arg0+0x80/0x84/0x88 and runs func_001B0FD0(self) (early-return 1 on success);
// else runs func_001B1020(self, type, -1, 0) (early-return 1 on success). On the
// fall-through path: func_001C6380(self), sets state byte arg0+0 = 1, arg0+8 = 3,
// installs vtable/handler &D_00275488 at arg0+0x30, then dispatches on the low
// nibble of arg0+3 to func_001F1110 with a mode arg (nibble 1 -> 1, nibble 2 -> 4,
// nibble 0 -> 5 if type==0x34 else 0, else 0), returning 0.
extern int func_001B0FD0(char *);
extern int func_001B1020(char *, unsigned char, int, int);
extern void func_001C6380(char *);
extern void func_001F1110(char *, int);
extern int D_00275488;

int func_0015AC00(char *arg0) {
    switch (*(unsigned char *)(arg0 + 0xD)) {
    case 0x40:
    case 0x41:
    case 0x42:
    case 0x45:
    case 0x4D:
    case 0x4E:
    case 0x4F:
    case 0x55:
    case 0x56:
    case 0x57:
    case 0x59:
    case 0x6C:
    case 0x6D:
        *(int *)(arg0 + 0x68) = 0x40000000;
        *(int *)(arg0 + 0x64) = 0x40000000;
        *(int *)(arg0 + 0x60) = 0x40000000;
        break;
    case 0x5B:
        *(int *)(arg0 + 0x68) = 0x3FC00000;
        *(int *)(arg0 + 0x64) = 0x3FC00000;
        *(int *)(arg0 + 0x60) = 0x3FC00000;
        break;
    default:
        *(int *)(arg0 + 0x68) = 0x3F800000;
        *(int *)(arg0 + 0x64) = 0x3F800000;
        *(int *)(arg0 + 0x60) = 0x3F800000;
        break;
    }

    if ((*(unsigned char *)(arg0 + 3) & 0xF) == 1) {
        *(int *)(arg0 + 0x80) = 0x40800000;
        *(int *)(arg0 + 0x84) = 0x40800000;
        *(int *)(arg0 + 0x88) = 0x40800000;
        if (func_001B0FD0(arg0) != 0) {
            return 1;
        }
    } else {
        if (func_001B1020(arg0, *(unsigned char *)(arg0 + 0xD), -1, 0) != 0) {
            return 1;
        }
    }
    func_001C6380(arg0);
    *(char *)(arg0 + 0) = 1;
    *(char *)(arg0 + 8) = 3;
    *(int *)(arg0 + 0x30) = (int)&D_00275488;
    switch (*(unsigned char *)(arg0 + 3) & 0xF) {
    case 1:
        func_001F1110(arg0, 1);
        break;
    case 2:
        func_001F1110(arg0, 4);
        break;
    case 0:
        if (*(unsigned char *)(arg0 + 0xD) == 0x34) {
            func_001F1110(arg0, 5);
            break;
        }
    default:
        func_001F1110(arg0, 0);
        break;
    }
    return 0;
}
