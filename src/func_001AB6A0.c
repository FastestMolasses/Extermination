// NEARMISS func_001AB6A0  (vram 0x001AB6A0, 0xA0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.38% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// CW-vs-mwcc branch lowering + delay-slot address hoist. Two residuals: (1) mwcc fills the `beq state==2` delay slot with the `lui $at,0x7000` rematerialization of the volatile 0x70003B6C reload, CW leaves nop there; (2) CW emits `beq state,4,promote; nop; b next; nop` (explicit dead-b) while mwcc ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern char D_0028A750[];
extern char D_0028A7B0[];

void func_001AB6A0(void) {
    char *slot;
    unsigned char state;
    *(char * volatile *)0x70003B6C = D_0028A750;
    do {
        slot = *(char * volatile *)0x70003B6C;
        state = *(unsigned char *)slot;
        if (state == 2) goto run;
        if (state == 1) goto promote;
        if (state == 4) goto promote;
        goto next;
    promote:
        *(unsigned char *)slot = 2;
    run:
        slot = *(char * volatile *)0x70003B6C;
        (*(void (**)(void))(slot + 4))();
    next:
        slot = *(char * volatile *)0x70003B6C;
        slot += 0x20;
        *(char * volatile *)0x70003B6C = slot;
    } while (slot < D_0028A7B0);
}
