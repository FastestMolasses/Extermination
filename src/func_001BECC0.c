// NEARMISS func_001BECC0  (vram 0x001BECC0, 0x6FC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.21% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// residual register-allocation / instruction-scheduling deltas (a couple of lh/sh reg-name swaps on the arg0+0x34/+0x36 timer arithmetic, and a dsll32/dsra32 sign-extend the target keeps but our codegen elides). Body/structure fully recovered and byte-verified for the actual field offsets, constant...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

extern void bone_init_default_1(char *);
extern void func_00187EC0(int, int);
extern void func_001AF890(int, unsigned char);
extern void func_001AFC10(char *);
extern void func_001B1190(unsigned char);
extern void func_001B17A0(void *);
extern int func_001BE5F0(char *, char *, char *);
extern int func_001BEAC0(char *, char *, int, int);
extern void func_001BEB70(char *);
extern void func_001BEC40(char *, char *, int);
extern void func_001C1500(char *, int, float, float, float);
extern int func_001C1570(unsigned char);
extern void func_001C6380(char *);
extern int func_001D0C80(char *, int);
extern void func_001D0D40(char *, char *, int, int);
extern short func_001D0D60(int, float);
extern void func_001EFD90(int, char *, char *);
extern void func_001EFE00(int, char *);
extern void func_001FBD50(char *, int, int, float);

extern char D_0024E960[];
extern char D_00275620[4];
extern char D_00275628[4];
extern int D_0028A500[2];
extern int D_0028A504[2];
extern char D_008102B0[];
extern char D_700038A0[];

