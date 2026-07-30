// NEARMISS func_00118418  (vram 0x00118418, 0x294 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 77.58% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc loop-IV strength-reduction wall — the target recomputes `mult i,0x6A` per iteration and carries 7 callee-saved values (frame 0x80); ee-gcc strength-reduces to a pointer IV and needs only 5 (frame 0x60), costing ~17 words. Compounded by an LICM hoist of *(int *)(D_00281AC0+0x14) out of the f...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

//
// Sequencer command handler, dispatched on the event type at a0+0x34.
//   type == 1: broadcast update — walk every playing channel (f00 == 1, state 2) that belongs to
//              this event (f22 == a0->f24, f3E/f02 == the command byte pair, f06 == a0->f18) and
//              re-arm it: set f48, copy the command's byte 3 into f58, mirror f4C into f5A, and
//              recompute both f5C/f5E envelope times as (byte2 * 4) * D_0027F77A / 60 (D_0027F77A
//              is the current tick rate; 60 = NTSC frames/sec).  Advances the stream by 6 bytes.
//   otherwise: note-on — publish the command byte into the mixer block at D_00281AC0+0xC, then
//              find the idle channel (f08 != 1, state 1, f00 == 1) matching this event's low
//              nibble / id / handle, stamp its f4C, look its pitch up in the D_00242630 table via
//              func_00117BA0, then hand the packed (w<<16)|h from func_001179E0 to func_001157F0.
//              Advances the stream by 3 bytes.
// q walks the command stream: *(int *)(a0 + 8) is the byte cursor, D_00281AC0+0x14 the base.
//

extern unsigned char D_0027CCC0[];
extern unsigned char D_0027F740[];
extern unsigned char D_00281AC0[];
extern unsigned char D_00242630[];

extern int func_00117BA0(int, int);
extern int func_001179E0(int, unsigned char *);
extern int func_001157F0(int, int, int, int);

int func_00118418(unsigned char *a0)
{
    unsigned char *p;
    unsigned char *q;
    unsigned char *base;
    unsigned char *rate;
    int i;
    int v;

    if (*(unsigned short *)(a0 + 0x34) == 1) {
        base = D_00281AC0;
        rate = D_0027F740;
        p = D_0027CCC0;
        do {
            if (*(unsigned short *)(p + 0x0) == 1 &&
                *(unsigned short *)(p + 0x1A) == 2 &&
                *(unsigned short *)(p + 0x22) == *(unsigned short *)(a0 + 0x24)) {
                q = (unsigned char *)(*(int *)(a0 + 8) + *(int *)(base + 0x14));
                if (*(unsigned short *)(p + 0x3E) == q[4] &&
                    *(unsigned short *)(p + 0x2) == q[5] &&
                    *(unsigned short *)(p + 0x6) == *(int *)(a0 + 0x18)) {
                    *(unsigned short *)(p + 0x48) = 1;
                    *(unsigned short *)(p + 0x58) = q[3];
                    *(unsigned short *)(p + 0x5A) = *(unsigned short *)(p + 0x4C);
                    *(unsigned short *)(p + 0x5C) =
                        (q[2] << 2) * *(unsigned short *)(rate + 0x3A) / 60;
                    *(unsigned short *)(p + 0x5E) =
                        (q[2] << 2) * *(unsigned short *)(rate + 0x3A) / 60;
                }
            }
            p += 0x6A;
        } while ((int)p < (int)(D_0027CCC0 + 0x13E0));
        *(int *)(a0 + 8) = *(int *)(a0 + 8) + 6;
    } else {
        base = D_00281AC0;
        *(unsigned char *)(*(int *)(base + 0xC) + 4) =
            *(unsigned char *)(*(int *)(a0 + 8) + *(int *)(base + 0x14) + 2);

        for (i = 0; i < 0x30; i++) {
            p = D_0027CCC0 + i * 0x6A;
            if (*(unsigned short *)(p + 0x4) == (*a0 & 0xF) &&
                *(unsigned short *)(p + 0x22) == *(unsigned short *)(a0 + 0x24) &&
                *(unsigned short *)(p + 0x6) == *(int *)(a0 + 0x18) &&
                *(unsigned short *)(p + 0x8) != 1 &&
                *(unsigned short *)(p + 0x1A) == 1 &&
                *(unsigned short *)(p + 0x0) == 1) {
                q = (unsigned char *)(*(int *)(a0 + 8) + *(int *)(base + 0x14));
                *(unsigned short *)(p + 0x4C) = q[2];
                *(unsigned short *)(p + 0x32) = *(unsigned short *)(
                    D_00242630 + ((func_00117BA0(0, *(unsigned short *)(p + 0xE)) >> 2) << 1));
                v = func_001179E0(i, a0);
                func_001157F0(1, i, v >> 16, v & 0xFFFF);
            }
        }
        *(int *)(a0 + 8) = *(int *)(a0 + 8) + 3;
    }
    return 0;
}
