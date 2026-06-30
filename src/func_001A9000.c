// NEARMISS func_001A9000  (vram 0x001A9000, 0x1B8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.55% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// CW-vs-mwcc branch-delay-slot fill at the two loop back-edges: the CW target leaves the back-edge bnez delay slots as nop and emits the next-iteration scratchpad-address lui after the branch, but mwcc 2.3.3 hoists that lui INTO the branch delay slot, shifting branch offsets by one instruction. Pur...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// NEARMISS 94.5% (mwcc 2.3.3; 991202 caps 83.9%) — logic fully recovered.
// Sole residual: CW-vs-mwcc branch-delay-slot fill at the two loop back-edges.
// The CW target leaves the back-edge `bnez` delay slots as `nop` and emits the
// next-iteration scratchpad-address materialization (`lui at,0x7000`) after the
// branch; mwcc 2.3.3 instead hoists that `lui` INTO the branch delay slot,
// shifting the branch offsets by one instruction. Every other instruction
// (frame, gp-rel loads, the volatile counter reload+decrement, the e-header
// gate, the o[3] switch dispatch order, both E80 call sites merged) is
// byte-identical. Pure scheduling artifact -> not the clean-store nop;
// documented delay-slot-fill wall.
//
// Two-level scan over two gp-relative object lists. Outer list: pointer array
// D_00275BA0, length D_00275BA8 (mirrored into the scratchpad counter at
// 0x70003B86 which is decremented each step). For each outer entry e whose
// header says e[3]==5, e[0xD]!=0xB and e[0]==1, scan the inner list: pointer
// array D_00275B80, length D_00275B88 (counter at 0x70003B88). For each inner
// entry o with o[0]==1, dispatch on the type byte o[3]:
//   0x1E,0x06              -> func_001A8E80(e, o)
//   0x1F,0x50,0x1C         -> func_001A8E80(e, o) unless (D_00810700==0 && D_00810702==5)
//   0x2A,0x18,0x0C,0x0A    -> func_001A8F40(e, o)
extern void func_001A8E80(int *e, unsigned char *o);
extern void func_001A8F40(int *e, unsigned char *o);
extern int **D_00275BA0;
extern short D_00275BA8;
extern int **D_00275B80;
extern short D_00275B88;
extern unsigned char D_00810700[16];
extern unsigned char D_00810702[16];

void func_001A9000(void) {
    int *e;
    int **outer;
    int **inner;
    unsigned char *o;
    unsigned char t;
    short n;

    n = D_00275BA8;
    *(short *)0x70003B86 = n;
    if (n == 0) {
        return;
    }
    if (D_00275B88 == 0) {
        return;
    }
    outer = D_00275BA0;
    while (*(volatile short *)0x70003B86 != 0) {
        n = *(volatile short *)0x70003B86;
        e = *outer;
        *(volatile short *)0x70003B86 = n - 1;
        outer += 1;
        if (*(unsigned char *)((char *)e + 3) == 5 &&
            *(unsigned char *)((char *)e + 0xD) != 0xB &&
            *(unsigned char *)((char *)e + 0) == 1) {
            inner = D_00275B80;
            *(short *)0x70003B88 = D_00275B88;
            while (*(volatile short *)0x70003B88 != 0) {
                n = *(volatile short *)0x70003B88;
                o = (unsigned char *)*inner;
                *(volatile short *)0x70003B88 = n - 1;
                inner += 1;
                if (o[0] == 1) {
                    t = o[3];
                    switch (t) {
                    case 0xA:
                    case 0xC:
                    case 0x18:
                    case 0x2A:
                        func_001A8F40(e, o);
                        break;
                    case 0x1C:
                    case 0x50:
                    case 0x1F:
                        if (D_00810700[0] != 0) {
                            goto call_e80;
                        }
                        if (D_00810702[0] != 5) {
                            goto call_e80;
                        }
                        break;
                    case 0x6:
                    case 0x1E:
                    call_e80:
                        func_001A8E80(e, o);
                        break;
                    }
                }
            }
        }
    }
}
