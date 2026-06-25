// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-region effect/spawn setup, dispatched on the region byte at arg0+0x11.
// Calls func_0020D930(self, 1), writes two float configs to the scratchpad
// pair at 0x700038A0/0x700038A4, then func_0020AC70(self, &cfg, 0) and
// func_00207D00(1, 0). Then five region-conditioned blocks: each tests
// arg0+0x11 == N (N=1..5) and emits a triple of func_00207E40 spawn calls when
// it matches, or a pair (the shared base spawns) otherwise. Finishes with
// func_00207D00(1, 3) and a final func_00207E40. The 7th arg of func_00207E40
// is a 64-bit packed value (the asm builds it as v0|(hi<<32)).
//
// Matched 100.0 with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build
// reaches 84.3% (it schedules the prologue/store sequence differently).
// Two keys: (1) func_0020D930 takes (self, 1) -- a0 stays the incoming arg0 and
// 1 goes in a1; (2) the 0x700038A0/A4 config stores are absolute-address
// (lui at,0x7000 / sw), so they are written via literal pointers, while the
// &cfg passed to func_0020AC70 is the relocated &D_700038A0 symbol.
extern void func_0020D930(char *self, int a);
extern void func_0020AC70(char *self, void *p, int z);
extern void func_00207D00(int a, int b);
extern void func_00207E40(int a0, int a1, int a2, int a3, int t0, unsigned int t1, long long t2);

extern int D_700038A0;

void func_0020F2A0(char *arg0) {
    func_0020D930(arg0, 1);
    *(int *)0x700038A0 = 0x43780000;
    *(int *)0x700038A4 = 0x43500000;
    func_0020AC70(arg0, &D_700038A0, 0);
    func_00207D00(1, 0);

    if (*(unsigned char *)(arg0 + 0x11) == 1) {
        func_00207E40(1, 0x80D0, 0x8010, 0x80, 0x20, 0x805FFF6E, 0x2003C6855D421DD4LL);
        func_00207E40(1, 0x7F60, 0x7FC0, 0x20, 0x20, 0x805FFF6E, 0x2003C40555321E24LL);
        func_00207E40(1, 0x85C0, 0x8070, 0x40, 0x40, 0x80808080, 0x2003CA0599421DF8LL);
    } else {
        func_00207E40(1, 0x80D0, 0x8010, 0x80, 0x20, 0x80808080, 0x2003C6855D421DD4LL);
        func_00207E40(1, 0x85C0, 0x8070, 0x40, 0x40, 0x40808080, 0x2003CA0599421DF8LL);
    }

    if (*(unsigned char *)(arg0 + 0x11) == 2) {
        func_00207E40(1, 0x8000, 0x7E80, 0x80, 0x20, 0x805FFF6E, 0x2003C6A55D421DC4LL);
        func_00207E40(1, 0x8800, 0x7E80, 0x80, 0x20, 0x805FFF6E, 0x2003C8055D421DD0LL);
        func_00207E40(1, 0x8190, 0x7E70, 0x20, 0x20, 0x805FFF6E, 0x2003C40555321E24LL);
    } else {
        func_00207E40(1, 0x8000, 0x7E80, 0x80, 0x20, 0x80808080, 0x2003C6A55D421DC4LL);
        func_00207E40(1, 0x8800, 0x7E80, 0x80, 0x20, 0x80808080, 0x2003C8055D421DD0LL);
    }

    if (*(unsigned char *)(arg0 + 0x11) == 3) {
        func_00207E40(1, 0x80D0, 0x7CE0, 0x80, 0x20, 0x805FFF6E, 0x2003C6855D421DC0LL);
        func_00207E40(1, 0x7F50, 0x7D10, 0x20, 0x20, 0x805FFF6E, 0x2003C40555321E24LL);
        func_00207E40(1, 0x85C0, 0x7B90, 0x40, 0x40, 0x80808080, 0x2003CA0599421DF0LL);
    } else {
        func_00207E40(1, 0x80D0, 0x7CE0, 0x80, 0x20, 0x80808080, 0x2003C6855D421DC0LL);
        func_00207E40(1, 0x85C0, 0x7B90, 0x40, 0x40, 0x40808080, 0x2003CA0599421DF0LL);
    }

    if (*(unsigned char *)(arg0 + 0x11) == 4) {
        func_00207E40(1, 0x7590, 0x7D10, 0x80, 0x20, 0x805FFF6E, 0x2003C6855D421DE0LL);
        func_00207E40(1, 0x7C20, 0x7DA0, 0x20, 0x20, 0x805FFF6E, 0x2003C40555321E24LL);
        func_00207E40(1, 0x7550, 0x7B90, 0x40, 0x40, 0x80808080, 0x2003CA0599421E00LL);
    } else {
        func_00207E40(1, 0x7590, 0x7D10, 0x80, 0x20, 0x80808080, 0x2003C6855D421DE0LL);
        func_00207E40(1, 0x7550, 0x7B90, 0x40, 0x40, 0x40808080, 0x2003CA0599421E00LL);
    }

    if (*(unsigned char *)(arg0 + 0x11) == 5) {
        func_00207E40(1, 0x7590, 0x7FE0, 0x80, 0x20, 0x805FFF6E, 0x2003C6855D421DE4LL);
        func_00207E40(1, 0x7C20, 0x7F60, 0x20, 0x20, 0x805FFF6E, 0x2003C40555321E24LL);
        func_00207E40(1, 0x7550, 0x8070, 0x40, 0x40, 0x80808080, 0x2003CA0599421E10LL);
    } else {
        func_00207E40(1, 0x7590, 0x7FE0, 0x80, 0x20, 0x80808080, 0x2003C6855D421DE4LL);
        func_00207E40(1, 0x7550, 0x8070, 0x40, 0x40, 0x40808080, 0x2003CA0599421E10LL);
    }

    func_00207D00(1, 3);
    func_00207E40(1, 0x8D50, 0x7E80, 0x20, 0x20, 0x80808080, 0x2003C82555421E30LL);
}
