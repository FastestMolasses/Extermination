// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// If the float timer at arg0+0x220 has run out (<= 0.0f) and the global gate
// byte D_008106F1 is set, fire a 300.0f / id 0x14D event via func_001FBD50 and
// register a 0x80000048 handler for this object via func_001EFE00. Otherwise
// no-op. Returns nothing (void).
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build reaches
// 88.5%. Key: the function is VOID -- materializing a `return 0` adds a trailing
// `paddub v0,zero,zero` and shifts the D_008106F1 byte from v1 into v0, which is
// the residual 991202/CW cannot avoid here.
extern int func_001FBD50(void *, int, int, float);
extern int func_001EFE00(int, void *);
extern unsigned char D_008106F1;

void func_0021C200(unsigned char *arg0) {
    if (*(float *)(arg0 + 0x220) <= 0.0f) {
        if (D_008106F1 != 0) {
            func_001FBD50(arg0, 0x14D, 0, 300.0f);
            func_001EFE00(0x80000048, arg0);
        }
    }
}
