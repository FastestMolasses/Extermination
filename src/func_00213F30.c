// NEARMISS func_00213F30  (vram 0x00213F30, 0xF0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.33% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring permutation: hoisted fill const 0x6D and the loop's 0x18 counter swap between $a1/$a2 (8 arg-mismatch instrs); identical instruction sequence and branch-likely dispatch otherwise. Permuter territory.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS: 99.33% (mwcc 2.3.3); 78.95% (991202). Logic and control flow fully
// recovered (identical instruction sequence incl. the beql/beqzl branch-likely
// switch dispatch). Sole residual is a register-coloring permutation: the target
// keeps the hoisted fill constant 0x6D in $a2 and the loop's running 0x18 byte
// counter in $a1; mwcc swaps the two ($a1<->$a2) -- 8 arg-mismatch instructions,
// no structural difference. Coloring permuter territory, not a clean-store nop.
//
// Clears the count byte at arg0+0x18, then for state byte arg0+0x12 in 0..4 maps
// it to an inclusive id range [lo,hi): {0:[0,0x20),1:[0x20,0x32),2:[0x32,0x48),
// 3:[0x48,0x5D),4:[0x5D,0x6D)} (default keeps the passed-in lo/hi). For each id i
// in the range it appends a byte to arg0+0x50[count++]: the id i itself if the
// gate table D_00810700[i+0x5C3] is set, else the sentinel 0x6D. Finally zeroes
// arg0+0x19.
extern unsigned char D_00810700[];

void func_00213F30(char *arg0, int a1u, int a2u, int lo, int hi) {
    int i;

    *(unsigned char *)(arg0 + 0x18) = 0;
    switch (*(unsigned char *)(arg0 + 0x12)) {
    case 0:
        lo = 0;
        hi = 0x20;
        break;
    case 1:
        lo = 0x20;
        hi = 0x32;
        break;
    case 2:
        lo = 0x32;
        hi = 0x48;
        break;
    case 3:
        lo = 0x48;
        hi = 0x5D;
        break;
    case 4:
        lo = 0x5D;
        hi = 0x6D;
        break;
    }
    for (i = lo; i < hi; i++) {
        int n;
        if (D_00810700[i + 0x5C3] != 0) {
            n = *(unsigned char *)(arg0 + 0x18);
            *(unsigned char *)(arg0 + 0x18) = n + 1;
            *(char *)(arg0 + n + 0x50) = i;
        } else {
            n = *(unsigned char *)(arg0 + 0x18);
            *(unsigned char *)(arg0 + 0x18) = n + 1;
            *(char *)(arg0 + n + 0x50) = 0x6D;
        }
    }
    *(char *)(arg0 + 0x19) = 0;
}
