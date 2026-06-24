// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Engine frame-task sub-state machine (called from func_001AD250 case 2). Dispatch
// on state byte +0xA of the task slot (ptr in scratchpad 0x70003B6C), if-chain over
// states 0..4 (no jr-table). 0: bring-up func_001D2880()+func_001D1EF0(), bump +0xA,
// set u16 +0x18 = 0xF0, kick func_001AEDB0(0). 1: func_001D1EF0(), set gp-rel
// D_00275BD8=1, func_001FF080(0,0x27), bump. 2: gated on D_00275BD8==0 -> func_001AEE10
// +bump+func_001FA790+func_001D2830. 3: push GIF packet (func_001ABF90, four 64-bit
// reg packs), count down the u16 at +0x18 (via a precomputed slot+0x18 pointer so the
// test reads base+0x18 and the decrement uses that pointer), and when the run is done
// (D_0028A9A0==0 && (counter==0 || D_00810E74&0x40)) run func_001AEDE0(4,0)+bump.
// 4: push packet, and once D_0028A9A0==2 finish (func_001FAB50, +9=4, +0xA=0).
// D_0028A9A0/D_00810E74 over-declared short[8]/u16[8] to force absolute addressing
// (idiom #20) while D_00275BD8 stays gp-rel at -sdatathreshold 8.
// Matched 100.0 with mwcc 2.3.3 (mwcps2-2.3.3-000906); 991202 walls at 96.93%
// (delay-slot residual). Verified objdiff 100% vs build/expected/func_001AD4E0.o.
extern void func_001ABF90(long long, long long, long long, long long);
extern void func_001AEDB0(int);
extern void func_001AEDE0(short, unsigned char);
extern void func_001AEE10(short, unsigned char);
extern void func_001D1EF0(void);
extern void func_001D2830(int, int);
extern void func_001D2880(void);
extern void func_001FA790(int, int);
extern void func_001FAB50(void);
extern void func_001FF080(int, int);
extern unsigned char D_00275BD8;
extern short D_0028A9A0[8];
extern unsigned short D_00810E74[8];

void func_001AD4E0(void) {
    unsigned char *p;
    int state;

    p = *(unsigned char **)0x70003B6C;
    state = p[0xA];
    switch (state) {
    case 0:
        func_001D2880();
        func_001D1EF0();
        p = *(unsigned char **)0x70003B6C;
        p[0xA] = p[0xA] + 1;
        (*(unsigned short **)0x70003B6C)[0xC] = 0xF0;
        func_001AEDB0(0);
        return;
    case 1:
        func_001D1EF0();
        D_00275BD8 = 1;
        func_001FF080(0, 0x27);
        p = *(unsigned char **)0x70003B6C;
        p[0xA] = p[0xA] + 1;
        return;
    case 2:
        if (D_00275BD8 == 0) {
            func_001AEE10(4, 0);
            p = *(unsigned char **)0x70003B6C;
            p[0xA] = p[0xA] + 1;
            func_001FA790(0, 0x1B);
            func_001D2830(3, 1);
        }
        return;
    case 3:
        func_001ABF90(0x21322A00LL | (0x2005C006LL << 32),
                       0x21322A40LL | (0x2005C086LL << 32),
                       0x21322C00LL | (0x2005C206LL << 32),
                       0x21322C40LL | (0x2005C286LL << 32));
        {
            unsigned char *q = *(unsigned char **)0x70003B6C;
            unsigned short *r = (unsigned short *)(q + 0x18);
            if (*(unsigned short *)(q + 0x18) != 0) {
                *r = *r - 1;
            }
        }
        if (D_0028A9A0[0] == 0 &&
            ((*(unsigned short **)0x70003B6C)[0xC] == 0 || (D_00810E74[0] & 0x40))) {
            func_001AEDE0(4, 0);
            p = *(unsigned char **)0x70003B6C;
            p[0xA] = p[0xA] + 1;
        }
        break;
    case 4:
        func_001ABF90(0x21322A00LL | (0x2005C006LL << 32),
                       0x21322A40LL | (0x2005C086LL << 32),
                       0x21322C00LL | (0x2005C206LL << 32),
                       0x21322C40LL | (0x2005C286LL << 32));
        if (D_0028A9A0[0] == 2) {
            func_001FAB50();
            (*(unsigned char **)0x70003B6C)[9] = 4;
            (*(unsigned char **)0x70003B6C)[0xA] = 0;
        }
        break;
    }
}
