// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Sets actor byte +0xB bit 4 path: writes a 4-word param block at absolute
// hardware address 0x700038A0 (0x3E99999A, 0, 0x40A00000, 0x3F800000) then
// calls func_001B6F00(actor, PI(0x40490FDB), &D_700038A0). Sets actor byte 0 = 2,
// then branches on actor byte +3: for 0x12/0x2F vs other, depending on global
// flags D_00810C87/D_00810C88, either writes D_00246CB4=0x154 and arms list
// D_00246C20 (returns 1) or writes D_00246FB4 (0x80000014 / 0x80000016) and arms
// list D_00246F20 (returns 2), each via func_001BA1A0(arg1,list)/func_001BA1F0(actor).
// Returns 0 when bit 4 is clear.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202 (86.5%):
// 991202 residual is the idiom-13 clean-store delay-slot fill; 2.3.3 is
// byte-identical. Verified objdiff 100% vs build/expected/func_001576E0.o.
// Notes: the 0x700038A0 block is written via absolute literal pointers (CW emits
// lui/sw, not a symbol reloc); func_001B6F00's float arg (PI) does NOT consume an
// integer arg slot in the EE mwcc ABI, so actor->a0, float->f12, &D_700038A0->a1.
extern int func_001B6F00(void *, float, void *);
extern int func_001BA1A0(int, void *);
extern int func_001BA1F0(char *);
extern char D_00246C20;
extern int D_00246CB4;
extern char D_00246F20;
extern int D_00246FB4;
extern unsigned char D_00810C87;
extern unsigned char D_00810C88;
extern int D_700038A0;

int func_001576E0(char *arg0, int arg1) {
    unsigned char v1;

    if (*(unsigned char *)(arg0 + 0xB) & 4) {
        *(int *)0x700038A0 = 0x3E99999A;
        *(int *)0x700038A4 = 0;
        *(int *)0x700038A8 = 0x40A00000;
        *(int *)0x700038AC = 0x3F800000;
        func_001B6F00(arg0, 3.14159265f, &D_700038A0);
        *(char *)arg0 = 2;
        v1 = *(unsigned char *)(arg0 + 3);
        if (v1 == 0x12 || v1 == 0x2F) {
            if (D_00810C87 != 0) {
                D_00246CB4 = 0x154;
                func_001BA1A0(arg1, &D_00246C20);
                func_001BA1F0(arg0);
                return 1;
            }
            D_00246FB4 = 0x80000014;
            func_001BA1A0(arg1, &D_00246F20);
            func_001BA1F0(arg0);
            return 2;
        }
        if (D_00810C88 != 0) {
            D_00246CB4 = 0x154;
            func_001BA1A0(arg1, &D_00246C20);
            func_001BA1F0(arg0);
            return 1;
        }
        D_00246FB4 = 0x80000016;
        func_001BA1A0(arg1, &D_00246F20);
        func_001BA1F0(arg0);
        return 2;
    }
    return 0;
}
