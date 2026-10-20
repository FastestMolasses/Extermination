// NEARMISS func_001D4A90  (vram 0x001D4A90, 0x80 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.91% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// Two shifts swapped: the target forms qwc (n*65*2) before the byte step (n*65*32) from the shared n*65; mwcc 2.3.3 emits the byte step first. Statement order, a temp and shift spellings do not move it.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Streams the data after the 0x40-byte header at src to DMA list ch as ref
// tags (id 0x30): each covers n = min(left, 504) units (qwc n * 130, address
// advanced by n * 2080 bytes), and left -= 504 until it is <= 0; left starts
// at src[0].

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

void func_001D4A90(int ch, int *src) {
    char *data;
    int left;
    int n;
    DmaTag **pp;

    left = src[0];
    data = (char *)src + 0x40;
    while (left > 0) {
        if (left < 504) {
            n = left;
        } else {
            n = 504;
        }
        pp = &D_00275670->cur[ch];
        left -= 504;
        (*pp)->id = 0x30;
        (*pp)->addr = (int)data;
        data += (n * 65) * 32;
        (*pp)->qwc = (n * 65) * 2;
        (*pp)++;
    }
}
