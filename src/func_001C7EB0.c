// NEARMISS func_001C7EB0  (vram 0x001C7EB0, 0x28C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 89.07% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Same class of mwcc saved-register-coloring wall as func_0018CBD0: target assigns s1=tail/s0=loop-index/s3=array-cursor while every C variant tried reproduces the identical instruction stream with s0<->s1 (and the paired jalr temp register) permuted. All logic recovered: 4-state actor-animation up...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// NEARMISS objdiff 89.07% via mwcc233. Logic/structure fully recovered (a 4-state
// actor-animation update: 0=allocate bones + first-frame init, 1=advance the clip
// timer and rebuild the transform from the shared pose D_00275B40, dispatching a
// user callback (tail+0xFC) on either a state-flag reject or a low-scale guard;
// 2/3=free via func_001AFC10). Residual is a pure mwcc register-coloring wall: the
// target assigns s1=tail/s0=loop-index/s3=array-cursor; every source reordering
// tried (decl order, statement order, loop-var type) reproduces the identical
// instruction stream with s0<->s1 (and the paired jalr temp reg) permuted instead
// -- same class of saved-float-register-numbering wall proven on func_0018CBD0.
typedef unsigned __int128 uint128;
extern int func_001AF780(void);
extern void func_001AFC10(unsigned char *actor);
extern void func_001C68C0(unsigned char *o);
extern void anim_bone_array_setup(unsigned char count);
extern void bone_init_default_2(unsigned char *o, short a1);
extern int anim_advance_time(int self, float t);
extern unsigned char **D_00275B40;
extern short D_00275BCC;

void func_001C7EB0(unsigned char *arg0) {
    int i;
    unsigned char *tail;
    unsigned char *cur;
    unsigned char *rec;
    unsigned char st;
    int n;
    int flags;
    void (*cb)(unsigned char *);
    unsigned char *src;

    rec = *(unsigned char **)(arg0 + 0x24);
    tail = arg0 + 0x1F0;
    st = arg0[4];

    switch (st) {
    case 0:
        arg0[0xC] = 1;
        if (D_00275BCC < arg0[0xC]) {
            arg0[4] = 3;
            return;
        }
        cur = arg0;
        i = 0;
        while (i < arg0[0xC]) {
            *(int *)(cur + 0x110) = func_001AF780();
            cur += 4;
            i++;
        }
        arg0[9] = arg0[0xC];
        anim_bone_array_setup(arg0[0xC]);
        cb = *(void (**)(unsigned char *))(tail + 0xFC);
        if (cb != 0) {
            cb(arg0);
        }
        bone_init_default_2(arg0, *(short *)(arg0 + 0x94));
        *(int *)(tail + 0xF4) = 1;
        arg0[4] = 1;
        return;

    case 1:
        if (rec != 0 && (*(unsigned short *)(rec + 0x2E) & (1 << *(unsigned short *)(arg0 + 0x2E)))) {
            arg0[4] = 3;
            return;
        }
        *(int *)(tail + 0xF4) = *(int *)(tail + 0xF4) - 1;
        if (*(int *)(tail + 0xF4) < 0) {
            n = anim_advance_time((int)arg0, *(float *)(tail + 0xF8));
        }
        *(int *)(arg0 + 0xB0) = 0;
        *(int *)(arg0 + 0xB4) = 0;
        *(int *)(arg0 + 0xB8) = 0;
        *(int *)(arg0 + 0xBC) = 0x3F800000;
        *(int *)(arg0 + 0xC0) = 0;
        *(int *)(arg0 + 0xC4) = 0;
        *(int *)(arg0 + 0xC8) = 0;
        *(int *)(arg0 + 0xCC) = 0x3F800000;
        func_001C68C0(arg0);
        *(float *)(arg0 + 0xB0) = *(float *)(*D_00275B40 + 0xC0);
        *(float *)(arg0 + 0xB4) = *(float *)(*D_00275B40 + 0xC4);
        *(float *)(arg0 + 0xB8) = *(float *)(*D_00275B40 + 0xC8);
        *(int *)(arg0 + 0xBC) = 0x3F800000;
        src = *D_00275B40;
        *(uint128 *)(arg0 + 0xD0) = *(uint128 *)(src + 0x90);
        *(uint128 *)(arg0 + 0xE0) = *(uint128 *)(src + 0xA0);
        *(uint128 *)(arg0 + 0xF0) = *(uint128 *)(src + 0xB0);
        *(uint128 *)(arg0 + 0x100) = *(uint128 *)(src + 0xC0);

        flags = n & 0x3000;
        *(int *)(arg0 + 0x10C) = 0x3F800000;
        if (flags) {
            arg0[4] = 3;
            cb = *(void (**)(unsigned char *))(tail + 0xFC);
            if (cb != 0) {
                cb(arg0);
                return;
            }
        } else {
            src = *D_00275B40;
            if (*(float *)(src + 0x18) < 0.8999999761581421f &&
                *(float *)(src + 0x1C) < 0.8999999761581421f &&
                *(float *)(src + 0x20) < 0.8999999761581421f) {
                return;
            }
            cb = *(void (**)(unsigned char *))(tail + 0xFC);
            if (cb != 0) {
                cb(arg0);
                return;
            }
        }
        return;

    case 2:
    case 3:
        func_001AFC10(arg0);
        return;
    }
}
