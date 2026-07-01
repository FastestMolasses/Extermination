// NEARMISS func_001D2300  (vram 0x001D2300, 0x280 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 80.44% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation-order permutation (extra callee-saved register / one different cached pointer register: target uses t0 for D_00275670, mwcc233 promotes it to s2 plus reuses D_00275674 differently across the two matrix-setup calls, inflating the frame from 0x30 to 0x40). Logic and all field of...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Per-frame render/VIF display-list setup for the current entity slot
// (D_00275670->field_9C, sub-object base D_00275674 + slot*0x190). Appends a
// GIF/DMA tag block into the channel-5 cursor at D_00275670+0x14 (byte+3=0x30,
// word+4=D_00275674, half+0=1, advance +0x10), calls func_001D1F80(1,0,7) to
// push a fixed sync command, sets up two matrices via func_001015A8/
// func_00101810 into slot+0x40/slot+0xC0 with a signed 16-bit toggle from
// (1-D_00810E88) sign-extended through a 64-bit intermediate, refreshes the
// display-list buffer pointer (func_001D2110), and appends command 0x19 at
// slot+0x20. Conditionally appends a command at D_00275674+0x420 (with
// func_001D2830(3)) if func_001D2910(3), else at D_00275674+0x3A0. Unless
// D_008106C4 is set, pushes two texture blocks at fixed offsets in
// D_0028F700 (indexed by the slot number, possibly doubled if
// func_001D2910(4)/func_001D2910(0x20) trigger a teardown via
// func_001E0DF0), each followed by func_001D2160(D_00275670->0x10 or
// ->0x0/->0x4). Always pushes a third texture block at D_0028F700 +
// slot*0x95760 + 0xC9000. If D_008106C4 is set, pushes yet another pair of
// blocks (variant stride 0x60800). Finally dispatches via func_001D21E0.

typedef struct {
    int f0;
    int f4;
    char pad_8[0x08];
    int *ptr10;
    char *buf14;
    char pad_18[0x84];
    int field_9C;
} State;

extern State *D_00275670;
extern char *D_00275674;
extern char D_0028F700[];
extern short D_00810E88;
extern unsigned char D_008106C4;

extern void func_001D1F80(int a0, int a1, int a2);
extern void func_001015A8(int a0, short a1, short a2, int a3);
extern void func_00101810(int a0, short a1, short a2, int a3);
extern void func_001D2110(void);
extern void func_001D2130(int a0);
extern void func_001D2160(int a0);
extern void func_001D2180(int a0, short a1);
extern void func_001D21E0(void);
extern void func_001D2830(int a0, int a1);
extern int func_001D2910(int a0);
extern void func_001E0DF0(void);

void func_001D2300(void) {
    State *st;
    int slot;
    int off;
    int mode;
    char *ctx;

    st = D_00275670;
    ctx = D_00275674;
    slot = st->field_9C;

    st->buf14[3] = 0x30;
    *(int *)(st->buf14 + 4) = (int)ctx;
    *(short *)st->buf14 = 1;
    st->buf14 = st->buf14 + 0x10;

    func_001D1F80(1, 0, 7);

    off = slot * 0x190;
    func_001015A8((int)(D_00275674 + off) + 0x40, *(short *)0x70003B70, *(short *)0x70003B72,
                   (int)(long long)(short)(1 - D_00810E88));
    func_00101810((int)(D_00275674 + off) + 0xC0, *(short *)0x70003B70, *(short *)0x70003B72,
                   (int)(long long)(short)(1 - D_00810E88));
    func_001D2110();
    func_001D2180((int)(D_00275674 + off) + 0x20, 0x19);

    if (func_001D2910(3) != 0) {
        func_001D2180((int)(D_00275674 + 0x420), 8);
        func_001D2830(3, 0);
    } else {
        func_001D2180((int)(D_00275674 + 0x3A0), 8);
    }

    if (D_008106C4 == 0) {
        mode = slot * 2;
        if (func_001D2910(4) == 0) {
            if (func_001D2910(0x20) != 0) {
                func_001E0DF0();
            }
            mode = slot * 2;
        }
        func_001D2130((int)(D_0028F700 + ((((mode + slot) << 6) + slot) << 0xB) + 8 + 0x7FF8));
        func_001D2160((int)st->ptr10);
    }
    if (D_008106C4 == 0) {
        func_001D2130(*(int *)st);
        func_001D2160(*(int *)((char *)st + 4));
    }

    func_001D2130((int)(D_0028F700 + (slot * 0x95760) + 0xC9000));
    func_001D2160((int)st->buf14);

    if (D_008106C4 != 0) {
        func_001D2130((int)(D_0028F700 + (slot * 0x60800) + 8 + 0x7FF8));
        func_001D2160((int)st->ptr10);
        func_001D2130(*(int *)st);
        func_001D2160(*(int *)((char *)st + 4));
    }

    func_001D21E0();
}
