// NEARMISS func_001D2960  (vram 0x001D2960, 0x3C0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.71% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two tiny residual scheduling clusters remain out of ~240 instructions: (1) the FIRST func_001D2D20 call's w/h float-const materialization order (target emits f14=560.0 before f13=1280.0; mine emits f13 then f14 -- the FP-arg-order idiom did not flip this particular pair), and (2) the guard-band-c...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// SEMANTICS (FINDINGS "ENGINE PROJECTION EXACTLY DERIVED" / "P builder pair"):
// per-frame main-P builder. Builds the projection matrix P at ctx+0x2340 from
// zoom s = ctx+0x2468 (rows (0.8s,0,0,0) (0,0.5s,0,0) (2048,2048,0.8996,1)
// (0,0,1677721.5,0)); copies the view matrix arg0 to ctx+0x2380 and composes
// K = P*V into ctx+0x23C0 via func_001026D0. Then builds four offscreen/
// shadow/reflection alt-P*V variants at ctx+0x2240/0x2280/0x22C0/0x2300 via
// func_001D2D20(zoom, 1280, 560, near, far=16711680) with near = 0.1 (two
// variants) / 20 (two variants), composed against the same view (ctx+0x2380).
// Finally builds the frustum cull-plane pair at ctx+0x2410/0x2420 and
// ctx+0x2430/0x2440 (normal = (+-s,0,-1023)/sqrt(s^2+1023^2)) plus the
// ctx+0x2220 guard-band clip constants.
extern void copy_qw4(void *dst, void *src);
extern void func_001026D0(void *dst, void *m, void *src);
extern void func_001D2D20(void *m, float zoom, float w, float h, float near, float far);
extern float func_0011E748(float x);
extern char *D_00275670;

#define CTX ((char *)D_00275670)

void func_001D2960(void *arg0) {
    float s;
    float sp20[4];
    float t21, t22, t1, t1_2;

    s = *(float *)(CTX + 0x2468);
    *(float *)(CTX + 0x2340) = 0.8f * s;
    *(int *)(CTX + 0x2350) = 0;
    *(int *)(CTX + 0x2360) = 0x45000000;
    *(int *)(CTX + 0x2370) = 0;
    *(int *)(CTX + 0x2344) = 0;
    *(float *)(CTX + 0x2354) = 0.5f * s;
    *(int *)(CTX + 0x2364) = 0x45000000;
    *(int *)(CTX + 0x2374) = 0;
    *(int *)(CTX + 0x2348) = 0;
    *(int *)(CTX + 0x2358) = 0;
    *(int *)(CTX + 0x2368) = 0x3F664CB3;
    *(int *)(CTX + 0x2378) = 0x49CCCCCC;
    *(int *)(CTX + 0x234C) = 0;
    *(int *)(CTX + 0x235C) = 0;
    *(int *)(CTX + 0x236C) = 0x3F800000;
    *(int *)(CTX + 0x237C) = 0;

    copy_qw4(CTX + 0x2380, arg0);
    func_001026D0(CTX + 0x23C0, CTX + 0x2340, CTX + 0x2380);

    func_001D2D20(sp20, s, 1280.0f, 560.0f, 0.1f, 16711680.0f);
    func_001026D0(CTX + 0x2240, sp20, CTX + 0x2380);
    func_001D2D20(sp20, s, 1280.0f, 560.0f, 20.0f, 16711680.0f);
    func_001026D0(CTX + 0x2280, sp20, CTX + 0x2380);
    func_001D2D20(sp20, s, 3584.0f, 3584.0f, 0.1f, 16711680.0f);
    func_001026D0(CTX + 0x22C0, sp20, CTX + 0x2380);
    func_001D2D20(sp20, s, 2048.0f, 2048.0f, 0.1f, 16711680.0f);
    func_001026D0(CTX + 0x2300, sp20, CTX + 0x2380);

    *(int *)(CTX + 0x2220) = 0x3A008081;
    *(int *)(CTX + 0x2224) = 0x3A008081;
    t21 = 1046529.0f + (s * s);
    *(int *)(CTX + 0x2228) = 0x34000000;
    *(int *)(CTX + 0x222C) = 0x3F800000;
    *(int *)(CTX + 0x2230) = 0xBF808081;
    *(int *)(CTX + 0x2234) = 0xBF808081;
    *(int *)(CTX + 0x2238) = 0xBF7FFFFE;
    *(int *)(CTX + 0x223C) = 0;

    t1 = 1.0f / func_0011E748(t21);
    *(float *)(CTX + 0x2410) = s * t1;
    *(int *)(CTX + 0x2414) = 0;
    *(float *)(CTX + 0x2418) = -(-1023.0f * t1);
    *(int *)(CTX + 0x241C) = 0;

    t1_2 = 1.0f / func_0011E748(t21);
    t22 = -s;
    *(float *)(CTX + 0x2420) = t22 * t1_2;
    *(int *)(CTX + 0x2424) = 0;
    *(float *)(CTX + 0x2428) = -(-1023.0f * t1_2);
    *(int *)(CTX + 0x242C) = 0;

    t1 = 1.0f / func_0011E748(t21);
    *(int *)(CTX + 0x2430) = 0;
    *(float *)(CTX + 0x2434) = s * t1;
    *(float *)(CTX + 0x2438) = -(-1023.0f * t1);
    *(int *)(CTX + 0x243C) = 0;

    t1_2 = 1.0f / func_0011E748(t21);
    *(int *)(CTX + 0x2440) = 0;
    *(float *)(CTX + 0x2444) = t22 * t1_2;
    *(float *)(CTX + 0x2448) = -(-1023.0f * t1_2);
    *(int *)(CTX + 0x244C) = 0;
}
