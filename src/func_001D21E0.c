// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 70.78% NEARMISS).
// Fixes read from the original instructions:
// - The hardware registers are CodeWarrior bitfield structs (the matched
//   func_001AB590 spelling): the VIF1 CHCR byte at 0x10009000 is a volatile
//   DmaChcrByte whose ASP field (bits 5:4) is cleared if nonzero; the channel
//   base byte from dmac_channel_base(1) gets its TTE bit (bit 6) cleared; the
//   byte at 0x10003C20 gets bit 1 set through a 1-bit field. The 64-bit
//   shift extract and the dead constant masks are CW's bitfield codegen, not
//   a wall.
// - D_00275674 + 0x10 is computed into a local before the tag-block stores.
// - FlushCache takes the cache id (0) as in the SDK.

// DMA list setup + DMAC/GIF hardware-register reconfigure, then dispatch:
// writes a tag block at D_00275670->buf (byte+3 = 0x20, word+4 =
// D_00275674 + 0x10, half+0 = 0), advances buf by 0x10, submits it with
// dma_wait_and_submit(0, 0), clears the VIF1 CHCR ASP field and the
// channel-1 TTE bit, sets bit 1 of 0x10003C20, calls func_0011B9E0(1, 1, 0)
// and FlushCache(0), then func_00101F08 with the channel base and
// D_0028F700 + (D_00275670->field_9C << 14).

typedef struct { int pad[2]; char *buf; char pad_C[0x90]; int field_9C; } State;
typedef struct {            // DMA CHCR byte 0 as CW bitfields
    unsigned char dir : 1;
    unsigned char pad : 1;
    unsigned char mod : 2;
    unsigned char asp : 2;
    unsigned char tte : 1;
    unsigned char tie : 1;
} DmaChcrByte;
typedef struct {            // byte at 0x10003C20, bit 1 is set here
    unsigned char b0 : 1;
    unsigned char b1 : 1;
    unsigned char b2 : 1;
    unsigned char rest : 5;
} CtrlByte;
extern State *D_00275670;
extern char *D_00275674;
extern char D_0028F700[];
extern int dmac_channel_base(unsigned int ch);
extern int dma_wait_and_submit(int, int);
extern void func_0011B9E0(int a0, int a1, int a2);
extern void FlushCache(int id);
extern void func_00101F08(int *s0, int arg1);

void func_001D21E0(void) {
    DmaChcrByte *s0 = (DmaChcrByte *)dmac_channel_base(1);
    char *buf = D_00275670->buf;
    char *r = D_00275674 + 0x10;
    buf[3] = 0x20;
    *(char **)(buf + 4) = r;
    *(short *)buf = 0;
    D_00275670->buf += 0x10;
    dma_wait_and_submit(0, 0);
    if (((volatile DmaChcrByte *)0x10009000)->asp)
        ((volatile DmaChcrByte *)0x10009000)->asp = 0;
    s0->tte = 0;
    ((volatile CtrlByte *)0x10003C20)->b1 = 1;
    func_0011B9E0(1, 1, 0);
    FlushCache(0);
    func_00101F08((int *)s0, (int)(D_0028F700 + (D_00275670->field_9C << 14)));
}
