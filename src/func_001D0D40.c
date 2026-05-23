void func_001D0D40(int *a0, int a1, int a2, int a3) {
    int *p = (int *)*(int *)((char *)a0 + 0x90);
    p[0] = a1;
    *(float *)&p[1] = (float)a2;
    p[2] = 0;
    *((char *)p + 0xC) = (char)a3;
}
