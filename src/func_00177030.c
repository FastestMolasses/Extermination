// NEARMISS func_00177030  (vram 0x00177030, 0x430 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.32% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register-coloring + list-scheduling residual: at the switch-dispatch delay slots (case 2/1/0 beq targets) the target keeps a plain nop while this compile speculates a redundant lui/reload; and in case 2's dx/dy computation the target loads D_700038B0/arg0+0xB0/D_700038B8 in one specific schedu...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// actor_anim: aim/turn state machine dispatched by arg1 (0-4). Computes the
// camera-relative yaw wrap via atan2f(func_0011E620)/func_001B1470, then per
// mode either latches +0xC4 (facing), blends +0xB0/+0xB8 toward an object AABB
// center (func_00199DB0), or (case 2) steers a turret blend by side/step using
// func_00102738 dot-product gating. Logic/structure fully recovered; residual
// is FP register-coloring / list-scheduling load ordering in the case-2 delta
// computation and the switch dispatch's delay-slot fill.
//
extern float func_0011E620(float y, float x);
extern float func_001B1470(float rad);
extern float func_0011DF78(float rad);
extern float func_0011DE90(float rad);
extern float func_0011E748(float x);
extern void func_00199DB0(void *out);
extern void func_001026A0(void *out, void *a, void *b);
extern float func_00102738(void *a, void *b);
extern void func_00102760(void *out, void *a, void *b);

extern float D_700038A0[];
extern float D_700038A8[];
extern float D_700038B0[];
extern float D_700038C0[];
extern float D_700038D0[];

int func_00177030(char *arg0, int arg1) {
    void *p = *(void **)0x700031D0;
    *(float *)0x70003A20 = func_0011E620(-*(float *)((char *)p + 0x3C), *(float *)((char *)p + 0x34));
    *(float *)0x70003A20 = func_001B1470(1.5707964f + *(float *)0x70003A20);
    *(float *)0x70003A24 = func_001B1470(*(float *)0x70003A20 - *(float *)(arg0 + 0xC4));

    switch (arg1) {
    case 0:
        if (!(*(float *)0x70003A24 < -0.7853982f) && *(float *)0x70003A24 <= 0.7853982f) {
            *(float *)(arg0 + 0xC4) = *(float *)0x70003A20;
            return 1;
        }
        return 0;
    case 1:
        if (!(*(float *)0x70003A24 < -0.7853982f) && *(float *)0x70003A24 <= 0.7853982f) {
            *(float *)(arg0 + 0xC4) = *(float *)0x70003A20;
            func_00199DB0(D_700038B0);
            *(float *)(arg0 + 0xB0) = *(float *)0x700038B0;
            *(float *)(arg0 + 0xB8) = *(float *)0x700038B8;
            return 1;
        }
        return 0;
    case 2:
    {
        float side;
        if (func_0011DF78(*(float *)0x70003A24) <= 1.5707964f) {
            *(float *)(arg0 + 0x218) = *(float *)0x70003A20;
            side = 1.0f;
        } else {
            *(float *)(arg0 + 0x218) = func_001B1470(3.1415927f + *(float *)0x70003A20);
            side = -1.0f;
        }
        func_00199DB0(D_700038B0);
        {
            float b0 = *(float *)0x700038B0;
            float b8 = *(float *)0x700038B8;
            float ab0 = *(float *)(arg0 + 0xB0);
            *(float *)0x70003A20 = b0 - ab0;
            {
                float dy = b8 - *(float *)(arg0 + 0xB8);
                *(float *)0x70003A28 = dy;
                *(float *)0x70003A24 = func_0011E620(-dy, *(float *)0x70003A20);
                *(float *)0x70003A2C = func_0011E748((*(float *)0x70003A20 * *(float *)0x70003A20) + (*(float *)0x70003A28 * *(float *)0x70003A28));
                {
                    float step = *(float *)0x70003A2C * func_0011DE90(func_001B1470((1.5707964f + *(float *)0x70003A24) - *(float *)(arg0 + 0x218)));
                    void *p2 = *(void **)0x700031D0;
                    *(float *)(arg0 + 0xB0) = *(float *)0x700038B0 - (side * (*(float *)((char *)p2 + 0x34) * step));
                    p2 = *(void **)0x700031D0;
                    *(float *)(arg0 + 0xB8) = *(float *)0x700038B8 - (side * (*(float *)((char *)p2 + 0x3C) * step));
                }
            }
        }
        return 1;
    }
    case 3:
        func_00199DB0(D_700038B0);
        *(float *)(arg0 + 0xB0) = *(float *)0x700038B0;
        *(float *)(arg0 + 0xB8) = *(float *)0x700038B8;
        return 1;
    case 4:
    {
        void *p2 = *(void **)0x700031D0;
        *(float *)0x700038B0 = *(float *)((char *)p2 + 0x34);
        *(int *)0x700038B4 = 0;
        *(float *)0x700038B8 = *(float *)((char *)p2 + 0x3C);
        *(int *)0x700038BC = 0x3F800000;
        func_00102760(D_700038B0, D_700038B0, p2);
        *(int *)0x700038C0 = 0;
        *(int *)0x700038C4 = 0;
        *(int *)0x700038C8 = 0x3F800000;
        *(int *)0x700038CC = 0;
        func_001026A0(D_700038D0, arg0 + 0xD0, D_700038C0);
        {
            float dot = func_00102738(D_700038B0, D_700038D0);
            if (!(dot < 0.5f)) {
                *(float *)(arg0 + 0xC4) = *(float *)0x70003A20;
                func_00199DB0(D_700038A0);
                *(float *)(arg0 + 0xB0) = *(float *)0x700038A0;
                *(float *)(arg0 + 0xB8) = *(float *)0x700038A8;
                return 1;
            }
        }
        return 0;
    }
    default:
        return 0;
    }
}
