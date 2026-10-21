// NEARMISS func_00158D30  (vram 0x00158D30, 0x184 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.65% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// Corrected body (func_001B0FD0(p), per-branch 0x700038BC store). Left: idiom-32, the scratchpad upper-half lui that mwcc fills into branch delay slots where the target leaves nop; needs per-address extern symbols plus build.py _SPAD_SYMS.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// CORRECTED (decomp lane round 5, 2026-10-20) against the original instructions:
// func_001B0FD0 takes the entity (the original calls it with its own a0 untouched;
// the old C passed nothing), and in state 1 / mode 0 each branch stores its own
// 0x700038BC = 0x80 (the target has the store in both arms).

extern void func_001AFC10(char *p);
extern int func_001B0FD0(char *);
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
        func_001B0FD0(p);
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
                *(int *)0x700038BC = 0x80;
            } else {
                *(int *)0x700038B0 = 0;
                *(int *)0x700038B4 = 0x80;
                *(int *)0x700038B8 = 0;
                *(int *)0x700038BC = 0x80;
            }
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
