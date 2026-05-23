// Returns 1 if byte[2] masked with 0x1F has value 4 AND byte[3] == 2, else 0
int func_0017D040(unsigned char *a0) {
    int b2 = a0[2] & 0x1F;
    if (b2 != 4)
        return 0;
    if (a0[3] != 2)
        return 0;
    return 1;
}
