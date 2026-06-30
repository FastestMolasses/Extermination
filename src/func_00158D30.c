// NEARMISS func_00158D30  (vram 0x00158D30, 0x184 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 86.86% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation coloring on the outer 4-way dispatch (dispatch-save-sink): the target keeps the p->byte[4] switch selector in a1 and sinks the p->s0 save into the case-3 beq delay slot (with case-2 emitted as a branch-likely beql whose slot sets a0=p); mwcc 2.3.3 loads the selector into a0 an...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001AFC10(char *p);
extern void func_001B0FD0(void);
extern int func_001B17A0(char *p);
extern void func_001C6380(char *p);
extern void func_001F4A10(void *a, void *b, void *c);
extern char D_700038B0[];
extern char D_700038C0[];

void func_00158D30(char *p) {
    int t;

    t = *(unsigned char *)(p + 4);
    switch (t) {
    case 0:
        func_001B0FD0();
        *(int *)(p + 0x80) = 0x40000000;
        *(int *)(p + 0x84) = 0x40000000;
        *(int *)(p + 0x88) = 0x40000000;
        func_001C6380(p);
        return;
    case 1:
        switch (*(unsigned short *)(p + 0x2E)) {
        case 0:
            if (*(unsigned char *)(p + 0xB) == 0) {
                *(int *)0x700038B0 = 0x80;
                *(int *)0x700038B4 = 0;
                *(int *)0x700038B8 = 0;
            } else {
                *(int *)0x700038B0 = 0;
                *(int *)0x700038B4 = 0x80;
                *(int *)0x700038B8 = 0;
            }
            *(int *)0x700038BC = 0x80;
            break;
        case 1:
            *(int *)0x700038B0 = 0;
            *(int *)0x700038B4 = 0x80;
            *(int *)0x700038B8 = 0;
            *(int *)0x700038BC = 0x80;
            break;
        }
        *(int *)0x700038C0 = 0x3F800000;
        *(int *)0x700038C4 = 0x3F800000;
        *(int *)0x700038C8 = 0x3F800000;
        *(int *)0x700038CC = 0x3F800000;
        if (func_001B17A0(p) != 0) {
            (*(void (**)(char *))(p + 0x4C))(p);
        }
        func_001F4A10(p + 0xD0, D_700038B0, D_700038C0);
        return;
    case 2:
    case 3:
        func_001AFC10(p);
        return;
    }
}
