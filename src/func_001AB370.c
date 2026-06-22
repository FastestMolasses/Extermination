// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Graphics/VU1 subsystem cold-init. Resets VU1 (vu1_cold_start) and the DMAC
// twice (dmac_reset(1)), builds a small DMA/packet descriptor on the stack via
// func_00101EA8 + func_00101CD0 (with field +6 = 0x100), then issues two GS
// setup calls: func_00100158(0,1,2,1) and func_00101898(&D_00810EA0,0,0x200,
// 0xE0,2,0x31,1) (framebuffer 0x200x0xE0, psm 2, etc). Writes the 64-bit value
// 0x80000000 (0x8000<<16) into the two DMA-tag templates D_00811020/D_00811190,
// and zeroes the 0x70003B94/96 counters while setting the 0x70003B70/72 pair to
// 0x800. Returns void.
//
// The 0x30 stack frame holds a >=0x18-byte descriptor at sp+0x10 (modeled as
// struct S so field f6 stores sp-relative as `sh,0x16(sp)`). The two 64-bit
// DMA-tag stores use explicit `*(long long *)&sym` to force CW's full-address
// (lui+addiu;sd val,0(reg)) materialization rather than the lui-hi/lo-in-store
// form. 100% on mwcc 2.3.3; 991202 = 82.9%. Verified objdiff 100% vs
// build/expected/func_001AB370.o.
extern int vu1_cold_start(void);
extern int dmac_reset(int);
extern int func_00101EA8(void *);
extern int func_00101CD0(void *);
extern int func_00100158(int, int, int, int);
extern int func_00101898(void *, int, int, int, int, int, int);
extern char D_00810EA0;
extern long long D_00811020;
extern long long D_00811190;

struct S { char pad[6]; short f6; char pad2[24]; };

void func_001AB370(void) {
    struct S sp10;
    long long temp_a0;

    vu1_cold_start();
    dmac_reset(1);
    func_00101EA8(&sp10);
    sp10.f6 = 0x100;
    func_00101CD0(&sp10);
    dmac_reset(1);
    func_00100158(0, 1, 2, 1);
    func_00101898(&D_00810EA0, 0, 0x200, 0xE0, 2, 0x31, 1);
    temp_a0 = (long long)0x8000 << 0x10;
    *(long long *)&D_00811020 = temp_a0;
    *(long long *)&D_00811190 = temp_a0;
    *(short *)0x70003B94 = 0;
    *(short *)0x70003B96 = 0;
    *(short *)0x70003B70 = 0x800;
    *(short *)0x70003B72 = 0x800;
}
