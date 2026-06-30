// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Entity/actor init: zeroes anim slots, sets default pose ids from a small
// state table at +0xE, kicks off bone setup, and arms the per-mode flags.
// NOTE: D_0028A580/D_0028A578/D_008106F1 declared as arrays purely to force
// non-gp-rel (lui %hi / lw %lo) addressing the target uses; read as scalars.
// func_001CA6F0 takes (actor,1): target keeps arg0 in a0 across the call.
extern void func_001CA6F0(char *, int);
extern int func_001AF780(void);
extern void anim_bone_array_setup(unsigned char);
extern void bone_init_default_2(char *, short);
extern void func_001C68C0(char *);
extern int func_0018A880(int, int);
extern void func_0015C310(char *, int);
extern void func_001F0120(char *, int);
extern int func_001EFE00(int, char *);

extern int D_0028A580[2];
extern int D_0028A578[2];
extern short D_00248A00[];
extern short D_00275BCC;
extern int D_00275490;

int func_0015C420(char *arg0) {
    int i;
    char *p;
    unsigned char e;

    func_001CA6F0(arg0, 1);
    *(int *)(arg0 + 0x40) = D_0028A580[0];
    *(int *)(arg0 + 0x58) = D_0028A578[0];
    *(int *)(arg0 + 0x5C) = 0x10101;
    *(short *)(arg0 + 0x20C) = D_00248A00[*(unsigned char *)(arg0 + 0x235)];
    *(char *)(arg0 + 0x1F0) = 0;
    if (D_00275BCC < (int)*(unsigned char *)(arg0 + 0xC)) {
        return 1;
    }
    for (i = 0, p = arg0; i < (int)*(unsigned char *)(arg0 + 0xC); i++) {
        *(int *)(p + 0x110) = func_001AF780();
        p += 4;
    }
    *(unsigned char *)(arg0 + 9) = *(unsigned char *)(arg0 + 0xC);
    anim_bone_array_setup(*(unsigned char *)(arg0 + 0xC));
    bone_init_default_2(arg0, *(short *)(arg0 + 0x20C));
    func_001C68C0(arg0);
    *(int *)(arg0 + 0x280) = 0;
    *(int *)(arg0 + 0x284) = 0xC15CCCCD;
    *(int *)(arg0 + 0x288) = 0;
    *(int *)(arg0 + 0x28C) = 0x3F800000;
    *(int *)(arg0 + 0x18) = func_0018A880(4, 0);
    *(char *)(arg0 + 0x275) = 0;
    func_0015C310(arg0, 0);
    *(int *)(arg0 + 0x30) = (int)&D_00275490;
    e = *(unsigned char *)(arg0 + 0xE);
    if (e == 1) {
        if (*(unsigned char *)0x70003B8D == 0) {
            *(unsigned char *)0x70003B8D = 4;
        }
        *(char *)(arg0 + 4) = 5;
        *(char *)(arg0 + 5) = 1;
        *(char *)(arg0 + 6) = 0;
        *(char *)(arg0 + 0x1F0) = 1;
    } else if (e == 2) {
        *(char *)(arg0 + 4) = 1;
        *(char *)(arg0 + 5) = 0x1A;
        *(char *)(arg0 + 6) = 0;
        *(char *)(arg0 + 0x1F0) = 0x2E;
        *(char *)(arg0 + 0xD) = 0;
    } else if (e == 3) {
        if (*(unsigned char *)0x70003B8D == 0) {
            *(unsigned char *)0x70003B8D = 4;
        }
        *(char *)(arg0 + 4) = 5;
        *(char *)(arg0 + 5) = 4;
        *(char *)(arg0 + 6) = 0;
        *(char *)(arg0 + 0x1F0) = 0x45;
    } else if (e == 4) {
        if (*(unsigned char *)0x70003B8D == 0) {
            *(unsigned char *)0x70003B8D = 4;
        }
        *(char *)(arg0 + 4) = 5;
        *(char *)(arg0 + 5) = 2;
        *(char *)(arg0 + 6) = 0;
        *(char *)(arg0 + 0x1F0) = 0x43;
        *(char *)(arg0 + 0x2F1) = 0;
    } else if (e == 5) {
        if (*(unsigned char *)0x70003B8D == 0) {
            *(unsigned char *)0x70003B8D = 4;
        }
        *(char *)(arg0 + 4) = 5;
        *(char *)(arg0 + 5) = 3;
        *(char *)(arg0 + 6) = 0;
        *(char *)(arg0 + 0x1F0) = 0x44;
        *(char *)(arg0 + 0x2F1) = 0;
        *(int *)(arg0 + 0x268) = 0;
    } else {
        *(char *)(arg0 + 4) = 1;
        *(char *)(arg0 + 5) = 0;
        *(char *)(arg0 + 6) = 0;
    }
    *(int *)(arg0 + 0x34) = 0x3F800000;
    *(int *)(arg0 + 0x204) = 0x3F800000;
    *(char *)(arg0 + 0) = 1;
    func_001F0120(arg0, 0x3B);
    if (*(unsigned char *)(arg0 + 0x234) != 0) {
        *(int *)(arg0 + 0x1C) = func_001EFE00(0x80000048, arg0);
    }
    *(char *)(arg0 + 0x31A) = 0;
    *(char *)(arg0 + 0x31B) = -1;
    return 0;
}
