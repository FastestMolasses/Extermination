// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Puts every bone of an actor in its model's rest pose. The model (+0x44)
// holds 0x50-byte rest records at model + model[+0xC]; for each of the
// actor's +0xC bones, the bone node (actor + 0x110)[i] gets the rest
// record's +4 halfword at +0x64, unit scale 0x1000 at +0x88/+0x8A/+0x8C,
// zero rotation / translation words at +0x70..+0x84, and the record's four
// quadwords +0x10..+0x4F (its rest matrix) at +0x00..+0x3F.
typedef unsigned int u128 __attribute__((mode(TI)));

void bone_init_default_1(char *actor) {
    int i;
    char *rest;
    char *p;
    char *node;
    char *model = *(char **)(actor + 0x44);

    i = 0;
    p = actor;
    rest = model + *(int *)(model + 0xC);
    while (i < *(unsigned char *)(actor + 0xC)) {
        i++;
        *(short *)(*(char **)(p + 0x110) + 0x64) = *(short *)(rest + 4);
        *(short *)(*(char **)(p + 0x110) + 0x88) = 0x1000;
        *(short *)(*(char **)(p + 0x110) + 0x8A) = 0x1000;
        *(short *)(*(char **)(p + 0x110) + 0x8C) = 0x1000;
        *(int *)(*(char **)(p + 0x110) + 0x7C) = 0;
        *(int *)(*(char **)(p + 0x110) + 0x80) = 0;
        *(int *)(*(char **)(p + 0x110) + 0x84) = 0;
        *(int *)(*(char **)(p + 0x110) + 0x70) = 0;
        *(int *)(*(char **)(p + 0x110) + 0x74) = 0;
        *(int *)(*(char **)(p + 0x110) + 0x78) = 0;
        node = *(char **)(p + 0x110);
        p += 4;
        ((u128 *)node)[0] = ((u128 *)rest)[1];
        ((u128 *)node)[1] = ((u128 *)rest)[2];
        ((u128 *)node)[2] = ((u128 *)rest)[3];
        ((u128 *)node)[3] = ((u128 *)rest)[4];
        rest += 0x50;
    }
}
