// NEARMISS func_001A97B0  (vram 0x001A97B0, 0x228 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.58% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// jr-table external-dispatch wall (proven s84): lui/addiu %hi/%lo(jtbl_0026DAE0) vs local @65. Second, independent residual: 4 occurrences of `lui $at, 0x7000` speculated by mwcc into a branch delay slot where the target has nop (plus one coupled lw reschedule). The original build clearly emitted t...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// SEMANTICS: pairwise interaction pass between two global object lists.
// D_00275BA0 / D_00275BA8 are the "A" list (pointer array + count) and
// D_00275B90 / D_00275B98 the "B" list.  Both loop counters live in EE
// scratchpad shorts: 0x70003B86 for the outer (A) loop, 0x70003B88 for the
// inner (B) loop -- they are decremented in place, and the loop tests re-read
// them from scratchpad, so a callee that zeroes 0x70003B88 aborts the inner scan
// (func_001A9360 is known to do exactly that).
//
// Outer: skip A entries whose byte 0 != 1.  A qualifies (hit) when the class
// byte A[3] is 3 with A[0xD]==0 and the short at A+0x56 nonzero, or 5
// unconditionally, or 6 with A[0xD]==2.
//
// Inner (only for qualifying A): skip B entries whose byte 0 != 1, and skip when
// (B[2] & ~0xE0) == 0xA.  Then dispatch on the class byte B[3] via
// jtbl_0026DAE0 (20 entries, default = skip):
//   class 1  -- skipped when B[0xD]==3 and (B[5]==9 or D_00810354 < 50.0);
//               otherwise falls through to the interaction.
//   classes 0,2..7,9..11,16..19 -- interaction.
//   classes 8,12..15           -- skipped.
// Interaction picks the handler from the A class: 5 -> func_001A9360,
// 3 -> func_001A96F0, otherwise func_001A9480.

extern void func_001A9360(char *a, char *b);
extern void func_001A9480(char *a, char *b);
extern void func_001A96F0(char *a, char *b);

extern char **D_00275B90;
extern short  D_00275B98;
extern char **D_00275BA0;
extern short  D_00275BA8;
extern float  D_00810354[];

void func_001A97B0(void) {
    char *a;
    char **pa;
    char **pb;
    char *b;
    int hit;
    short n;

    n = D_00275BA8;
    *(short *)0x70003B86 = n;
    if ((n != 0) && (D_00275B98 != 0)) {
        pa = D_00275BA0;
        while (*(short *)0x70003B86 != 0) {
            a = *pa;
            hit = 0;
            (*(short *)0x70003B86)--;
            pa++;
            if (*(unsigned char *)a != 1) {
                continue;
            }
            switch (*(unsigned char *)(a + 3)) {
            case 3:
                if ((*(unsigned char *)(a + 0xD) == 0) && (*(short *)(a + 0x56) != 0)) {
                    hit = 1;
                }
                break;
            case 5:
                hit = 1;
                break;
            case 6:
                if (*(unsigned char *)(a + 0xD) == 2) {
                    hit = 1;
                }
                break;
            }
            if (hit == 0) {
                continue;
            }
            pb = D_00275B90;
            *(short *)0x70003B88 = D_00275B98;
            while (*(short *)0x70003B88 != 0) {
                b = *pb;
                (*(short *)0x70003B88)--;
                pb++;
                if (*(unsigned char *)b != 1) {
                    continue;
                }
                if ((*(unsigned char *)(b + 2) & ~0xE0) == 0xA) {
                    continue;
                }
                switch (*(unsigned char *)(b + 3)) {
                case 1:
                    if (*(unsigned char *)(b + 0xD) == 3) {
                        if (*(unsigned char *)(b + 5) == 9) {
                            break;
                        }
                        if (D_00810354[0] < 50.0f) {
                            break;
                        }
                    }
                    /* fall through */
                case 0:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 9:
                case 10:
                case 11:
                case 16:
                case 17:
                case 18:
                case 19:
                    if (*(unsigned char *)(a + 3) == 5) {
                        func_001A9360(a, b);
                    } else if (*(unsigned char *)(a + 3) == 3) {
                        func_001A96F0(a, b);
                    } else {
                        func_001A9480(a, b);
                    }
                    break;
                case 8:
                case 12:
                case 13:
                case 14:
                case 15:
                    break;
                }
            }
        }
    }
}
