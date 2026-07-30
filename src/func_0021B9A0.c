// NEARMISS func_0021B9A0  (vram 0x0021B9A0, 0xC4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.20% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// jr-table external-dispatch wall (proven s84) — local @17 table vs external jtbl_00273790 (lui/addiu/sll/addu/lw/jr). SECOND residual: same-TU interprocedural register-usage. The target keeps `mode` in caller-saved $a2 (paddub $a2,$a0,$zero) across BOTH jals with a 0x10 frame saving only $ra; a st...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p

//
// SEMANTICS: fog / depth-range programmer for the per-frame render context
// (*D_00275670).  Context fields +0xB8 / +0xBC hold the current fog NEAR and
// FAR distances (see func_0021B970, which writes the same pair verbatim);
// +0xD8 / +0xDC and +0xF8 / +0xFC hold two preset near/far pairs.
// `mode` selects how the new pair is produced:
//   1        -> preset pair at +0xD8 / +0xDC
//   2, 4     -> near = bias + near * scale, far unchanged
//   3, 5     -> near unchanged, far  = bias + far  * scale
//   0, other -> preset pair at +0xF8 / +0xFC
// The pair is stored back to +0xB8 / +0xBC and handed to func_0021B920, which
// derives the GS fog coefficients (255.0f / (far - near)) into +0xA0..+0xAC.
// For modes 0, 1, 4 and 5 func_0021B900 then latches +0xA0..+0xBF into +0xC0.

extern int *D_00275670;
extern void func_0021B920(float, float);
extern void func_0021B900(void);

void func_0021B9A0(int mode, float scale, float bias) {
    int *p;
    float near, far;

    switch (mode) {
    case 0:
    default:
        p = D_00275670;
        near = *(float *)((char *)p + 0xF8);
        far  = *(float *)((char *)p + 0xFC);
        break;
    case 1:
        p = D_00275670;
        near = *(float *)((char *)p + 0xD8);
        far  = *(float *)((char *)p + 0xDC);
        break;
    case 2:
    case 4:
        p = D_00275670;
        near = bias + *(float *)((char *)p + 0xB8) * scale;
        far  = *(float *)((char *)p + 0xBC);
        break;
    case 3:
    case 5:
        p = D_00275670;
        near = *(float *)((char *)p + 0xB8);
        far  = bias + *(float *)((char *)p + 0xBC) * scale;
        break;
    }
    p = D_00275670;
    *(float *)((char *)p + 0xB8) = near;
    p = D_00275670;
    *(float *)((char *)p + 0xBC) = far;
    func_0021B920(near, far);
    if ((unsigned int)(mode - 4) < 2 || (unsigned int)mode < 2) {
        func_0021B900();
    }
}
