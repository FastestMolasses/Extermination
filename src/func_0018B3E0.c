// NEARMISS func_0018B3E0  (vram 0x0018B3E0, 0x5D4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.06% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/structure fully recovered (state machine, all field offsets, calls, and idiom-15 reloads confirmed). Two genuine residuals remain at 97.06%/233: (1) commutative add.s FP-operand register coloring on the 3 timer-tick accumulations (arg0+0x70 + arg0+0xB0) -- both source orders tried give the s...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Anim/clip state-machine driver dispatched on the state byte at arg0+4
// (cases 0/1/2/3). State 0: registers a bone-count via func_001C6120/
// func_001CA6E0, allocates the bone array, calls anim_bone_array_setup +
// bone_init_default_1, kicks off a clip via func_00103230/func_001028D0,
// links an entity via func_001EFE00, arms a timer via func_001FBD50, then
// copies 12 D_00810550.. float constants into *D_00275B40+0x90.. and,
// depending on arg0+0xD, picks one of three (timer, angle-const, event-id)
// triples. State 1: drives the physics/anim blend each frame (func_0019A570/
// func_0019B6C0/build_trs_matrix), then dispatches to the linked entity.
// State 2: bumps the state, re-links entity, zeroes the D_700038A0 vector,
// calls func_001EFFD0 + arms another timer, clears D_00810CB6. State 3:
// tears down via func_001AFC10.
extern void *anim_bone_array_setup(unsigned char n);
extern void bone_init_default_1(char *p);
extern void build_trs_matrix(char *dst, char *pos, char *rot, char *scale);
extern int func_001028D0(char *dst, char *a, char *b);
extern void func_00102948(char *dst, char *src);
extern int func_001029C0(int a);
extern void func_001031E0(char *dst, char *src);
extern void func_00103230(char *dst, char *src, float f);
extern void func_0011A070(unsigned char a, int b, unsigned char c);
extern int func_0019A570(char *a, char *b, int c, int d);
extern int func_0019B6C0(char *a, char *b);
extern int func_001AF780(void);
extern void func_001AFC10(unsigned char a);
extern void func_001B41F0(char *p, char *a, char *b, int c);
extern unsigned char func_001C6120(int a, int b);
extern unsigned char func_001C6150(int a);
extern char *func_001CA6E0(char *p, unsigned char n);
extern void func_001E8B90(char *p, float f);
extern char *func_001EFE00(int a, char *p);
extern void func_001EFFD0(int a, char *b, char *c, unsigned char d, int e);
extern unsigned char func_001FBD50(char *p, int a, int b, float f);

extern char **D_00275B40;
extern short D_00275BCC;
extern int D_0028A56C[2];
extern float D_00810550[3];
extern float D_00810554[3];
extern float D_00810558[3];
extern float D_0081055C[3];
extern float D_00810560[3];
extern float D_00810564[3];
extern float D_00810568[3];
extern float D_0081056C[3];
extern float D_00810570[3];
extern float D_00810574[3];
extern float D_00810578[3];
extern float D_0081057C[3];
extern unsigned char D_00810CB6[8];
extern float D_700031B0[3];
extern float D_700038A0[3];

