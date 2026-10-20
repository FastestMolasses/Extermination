// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Channel 3 model submit (sibling of func_001D4FC0 without the 0xE0-byte
// header block): keeps the channel's list position (first), runs
// vif_build_unpack_const(3) and vif_append_ref_tag(3, D_00237720), a ref tag
// (id 0x30, qwc 8) for D_00816A40 + frame * 128 and func_001D1F80(3, 2, 1);
// then the model's vertex data at model + 0x40 in ref tags of at most 0x1F8
// vertices (qwc = n * 0x82), an end tag (id 0x60), and
// func_001CAAC0({model[0x34..0x3C], 1.0}, first).
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 4, 2026-10-20). The
// 78.6 NEARMISS passed invented arguments to func_001D1F80 / func_001CAAC0;
// the register order needs the model pointer as a cast local declared first.

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
extern char D_00237720[];
extern char D_00816A40[];
extern void vif_build_unpack_const(int ch);
extern void vif_append_ref_tag(int ch, char *addr);
extern void func_001D1F80(int ch, int a, int b);
extern int func_001CAAC0(const Vec4 *world_pos, void *payload);

void func_001D5170(int param) {
    int *arg0 = (int *)param;
    char *run;
    DmaTag *first;
    DmaTag **pp;
    char *addr;
    int n;
    int chunk;
    Vec4 v;

    first = D_00275670->cur[3];
    run = (char *)arg0 + 0x40;
    vif_build_unpack_const(3);
    vif_append_ref_tag(3, D_00237720);
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
