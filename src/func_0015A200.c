// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// SEMANTICS (FINDINGS "GENERATOR" / "KIND-0xE COMPANION RESOLVED"):
// generator child spawn helper — allocates a class-2 actor and wires a
// kind-0xD worm (brain func_00153F10) or kind-0xE tendril field (brain
// func_001546C0) off a generator pad: +0x03 = kind byte, +0x0D =
// parent pad kind, +0x2E = pair index, pos +0xB0 copied verbatim from
// the pad origin (vec4 copy func_00102948), +0xC0/C4/C8 zeroed, +0x20 =
// parent's +0x14 canonical pointer. Returns 1 on spawn, 0 if the actor
// pool is full.
extern unsigned char *func_001AFA90(int cls);
extern void func_00102948(unsigned char *dst, unsigned char *src);
extern int func_00153F10();
extern int func_001546C0();

int func_0015A200(unsigned char *parent, int kind, int pairIdx) {
    int cls;
    unsigned char *e;
    cls = 2;
    e = func_001AFA90(cls);
    if (e == 0) goto nospawn;
    e[0x3] = kind;
    e[0xD] = parent[0x54];
    e[0x9A] = 0;
    *(short *)(e + 0x2E) = pairIdx;
    func_00102948(e + 0xB0, parent + 0xB0);
    *(int *)(e + 0xC0) = 0;
    *(int *)(e + 0xC4) = 0;
    *(int *)(e + 0xC8) = 0;
    if (kind == 0xD) *(int (**)())(e + 0x10) = func_00153F10;
    else             *(int (**)())(e + 0x10) = func_001546C0;
    *(int *)(e + 0x20) = *(int *)(parent + 0x14);
    return 1;
nospawn:
    return 0;
}
