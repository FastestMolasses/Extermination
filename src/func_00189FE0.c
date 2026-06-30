// NEARMISS func_00189FE0  (vram 0x00189FE0, 0x1A0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.26% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two artifacts at 98.26% (233): (1) mwcc materializes the sp+0x3C store address (addiu v0,sp,0x3c; sw v1,0(v0)) with a v0/v1 coloring diff where target stores direct (sw v0,0x3c(sp)); (2) mwcc hoists lui at,0x7000 into the bc1f delay slot, target leaves nop. Both scheduling/coloring permutations, ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 98.26% (mwcc 2.3.3; 991202=82.68%). Body/structure byte-identical.
// Two residuals, both compiler artifacts: (1) for the `*(int*)(sp30+0xC) =
// 0x3F800000` store, mwcc 2.3.3 materializes the stack address into a reg
// (`addiu v0,sp,0x3c; sw v1,0(v0)`) plus a v0/v1 coloring difference where the
// target stores sp-relative directly (`sw v0,0x3c(sp)`); (2) mwcc hoists the
// next `lui at,0x7000` into the `bc1f` delay slot where the target leaves a
// nop. Both are scheduling/coloring permutations, NOT the clean-store nop.
// func_001B41F0 is a 6-arg call (recovered: ...,0x1000, *(short*)(arg0+0x36)).

extern void func_001028D0(void *);
extern void func_00102760(void *, void *);
extern void func_00102948(void *, void *);
extern int func_00189EC0(void *, unsigned char);
extern float func_001B1470(float);
extern void func_001B41F0(void *, void *, void *, int, int, int);
extern void func_001EFD20(int, void *);
extern void func_001EFD90(int, void *, void *);

extern float D_00810374;
extern int D_700031B0;
extern int D_700038D0;

void func_00189FE0(char *arg0) {
    char sp30[16];
    char *e;
    int kind;
    int st;

    e = *(char **)0x700031D4;
    if (e != 0 && *(unsigned char *)e == 1) {
        func_001028D0(sp30);
        *(int *)(sp30 + 0xC) = 0x3F800000;
        func_00102760(sp30, sp30);

        kind = *(unsigned char *)(e + 2) & ~0xE0;
        if (kind == 2) {
            if (*(float *)(sp30 + 4) < 0.0f) {
                *(int *)(sp30 + 4) = 0;
                func_00102760(sp30, sp30);
            }
            func_001B41F0(e, &D_700031B0, sp30, *(int *)(*(char **)0x700031D0 + 0x1C), 0x1000, *(short *)(arg0 + 0x36));
            return;
        }
        if (kind != 4) {
            return;
        }

        st = func_00189EC0(e, *(unsigned char *)(e + 2));
        if (st == 0) {
            return;
        }
        if (st == 2) {
            *(int *)0x700038D0 = 0;
            *(float *)0x700038D4 = func_001B1470(3.1415927f + D_00810374);
            *(int *)0x700038D8 = 0;
            *(int *)0x700038DC = 0x3F800000;
            func_001EFD90(0x80000007, &D_700031B0, &D_700038D0);
        } else if (st == 3) {
            func_001EFD20(0x80000019, &D_700031B0);
        }

        *(short *)(e + 0x36) = *(short *)(arg0 + 0x36);
        *(short *)(e + 0x36) = *(short *)(e + 0x36) | 0x1000;
        func_00102948(e + 0x70, sp30);
    }
}