void func_0018B3E0(char *arg0) {
    unsigned char st;
    int i;
    char *p;

    st = *(unsigned char *)(arg0 + 4);
    switch (st) {
    case 0: {
        unsigned char n;

        func_001CA6E0(arg0, func_001C6120(D_0028A56C[0], 0x25));
        *(unsigned char *)(arg0 + 0xC) = func_001C6150(*(int *)(arg0 + 0x44));
        if (D_00275BCC < (int)*(unsigned char *)(arg0 + 0xC)) {
            return;
        }
        for (i = 0; i < (int)*(unsigned char *)(arg0 + 0xC); i++) {
            *(int *)(arg0 + 0x110 + i * 4) = func_001AF780();
        }
        n = *(unsigned char *)(arg0 + 0xC);
        *(unsigned char *)(arg0 + 9) = n;
        anim_bone_array_setup(*(unsigned char *)(arg0 + 0xC));
        bone_init_default_1(arg0);
        *(unsigned char *)(arg0 + 4) = *(unsigned char *)(arg0 + 4) + 1;
        func_00103230(arg0 + 0xA0, arg0 + 0x70, 6.0f);
        func_001028D0(arg0 + 0xA0, arg0 + 0xB0, arg0 + 0xA0);
        *(char **)(arg0 + 0x20) = func_001EFE00(0x80000038, arg0);
        *(unsigned char *)(arg0 + 0x1F0) = func_001FBD50(arg0, 0x5DE, 0, 300.0f);
        func_001029C0(*(int *)(arg0 + 0x110));
        (*(float **)D_00275B40)[0x90 / 4] = D_00810550[0];
        (*(float **)D_00275B40)[0x94 / 4] = D_00810554[0];
        (*(float **)D_00275B40)[0x98 / 4] = D_00810558[0];
        (*(float **)D_00275B40)[0x9C / 4] = D_0081055C[0];
        (*(float **)D_00275B40)[0xA0 / 4] = D_00810560[0];
        (*(float **)D_00275B40)[0xA4 / 4] = D_00810564[0];
        (*(float **)D_00275B40)[0xA8 / 4] = D_00810568[0];
        (*(float **)D_00275B40)[0xAC / 4] = D_0081056C[0];
        (*(float **)D_00275B40)[0xB0 / 4] = D_00810570[0];
        (*(float **)D_00275B40)[0xB4 / 4] = D_00810574[0];
        (*(float **)D_00275B40)[0xB8 / 4] = D_00810578[0];
        (*(float **)D_00275B40)[0xBC / 4] = D_0081057C[0];
        if (*(unsigned char *)(arg0 + 0xD) == 0) {
            *(short *)(arg0 + 0x34) = 0x118;
            func_00103230(arg0 + 0x70, arg0 + 0x70, 1.67f);
            *(short *)(arg0 + 0x28) = 0xB3;
            return;
        }
        if (*(unsigned char *)(arg0 + 0xD) == 1) {
            *(short *)(arg0 + 0x34) = 0xB4;
            func_00103230(arg0 + 0x70, arg0 + 0x70, 3.36f);
            *(short *)(arg0 + 0x28) = 0x59;
            return;
        }
        *(short *)(arg0 + 0x34) = 0x17C;
        func_00103230(arg0 + 0x70, arg0 + 0x70, 1.67f);
        *(short *)(arg0 + 0x28) = 0x5A;
        return;
    }
    case 1: {
        int r;

        if (*(unsigned char *)0x70003B8D != 0) {
            p = *(char **)(arg0 + 0x20);
            *(char *)(p + 4) = 2;
            if ((int)*(unsigned char *)(arg0 + 0x1F0) != -1) {
                func_0011A070(*(unsigned char *)(arg0 + 0x1F0), 2, st);
            }
            D_00810CB6[0] = 0;
            *(unsigned char *)(arg0 + 4) = 3;
            return;
        }
        {
            short timer = *(short *)(arg0 + 0x28);

            *(short *)(arg0 + 0x28) = timer - 1;
            if (timer == 0) {
                *(unsigned char *)(arg0 + 4) = 2;
            } else if (D_00810CB6[0] == 2) {
                *(unsigned char *)(arg0 + 4) = 2;
            } else {
                *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0x70) + *(float *)(arg0 + 0xB0);
                *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0x74) + *(float *)(arg0 + 0xB4);
                *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0x78) + *(float *)(arg0 + 0xB8);
            }
        }
        if ((r = func_0019A570(arg0 + 0xA0, arg0 + 0xB0, 7, 0x20)) != 0) {
            func_001031E0((char *)D_700038A0, (char *)D_700031B0);
            *(int *)0x700038AC = 0x3F800000;
            func_00102948(arg0 + 0xB0, (char *)D_700038A0);
            if ((unsigned int)(r - 1) < 2U) {
                p = *(char **)0x700031D4;
                if (p != 0) {
                    unsigned char kind = *(unsigned char *)(p + 0);
                    if (kind != 0) {
                        if ((*(unsigned char *)(p + 2) & ~0xE0) == 2) {
                            if (kind == 1) {
                                func_001B41F0(p, (char *)D_700038A0, arg0 + 0xC0,
                                              *(int *)(*(char **)0x700031D0 + 0x1C));
                            }
                        } else {
                            *(short *)(p + 0x36) = *(short *)(arg0 + 0x34);
                            func_00102948(p + 0x70, arg0 + 0xC0);
                        }
                    }
                }
            }
            *(unsigned char *)(arg0 + 4) = 2;
        } else if (func_0019B6C0(arg0 + 0xA0, arg0 + 0xB0) != 0) {
            func_001031E0(arg0 + 0xB0, (char *)D_700031B0);
            if (*(unsigned char *)(*(char **)0x700031D0 + 0x1A) == 0x5B) {
                func_001E8B90(arg0 + 0xB0, 4.0f);
            }
            *(unsigned char *)(arg0 + 4) = 2;
        }
        *(float *)(arg0 + 0xA0) = *(float *)(arg0 + 0xB0);
        *(float *)(arg0 + 0xA4) = *(float *)(arg0 + 0xB4);
        *(float *)(arg0 + 0xA8) = *(float *)(arg0 + 0xB8);
        *(float *)(arg0 + 0xAC) = *(float *)(arg0 + 0xBC);
        build_trs_matrix(arg0 + 0xD0, arg0 + 0xB0, arg0 + 0xC0, arg0 + 0x60);
        (*(float **)D_00275B40)[0xC0 / 4] = *(float *)(arg0 + 0xB0);
        (*(float **)D_00275B40)[0xC4 / 4] = *(float *)(arg0 + 0xB4);
        (*(float **)D_00275B40)[0xC8 / 4] = *(float *)(arg0 + 0xB8);
        (*(float **)D_00275B40)[0xCC / 4] = *(float *)(arg0 + 0xBC);
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        return;
    }
    case 2:
        *(unsigned char *)(arg0 + 4) = st + 1;
        p = *(char **)(arg0 + 0x20);
        *(char *)(p + 4) = 2;
        if ((int)*(unsigned char *)(arg0 + 0x1F0) != -1) {
            func_0011A070(*(unsigned char *)(arg0 + 0x1F0), 2, st);
        }
        *(int *)0x700038A0 = 0;
        *(int *)0x700038A4 = 0;
        *(int *)0x700038A8 = 0;
        *(int *)0x700038AC = 0;
        func_001EFFD0(0x8000006A, arg0 + 0xB0, (char *)D_700038A0, *(unsigned char *)(arg0 + 0xD), 0);
        func_001FBD50(arg0, 0x5DF, 0, 300.0f);
        D_00810CB6[0] = 0;
        return;
    case 3:
        func_001AFC10(st);
        break;
    }
}
