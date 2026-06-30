// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Predicate: returns 1 if the entity at a0 is "active/displayable", else 0.
// Reads the type/state byte at a0[3]; the types {0xC, 0x2A, 0xA, 0x18, 0x2}
// qualify, as does any entity whose handler/vtable pointer at a0[0x10] equals
// func_00156F30, D_827880, or D_828700. NULL a0 -> 0. The shared 'return 1'
// tail (label ret1) must be emitted BEFORE the 'return 0' tail (ret0): writing
// the final type check as 'if (v1 != 2) goto ret0;' (inverse sense, ret1 the
// fall-through) is what makes mwcc fill the bne delay slot with v0=0 and lay
// out the blocks exactly like the target. mwcc 2.3.3 byte-matches; 991202 does not.
extern void func_00156F30();
extern char D_827880[];
extern char D_828700[];

int func_00175640(char *a0) {
    unsigned char v1;
    int w;

    if (a0 == 0) return 0;
    v1 = *(unsigned char *)(a0 + 3);
    if (v1 == 0xC) goto ret1;
    if (v1 == 0x2A) goto ret1;
    if (v1 == 0xA) goto ret1;
    if (v1 == 0x18) goto ret1;
    w = *(int *)(a0 + 0x10);
    if (w == (int)func_00156F30) goto ret1;
    if (w == (int)D_827880) goto ret1;
    if (w == (int)D_828700) goto ret1;
    if (v1 != 2) goto ret0;
ret1:
    return 1;
ret0:
    return 0;
}
