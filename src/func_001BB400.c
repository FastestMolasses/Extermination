// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

extern unsigned char **D_00275B40;

int func_001BB400(unsigned char *arg0) {
    unsigned char type;
    unsigned char *p;

    type = arg0[3];
    if (type == 8 || type == 0x16) {
        p = D_00275B40[0];
        *(float *)(p + 0x7C) = *(float *)(p + 0x7C) - 0.2f;
        if (*(float *)(D_00275B40[0] + 0x7C) < -9.0f) {
            return 1;
        }
    } else {
        p = D_00275B40[1];
        *(float *)(p + 0x7C) -= 0.2f;
        p = D_00275B40[2];
        *(float *)(p + 0x7C) += 0.2f;
        type = arg0[3];
        if (type == 0x3E || type == 0x3D) {
            if (*(float *)(D_00275B40[1] + 0x7C) < -13.0f) {
                return 1;
            }
        } else {
            if (*(float *)(D_00275B40[1] + 0x7C) < -9.0f) {
                return 1;
            }
        }
    }
    return 0;
}
