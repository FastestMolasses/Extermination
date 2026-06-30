// NEARMISS func_001D21E0  (vram 0x001D21E0, 0x114 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 70.78% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Head (dma_wait_and_submit block, insns 18-22) and tail (func_00101F08 with field_9C<<14, insns 60-71) match exactly. Two walls: (1) middle hardware-register bitfield block -- target emits 64-bit `dsll32 v0,26 / dsrl32 v0,30` field extraction and `andi $zero,0x3` / `andi $zero,0x1` / `daddiu v1,0x...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// DMA list setup + DMAC/GIF hardware-register reconfigure, then dispatch.
// Logic fully recovered; the middle hardware-register bitfield block compiles
// to sra/andi/ori where the target uses 64-bit dsll32/dsrl32 extraction and
// `andi $zero` constant-field inserts (PS2 hw-register bitfield-union codegen);
// the leading DMA-list block differs only by regalloc/paddub scheduling.
typedef struct { int pad[2]; char *buf; char pad_C[0x90]; int field_9C; } State;
extern State *D_00275670;
extern char *D_00275674;
extern char D_0028F700[];
extern int dmac_channel_base(int);
extern void dma_wait_and_submit(int, int);
extern void func_0011B9E0(int, int, int);
extern int DisableDmacHandler(int);
extern void func_00101F08(int, int);

void func_001D21E0(void) {
    char *s0 = (char *)dmac_channel_base(1);
    State *a1 = D_00275670;
    char *buf = a1->buf;
    char *r = D_00275674;
    buf[3] = 0x20;
    *(int *)(buf + 4) = (int)(r + 0x10);
    *(short *)buf = 0;
    a1 = D_00275670;
    a1->buf = a1->buf + 0x10;
    dma_wait_and_submit(0, 0);

    if ((*(unsigned char *)0x10009000 >> 4) & 0x3) {
        unsigned char x = *(unsigned char *)0x10009000;
        *(unsigned char *)0x10009000 = (x & ~0x30);
    }
    *s0 = (*s0 & ~0x40);
    *(unsigned char *)0x10003C20 = (*(unsigned char *)0x10003C20 & ~0x2) | 0x2;
    func_0011B9E0(1, 1, 0);
    DisableDmacHandler(0);
    func_00101F08((int)s0, (int)(D_0028F700 + (D_00275670->field_9C << 14)));
}
