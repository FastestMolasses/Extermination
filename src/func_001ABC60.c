// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Engine frame-task sub-state machine (sibling of func_001ADF50). Dispatches on
// the state byte +9 of the current task slot (slot ptr in scratchpad 0x70003B6C),
// if-chain over states 0..3 (no jr-table). State 0 inits the slot (+0x16 u16 =
// 0x12C, +0xF = 0, gp-rel D_00275BD8 = 1) and kicks func_001FF080(0,0x29,1,&slot[9]).
// State 1: gated on D_00275BD8==0, func_001AEE10(...) then bump +9. States 2/3 push
// a GIF/DMA packet via func_001ABF90 (four 64-bit reg packs); state 2 counts down
// the u16 at +0x16 and on underflow runs func_001AEDE0(4,0) + bump; state 3 returns
// 1 once D_0028A9A0==2. D_0028A9A0 over-declared short[8] to force absolute
// addressing (idiom #20) while D_00275BD8 stays gp-rel at -sdatathreshold 8.
// Matched 100.0 with mwcc 2.3.3 (mwcps2-2.3.3-000906); 991202 walls at 94.16%
// (dead-block / delay-slot residual). Verified objdiff 100% vs build/expected/func_001ABC60.o.
extern void func_001ABF90(long long, long long, long long, long long);
extern void func_001AEDE0(int, int);
extern void func_001AEE10(int, int, int, int);
extern void func_001FF080(int, int, int, int);
extern unsigned char D_00275BD8;
extern short D_0028A9A0[8];

int func_001ABC60(void) {
    unsigned char *p;
    unsigned char *pp;
    int state;

    p = *(unsigned char **)0x70003B6C;
    pp = p + 9;
    state = p[9];
    switch (state) {
    case 0:
        *pp = *pp + 1;
        (*(unsigned short **)0x70003B6C)[0xB] = 0x12C;
        (*(unsigned char **)0x70003B6C)[0xF] = 0;
        D_00275BD8 = 1;
        func_001FF080(0, 0x29, 1, (int)pp);
        break;
    case 1:
        if (D_00275BD8 == 0) {
            func_001AEE10(4, 0, 1, (int)pp);
            p = *(unsigned char **)0x70003B6C;
            p[9] = p[9] + 1;
        }
        break;
    case 2:
        func_001ABF90(0x21422E00LL | (0x20060406LL << 32),
                       0x21422E40LL | (0x20060426LL << 32),
                       0x21422F00LL | (0x20060486LL << 32),
                       0x21422F40LL | (0x20060486LL << 32));
        {
            unsigned short *q = *(unsigned short **)0x70003B6C;
            unsigned short cnt = q[0xB];
            q[0xB] = cnt - 1;
            if (cnt == 0) {
                func_001AEDE0(4, 0);
                p = *(unsigned char **)0x70003B6C;
                p[9] = p[9] + 1;
            }
        }
        break;
    case 3:
        func_001ABF90(0x21422E00LL | (0x20060406LL << 32),
                       0x21422E40LL | (0x20060426LL << 32),
                       0x21422F00LL | (0x20060486LL << 32),
                       0x21422F40LL | (0x20060486LL << 32));
        if (D_0028A9A0[0] == 2) {
            return 1;
        }
        break;
    }
    return 0;
}
