// NEARMISS func_0015AC00  (vram 0x0015AC00, 0x214 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.09% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Instruction-scheduling/CSE artifact (NOT clean-store nop): target folds the shared 2.0f constant-load into the 0x6D/group beq delay slots (mwcc emits nop), and the trailing switch's func_001F1110(self,0) arm is placed via a scheduled block move. Body/logic fully correct. 233 best at 91.09% (thres...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// NEARMISS (233: 91.1%, 991202: 72.3%). Weapon/entity setup dispatched on the
// type byte at arg0+0xD. Selects a float written to arg0+0x60/0x64/0x68:
// 1.5f for type 0x5B, 2.0f for 0x6D and the {0x40,0x41,0x42,0x45,0x4D,0x4E,0x4F,
// 0x55,0x56,0x57,0x59,0x6C} group, 1.0f otherwise. Then, on the low nibble of
// arg0+3: if ==1, writes 4.0f to arg0+0x80/0x84/0x88 and runs func_001B0FD0(self)
// (early-return 1 on success); else runs func_001B1020(self, type, -1, 0)
// (early-return 1 on success). On the fall-through path: func_001C6380(self),
// sets state byte arg0+0 = 1, arg0+8 = 3, installs vtable/handler &D_00275488 at
// arg0+0x30, then dispatches on the low nibble of arg0+3 to func_001F1110 with a
// mode arg (nibble 1 -> 1, nibble 2 -> 4, nibble 0 -> 5 if type==0x34 else 0,
// else 0), returning 0.
//
// WALL: body/logic fully recovered. Residual is compiler instruction-scheduling:
// (1) the target folds the shared 2.0f constant-load into the 0x6D/group `beq`
// delay slots (mwcc emits `nop` there instead), and (2) the trailing switch
// arm ordering/duplication of the func_001F1110(self,0) path differ by a
// scheduled block move. Not the clean-store delay-slot nop; 233 does not close
// it. Genuine schedule/CSE artifact -> parked as readable near-miss.
extern int func_001B0FD0(char *);
extern int func_001B1020(char *, unsigned char, int, int);
extern void func_001C6380(char *);
extern void func_001F1110(char *, int);
extern int D_00275488;

int func_0015AC00(char *arg0) {
    unsigned char type;
    int v;
    int sub;

    type = *(unsigned char *)(arg0 + 0xD);
    switch (type) {
    case 0x5B:
        v = 0x3FC00000;
        *(int *)(arg0 + 0x68) = v;
        *(int *)(arg0 + 0x64) = v;
        break;
    case 0x6D:
        v = 0x40000000;
        *(int *)(arg0 + 0x68) = v;
        *(int *)(arg0 + 0x64) = v;
        break;
    case 0x6C:
    case 0x59:
    case 0x57:
    case 0x56:
    case 0x55:
    case 0x4F:
    case 0x4E:
    case 0x4D:
    case 0x45:
    case 0x42:
    case 0x41:
    case 0x40:
        v = 0x40000000;
        *(int *)(arg0 + 0x68) = v;
        *(int *)(arg0 + 0x64) = v;
        break;
    default:
        v = 0x3F800000;
        *(int *)(arg0 + 0x68) = v;
        *(int *)(arg0 + 0x64) = v;
        break;
    }
    *(int *)(arg0 + 0x60) = v;

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
    sub = *(unsigned char *)(arg0 + 3) & 0xF;
    switch (sub) {
    case 1:
        func_001F1110(arg0, 1);
        break;
    case 2:
        func_001F1110(arg0, 4);
        break;
    case 0:
        if (*(unsigned char *)(arg0 + 0xD) == 0x34) {
            func_001F1110(arg0, 5);
        } else {
            func_001F1110(arg0, 0);
        }
        break;
    default:
        func_001F1110(arg0, 0);
        break;
    }
    return 0;
}
