// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Animation-clip change arbiter: runs anim_clip_init(p, v) for side effects,
// then compares the requested clip id (sign-extended low 16 bits of v) against
// the currently-stored id at p+0x20C. If they differ it commits the new id
// (sh) and returns 1; if unchanged it returns 0. The explicit
// if(ne){store;return 1;}else{return 0;} shape reproduces CW's branch layout:
// beq skips the store, the store sits in the unconditional `b` delay slot, and
// a dead `paddub v0,zero` trails the branch. The pinned 991202 build inverts
// to bne and merges that dead return block (walled at 87.6%); mwcc 2.3.3
// (mwcps2-2.3.3-000906) emits the dead block and is byte-identical. Verified
// objdiff 100% vs build/expected/anim_clip_arbiter.o.
extern void anim_clip_init(int, int);

int anim_clip_arbiter(char *p, int v) {
    anim_clip_init((int)p, v);
    if ((short)v != *(short *)(p + 0x20C)) {
        *(short *)(p + 0x20C) = (short)v;
        return 1;
    } else {
        return 0;
    }
}
