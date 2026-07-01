// NEARMISS func_00131F90  (vram 0x00131F90, 0x500 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.93% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation-order artifact: target always keeps the local angular-error bucket value (sp5C) on the stack, reloading it (lwc1) at every func_00128250(sp5C) call site, spilling only 2 callee-saved FP regs (fs0/fs1, -0x60 frame); both mwcc builds instead promote sp5C into a 3rd callee-saved ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Steers the heading at self+0xC4 toward the desired heading at owner+0x30.
// arg0 = self, arg1 = owner. Picks a per-frame angular step (var_f20) from the
// owner state: D_00244FD8 when stunned (0x62), a randomized 4..8 deg when the
// 0x56 "snap" flag is set, else D_00244FD0. If already aligned, clears the
// 0x63 turn-phase and returns. The 0x56-snap path just slews via func_001B12B0.
// Otherwise it samples the signed angular error (func_00128250 bucket of the
// 0x5C temp), and depending on the bucket and the error sign nudges 0xC4 by the
// step (clamped to the target), re-normalizes via func_001B1470, rebuilds the
// 0xD0 basis (func_001029C0/func_00102C58), and on a phase change pushes the
// per-bucket offset rows (D_00242F50/54 forward, D_00243180/84 reverse) through
// func_001026A0/func_001028B8 into owner+0x10 and self+0xB0.
extern int float_to_int(float);
extern void func_001026A0(void *a, char *b, char *c);
extern void func_001028B8(void *a, char *b, void *c);
extern void func_001029C0(char *p);
extern void func_00102C58(char *a, char *b, char *c);
extern void func_0011E0A8(float *p, float v);
extern int func_00122BB8(void);
extern unsigned int func_00128250(float);
extern float func_001B12B0(float a, float b, float c);
extern float func_001B1470(float);
extern int func_001C6160(char *p);
extern float D_00242F50;
extern float D_00242F54;
extern float D_00243180;
extern float D_00243184;
extern float D_00244FD0;
extern float D_00244FD8;
extern char D_700038A0[];
extern char D_700038B0[];

void func_00131F90(char *arg0, char *arg1) {
    float sp5C;
    float step;
    float err;
    float ang;
    int t;
    int idx;

    if (*(unsigned char *)(arg1 + 0x62) != 0) {
        step = D_00244FD8;
    } else if (*(unsigned short *)(arg1 + 0x56) != 0) {
        step = 4.0f + 4.0f * ((float)((func_00122BB8() >> 0xE) & 0xFF) / 255.0f);
    } else {
        step = D_00244FD0;
    }

    if (*(float *)(arg0 + 0xC4) == *(float *)(arg1 + 0x30)) {
        *(char *)(arg1 + 0x63) = 0;
        return;
    }

    if (*(unsigned short *)(arg1 + 0x56) != 0 ||
        *(unsigned char *)(arg1 + 0x62) != 0 ||
        *(short *)(arg0 + 0x2C) != 7 ||
        (*(unsigned char *)(arg1 + 0x60) & 4)) {
        *(char *)(arg1 + 0x63) = 0;
        *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg1 + 0x30), *(float *)(arg0 + 0xC4), (3.1415927f * step) / 180.0f);
        return;
    }

    func_0011E0A8(&sp5C, *(float *)(arg0 + 0x3C));
    t = func_001C6160(arg0);
    sp5C = (float)(t - float_to_int(sp5C));
    err = func_001B1470(*(float *)(arg1 + 0x30) - *(float *)(arg0 + 0xC4));
    if (!(err <= 0.0f)) {
        if (func_00128250(sp5C) >= 0x17 && func_00128250(sp5C) < 0x30) {
            ang = (3.1415927f * step) / 180.0f;
            if (!(err < ang)) {
                *(float *)(arg0 + 0xC4) = *(float *)(arg0 + 0xC4) + ang;
            } else {
                *(float *)(arg0 + 0xC4) = *(float *)(arg1 + 0x30);
            }
            *(float *)(arg0 + 0xC4) = func_001B1470(*(float *)(arg0 + 0xC4));
            func_001029C0(arg0 + 0xD0);
            func_00102C58(arg0 + 0xD0, arg0 + 0xD0, arg0 + 0xC0);
            if (*(unsigned char *)(arg1 + 0x63) != 1) {
                *(char *)(arg1 + 0x63) = 1;
                idx = func_00128250(sp5C) * 8;
                *(float *)0x700038A0 = *(float *)((char *)&D_00242F50 + idx);
                *(int *)0x700038A4 = 0;
                *(float *)0x700038A8 = *(float *)((char *)&D_00242F54 + idx);
                func_001026A0(arg1 + 0x10, arg0 + 0xD0, D_700038A0);
                func_001028B8(arg1 + 0x10, arg0 + 0xB0, arg1 + 0x10);
            }
            idx = func_00128250(sp5C) * 8;
            *(float *)0x700038A0 = -*(float *)((char *)&D_00242F50 + idx);
            *(int *)0x700038A4 = 0;
            *(float *)0x700038A8 = -*(float *)((char *)&D_00242F54 + idx);
            func_001026A0(D_700038B0, arg0 + 0xD0, D_700038A0);
            func_001028B8(arg0 + 0xB0, arg1 + 0x10, D_700038B0);
            return;
        }
        *(char *)(arg1 + 0x63) = 0;
        return;
    }

    if (func_00128250(sp5C) < 0xB || func_00128250(sp5C) >= 0x38) {
        ang = (3.1415927f * step) / 180.0f;
        if (!(-err < ang)) {
            *(float *)(arg0 + 0xC4) = *(float *)(arg0 + 0xC4) - ang;
        } else {
            *(float *)(arg0 + 0xC4) = *(float *)(arg1 + 0x30);
        }
        *(float *)(arg0 + 0xC4) = func_001B1470(*(float *)(arg0 + 0xC4));
        func_001029C0(arg0 + 0xD0);
        func_00102C58(arg0 + 0xD0, arg0 + 0xD0, arg0 + 0xC0);
        if (*(unsigned char *)(arg1 + 0x63) != 2) {
            *(char *)(arg1 + 0x63) = 2;
            idx = func_00128250(sp5C) * 8;
            *(float *)0x700038A0 = *(float *)((char *)&D_00243180 + idx);
            *(int *)0x700038A4 = 0;
            *(float *)0x700038A8 = *(float *)((char *)&D_00243184 + idx);
            func_001026A0(arg1 + 0x10, arg0 + 0xD0, D_700038A0);
            func_001028B8(arg1 + 0x10, arg0 + 0xB0, arg1 + 0x10);
        }
        idx = func_00128250(sp5C) * 8;
        *(float *)0x700038A0 = -*(float *)((char *)&D_00243180 + idx);
        *(int *)0x700038A4 = 0;
        *(float *)0x700038A8 = -*(float *)((char *)&D_00243184 + idx);
        func_001026A0(D_700038B0, arg0 + 0xD0, D_700038A0);
        func_001028B8(arg0 + 0xB0, arg1 + 0x10, D_700038B0);
        return;
    }
    *(char *)(arg1 + 0x63) = 0;
}
