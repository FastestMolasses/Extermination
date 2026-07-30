// NEARMISS func_00118CF8  (vram 0x00118CF8, 0x168 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 71.91% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc induction-variable-selection wall — target keeps base (s1) + byte-offset iv (s0) + combined pointer iv (s3) and recomputes `addu v0,s1,s0`; our ee-gcc collapses to one pointer iv, and with the freed register also hoists the loop-invariant literals 1 and 44100 into s6/s5 instead of remateria...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

//
// NEARMISS 71.91 — eegcc induction-variable-selection + loop-invariant-hoist wall.
// Logic fully recovered and verified instruction-by-instruction: the 0x30-entry /
// 0x6A-stride voice-table scan, all five annulled `bnel` continue edges, the CSE
// of the compared constant 1 into the fifth compare, the 5-argument (a0-a3 + t0)
// call to func_00117918, the gcc div-by-zero `beql/break 7` guard, the r5900
// 3-operand `mult`, `div/mflo` and the trailing `ev->8 += 2` all reproduce, and
// the frame (0x80, s0-s6 saved) matches exactly.
// Residual is iv selection: the target keeps THREE loop registers for the table —
// s1 = base, s0 = byte offset (+0x6A), s3 = combined pointer (+0x6A) — and
// recomputes `addu v0,s1,s0` per iteration, while our ee-gcc collapses them into
// a single pointer iv. With a register thus freed it also hoists the loop-invariant
// literals 1 and 44100 into s6/s5, whereas the target rematerialises them inside
// the loop (`addiu v1,zero,1`, `ori v1,zero,0xac44`). Tried: pointer local
// (`ch = base + i*0x6A`, 68.48%), fully inline `D_0027CCC0 + i*0x6A + off`
// (70.58%, degrades to a runtime `mult`), separate `idx` biv in the for-update
// (71.91%), `idx = i*0x6A` in the body (71.91%), and an explicit dual base+pointer
// pair mirroring the target (68.58%).
// NOTE: the divisor MUST stay a variable (`rate`) — the target's `beql/break 7`
// zero-check proves ee-gcc did not see a literal divisor (a literal 48000 would
// have been strength-reduced to a magic multiply).

extern unsigned char D_00281AC0[];
extern unsigned char D_0027CCC0[];

extern int func_00117918(int a0, int a1, int a2, int a3, int a4);
extern int func_001157F0(int a0, int a1, int a2, int a3);

void func_00118CF8(unsigned char *ev)
{
    unsigned char *snd = D_00281AC0;
    unsigned char *tbl = D_0027CCC0;
    int rate = 48000;
    int idx;
    int i;
    int v;

    *(unsigned char *)(*(int *)(snd + 0xC) + 0xA) =
        *(unsigned char *)(*(int *)(ev + 8) + *(int *)(snd + 0x14) + 1);

    for (i = 0; i < 0x30; i++) {
        idx = i * 0x6A;
        if (*(unsigned short *)(tbl + idx + 4) != (*(unsigned char *)ev & 0xF))
            continue;
        if (*(unsigned short *)(tbl + idx + 0x22) != *(unsigned short *)(ev + 0x24))
            continue;
        if (*(unsigned short *)(tbl + idx + 6) != *(int *)(ev + 0x18))
            continue;
        if (*(unsigned short *)(tbl + idx + 0x1A) != 1)
            continue;
        if (*(unsigned short *)(tbl + idx + 0) != 1)
            continue;

        *(unsigned short *)(tbl + idx + 0x38) =
            *(unsigned char *)(*(int *)(ev + 8) + *(int *)(snd + 0x14) + 1);

        v = func_00117918(*(unsigned short *)(tbl + idx + 0x40),
                          *(unsigned short *)(tbl + idx + 2),
                          *(short *)(tbl + idx + 0x36),
                          *(unsigned char *)(*(int *)(snd + 0xC) + 0xA),
                          *(unsigned short *)(tbl + idx + 0x3A));

        func_001157F0(6, i, v * 44100 / rate, 0);
    }

    *(int *)(ev + 8) = *(int *)(ev + 8) + 2;
}
