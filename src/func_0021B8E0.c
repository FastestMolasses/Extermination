extern int *D_00275670;
void block_copy(void *dst, void *src, int count);

void func_0021B8E0(void) {
    int *v0 = D_00275670;
    block_copy((char *)v0 + 0xE0, (char *)v0 + 0xA0, 0x20);
}
