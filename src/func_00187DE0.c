// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// WATER-ENTRY one-shot, called by the footing update func_00175900 on first
// contact with floor attr 0x5B (after the depth probe set +0x23C to 1 shallow
// / 2 deep): copies the probe hit point (spad 0x700031B0) into spad
// 0x700038B0, fires splash effect 0x80000016 at that point (normal = self
// +0xC0), runs the loop level func_001E8B90(pos, 5.0f) unless D_00810700 ==
// 0x15, then func_001FB9F0(0xCA shallow / 0xDB deep, 0x1000, 0x1000, 0x1000).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: the
// residuals under 991202 were wall #13 (delay-slot fill) — the D_00810700
// beq;nop slot and the CW dup'd-head 0x1000 shape. 2.3.3 reproduces them, so
// this readable C is byte-identical. Verified objdiff 100% vs
// build/expected/func_00187DE0.o (991202 = 95.3%).
extern void func_001031E0(void *dst, void *src);
extern void func_001EFD90(int id, void *pos, void *normal);
extern void func_001E8B90(void *pos, float f);
extern void func_001FB9F0(int sel, int a1, int a2, int a3);
extern char D_700031B0;
extern char D_700038B0;
extern unsigned char D_00810700;

void func_00187DE0(char *arg0) {
    func_001031E0(&D_700038B0, &D_700031B0);
    func_001EFD90(0x80000016, &D_700038B0, arg0 + 0xC0);
    if (D_00810700 != 0x15) {
        func_001E8B90(&D_700038B0, 5.0f);
    }
    if (*(unsigned char *)(arg0 + 0x23C) == 1) {
        func_001FB9F0(0xCA, 0x1000, 0x1000, 0x1000);
    } else {
        func_001FB9F0(0xDB, 0x1000, 0x1000, 0x1000);
    }
}
