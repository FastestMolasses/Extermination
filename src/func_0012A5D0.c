// NEARMISS func_0012A5D0  (vram 0x0012A5D0, 0x7F0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.72% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 5 residual instructions out of 508; instruction count, order and every reloc (incl. the jtbl_0026D000 dispatch) match. (1) 2x conditional-branch delay-slot speculation: at the outer dispatch `beq a2,v1,<case2>` and at case 3's `beq v1,v0,<0xA-path>` the target leaves the slot NOP, but mwcc233 hoi...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// SEMANTICS: Player actor main tick. `p` is the player actor, `sub` = p+0x1F0 is
// its animation/controller sub-block. Dispatches on the actor state byte p[4]:
//   state 0 - spawn: func_00128AB0 gate, then bone_init_default_2 + timer reset;
//             sub-state 1 finishes the spawn via func_00129780 and jumps to
//             sub-state 9 for the 4/9 spawn-pose variants.
//   state 1 - the live gameplay tick. Early-outs on func_001B2140, on the
//             pause/mode byte 0x70003B8D (>= 2 with sub->0xF6 == 0) and on the
//             64-frame-phase collision poll func_001B0D80. Runs func_001B17A0
//             then, unless func_00128B80 consumed the frame, a 14-way
//             jump-table (jtbl_0026D000) over the movement sub-state p[5]:
//             idle/walk (func_00128640, func_0012AFC0, func_0012B410), the
//             damage/knockback set (func_0012B970/BE20/C490/CAA0/D240), the
//             ladder & climb set (func_001C2770 + func_0012D580/DD70 +
//             func_001C3D60), func_0012D850/D940 and the death path
//             func_0012B850. Sub-states 1/2/13 test the sub-block float at
//             sub+0xD8 and, when it is 0, push the (0,-1.4,0,1) probe vector
//             through 0x700038A0 into func_001C25E0 to enter sub-state 8.
//             Afterwards the anim rate at sub+0xEC is 1.0 or 1.8 depending on
//             sub->0xD8, anim_advance_time stores its result at sub+0xF4, the
//             camera matrix is republished (copy_qw4 + func_001C69A0) unless in
//             sub-state 7, and p[1] triggers func_001288D0 + the actor vfunc at
//             p+0x4C. Ends by clearing p[0xA], p[0xB] and the short at p+0x54.
//   state 2 - paused/scripted: same 64-frame poll, blocked by D_0081078F, then
//             func_00129FC0.
//   state 3 - death/teardown: func_0012E070(sub); for death types 0xA..0xC it
//             decrements the scratch-pad counter at 0x700031F4, otherwise a set
//             sub->0xE0 respawns in place (state 4, position copied from
//             sub+0x50). Falls through to func_001B1190(p[0x9A]) +
//             func_001AFC10 (despawn).
//   state 4 - respawn: init, then func_00129780, a random hold time picked from
//             the gp-relative table D_00275380, the countdown at p+0x28, and
//             finally the ground/space test func_001B13F0(player+0xA0, p+0xB0,
//             80.0) + func_001B1630 before returning to state 1.

extern char D_008102B0[];
extern unsigned char D_0081078F[8];
extern short D_00275380[2];
extern char D_70003000[];
extern char D_70003400[];
extern char D_700038A0[];

extern void bone_init_default_2(char *arg0, short arg1);
extern short anim_advance_time(char *arg0, float dt);
extern void copy_qw4(void *a0, void *a1);
extern void func_00102948(void *a0, void *a1);
extern void func_001029C0(void *a0);
extern int func_00122BB8(void);
extern int func_00128640(char *arg0);
extern void func_001288D0(char *arg0, char *p);
extern void func_001289C0(char *a0, char *a1);
extern int func_00128AB0(unsigned char *a0, unsigned char *a1);
extern int func_00128B80(char *arg0, char *arg1);
extern int func_00129780(unsigned char *a, unsigned char *b, unsigned char sel);
extern void func_00129FC0(char *arg0, char *arg1);
extern void func_0012AFC0(char *arg0, char *arg1);
extern void func_0012B410(char *arg0, char *arg1);
extern void func_0012B850(char *arg0, char *arg1);
extern void func_0012B970(char *arg0, char *arg1);
extern void func_0012BE20(char *arg0, char *arg1);
extern void func_0012C490(unsigned char *arg0, unsigned char *arg1);
extern void func_0012CAA0(char *arg0, char *arg1);
extern void func_0012D240(char *arg0, char *arg1);
extern void func_0012D580(unsigned char *arg0, unsigned char *arg1, int arg2);
extern void func_0012D850(char *arg0, char *arg1);
extern void func_0012D940(char *arg0, char *arg1);
extern void func_0012DD70(char *arg0, char *arg1);
extern void func_0012E070(char *a0);
extern void func_001AFC10(unsigned char *p);
extern int func_001B0D80(char *arg0);
extern void func_001B1190(int a0);
extern int func_001B13F0(int arg0, int arg1, float fparg0);
extern int func_001B1630(float fx, float fy, float fz);
extern void func_001B17A0(unsigned char *p);
extern int func_001B2140(unsigned char *p);
extern int func_001C25E0(void *a0, void *a1);
extern int func_001C2770(unsigned char *a0, unsigned char *a1, int a2);
extern void func_001C3D60(unsigned char *a0, unsigned char *a1);
extern void func_001C69A0(unsigned char *a0);

