// NEARMISS func_001FF1E0  (vram 0x001FF1E0, 0x210 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 82.06% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body fully correct (resource-bank streamer): gp-rel D_00275C70 vs non-gp 0x28xxxx symbols resolved via sized-array decls (>sdatathreshold 8), all four loops + the (t2&0xFFFFFF)+base / D_0028A490[t2>>24] relocation reproduced, register reuse of the 0xE count as loop2/3 base index modeled. Residual...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// Streams a packed resource bank into memory. arg0 selects a base: 0 ->
// 0xB00000, else the running pointer in D_0028A734. It DMA-loads the header
// (func_00200780/func_00200700 retry pair) into D_00289BC0 from offset
// arg0<<11, publishes it as the current bank D_00275C70, then for each of the
// header's [0xE] sub-chunks (n0) copies [0x20]/[0x24] (offset+size) into the
// bank base. After a trailing [0x14]/[8] copy it advances D_0028A734/D_0028A738
// by the consumed size, walks the [0x10] table (cnt10) running func_00200830
// over each stored chunk (chunk sizes read from the [n0+i] slot, field 0x24),
// then relocates the [0x1C] pointer table (cnt1c) starting at slot [n0+cnt10]:
// for each entry e, D_0028A490[e>>24] = (e & 0xFFFFFF) + base.
//
// NEARMISS: readable C, body byte-correct, 82% on mwcc 2.3.3 (991202 79%).
// Residuals are register-allocation renumbering, one extra callee-saved reg
// (frame 0x80 vs 0x70), and arg-materialization/scheduling order; no single
// idiom -- permuter/scheduling class. The sized-array externs below only push
// the 0x28xxxx symbols past sdatathreshold (non-gp); their real sizes differ.

extern int func_00200700(void);
extern void func_00200780(int a, int b, int c, int d);
extern void func_00200830(int a);
extern void *D_00275C70;
extern char D_00289BC0[0x800];
extern char D_0028A480[16];
extern char D_0028A488[16];
extern int D_0028A490[64];
extern void *D_0028A734[16];
extern void *D_0028A738[16];

void func_001FF1E0(int arg0) {
    char *base;
    char *bank;
    int off;
    int n0;
    int cnt10;
    int cnt1c;
    int i;
    int j;
    unsigned int e;
    int *p;

    if (arg0 == 0) {
        base = (char *)0xB00000;
    } else {
        base = (char *)D_0028A734[0];
    }
    do {
        func_00200780((int)D_0028A480, (int)D_00289BC0, arg0 << 0xB, 0x800);
    } while (func_00200700() == 0);

    D_00275C70 = D_00289BC0;
    n0 = *(unsigned short *)(D_00289BC0 + 0xE);
    off = *(int *)(D_00289BC0 + 4);
    if (n0 != 0 && (unsigned int)n0 != 0) {
        i = 0;
        j = 0;
        do {
            bank = (char *)D_00275C70 + j;
            do {
                func_00200780((int)D_0028A488, (int)base,
                              *(int *)(bank + 0x20) + off, *(int *)(bank + 0x24));
            } while (func_00200700() == 0);
            func_00200830((int)base);
            i += 1;
            j += 8;
        } while ((unsigned int)i < (unsigned int)n0);
    }

    {
        int sz = *(int *)((char *)D_00275C70 + 0x14);
        int rem = *(int *)((char *)D_00275C70 + 8) - sz;
        off += sz;
        do {
            func_00200780((int)D_0028A488, (int)base, off, rem);
        } while (func_00200700() == 0);
        base += rem;
        D_0028A734[0] = base;
        D_0028A738[0] = base;
    }

    cnt10 = *(int *)((char *)D_00275C70 + 0x10);
    j = 0;
    if (cnt10 != 0 && (unsigned int)cnt10 != 0) {
        i = 0;
        do {
            func_00200830((int)(base + j));
            j += *(int *)((char *)D_00275C70 + (n0 + i) * 8 + 0x24);
            i += 1;
        } while ((unsigned int)i < (unsigned int)cnt10);
    }

    cnt1c = *(int *)((char *)D_00275C70 + 0x1C);
    if (cnt1c != 0 && (unsigned int)cnt1c != 0) {
        p = (int *)((char *)D_00275C70 + (n0 + cnt10) * 8 + 0x20);
        i = 0;
        do {
            e = *(unsigned int *)p;
            i += 1;
            D_0028A490[e >> 0x18] = (e & 0xFFFFFF) + (int)base;
            p += 1;
        } while ((unsigned int)i < (unsigned int)cnt1c);
    }
}
