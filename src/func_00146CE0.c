// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Enemy/actor state-machine tick: advances a 0x34 countdown by a sign-extended
// per-frame delta (derived from flags at 0x36), fires sub-behaviors, and on
// expiry transitions the actor's action bytes (offsets 0,4,5,6) and timers.
int func_001B4CF0(char *);
int func_001EFE00(int, char *);
int func_0021C040(void *, char *);
extern char D_008102B0;
extern unsigned char D_008106BD;

int func_00146CE0(char *arg0, char *arg1) {
    short temp_a0;
    short temp_v1;
    short temp_v1_2;
    int var_v0;
    int var_s0;

    if ((*(short *)(arg0 + 0x34) == 0) || (*(short *)(arg0 + 0x36) == 0)) {
        return 0;
    }
    if ((*(unsigned char *)(arg1 + 0x72) != 0) || (*(char *)(arg1 + 0x7B) != 0)) {
        *(short *)(arg0 + 0x36) = 0;
        return 0;
    }
    *(char *)(arg0 + 0) = 2;
    *(short *)(arg1 + 0x64) = 0x12C;
    *(unsigned char *)(arg1 + 0x72) = 0xFF;
    temp_v1 = *(short *)(arg0 + 0x36);
    var_s0 = (short)(temp_v1 & 0xFFF);
    if (temp_v1 & 0x8000) {
        var_s0 = (short)(var_s0 * 5);
    }
    if (temp_v1 & 0x4000) {
        var_s0 = (short)(var_s0 * 2);
    }
    if (*(char *)(arg1 + 0x7B) != 0) {
        *(char *)(arg1 + 0x7B) = 0;
        func_0021C040(&D_008102B0, arg0);
        if (D_008106BD == 1) {
            D_008106BD = 0;
        }
    }
    if (*(unsigned char *)(arg1 + 0x73) != 0) {
        *(short *)(arg1 + 0x66) = (short)(*(short *)(arg1 + 0x66) + var_s0);
    } else {
        *(short *)(arg1 + 0x66) = (short)var_s0;
    }
    *(unsigned char *)(arg1 + 0x73) = 0x19;
    if ((*(char *)(arg1 + 0x7D) == 0) && (*(short *)(arg0 + 0x36) & 0x4000)) {
        *(char *)(arg1 + 0x7D) = 0x3C;
        func_001EFE00(0x80000027, arg0);
    }
    temp_v1_2 = *(short *)(arg0 + 0x34);
    if (temp_v1_2 <= (short)var_s0) {
        *(short *)(arg0 + 0x34) = 0;
        *(char *)(arg1 + 0x7C) = 0;
        *(unsigned char *)(arg0 + 4) = 2;
        *(char *)(arg0 + 5) = 2;
        *(char *)(arg0 + 6) = 0;
        func_001B4CF0(arg0);
        if (*(char *)(arg1 + 0x74) & 2) {
            if (*(short *)(arg0 + 0x36) & 0xA000) {
                goto block_24;
            }
        } else {
block_24:
            *(char *)(arg0 + 5) = 1;
        }
        return 1;
    }
    *(short *)(arg0 + 0x34) = (short)(temp_v1_2 - var_s0);
    temp_a0 = *(short *)(arg0 + 0x36);
    if (temp_a0 & 0x2000) {
        *(unsigned char *)(arg0 + 4) = 2;
        *(char *)(arg0 + 5) = 1;
        *(char *)(arg0 + 6) = 0;
        *(short *)(arg1 + 0x66) = 0;
        return 1;
    }
    var_v0 = temp_a0 & 0x8000;
    if (*(unsigned char *)(arg0 + 4) == 1) {
        if (*(short *)(arg1 + 0x66) < 0x19) {
            var_v0 = temp_a0 & 0x8000;
            goto block_32;
        }
        goto block_34;
    }
block_32:
    if ((temp_a0 & 0x8000) != 0) {
block_34:
        *(unsigned char *)(arg0 + 4) = 2;
        *(char *)(arg0 + 6) = 0;
        *(short *)(arg1 + 0x66) = 0;
        if (*(char *)(arg1 + 0x74) & 2) {
            *(char *)(arg0 + 5) = 0;
        } else {
            *(char *)(arg0 + 5) = 1;
        }
        return 1;
    }
    if (temp_a0 & 0x5000) {
        *(unsigned char *)(arg1 + 0x72) = 0x1E;
    } else {
        *(unsigned char *)(arg1 + 0x72) = 0;
    }
    *(char *)(arg0 + 0) = 1;
    *(short *)(arg0 + 0x36) = 0;
    return 0;
}
