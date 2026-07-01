// NEARMISS func_00211400  (vram 0x00211400, 0x3D0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.08% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register-coloring/scheduling: at each of the 5 dispatch sites, mwcc233 loads the 3 seeded float constants in order y,z,x into fv1/ft0/fv0 before storing x,y,z to the 0x700038A0 scratch vec, while this build colors them x,y,z into fv1/fv0f/fv0 (same store order/addresses/values, only load-regis...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// UI game-region "pickup prompt" state machine. If the low 2 bits of the global flag word
// at 0x70003B64 are clear, does nothing. Otherwise calls func_00207D00(1,0) to open/refresh
// the prompt overlay. When arg1==0 (init/close pass), independently re-triggers the 5 icon
// slots (indices 0-4) via func_00211240 whenever their paired state-byte pair indicates a
// newly-available pickup (each pair is a "seen" flag != a sentinel and a "value" byte != 0xFF,
// except slot 2 which just checks D_00810784==1). When arg1!=0, dispatches on the record's
// kind byte at arg0+0x12 (kind values 5,6,0,3,4 map to the same five pickup slots) and, if the
// slot's guard condition holds, seeds the scratch vec3 at 0x700038A0 from a per-kind constant
// triple (D_00265940.. table) and calls func_002117D0(arg0, &scratch, kind, tableIdx) followed
// by func_00211310(&scratch) to project/draw the prompt icon for that record.
extern void func_00207D00(int arg0, int arg1);
extern void func_00211240(int arg0);
extern void func_00211310(float *arg0);
extern void func_002117D0(void *arg0, float *arg1, int arg2, int arg3);
extern float D_00265940;
extern float D_00265944;
extern float D_00265948;
extern float D_0026594C;
extern float D_00265950;
extern float D_00265954;
extern float D_00265958;
extern float D_0026595C;
extern float D_00265960;
extern float D_00265964;
extern float D_00265968;
extern float D_0026596C;
extern float D_00265970;
extern float D_00265974;
extern float D_00265978;
extern unsigned char D_0081076D;
extern unsigned char D_00810770;
extern unsigned char D_0081077F;
extern unsigned char D_00810782;
extern unsigned char D_00810784;
extern unsigned char D_00810789;
extern unsigned char D_0081078C;
extern unsigned char D_00810C8D;
extern unsigned char D_00810C8E;
extern char D_700038A0[];

void func_00211400(unsigned char *arg0, int arg1) {
    unsigned char kind;
    float x, y, z;

    if ((*(int *)0x70003B64 & 3) == 0) {
        return;
    }
    func_00207D00(1, 0);
    if (arg1 == 0) {
        if ((D_00810C8D != 0) && (D_0081076D != 0xFF)) {
            func_00211240(0);
        }
        if ((D_00810C8E != 0) && (D_00810770 != 0xFF)) {
            func_00211240(1);
        }
        if (D_00810784 == 1) {
            func_00211240(2);
        }
        if ((D_0081077F == 0xFF) && (D_00810789 != 0xFF)) {
            func_00211240(3);
        }
        if ((D_00810782 == 0xFF) && (D_0081078C == 0)) {
            func_00211240(4);
        }
    } else {
        kind = arg0[0x12];
        switch (kind) {
        case 5:
            if ((D_00810C8D != 0) && (D_0081076D != 0xFF)) {
                x = D_00265940;
                y = D_00265944;
                z = D_00265948;
                *(float *)0x700038A0 = x;
                *(float *)0x700038A4 = y;
                *(float *)0x700038A8 = z;
                func_002117D0(arg0, (float *)D_700038A0, arg0[0x12], 2);
                func_00211310((float *)D_700038A0);
            }
            break;
        case 6:
            if ((D_00810C8E != 0) && (D_00810770 != 0xFF)) {
                x = D_0026594C;
                y = D_00265950;
                z = D_00265954;
                *(float *)0x700038A0 = x;
                *(float *)0x700038A4 = y;
                *(float *)0x700038A8 = z;
                func_002117D0(arg0, (float *)D_700038A0, arg0[0x12], 2);
                func_00211310((float *)D_700038A0);
            }
            break;
        case 0:
            if (D_00810784 == 1) {
                x = D_00265958;
                y = D_0026595C;
                z = D_00265960;
                *(float *)0x700038A0 = x;
                *(float *)0x700038A4 = y;
                *(float *)0x700038A8 = z;
                func_002117D0(arg0, (float *)D_700038A0, arg0[0x12], 0);
                func_00211310((float *)D_700038A0);
            }
            break;
        case 3:
            if ((D_0081077F == 0xFF) && (D_00810789 != 0xFF)) {
                x = D_00265964;
                y = D_00265968;
                z = D_0026596C;
                *(float *)0x700038A0 = x;
                *(float *)0x700038A4 = y;
                *(float *)0x700038A8 = z;
                func_002117D0(arg0, (float *)D_700038A0, arg0[0x12], 0);
                func_00211310((float *)D_700038A0);
            }
            break;
        case 4:
            if ((D_00810782 == 0xFF) && (D_0081078C == 0)) {
                x = D_00265970;
                y = D_00265974;
                z = D_00265978;
                *(float *)0x700038A0 = x;
                *(float *)0x700038A4 = y;
                *(float *)0x700038A8 = z;
                func_002117D0(arg0, (float *)D_700038A0, arg0[0x12], 3);
                func_00211310((float *)D_700038A0);
            }
            break;
        }
    }
}
