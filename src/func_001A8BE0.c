// NEARMISS func_001A8BE0  (vram 0x001A8BE0, 0xF4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.59% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Instruction-scheduling / delay-slot-fill artifact: target emits the two init loads (D_00275BA0/D_00275BA8) in opposite order and leaves two branch delay slots as nop where mwcc hoists the counter-reload lui into them. Body/structure/logic fully recovered (94.59% on 233, 88.03% on 991). Not the cl...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

extern void func_001A8660(int, unsigned char *);
extern void func_001A8840(int, unsigned char *);
extern void func_001A8970(int, unsigned char *);
extern unsigned char **D_00275BA0;
extern short D_00275BA8;
extern short D_0028A9A0[16];

void func_001A8BE0(int arg0) {
    unsigned char **p;
    unsigned char *e;

    if (D_0028A9A0[0] == 0 && *(unsigned char *)0x70003B8D == 0) {
        p = D_00275BA0;
        *(short *)0x70003B86 = D_00275BA8;
        while (*(volatile short *)0x70003B86 != 0) {
            e = *p;
            *(short *)0x70003B86 = *(volatile short *)0x70003B86 - 1;
            p += 1;
            if (*(unsigned char *)e & 1) {
                switch (*(unsigned char *)(e + 3)) {
                case 1:
                    func_001A8660(arg0, e);
                    break;
                case 3:
                    func_001A8840(arg0, e);
                    break;
                case 5:
                    func_001A8970(arg0, e);
                    break;
                }
            }
        }
    }
}
