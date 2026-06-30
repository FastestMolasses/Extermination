// NEARMISS func_0018D7B0  (vram 0x0018D7B0, 0x154 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.94% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// CW-vs-mwcc branch lowering: target keeps explicit `beq s1,1,body / nop / b epilogue` (enter-then-skip pair) for the inner `if (arg1 == 1)`; mwcc collapses it to a single fall-through `bne s1,1,epilogue`. 3 of 85 instructions differ (the merged branch + its delay-slot nop + the b epilogue); all ot...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 96.9% (mwcc233; 991202 = 86.2%) — logic fully recovered.
// Sole residual: CW-vs-mwcc branch lowering for the inner `if (arg1 == 1)`.
// Target emits `beq s1,1,body / nop / b epilogue` (CW keeps the explicit
// enter-then-skip pair, body reached by forward beq); mwcc merges this into
// a single fall-through `bne s1,1,epilogue`. Every other instruction is
// byte-identical (frame, callee-saved set, the &D_008102B0 CSE into s0, all
// three dispatch calls, arg order, the float branch). Documented wall —
// 2.3.3 does not fix mwcc-vs-CW branch lowering.
//
// Semantics: dispatch on mode arg1.
//   s3 = (arg1==2) ? 7 : 6
//   func_0018D330(arg0, &D_008102B0, arg1, s3)
//   mode 2 or 6 -> s0 = func_0018F870(...)
//   mode 5       -> func_0018D910(...); s0 = 0
//   otherwise    -> s0 = func_0018DD20(...)
//   arg0[7] = (u8)s0
//   mode 0 -> func_0018C6A0(arg0+0x10, &D_008105D0, 4.0f);
//             func_0018C4B0(&D_008105D0, *(float*)(arg0+0x14), 4.0f)
//   mode 1 -> func_00102948(&D_008105E0, arg0+0x20);
//             func_00102948(&D_008105D0, arg0+0x10)
//   return s0
extern int func_0018D330(unsigned char *, void *, int, int);
extern int func_0018F870(unsigned char *, void *, int, int);
extern int func_0018D910(unsigned char *, void *, int);
extern int func_0018DD20(unsigned char *, void *, int, int);
extern int func_00102948(void *, void *);
extern int func_0018C6A0(unsigned char *, void *, float);
extern int func_0018C4B0(void *, float, float);
extern char D_008102B0[];
extern char D_008105D0[];
extern char D_008105E0[];

int func_0018D7B0(unsigned char *arg0, int arg1) {
    char *d = D_008102B0;
    int s3;
    int s0;

    if (arg1 == 2) {
        s3 = 7;
    } else {
        s3 = 6;
    }
    func_0018D330(arg0, d, arg1, s3);
    if (arg1 == 2 || arg1 == 6) {
        s0 = func_0018F870(arg0, d, arg1, s3);
    } else if (arg1 == 5) {
        func_0018D910(arg0, d, s3);
        s0 = 0;
    } else {
        s0 = func_0018DD20(arg0, d, arg1, s3);
    }
    *(unsigned char *)(arg0 + 7) = s0;
    if (arg1 != 0) {
        if (arg1 == 1) {
            func_00102948(D_008105E0, arg0 + 0x20);
            func_00102948(D_008105D0, arg0 + 0x10);
        }
    } else {
        func_0018C6A0(arg0 + 0x10, D_008105D0, 4.0f);
        func_0018C4B0(D_008105D0, *(float *)(arg0 + 0x14), 4.0f);
    }
    return s0;
}
