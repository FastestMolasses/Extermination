// NEARMISS func_0018D7B0  (vram 0x0018D7B0, 0x154 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.24% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// Only the `b` after the mode-1 copies: target delay slot nop, mwcc233 copies the join's paddub v0,s0 into it (goto join / prototype / arg spelling variants unchanged).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// LANE NOTE (m1-firstlevel-matching): writing the mode-1 test as an early
// `return s0` for arg1 != 1 reproduces the target's `beq s1,1 / b epilogue`
// shape (96.94% -> 99.24%). The only residual is the delay slot of the `b` after
// the two func_00102948 copies: the target leaves a nop and mwcc copies the
// join's `paddub v0,s0,zero` into it. A goto join, a void callee prototype and
// float-pointer argument spellings did not change it.

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
        if (arg1 != 1) {
            return s0;
        }
            func_00102948(D_008105E0, arg0 + 0x20);
            func_00102948(D_008105D0, arg0 + 0x10);
    } else {
        func_0018C6A0(arg0 + 0x10, D_008105D0, 4.0f);
        func_0018C4B0(D_008105D0, *(float *)(arg0 + 0x14), 4.0f);
    }
    return s0;
}
