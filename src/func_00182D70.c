// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
void func_00182D70(char *a0) {
    int v1;
    char *a1;
    int a2;

    if (!(*(volatile unsigned char *)0x70003B8F)) {
        *(volatile unsigned char *)0x70003B8F = 1;
    }

    *(int *)(a0 + 0x224) = 0;
    *(int *)(a0 + 0x22C) = 0;
    a0[0xF] = 0;
    v1 = 1;
    *(short *)(a0 + 0x20E) = 0;
    a0[0x0] = v1;

    a1 = *(char **)(a0 + 0x1C);
    if (a1) {
        v1 = 2;
        a1[0x4] = v1;
    }

    a2 = *(short *)(a0 + 0x20C);
    v1 = -1;
    *(short *)(a0 + 0x1F2) = a2;
    *(int *)(a0 + 0x1F8) = 0;
    a0[0x2F3] = 0;
    *(int *)(a0 + 0x1F4) = 0x3F800000;
    a0[0x23F] = 0;
    *(int *)(a0 + 0x38) = 0;
    *(int *)(a0 + 0x240) = 0;
    *(int *)(a0 + 0x24C) = v1;
    *(short *)(a0 + 0x276) = 0;
}
