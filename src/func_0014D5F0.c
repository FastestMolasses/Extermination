// NEARMISS func_0014D5F0  (vram 0x0014D5F0, 0x1CC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.83% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP-arg-emit-order: the single anim_clip_init float-arg pair emits mtc1 f12(5.0) before mtc1 f13(0.0); target schedules the f13 zero-move first. Stable FP-coloring/scheduling artifact - both mwcc 991202 and 2.3.3 emit f12-first; FP-ARG-ORDER idiom variants (assign-in-arg, separate locals, comma-ex...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 99.83% (mwcc233). Body+structure fully recovered. Sole residual:
// the anim_clip_init(arg0,2,5.0f,0.0f) FP arg pair emits 'mtc1 v0,$f12'
// (5.0) before 'mtc1 zero,$f13' (0.0); target schedules the f13 zero-move
// first. Stable FP-arg-emit-order / FP-coloring artifact - both mwcc builds
// emit f12-first regardless of source arg reshaping (FP-ARG-ORDER idiom,
// assign-in-arg temp, double-local, comma-expr all tried, none flip it).
// Two-state anim/effect tick: case0 inits the timer (+4=0, copy +0x38->+0x24,
// clear +0x5D, clear linked +0x20's +5, kick anim_clip_init unless flag set);
// case1 counts the +4 timer down, reseeds from func_00122BB8()>>14 &0x7F on
// expiry, eases +0x28 toward +0x24, then picks state byte +5 (3/2/1) by
// distance +0x34<=25 / global D_008104E4 / RNG bit. Trailing: if +0x5E==0,
// reseed +0x5E and fire func_001FBD50 with a table-indexed arg.
extern void anim_clip_init(char *, int, float, float);
extern int func_00122BB8(void);
extern float func_001B12B0(float, float, float);
extern void func_001FBD50(char *, int, int, float);
extern int D_00245CF0;
extern unsigned char D_008104E4;

void func_0014D5F0(char *arg0, char *arg1) {
    int t;
    int st;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(char *)(arg0 + 6) = st + 1;
        *(int *)(arg1 + 4) = 0;
        *(float *)(arg1 + 0x24) = *(float *)(arg1 + 0x38);
        *(char *)(arg1 + 0x5D) = 0;
        if (*(int *)(arg0 + 0x20) != 0) {
            *(char *)(*(int *)(arg0 + 0x20) + 5) = 0;
        }
        if ((*(short *)(arg0 + 0x2C) & 0xFFFF7FFF) != 2) {
            anim_clip_init(arg0, 2, 5.0f, 0.0f);
        }
        break;
    case 1:
        t = *(int *)(arg1 + 4);
        if (t != 0) {
            *(int *)(arg1 + 4) = t - 1;
        } else {
            *(int *)(arg1 + 4) = (func_00122BB8() >> 14) & 0x7F;
            *(float *)(arg1 + 0x24) = *(float *)(arg1 + 0x38);
        }
        *(float *)(arg1 + 0x28) = func_001B12B0(*(float *)(arg1 + 0x24), *(float *)(arg1 + 0x28), 0.04363323375582695f);
        if (*(short *)(arg1 + 0x50) == 0) {
            if (*(float *)(arg1 + 0x34) <= 25.0f && D_008104E4 != 1) {
                *(char *)(arg0 + 5) = 3;
            } else {
                if ((func_00122BB8() >> 7) & 1) {
                    *(char *)(arg0 + 5) = 1;
                } else {
                    *(char *)(arg0 + 5) = 2;
                }
            }
            *(char *)(arg0 + 6) = 0;
        }
        break;
    }
    if (*(char *)(arg1 + 0x5E) == 0) {
        *(char *)(arg1 + 0x5E) = ((func_00122BB8() >> 11) & 0x3F) + 0x3C;
        func_001FBD50(arg0, *(int *)((char *)&D_00245CF0 + (((func_00122BB8() >> 7) & 3) * 4)), 0, 300.0f);
    }
}
