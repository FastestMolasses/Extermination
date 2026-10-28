// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 91.42% NEARMISS). Fixes read
// from the original instructions:
// - The glyph dispatch is a switch (case 0xA, case 0xC falling into case 0) inside the
//   "special byte" test, followed by a break out of the line loop: every special byte ends the
//   line, and the remaining-line counter carries across lines (it is seeded once with 4).
// - The render result test is a switch (case 0 return, case 1 break, case 2 return; other
//   values continue), not an if/goto chain.
// - The line-advance step updates +0x28 right after the line counter increment.
// - func_001FE4D0's third parameter is int; the line counter is declared before the slot pointer.
//
// Subtitle/caption-line "cue" state machine, driven by arg0+0x78 (0=idle/prime, 1=active/build,
// 2=done). arg0 is a caption-cue context: +0/4/8/0xC a 16-byte header mirrored through the globals
// D_00820EC0..CC around a call to func_001FC9B0 (a caption-buffer/state initializer) in state 0,
// which also seeds +0x18/+0x3C, advances the state to 1, and clears the busy counter +0x48.
//
// State 1 first re-validates +0x3C against +8 (the header's generation/id field); on mismatch it
// aborts by clearing D_00282228 (a "cue active" flag) and returning. On match it clears two stack
// scratch buffers (a 0x180-byte record-slot table and a 0x600-byte glyph-index table), re-derives
// +0x34/+0x1C via func_001FE480(D_0028A4EC, ...) (a table-lookup helper whose first arg is the
// live pointer variable D_0028A4EC, not its address), then walks 4 "lines" of up to 4 style
// records each: for each glyph offset it queries func_001FE4D0(D_0028A4EC, +0x34, +0x44) for a
// pending style/command record and, if its region tag (rec+8) matches the running glyph offset,
// stashes the record pointer into the record-slot table and advances +0x44 (consumed-record
// count). It then reads the glyph-index byte at +0x1C[offset]: 0xA (soft-wrap) advances the offset,
// decrements the remaining-line counter and ends the line; 0xC (hard-break) advances the offset,
// zeroes that counter and ends the line; 0 (terminator) zeroes it without advancing; any other byte is a literal glyph appended to the current
// line's byte buffer. After all 4 lines, +0x44 is corrected by subtracting the consumed-record
// count, then +0x24/+0x28/+0x20 are seeded (0x37, 0x1C, &D_00264C90) as layout constants and a
// second pass calls func_001FDDB0(line, *slot, arg0) per line to actually render/commit each
// line's records: a return of 0 or 2 aborts immediately, and 1 (or any other value) continues to
// the next line (accumulating +0x28's baseline via (D_00264C98+D_00264CA0)>>1 per line). If all 4
// lines render, +0x34 is advanced past the header id and func_001FE070(D_0028A4EC, +0x34, 0x39,
// 0x95) commits the next cue, with +0x10 set to 1 (render-ready flag). State 2 is a no-op (cue
// fully consumed, nothing to do until re-primed).

struct blk { int w[6]; };
extern void func_00121A28(char *p, int a, int n);
extern int func_001FC9B0(void);
extern int func_001FDDB0(int *a, int b, char *c);
extern void func_001FE070(int *a, int b, int c, int d);
extern int func_001FE480(int *a, int b);
extern int *func_001FE4D0(int a, int b, int c);
extern char D_00264C90;
extern int D_00264C98;
extern int D_00264CA0;
extern struct blk D_00264DB0;
extern int D_00282228;
extern int *D_0028A4EC;
extern int D_00820EC0;
extern int D_00820EC4;
extern int D_00820EC8;
extern int D_00820ECC;

