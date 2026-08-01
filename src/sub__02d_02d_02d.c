// NEARMISS sub__02d_02d_02d  (vram 0x02d_02d_02d, 0x288 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.77% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// TWO instructions, one store-sink: the `sb v0,%gp_rel(D_00275C94)(gp)` for `D_00275C94 = 0x9E` is emitted by CodeWarrior immediately after its `addiu v0,zero,0x9e` (which mwcc DOES place correctly), but mwcc sinks the store past all seven call-argument setups to just before the jal. This is idiom-...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Save-slot list renderer: draws the five memory-card slot rows of the
// load/save screen. arg0 is the browser context populated by
// sub_BASCUS_97112_DS00_00_EX_DATA_00_3 -- arg0[0x6C + n] is slot n's status
// (0 = empty, 1 = a valid save, 2 = unusable/corrupt) and arg0 + 12*n holds
// slot n's 12-byte record: +0x74 area id, +0x75 sub-area index, +0x7A a
// "modifier" flag and +0x7C the play time in 1/60 s ticks.
//
// For each slot the row baseline steps by 12: the slot number is formatted
// with the sprintf-like func_00122D10 into the gp-relative text scratch buffer
// D_00275C94 using format D_00273998 and drawn at x = 0x724, and the row body
// is drawn at x = 0x42 (func_001FCBD0 label) / 0x88C (play time). Then:
//   status 0 -> func_001FCBD0(..., string 7, variant 0, colour 0x70808080)
//   status 2 -> the same label with variant 1
//   status 1 -> resolve the location name: D_00289B40 is a 4-byte-stride table
//               of per-area first-string indices, so the string is
//               D_002671C0[sub_area + D_00289B40[area * 4]]; it is drawn with
//               func_001FC770, with the alternate glyph table D_002671A0 when
//               the modifier flag is set. The play time is then split into
//               hours / minutes / seconds (216000, 3600 and 60 ticks) and
//               formatted with D_002739A0, and when the modifier flag is set a
//               one-character string (glyph 0x9E) is drawn at x = 0x8D2 with
//               the alternate palette D_00275898.
extern void func_00122D10();
extern void func_001CC1E0(int a0, int a1, int a2, int a3, int t0, void *t1, void *t2);
extern void func_001FC770(int a0, int a1, int s, void *tbl);
extern void func_001FCBD0(int a0, int a1, int a2, int a3, unsigned int col);

extern char D_00273998[];
extern char D_002739A0[];
extern char D_002671A0[];
extern int D_002671C0[];
extern short D_00289B40[];
extern unsigned char D_00275898;
extern unsigned char D_00275C94;
extern unsigned char D_00275C95;

void sub__02d_02d_02d(unsigned char *arg0) {
    int i;
    int row1;
    int row2;
    unsigned char *q;
    int t;
    int s;
    int hh;
    int mm;
    int r1;
    int r2;

    row1 = 0x3D;
    row2 = 0x3C;
    i = 0;
    q = arg0;
    do {
        func_00122D10(&D_00275C94, D_00273998, i + 1);
        func_001CC1E0(1, 0x724, row1 + 0x790, 0xA, 0x14, &D_00275C94, 0);
        switch (arg0[i + 0x6C]) {
        case 0:
            func_001FCBD0(0x42, row2, 7, 0, 0x70808080);
            break;
        case 1:
            s = D_002671C0[q[0x75] + *(short *)((char *)D_00289B40 + q[0x74] * 4)];
            if (q[0x7A] == 0) {
                func_001FC770(0x42, row2, s, 0);
            } else {
                func_001FC770(0x42, row2, s, D_002671A0);
            }
            t = *(int *)(q + 0x7C);
            hh = t / 216000;
            r1 = t - hh * 216000;
            mm = r1 / 3600;
            r2 = r1 - mm * 3600;
            func_00122D10(&D_00275C94, D_002739A0, hh, mm, r2 / 60);
            func_001CC1E0(1, 0x88C, row2 + 0x790, 0xA, 0x14, &D_00275C94, 0);
            if (q[0x7A] != 0) {
                D_00275C94 = 0x9E;
                D_00275C95 = 0;
                func_001CC1E0(1, 0x8D2, row2 + 0x790, 0xA, 0x14, &D_00275C94, &D_00275898);
            }
            break;
        case 2:
            func_001FCBD0(0x42, row2, 7, 1, 0x70808080);
            break;
        }
        i += 1;
        row1 += 0xC;
        row2 += 0xC;
        q += 0xC;
    } while (i < 5);
}
