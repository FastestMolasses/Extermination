// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 82.04% NEARMISS).
// Fixes read from the original instructions:
//  - The type dispatch is a `switch (p[0xD])` with case 0 written before
//    case 1 (mwcc emits the compare chain in reverse), and the bone-default
//    index n is a `short` local (sign-extended in each arm).
//  - -sdatathreshold 4 (as in the byte-matched twin func_0018A8D0): the short
//    D_00275BCC is then gp-relative, while D_0028A56C/D_0028A574 are declared
//    as 2-word arrays so they stay absolute.
//  - bone_init_default_2 takes a `short` second parameter (its definition in
//    src/), so n is passed without a re-sign-extension.
//  - The "register coloring swap" was a consequence of the above, not a wall.
//
// Bone-system init for an actor (p). Picks a clip id + bone-default index from
// the actor type byte at p+0xD (type 1 -> n=0xC,id=0x47; type 0 -> n=0xE,id=0x46),
// resolves the clip via func_001C6120(D_0028A56C, id) and binds it with
// func_001CA6E0. Stores the bone count func_001C6150(p+0x44) at p+0xC; if it
// exceeds the global cap D_00275BCC, returns 1 (failure). Otherwise allocates a
// per-bone slot func_001AF780() into the p+0x110[] array for each bone, mirrors
// the count to p+9, runs anim_bone_array_setup(count) (it ignores the argument,
// but the original call site materialises it), records the default-set pointer
// D_0028A574 at p+0x40, applies bone_init_default_2(p, n), returns 0.

extern short D_00275BCC;
extern char *D_0028A56C[2];
extern int D_0028A574[2];
extern char *func_001C6120();
extern unsigned char func_001C6150();
extern void func_001CA6E0();
extern int func_001AF780();
extern void anim_bone_array_setup();
extern void bone_init_default_2(unsigned char *, short);

int func_00188430(unsigned char *p)
{
    short n;
    int i;
    unsigned char *e;
    int id;

    switch (p[0xD]) {
    case 0:
        n = 0xE;
        id = 0x46;
        break;
    case 1:
        n = 0xC;
        id = 0x47;
        break;
    }
    func_001CA6E0(p, func_001C6120(D_0028A56C[0], id));
    p[0xC] = func_001C6150(*(int *)(p + 0x44));
    if (D_00275BCC < (int)p[0xC]) {
        return 1;
    }
    i = 0;
    e = p;
    while (i < (int)p[0xC]) {
        *(int *)(e + 0x110) = func_001AF780();
        e += 4;
        i++;
    }
    p[9] = p[0xC];
    anim_bone_array_setup(p[0xC]);
    *(int *)(p + 0x40) = D_0028A574[0];
    bone_init_default_2(p, n);
    return 0;
}
