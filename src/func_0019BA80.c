// NEARMISS func_0019BA80  (vram 0x0019BA80, 0x1B4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.83% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Callee-saved register-coloring ROTATION at 92.84% (mwcc 2.3.3). Body fully correct, frame size and live-across-call array base now match. Target colors {s0=r, s1=&local, s2=arg3}; mwcc colors {s0=&local, s1=r} keeping arg3 in a caller-saved temp, which cascades into the temp/branch-delay-slot reg...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 92.84% (mwcc 2.3.3; mwcc 991202 = 91.09%). Body/structure fully
// recovered: loop1 broadcasts arg1[0..2] into a local float[3] and into the
// D_70003190 block (offsets 0/0x10); a bias of +/-0.001f (from sign of
// *(arg2+4)) is folded into the global at 0x70003194 around the func_001A3980 /
// func_0019E930 dispatch (gated by arg3 bits 2/4) which selects a result r in
// {0,2,4}; if r!=0 loop2 copies the saved locals back and computes a delta
// (0x20-0x10 -> 0x30), else clears 0x700031D0; result stored at 0x700031D8.
// Frame size now matches target (0x60, s0/s1/s2 saved) and the local-array base
// is held live across the calls. Sole residual is a 3-way callee-saved coloring
// ROTATION: target = {s0=r, s1=&local, s2=arg3}; mwcc = {s0=&local, s1=r, arg3
// in a caller-saved temp}, which cascades into the temp-register and
// delay-slot-filler choices. Body identical. Permuter territory.
extern int func_001A3980();
extern int func_0019E930();
extern char D_70003190[];

int func_0019BA80(char *arg0, float *arg1, char *arg2, int arg3) {
    float local[3];
    float *q;
    float *t;
    float *p;
    char *d;
    char *d2;
    float bias;
    int i;
    int r;

    q = local;
    r = 0;
    p = arg1;
    t = q;
    d = D_70003190;
    i = 0;
    do {
        float v = *p;
        i += 1;
        *(float *)(d + 0x10) = v;
        *(float *)(d + 0x0) = v;
        *t = v;
        p += 1;
        d += 4;
        t += 1;
    } while (i < 3);

    *(float *)0x70003194 = *(float *)0x70003194 - *(float *)(arg2 + 4);
    if (*(float *)(arg2 + 4) < 0.0f) {
        bias = 0.001f;
    } else {
        bias = -0.001f;
    }
    *(float *)0x70003194 = *(float *)0x70003194 + bias;
    *(int *)0x700031AC = 0;
    *(int *)0x7000319C = 0;
    *(int *)0x700031D4 = 0;
    *(short *)0x7000324E = *(unsigned char *)(arg0 + 2) & 0x1F;

    if (arg3 & 2) {
        *(int *)0x70003254 = *(int *)(arg0 + 0x14);
        if (func_001A3980() == 0) {
            r = 2;
        }
    }
    if ((arg3 & 4) && func_0019E930() == 0) {
        r = 4;
    }
    *(float *)0x70003194 = *(float *)0x70003194 - bias;

    if (r != 0) {
        d2 = D_70003190;
        i = 0;
        do {
            i += 1;
            *(float *)(d2 + 0x10) = *q;
            q += 1;
            *(float *)(d2 + 0x30) = *(float *)(d2 + 0x20) - *(float *)(d2 + 0x10);
            d2 += 4;
        } while (i < 3);
    } else {
        *(int *)0x700031D0 = 0;
    }
    *(int *)0x700031D8 = r;
    return r;
}
