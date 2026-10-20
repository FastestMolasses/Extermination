// NEARMISS func_001D71A0  (vram 0x001D71A0, 0x4C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 85.79% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// Register coloring: the target keeps the list pointer and slot address in a3, ch*4 in v1 and the tag id in a0; mwcc 2.3.3 uses t0, a0 and a3, and moves the OR of 0x14000000 two slots later. Declaration order and pointer spellings do not move it.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Appends a 2-qword tag to DMA list ch: an id 0x10 tag (addr 0, qwc 1)
// followed by a zeroed qword whose last word is val | 0x14000000; the cursor
// advances by 0x20.

typedef struct {
    unsigned short qwc;
    unsigned char pad;
    unsigned char id;
    int addr;
    int w2;
    int w3;
} DmaTag;
typedef struct {
    char pad[0x10];
    DmaTag *cur[16];
    int ref[16];
    int pad90[3];
    int frame;
} VifLists;
extern VifLists *D_00275670;
extern int D_00275674;
typedef unsigned int u128 __attribute__((mode(TI)));

void func_001D71A0(int ch, int val) {
    DmaTag **pp = (DmaTag **)((ch << 2) + (char *)D_00275670 + 0x10);
    int v = val | 0x14000000;
    DmaTag *t;

    (*pp)->id = 0x10;
    (*pp)->addr = 0;
    (*pp)->qwc = 1;
    t = *pp;
    *pp = t + 2;
    *(u128 *)(t + 1) = 0;
    t[1].w3 = v;
}