void func_001FD0E0(char *arg0) {
    char cbuf[0x180];
    int sbuf[0x180];
    struct blk idxblk;
    int adv;
    int cnt;
    int off;
    int spA0;
    int *spB0;
    char *fp;
    int *s7;
    int *s3;
    int *s4;
    char *s5;

    cnt = 0;
    idxblk = D_00264DB0;
    off = 0;
    switch (*(int *)(arg0 + 0x78)) {
    case 0:
        D_00820EC0 = *(int *)(arg0 + 0);
        D_00820EC4 = *(int *)(arg0 + 4);
        D_00820EC8 = *(int *)(arg0 + 8);
        D_00820ECC = *(int *)(arg0 + 0xC);
        func_001FC9B0();
        *(int *)(arg0 + 0) = D_00820EC0;
        *(int *)(arg0 + 4) = D_00820EC4;
        *(int *)(arg0 + 8) = D_00820EC8;
        *(int *)(arg0 + 0xC) = D_00820ECC;
        *(int *)(arg0 + 0x18) = func_001FE480(D_0028A4EC, *(int *)(arg0 + 0x34));
        *(int *)(arg0 + 0x3C) = *(int *)(arg0 + 8);
        *(int *)(arg0 + 0x78) = 1;
        *(int *)(arg0 + 0x48) = 0;
        return;
    case 1:
        if (*(int *)(arg0 + 0x3C) != *(int *)(arg0 + 8)) {
            D_00282228 = 0;
            return;
        }
        func_00121A28(cbuf, 0, 0x180);
        func_00121A28((char *)sbuf, 0, 0x600);
        *(int *)(arg0 + 0x34) = *(int *)(arg0 + 8);
        *(int *)(arg0 + 0x1C) = func_001FE480(D_0028A4EC, *(int *)(arg0 + 0x34));
        spB0 = idxblk.w;
        s4 = spB0;
        s7 = sbuf;
        fp = cbuf;
        adv = 4;
        spA0 = 0;
        s3 = s7;
        s5 = fp;
        do {
            while (adv != 0) {
                int *e = func_001FE4D0((int)D_0028A4EC, *(int *)(arg0 + 0x34), *(int *)(arg0 + 0x44));
                if (e != 0 && e[2] == *(int *)(arg0 + 0x38) + off) {
                    cnt += 1;
                    *(int **)((char *)s3 + s4[0] * 4) = e;
                    *(int *)(arg0 + 0x44) = *(int *)(arg0 + 0x44) + 1;
                }
                {
                    unsigned char st = *(unsigned char *)(*(int *)(arg0 + 0x1C) + off);
                    if (st == 0xA || st == 0xC || st == 0) {
                        switch (st) {
                        case 0xA:
                            off += 1;
                            adv -= 1;
                            break;
                        case 0xC:
                            off += 1;
                        case 0:
                            adv = 0;
                            break;
                        }
                        break;
                    }
                    off += 1;
                    s5[s4[0]] = st;
                    s4[0] += 1;
                }
            }
            s3 = (int *)((char *)s3 + 0x100);
            s4 += 1;
            spA0 += 1;
            s5 += 0x40;
        } while (spA0 < 4);
        *(int *)(arg0 + 0x44) = *(int *)(arg0 + 0x44) - cnt;
        *(int *)(arg0 + 0x24) = 0x37;
        *(int *)(arg0 + 0x28) = 0x1C;
        *(char **)(arg0 + 0x20) = &D_00264C90;
        spA0 = 0;
        do {
            *(char **)(arg0 + 0x2C) = fp;
            adv = func_001FDDB0(s7, *spB0, arg0);
            switch (adv) {
            case 0:
                return;
            case 1:
                break;
            case 2:
                return;
            }
            spA0 += 1;
            *(int *)(arg0 + 0x28) = *(int *)(arg0 + 0x28) + ((D_00264C98 + D_00264CA0) >> 1);
            fp += 0x40;
            spB0 += 1;
            s7 = (int *)((char *)s7 + 0x100);
        } while (spA0 < 4);
        *(int *)(arg0 + 0x34) = *(int *)(arg0 + 8) + 1;
        func_001FE070(D_0028A4EC, *(int *)(arg0 + 0x34), 0x39, 0x95);
        *(int *)(arg0 + 0x10) = 1;
        return;
    case 2:
        return;
    }
}
