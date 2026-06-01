// CFLAGS: -O4,p -sdatathreshold 4
extern char *D_00275670;
extern char D_00816640[0x10000];
extern char D_00237180[8];
extern void vif_build_unpack_const(int);
extern void vif_append_ref_tag(int, char *);

void func_001D4E20(int a0) {
    char *a2;
    char *v1;
    char *a3;
    int t0;
    int a1;
    int s0;

    s0 = a0;
    vif_build_unpack_const(0);
    vif_append_ref_tag(s0, D_00237180);

    t0 = (int)D_00275670;
    a2 = (char *)(t0 + s0 * 4);
    a3 = D_00816640;
    a1 = 0x30;
    t0 = *(int *)((char *)D_00275670 + 0x9c);
    v1 = *(char **)(a2 + 0x10);
    a0 = 8;
    t0 = t0 << 7;
    v1[3] = a1;
    v1 = *(char **)(a2 + 0x10);
    a1 = (int)(a3 + t0);
    *(int *)(v1 + 4) = a1;
    v1 = *(char **)(a2 + 0x10);
    *(short *)v1 = a0;
    v1 = *(char **)(a2 + 0x10);
    v1 = v1 + 0x10;
    *(char **)(a2 + 0x10) = v1;
}