void func_001BECC0(char *arg0) {
    unsigned char st;
    unsigned char st2;
    char *s0;
    char *s1;
    char *spad;
    int v0;
    short v1;

    st = *(unsigned char *)(arg0 + 4);
    s1 = D_008102B0;
    s0 = arg0 + 0x1F0;
    switch (st) {
    case 0:
        *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + 0.8f;
        spad = D_700038A0;
        *(volatile int *)0x700038A0 = 0;
        *(volatile int *)0x700038A4 = 0;
        *(volatile int *)0x700038A8 = 0;
        *(volatile float *)0x700038AC = 1.0f;
        *(int *)(arg0 + 0x20) = func_001BEAC0(D_700038A0, spad, 0x64, 0);
        if (*(int *)(arg0 + 0x20) == 0) {
            *(unsigned char *)(arg0 + 4) = 3;
            return;
        }
        func_001BEB70(arg0);
        if (func_001D0C80(arg0, D_0028A500[0]) == 0) {
            func_001D0D40(arg0, D_0024E960, 0x65, 1);
            *(short *)(s0 + 2) = 0;
            bone_init_default_1(arg0);
            *(int *)(arg0 + 0x58) = D_0028A504[0];
            *arg0 = 1;
            *(char **)(arg0 + 0x30) = D_00275620;
            *(char **)(s0 + 0x18) = D_00275628;
            *(unsigned char *)(arg0 + 4) = 1;
            *(short *)(s0 + 0) = 0;
            *(short *)(arg0 + 0x34) = 0x3C;
            *(float *)(arg0 + 0x38) = 0.0f;
            *(char *)(arg0 + 0xB) = 0;
            *(int *)(s0 + 0x1C) = 0;
        }
        return;
    case 1:
        func_001B17A0(arg0);
        *(char *)(arg0 + 1) = 1;
        st2 = *(unsigned char *)(arg0 + 5);
        switch (st2) {
        case 0:
            if (func_001BE5F0(s1, arg0, s0) != 0) {
                func_00187EC0(8, 0);
                if (!(*(float *)(s1 + 0x38) < 0.4f)) {
                    *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                    *(float *)(arg0 + 0x38) = 0.0f;
                    func_001FBD50(arg0, 0x435, 0, 300.0f);
                    func_001BEC40(arg0, s0, 1);
                } else {
                    func_001BEC40(arg0, s0, 0);
                }
            } else {
                func_001BEC40(arg0, s0, 0);
            }
            break;
        case 1:
            func_001BEC40(arg0, s0, 1);
            *(float *)(arg0 + 0x38) = *(float *)(arg0 + 0x38) + 0.6f;
            if (*(float *)(arg0 + 0x38) > 12.0f) {
                *(float *)(arg0 + 0x38) = 12.0f;
                *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                *(short *)(arg0 + 0x28) = 0x28;
            }
            if (func_001BE5F0(s1, arg0, s0) != 0) {
                func_00187EC0(8, 0);
                if (*(float *)(arg0 + 0x38) > 4.0f && *(unsigned char *)s1 == 1) {
                    *(float *)(s1 + 0x224) = 5.0f;
                    *(unsigned char *)s1 = 3;
                    func_001EFD90(0x80000023, s1 + 0xA0, s1 + 0xC0);
                }
            }
            break;
        case 2:
            v0 = func_001BE5F0(s1, arg0, s0);
            if (v0 != 0) {
                func_00187EC0(8, 0);
                if (*(unsigned char *)s1 == 1) {
                    *(float *)(s1 + 0x224) = 5.0f;
                    *(unsigned char *)s1 = 3;
                    func_001EFD90(0x80000023, s1 + 0xA0, s1 + 0xC0);
                }
            }
            func_001BEC40(arg0, s0, 1);
            v1 = *(short *)(arg0 + 0x28);
            if (v1 == 0) {
                if (v0 == 0) {
                    *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                    func_001FBD50(arg0, 0x436, 0, 300.0f);
                } else if (*(float *)(s1 + 0x38) < 0.4f) {
                    *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                    func_001FBD50(arg0, 0x436, 0, 300.0f);
                }
            } else {
                *(short *)(arg0 + 0x28) = v1 - 1;
            }
            break;
        case 3:
            if (func_001BE5F0(s1, arg0, s0) != 0) {
                func_00187EC0(8, 0);
            }
            func_001BEC40(arg0, s0, 0);
            *(float *)(arg0 + 0x38) = *(float *)(arg0 + 0x38) - 0.4f;
            if (*(float *)(arg0 + 0x38) < 0.0f) {
                *(float *)(arg0 + 0x38) = 0.0f;
                *(unsigned char *)(arg0 + 5) = 0;
            }
            break;
        }
        if (*(int *)(s0 + 0x1C) != 0) {
            *(int *)(s0 + 0x1C) = *(int *)(s0 + 0x1C) - 1;
        }
        if (*(short *)(arg0 + 0x36) != 0) {
            if (*(short *)(arg0 + 0x36) & 0x5000) {
                if (*(int *)(s0 + 0x1C) == 0) {
                    *(int *)(s0 + 0x1C) = 0x3C;
                    if (*(short *)(arg0 + 0x36) & 0x4000) {
                        func_001EFE00(0x80000027, arg0);
                    }
                } else {
                    *(short *)(arg0 + 0x36) = 0;
                }
            }
            v1 = *(short *)(arg0 + 0x36) & 0xFFF;
            *(short *)(arg0 + 0x34) = *(short *)(arg0 + 0x34) - v1;
            if (*(short *)(arg0 + 0x34) <= 0) {
                *arg0 = 2;
                *(short *)(arg0 + 0x34) = 0;
                *(unsigned char *)(arg0 + 4) = 2;
                *(unsigned char *)(arg0 + 5) = 0;
                func_001C1500(arg0, 0, 20.0f, 20.0f, 10.0f);
                func_001FBD50(arg0, 0x15D, 0, 300.0f);
                func_001FBD50(arg0, 0x43A, 0, 300.0f);
            } else {
                *arg0 = 1;
                *(short *)(arg0 + 0x36) = 0;
            }
        }
        func_001C6380(arg0);
        *(short *)(s0 + 0) = func_001D0D60(*(int *)(arg0 + 0x90), 1.0f);
        func_001B17A0(arg0);
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        return;
    case 2:
        if (func_001C1570(st) != 0) {
            *(unsigned char *)(arg0 + 4) = 3;
            *(float *)(arg0 + 0x38) = 0.0f;
        }
        *(float *)(arg0 + 0x64) = *(float *)(arg0 + 0x64) - 0.01f;
        if (*(float *)(arg0 + 0x64) < 0.0f) {
            *(float *)(arg0 + 0x64) = 0.0f;
        }
        *(float *)(arg0 + 0x38) = *(float *)(arg0 + 0x38) - 0.4f;
        if (*(float *)(arg0 + 0x38) < 0.0f) {
            *(float *)(arg0 + 0x38) = 0.0f;
        }
        *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) - 0.01f;
        func_001BEC40(arg0, s0, 0);
        *(short *)(s0 + 0) = func_001D0D60(*(int *)(arg0 + 0x90), 1.0f);
        func_001C6380(arg0);
        if (*(float *)(arg0 + 0x64) != 0.0f) {
            (*(void (**)(char *))(arg0 + 0x4C))(arg0);
            return;
        }
        break;
    case 3:
        func_001AF890(*(int *)(arg0 + 0x90), st);
        func_001B1190(*(unsigned char *)(arg0 + 0x9A));
        func_001AFC10(arg0);
        break;
    }
}
