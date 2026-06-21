// COMPILER: mwcc233
// CFLAGS: -O4,p
// Pad-actuator rumble request, func_001B61C0(big, small, dur, force).
// Gated on rumble-enable byte D_00810119, actuator-ready (pad block
// D_00810E40 +0x12) and pause/mode byte D_00275BE0 != 2; unless `force`,
// an active rumble (+0x16) is not restarted. Writes +0x16 active flag,
// +0x28 duration, +0x18 big-motor on/off, +0x19 small-motor power, then
// submits via libpad func_00111018(port +0x04, slot +0x08, &block[0x18]).
//
// The 991202 build stalls at 93.94% on the prologue address-pair-split
// wall (it fills the lui->addiu stall with `sq ra`). mwcc 2.3.3 keeps the
// pair adjacent before the ra save, matching CW. Two reloc details:
// D_00275BE0 is gp-rel (plain scalar, default sdata threshold);
// D_00810119 is a far global, so it is declared `[]` and accessed `[0]`
// to force a HI16/LO16 address pair instead of a gp-rel load. idiom-7
// fake params `mode`/`p` pin the pause byte to $t0 and the block ptr to $t1.
typedef struct PadBlock { char pad00[4]; int port; int slot;
    char pad0c[6]; volatile unsigned char ready; char pad13[3];
    volatile unsigned char active; char pad17; unsigned char bigOn;
    unsigned char smallPower; char pad1a[14];
    unsigned short duration; } PadBlock;
extern PadBlock D_00810E40;
extern volatile unsigned char D_00275BE0;
extern volatile unsigned char D_00810119[];
extern void func_00111018(int port, int slot, unsigned char *act);

void func_001B61C0(int big, int small, int dur, int force,
                   int mode, PadBlock *p) {
    int one;
    p = &D_00810E40;
    if (D_00810119[0] == 0) return;
    if (p->ready == 0) return;
    mode = D_00275BE0;
    if (mode == 2) return;
    if (force == 0) { if (p->active != 0) return; }
    one = 1;
    p->active = one;
    p->duration = dur;
    if (big != 0) p->bigOn = one;
    p->smallPower = small;
    func_00111018(p->port, p->slot, &p->bigOn);
}
