// NEARMISS func_001B5940  (vram 0x001B5940, 0x228 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 97.43% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Entry branch shape (the original reaches the failed-read exit through a plain branch with an
// empty slot) and register colouring of the repeat-mask compare.
//
// The function links from the asm body in src/func_001B5940.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Reads one pad and updates its button block (port em_input: the 001B5940
// pad block translation). port+4 / port+8 are the pad port and slot; the
// read (func_00110B38, scePadRead) fills a 32-byte buffer and the call
// returns 0 when it fails or the buffer's status byte is non-zero.
// pad->now = buttons (active high), pad->prev = previous buttons, pad->trig = newly
// pressed, pad->trig_prev = previous newly pressed, pad->rep = pressed with d-pad
// auto-repeat, pad[5] = the repeat timer (32 frames first, then every 10).
// In analog mode the stick bytes go to port+0x24..+0x27 (left x / y at
// +0x24 / +0x25), the left stick's deflection level (func_001B5CC0) to
// +0x17: a deflected stick on port 0 replaces the d-pad bits with the
// stick's direction (func_001B5D70) and the stick bytes are snapped
// (func_001B5C90); a centred stick on port 0 with a d-pad bit held, or
// digital mode, synthesises the stick from the d-pad (func_001B5E20).
// Returns 1 after an update.
typedef struct PadBlock {
    unsigned short now;     /* 0x0 */
    unsigned short prev;    /* 0x2 */
    unsigned short trig;    /* 0x4 */
    unsigned short trig_prev; /* 0x6 */
    unsigned short rep;     /* 0x8 */
    short timer;            /* 0xA */
} PadBlock;

extern int func_00110B38(int port, int slot, unsigned char *buf);
extern int func_001B5CC0(unsigned char x, unsigned char y);
extern unsigned short func_001B5D70(unsigned char v, int axis);
extern int func_001B5C90(unsigned char x);
extern void func_001B5E20(PadBlock *pad, unsigned char *out);

int func_001B5940(PadBlock *pad, unsigned char *port, int analog) {
    unsigned char buf[0x20];

    if (func_00110B38(*(int *)(port + 4), *(int *)(port + 8), buf) != 0) {
        if (buf[0] == 0) {
            pad->prev = pad->now;
            pad->now = ((buf[2] << 8) | buf[3]) ^ 0xFFFF;
            if (analog) {
                port[0x26] = buf[4];
                port[0x27] = buf[5];
                port[0x24] = buf[6];
                port[0x25] = buf[7];
                port[0x17] = func_001B5CC0(port[0x24], port[0x25]);
                if (port[0x17] == 0) {
                    if (*(int *)(port + 4) == 0 && (pad->now & 0xF000)) {
                        func_001B5E20(pad, port);
                    }
                } else {
                    if (*(int *)(port + 4) == 0) {
                        pad->now &= 0xFFF;
                        pad->now |= func_001B5D70(port[0x24], 0);
                        pad->now |= func_001B5D70(port[0x25], 1);
                    }
                    port[0x24] = func_001B5C90(port[0x24]);
                    port[0x25] = func_001B5C90(port[0x25]);
                }
            } else {
                func_001B5E20(pad, port);
            }
            pad->trig_prev = pad->trig;
            pad->trig = ~pad->prev & pad->now;
            if ((unsigned short)(pad->now & 0xF000) == (unsigned short)(pad->prev & 0xF000)
                && (pad->now & 0xF000)) {
                pad->timer--;
                if (pad->timer == 0) {
                    pad->rep = pad->now & 0xF000;
                    pad->rep = pad->rep | (pad->trig & 0xFFFF0FFF);
                    pad->timer = 10;
                } else {
                    pad->rep = pad->trig & 0xFFFF0FFF;
                }
            } else {
                pad->timer = 0x20;
                pad->rep = pad->trig;
            }
            return 1;
        }
    }
    return 0;
}
