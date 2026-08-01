// NEARMISS sub_sceMpegGetPicture_decode_error  (vram 0xceMpegGetPicture_decode_error, 0x2D8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.64% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two residual classes, both backend permutations on an otherwise byte-faithful 202-instruction body. (1) SAVED-REGISTER COLORING in the blit loop: CodeWarrior gives the loop-INVARIANT frame width/height s3/s4 and the loop-carried strip offsets s5/s6; mwcc inverts it (invariants -> s5/s6, offsets -...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Full-screen MPEG playback pump: decode one picture, DMA it to the GS, repeat.
// arg0 is the stream/player handle: h[0]/h[1] are the frame width/height that
// the blit routine wants as 64-bit stack arguments, h[2] is a "suppress
// display" flag. Returns 1 on a normal finish, -1 if the demuxer reported the
// terminal state 1 (end of stream / error), in which case func_00204090 posts
// the failure message D_002734C0.
//
// Per iteration:
//   * func_00206B80(h) == 1 -> terminal; post D_002734C0, latch -1 and stop.
//   * spin func_00203980() (the decoder pump) until func_00203C30(&D_007A55A0)
//     hands back a decoded picture.
//   * for DMA channels 8 and 9: wait idle (func_00102468), clear CHCR.STR,
//     point SADR (+0x80) at scratchpad 0x70003000, kick the 0x100-qword
//     transfer list at D_002822C0, wait idle again.
//   * hand the picture to the A/V sync (func_00109918(h, pic, 0x4B0)); a
//     negative result posts the warning D_002734E0 via func_00204080.
//   * unless display is suppressed (h[2] != 0), blit the picture: for each of
//     D_007A55B0 vertical strips call func_00205240 twice (field 0 then field
//     1) with destination D_007A55C0, source D_007A55A4 + strip*0x39640 (+0x40
//     for the even field, +0x1CB40 for the odd) and picture data
//     D_007A55A0 + strip*0x12C000, tile size 0x200 x 0xE0, plus the frame
//     width/height as the two 64-bit stack arguments.
//   * release the picture (func_00203BA0) and pump the decoder once more.
// The loop continues while func_00109A30(h) reports the stream is still live;
// func_00109A50(h) tears it down on the way out.
typedef struct {
    unsigned char chcr_lo : 6; /* CHCR bits 0-5 */
    unsigned char str : 1;     /* CHCR "start" bit (0x40) */
    unsigned char chcr_hi : 1;
    unsigned char pad[0x7F];
    int sadr; /* +0x80: scratchpad address register */
} DmaChan;

extern int func_00109A30(int *h);
extern void func_00109A50(int *h);
extern int func_00109918(int *h, int buf, int n);
extern int func_00206B80(int *h);
extern void func_00204090(void *msg);
extern void func_00204080(void *msg);
extern int func_00203C30(void *s);
extern void func_00203980(void);
extern void func_00203BA0(void *s);
extern DmaChan *dmac_channel_base(int ch);
extern void func_00102468(DmaChan *ch, int a, int b);
extern void dma_kick(DmaChan *ch, void *list, int n);
extern void func_00205240(void *dst, char *src, char *buf, int field, int a, int b, int c, int d,
                          long long w, long long h);

extern char D_002734C0[];
extern char D_002734E0[];
extern char D_002822C0[];
extern char *D_007A55A0;
extern char *D_007A55A4;
extern int D_007A55B0;
extern char D_007A55C0[];

int sub_sceMpegGetPicture_decode_error(int *h) {
    int r;
    int st;
    DmaChan *ch;
    int w;
    int ht;
    int i;
    int off1;
    int off2;

    st = 1;
    if (func_00109A30(h) == 0) {
        do {
            if (func_00206B80(h) == 1) {
                st = -1;
                func_00204090(D_002734C0);
                break;
            }
            r = func_00203C30(&D_007A55A0);
            if (r == 0) {
                do {
                    func_00203980();
                    r = func_00203C30(&D_007A55A0);
                } while (r == 0);
            }
            ch = dmac_channel_base(8);
            func_00102468(ch, 0, 0);
            ch->str = 0;
            ch->sadr = 0x70003000;
            dma_kick(ch, D_002822C0, 0x100);
            func_00102468(ch, 0, 0);
            r = func_00109918(h, r, 0x4B0);
            ch = dmac_channel_base(9);
            func_00102468(ch, 0, 0);
            ch->str = 0;
            ch->sadr = 0x70003000;
            dma_kick(ch, D_002822C0, 0x100);
            func_00102468(ch, 0, 0);
            if (r < 0) {
                func_00204080(D_002734E0);
            }
            if (h[2] == 0) {
                w = h[0];
                ht = h[1];
                i = 0;
                off1 = 0;
                off2 = 0;
                while (i < D_007A55B0) {
                    func_00205240(D_007A55C0, D_007A55A4 + off2 + 0x40, D_007A55A0 + off1, 0, 0, 0,
                                  0x200, 0xE0, w, ht);
                    func_00205240(D_007A55C0, D_007A55A4 + off2 + 0x1CB40, D_007A55A0 + off1, 1, 0,
                                  0, 0x200, 0xE0, w, ht);
                    off1 += 0x12C000;
                    off2 += 0x39640;
                    i += 1;
                }
            }
            func_00203BA0(&D_007A55A0);
            func_00203980();
        } while (func_00109A30(h) == 0);
    }
    func_00109A50(h);
    return st;
}
