extern int *D_00275670;
void block_copy(void *dst, void *src, int count);

void func_0021BAE0(int a0) {
    int *v1 = D_00275670;
    char *v0 = (char *)v1 + (a0 << 5);
    block_copy((char *)v1 + 0xA0, v0 + 0x120, 0x20);
}
