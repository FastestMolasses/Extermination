// CFLAGS: -O4,p -sdatathreshold 0
extern void build_trs_matrix(void *a, void *b, void *c, void *d);
extern void func_001C9940(void *a, int b, void *c);

void func_001C68C0(char *o) {
    build_trs_matrix(o + 0xD0, o + 0xB0, o + 0xC0, o + 0x60);
    func_001C9940(o + 0x110, *(unsigned char *)(o + 0xC), o + 0xD0);
}
