// COMPILER: eegcc
// CFLAGS: -O2
struct s109C28 {
    char pad[0x40];
    char *p;
};
void func_00109C28(struct s109C28 *a0, long a1) {
    char *v1 = a0->p;
    *(long *)(v1 + 0xF0) = a1;
    *(int *)(v1 + 0xF8) = 1;
}
