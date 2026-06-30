// NEARMISS func_001AA840  (vram 0x001AA840, 0x84 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 84.27% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation wall: live-across-call values in caller-saved temps vs mwcc's callee-saved. After dropping the unused arg0 from the func_001AA7A0 call (call site emits NO a0 setup in the target, confirming arg0 is not passed), the instruction sequence, scheduling, nops, and control flow are I...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

extern int func_001AA7A0();
extern char **D_00275B7C;
extern short D_00275B84;

void func_001AA840(void) {
    char **p;
    int count;
    char *e;

    count = D_00275B84;
    if (count != 0) {
        p = D_00275B7C;
        if (count != 0) {
            do {
                e = *p;
                count -= 1;
                p += 1;
                if ((e != 0) &&
                    ((*(unsigned char *)(e + 2) & 0x1F) == 4) &&
                    (*(unsigned char *)(e + 3) == 0x29) &&
                    (*(unsigned char *)(e + 0) == 1)) {
                    if (func_001AA7A0(e) != 0) {
                        return;
                    }
                }
            } while (count != 0);
        }
    }
}
