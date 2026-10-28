// NEARMISS func_0011A070  (vram 0x0011A070, 0x124 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.90% via ee-gcc 2.9-991111-01 (-O2). Body corrected in matching wave 2026-10-27: an
// index >= 0x30 returns at once (the old C still ran the 0x30-entry scan); the slot test reads the
// table directly while the memset call takes a separate local base pointer (not CSE'd, as in the
// original). Remaining differences: first-block register colouring only — the original keeps the
// table address in one register and a copy of it in a second (test sum overwrites the first), the
// mult product in a0; this C keeps one register for both. The scan loop is byte-identical.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

// D_0027E0C0: 0x30 records of 0x78 bytes.
struct E { char pad[0x2E]; unsigned short f2E; unsigned short f30; unsigned short f32; char pad2[0x78 - 0x34]; };
extern struct E D_0027E0C0[];
extern unsigned char D_0027CCC0[];
extern unsigned char D_0027F740[];
extern void func_00121A28(void *p, int a1, int a2);   /* memset */
extern int func_001157F0(int a0, int a1, int a2, int a3);

// Release record (arg0 & 0x7FFF): if it is in state f32 == 1 with f2E == 0, clear it. Then, for
// each of the 0x30 0x6A-byte D_0027CCC0 entries in state 2 bound to this record: when bit 0x8000
// of arg0 is set, call func_001157F0(3, i, 0, 0) and clear the entry's first halfword; set bit i
// of the 64-bit mask at D_0027F740+0x28 and set the entry's +8 halfword to 1.
void func_0011A070(int arg0) {
    unsigned int idx = (unsigned int)(arg0 & 0x7FFF);
    unsigned char *base = (unsigned char *)D_0027E0C0;
    if (idx >= 0x30) return;
    if (D_0027E0C0[idx].f32 == 1 && D_0027E0C0[idx].f2E == 0) {
        func_00121A28(base + idx * 0x78, 0, 0x78);
    }
    {
        int flag = arg0 & 0x8000;
        unsigned char *p = D_0027CCC0;
        unsigned char *st = D_0027F740;
        long mask = 1;
        int i = 0;
        do {
            if (*(unsigned short *)(p + 0x1A) == 2 &&
                *(unsigned short *)(p + 6) == idx) {
                if (flag) {
                    func_001157F0(3, i, 0, 0);
                    *(unsigned short *)p = 0;
                }
                *(long *)(st + 0x28) |= mask;
                *(unsigned short *)(p + 8) = 1;
            }
            p += 0x6A;
            i++;
            mask <<= 1;
        } while (i < 0x30);
    }
}
