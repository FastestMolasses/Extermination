// CFLAGS: -O4,p -sdatathreshold 0
void func_002041A0(unsigned long long *out, unsigned int a, unsigned int b, unsigned int c) {
    *out = ((unsigned long long)a << 32) | ((unsigned long long)b << 28) | (unsigned long long)c;
}
