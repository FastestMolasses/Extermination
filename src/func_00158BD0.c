// NEARMISS func_00158BD0  (vram 0x00158BD0, 0x154 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.62% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// two regalloc/scheduling permutations: (1) dispatch save-sink — target keeps switch byte in a1 and sinks p->s0 save into the first branch delay slot (case 0 is p-dead); mwcc loads byte into a0 and saves p->s0 eagerly. (2) mask-test block register coloring permuted (a1/a0/v1/v0 vs v1/v0/a1/a0). Cle...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Dispatch on p->byte[4]: case 0 -> func_001B0FD0(); case 1 -> func_001C6380()
// then optional vtable call (p+0x4C) if func_001B17A0(p), then build a scale
// matrix in D_700038B0/C0 (flag bit from D_00810841[D_00810700] vs p->halfword
// 0x2E selects which axis gets 0x80) and func_001F4A10(p+0xD0,...); cases 2/3
// -> func_001AFC10(p).
extern void func_001AFC10(char *p);
extern void func_001B0FD0(void);
extern int func_001B17A0(char *p);
extern void func_001C6380(void);
extern void func_001F4A10(void *a, void *b, void *c);
extern unsigned char D_00810700;
extern unsigned char D_00810841[];
extern char D_700038B0[];
extern char D_700038C0[];

void func_00158BD0(char *p) {
    int t;

    t = *(unsigned char *)(p + 4);
    switch (t) {
    case 0:
        func_001B0FD0();
        return;
    case 1:
        func_001C6380();
        if (func_001B17A0(p) != 0) {
            (*(void (**)(char *))(p + 0x4C))(p);
        }
        if (D_00810841[D_00810700] & (1 << *(unsigned short *)(p + 0x2E))) {
            *(int *)0x700038B0 = 0;
            *(int *)0x700038B4 = 0x80;
            *(int *)0x700038B8 = 0;
            *(int *)0x700038BC = 0x80;
        } else {
            *(int *)0x700038B0 = 0x80;
            *(int *)0x700038B4 = 0;
            *(int *)0x700038B8 = 0;
            *(int *)0x700038BC = 0x80;
        }
        *(int *)0x700038C0 = 0x3F800000;
        *(int *)0x700038C4 = 0x3F800000;
        *(int *)0x700038C8 = 0x3F800000;
        *(int *)0x700038CC = 0x3F800000;
        func_001F4A10(p + 0xD0, D_700038B0, D_700038C0);
        return;
    case 2:
    case 3:
        func_001AFC10(p);
        return;
    }
}
