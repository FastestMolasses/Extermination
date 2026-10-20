// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Channel 3 model submit. Keeps the channel's current list position (first),
// writes a tag there (id 0x10, addr 0, qwc 0xE) followed by 0xE0 bytes copied
// from D_002513D0, then vif_append_ref_tag(3, D_00237450), a ref tag (id 0x30,
// qwc 8) for D_00816A40 + frame * 128 and func_001D1F80(3, 2, 1). The model's
// vertex data at model + 0x40 goes out in ref tags of at most 0x1F8 vertices
// (qwc = n * 0x82, 0x820 bytes per vertex), then an end tag (id 0x60). Finally
// func_001CAAC0({model[0x34..0x3C], 1.0}, first).
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 4, 2026-10-20). The
// 81.12 NEARMISS called block_copy, func_001D1F80 and func_001CAAC0 with invented
// fourth / third arguments. Levers: the model pointer is a cast local declared
// first (s0), the chunk qwc is stored before the run pointer advances, and the
// cursor advance reads the cursor twice (dst = *pp + 1; *pp = *pp + 0xF).

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
typedef struct {
    float x, y, z, w;
} Vec4;
extern VifLists *D_00275670;
extern char D_00237450[];
extern char D_002513D0[];
extern char D_00816A40[];
extern void *block_copy(void *dst, const void *src, unsigned int len);
extern void vif_append_ref_tag(int ch, char *addr);
extern void func_001D1F80(int ch, int a, int b);
extern int func_001CAAC0(const Vec4 *world_pos, void *payload);

void func_001D4FC0(int param) {
    int *arg0 = (int *)param;
    char *run;
    DmaTag *first;
    DmaTag **pp;
    DmaTag *tmp;
    char *addr;
    int n;
    int chunk;
    Vec4 v;

    pp = &D_00275670->cur[3];
    run = (char *)arg0 + 0x40;
    first = *pp;
    first->id = 0x10;
    (*pp)->addr = 0;
    (*pp)->qwc = 0xE;
    {
        DmaTag *dst = *pp + 1;
        *pp = *pp + 0xF;
        block_copy(dst, D_002513D0, 0xE0);
    }
    vif_append_ref_tag(3, D_00237450);
    addr = D_00816A40 + D_00275670->frame * 128;
    pp = &D_00275670->cur[3];
    (*pp)->id = 0x30;
    (*pp)->addr = (int)addr;
    (*pp)->qwc = 8;
    (*pp)++;
    func_001D1F80(3, 2, 1);
    n = arg0[0];
    while (n > 0) {
        if (n < 0x1F8) {
            chunk = n;
        } else {
            chunk = 0x1F8;
        }
        n -= 0x1F8;
        pp = &D_00275670->cur[3];
        (*pp)->id = 0x30;
        (*pp)->addr = (int)run;
        (*pp)->qwc = chunk * 0x82;
        run += chunk * 0x820;
        (*pp)++;
    }
    pp = &D_00275670->cur[3];
    (*pp)->id = 0x60;
    (*pp)->addr = 0;
    (*pp)->qwc = 0;
    (*pp)++;
    v.x = *(float *)((char *)arg0 + 0x34);
    v.y = *(float *)((char *)arg0 + 0x38);
    v.z = *(float *)((char *)arg0 + 0x3C);
    v.w = 1.0f;
    func_001CAAC0(&v, first);
}
