// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// SEMANTICS:
//   Actor (re)initialisation. p[3] selects the init flavour and p[0xD] the
//   variant within it; both map onto an animation id that defaults to the
//   caller-supplied `id` when no variant matches. Flavour 0 also clears the
//   16-bit field at +0x2E and the two words at +0x210/+0x214 (q = p + 0x1F0);
//   flavour 4 clears the 16-bit field at +0x28; flavour 2 uses the jump table
//   and, for variants 8..12, additionally records 1..5 in the global
//   D_008106C6. Any other flavour bails out with 1.
//   The tail resolves the animation (func_001C6120 on the table pointed to by
//   D_0028A56C, then func_001CA6E0 to install it), reads the bone count via
//   func_001C6150(*(int *)(p + 0x44)) into p[0xC], and refuses with 1 if that
//   count exceeds the free-node counter D_00275BCC. Otherwise it pulls
//   p[0xC] nodes from the free list (func_001AF780) into the pointer array at
//   p + 0x110, mirrors the count into p[9], and runs the bone setup helpers.
//   anim_bone_array_setup ignores its argument (its body only touches
//   D_00275B40/D_00275B48) but the original call site does materialise
//   p[0xC] into $a0, so the argument is kept to reproduce that.

extern short D_00275BCC;
extern char *D_0028A56C[2];
extern unsigned char D_008106C6[8];

extern char *func_001C6120();
extern unsigned char func_001C6150();
extern void func_001CA6E0();
extern int func_001AF780();
extern void anim_bone_array_setup();
extern void bone_init_default_1();

int func_0018A8D0(unsigned char *p, int id)
{
    unsigned char *q;
    int i;
    unsigned char *e;

    q = p + 0x1F0;
    switch (p[3]) {
    case 0:
        id = 0x2F;
        *(short *)(p + 0x2E) = 0;
        *(int *)(q + 0x20) = 0;
        *(int *)(q + 0x24) = 0;
        break;
    case 1:
        switch (p[0xD]) {
        case 0:
            id = 0x30;
            break;
        case 0x10:
            id = 0x40;
            break;
        case 0x15:
            id = 0x6D;
            break;
        }
        break;
    case 2:
        switch (p[0xD]) {
        case 0:
            id = 0x32;
            break;
        case 1:
            id = 0x33;
            break;
        case 2:
            id = 0x34;
            break;
        case 3:
            id = 0x35;
            break;
        case 4:
            id = 0x36;
            break;
        case 5:
            id = 0x31;
            break;
        case 6:
            id = 0x37;
            break;
        case 7:
            id = 0x38;
            break;
        case 8:
            id = 0x39;
            D_008106C6[0] = 1;
            break;
        case 9:
            id = 0x3A;
            D_008106C6[0] = 2;
            break;
        case 10:
            id = 0x3B;
            D_008106C6[0] = 3;
            break;
        case 11:
            id = 0x3C;
            D_008106C6[0] = 4;
            break;
        case 12:
            id = 0x3D;
            D_008106C6[0] = 5;
            break;
        }
        break;
    case 4:
        id = 0x6A;
        *(short *)(p + 0x28) = 0;
        break;
    default:
        return 1;
    }

    func_001CA6E0(p, func_001C6120(D_0028A56C[0], id));
    p[0xC] = func_001C6150(*(int *)(p + 0x44));
    if (D_00275BCC < (int)p[0xC]) {
        return 1;
    }
    i = 0;
    e = p;
    while (i < (int)p[0xC]) {
        *(int *)(e + 0x110) = func_001AF780();
        e += 4;
        i++;
    }
    p[9] = p[0xC];
    anim_bone_array_setup(p[0xC]);
    bone_init_default_1(p);
    return 0;
}