void func_0012A5D0(unsigned char *p) {
    char *plr;
    unsigned char *sub;
    int m;
    int r;
    short t;
    int idx;

    plr = D_008102B0;
    sub = p + 0x1F0;
    switch (p[4]) {
    case 0:
        switch (p[5]) {
        case 0:
            if (func_00128AB0(p, sub)) {
                p[5] = p[5] + 1;
                *(short *)(sub + 0xF8) = 1;
                bone_init_default_2((char *)p, 1);
                *(short *)(p + 0x28) = 0;
            }
            break;
        case 1:
            if (func_00129780(p, sub, p[0xD])) {
                *(short *)(p + 0x54) = 0;
                p[0] = 1;
                if (p[0xD] == 4 || p[0xD] == 9) {
                    p[5] = 9;
                }
            }
            break;
        }
        break;
    case 1:
        if (func_001B2140(p) == 0) {
            break;
        }
        m = *(unsigned char *)0x70003B8D;
        if (m != 0 && (unsigned char)m >= 2 && *(short *)(sub + 0xF6) == 0) {
            break;
        }
        func_001029C0(D_70003000);
        if (((*(int *)0x70003B68 + *(short *)0x70003B8A) & 0x3F) == 0) {
            if (func_001B0D80((char *)p)) {
                break;
            }
        }
        func_001B17A0(p);
        if (func_00128B80((char *)p, (char *)sub) == 0) {
            sub[0xFA] = 0;
            switch (p[5]) {
            case 0:
                sub[0xFB] = 0;
                if (func_00128640((char *)p) == 0) {
                    p[5] = 1;
                    p[6] = 0;
                    p[7] = 0;
                }
                break;
            case 1:
                sub[0xFB] = 0;
                func_0012AFC0((char *)p, (char *)sub);
                if (p[0xA] & 1) {
                    func_00128640((char *)p);
                }
                if (0.0f == *(float *)(sub + 0xD8)) {
                    *(int *)0x700038A0 = 0;
                    *(int *)0x700038A4 = 0xBFB33333;
                    *(int *)0x700038A8 = 0;
                    *(int *)0x700038AC = 0x3F800000;
                    if (func_001C25E0(p, D_700038A0) == 0) {
                        p[5] = 8;
                        p[6] = 0;
                        p[7] = 0;
                    }
                }
                break;
            case 2:
                sub[0xFB] = 0;
                func_0012B410((char *)p, (char *)sub);
                if (p[0] == 2) {
                    break;
                }
                if (0.0f == *(float *)(sub + 0xD8)) {
                    *(int *)0x700038A0 = 0;
                    *(int *)0x700038A4 = 0xBFB33333;
                    *(int *)0x700038A8 = 0;
                    *(int *)0x700038AC = 0x3F800000;
                    if (func_001C25E0(p, D_700038A0) == 0) {
                        p[5] = 8;
                        p[6] = 0;
                        p[7] = 0;
                    }
                }
                break;
            case 3:
                sub[0xFB] = 0x80;
                sub[0xFA] = 1;
                func_0012B970((char *)p, (char *)sub);
                break;
            case 4:
                sub[0xFB] = 0x80;
                sub[0xFA] = 1;
                func_0012BE20((char *)p, (char *)sub);
                break;
            case 5:
                sub[0xFB] = 0x80;
                sub[0xFA] = 1;
                func_0012C490(p, sub);
                break;
            case 6:
                sub[0xFB] = 0x80;
                sub[0xFA] = 1;
                func_0012CAA0((char *)p, (char *)sub);
                break;
            case 7:
                sub[0xFB] = 7;
                func_0012D240((char *)p, (char *)sub);
                break;
            case 8:
                sub[0xFB] = 0x80;
                r = func_001C2770(p, sub, 2);
                func_0012D580(p, sub, r);
                if (r == 0) {
                    func_001C3D60(p, sub);
                }
                break;
            case 9:
                sub[0xFB] = 0;
                func_0012D850((char *)p, (char *)sub);
                break;
            case 10:
            case 11:
                sub[0xFB] = 0x80;
                func_0012D940((char *)p, (char *)sub);
                break;
            case 12:
                sub[0xFB] = 0x80;
                func_001C2770(p, sub, 2);
                func_0012DD70((char *)p, (char *)sub);
                func_001C3D60(p, sub);
                break;
            case 13:
                sub[0xFB] = 0x80;
                func_0012B850((char *)p, (char *)sub);
                if (0.0f == *(float *)(sub + 0xD8)) {
                    *(int *)0x700038A0 = 0;
                    *(int *)0x700038A4 = 0xBFB33333;
                    *(int *)0x700038A8 = 0;
                    *(int *)0x700038AC = 0x3F800000;
                    if (func_001C25E0(p, D_700038A0) == 0) {
                        p[5] = 8;
                        p[6] = 0;
                        p[7] = 0;
                    }
                }
                break;
            }
        }
        if (*(float *)(sub + 0xD8) != 0.0f) {
            *(float *)(sub + 0xEC) = 1.8f;
        } else {
            *(float *)(sub + 0xEC) = 1.0f;
        }
        *(short *)(sub + 0xF4) = anim_advance_time((char *)p, *(float *)(sub + 0xEC));
        if (p[5] != 7) {
            copy_qw4(D_70003400, D_70003000);
            func_001C69A0(p);
        }
        if (p[1]) {
            if (*(char *)(sub + 0xFA)) {
                func_001288D0((char *)p, (char *)sub);
            }
            (*(void (**)(unsigned char *))(p + 0x4C))(p);
        }
        p[0xA] = 0;
        p[0xB] = 0;
        *(short *)(p + 0x54) = 0;
        break;
    case 2:
        if (((*(int *)0x70003B68 + *(short *)0x70003B8A) & 0x3F) == 0) {
            if (func_001B0D80((char *)p)) {
                break;
            }
        }
        if (D_0081078F[0]) {
            break;
        }
        func_00129FC0((char *)p, (char *)sub);
        p[0xB] = 0;
        p[0xA] = 0;
        break;
    case 3:
        func_0012E070((char *)sub);
        if (p[0xD] == 0xA || p[0xD] == 0xB || p[0xD] == 0xC) {
            *(int *)0x700031F4 = *(int *)0x700031F4 - 1;
        } else if (sub[0xE0]) {
            p[0] = 2;
            p[4] = 4;
            p[5] = 0;
            func_00102948(p + 0xB0, sub + 0x50);
            break;
        }
        func_001B1190(p[0x9A]);
        func_001AFC10(p);
        break;
    case 4:
        switch (p[5]) {
        case 0:
            func_001289C0((char *)p, (char *)sub);
            *(short *)(sub + 0xF8) = 1;
            bone_init_default_2((char *)p, 1);
            *(short *)(p + 0x28) = 0;
            p[5] = p[5] + 1;
            *(float *)(p + 0x60) = 1.0f;
            *(float *)(p + 0x64) = 1.0f;
            *(float *)(p + 0x68) = 1.0f;
            *(float *)(p + 0x6C) = 1.0f;
            break;
        case 1:
            if (func_00129780(p, sub, p[0xD])) {
                p[4] = 4;
                p[5] = 2;
                idx = ((func_00122BB8() >> 16) << 2) >> 15;
                *(short *)(p + 0x28) = D_00275380[idx];
            }
            break;
        case 2:
            t = *(short *)(p + 0x28) - 1;
            *(short *)(p + 0x28) = t;
            if (t == 0) {
                p[5] = 3;
            }
            break;
        case 3:
            if (func_001B13F0((int)(plr + 0xA0), (int)(p + 0xB0), 80.0f) == 0) {
                if ((func_001B1630(*(float *)(p + 0xB0), *(float *)(p + 0xB4),
                                   *(float *)(p + 0xB8)) & 0xFF) == 0) {
                    p[0] = 1;
                    p[4] = 1;
                    p[5] = 0;
                    p[6] = 0;
                    p[7] = 0;
                }
            }
            break;
        }
        break;
    }
}
