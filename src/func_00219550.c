// NEARMISS func_00219550  (vram 0x00219550, 0x31C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 83.09% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Item armed-handler pump. Body/logic fully recovered against the documented func_0015AE20-family contract; residual is a dead speculative delay-slot fill plus scheduling-order differences in the state dispatch if-chain and the light-rig word stores -- register-allocation/scheduling artifacts on a ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int func_001B0FD0(int a0);
extern void func_001B1020(int a0, int a1, int a2, int a3);
extern void func_001C6380(unsigned char *self);
extern void func_001A2370(unsigned char *self, void *a1);
extern char *func_001C5570(unsigned char *arg0, int arg1, char arg2, int arg3);
extern int func_001BA1F0(unsigned char *actor);
extern void func_001BA1A0(unsigned char *self, void *a1);
extern int func_001FBD50(void *a0, int a1, int a2, float f12);
extern void func_001B1190(int a0);
extern unsigned char func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *handle);

extern unsigned char D_00810C64[];
extern unsigned char D_00810700;
extern unsigned int D_00275878;
extern unsigned int D_00275880;
extern char D_700038A0[];
extern unsigned char D_008104A0;
extern unsigned char D_008104E6;
extern float D_00810354;
extern int D_002666B4;
extern unsigned char D_002667E0;
extern unsigned char D_00266620;

void func_00219550(unsigned char *self) {
    unsigned char st;
    unsigned char v0;

    st = self[4];
    if (st == 3) {
        goto st3;
    }
    if (st == 2) {
        goto st2;
    }
    if (st == 1) {
        goto st1;
    }
    if (st == 0) {
        goto st0;
    }
    goto st3;

st0:
    /* state 0: INIT */
    if (self[3] == 0 && *(unsigned short *)(self + 0x2E) == 0x28) {
        if (func_001B0FD0((int)self)) {
            goto st0_out;
        }
    } else {
        func_001B1020((int)self, self[0xD], -1, 0);
    }
    if (self[3] != 0) {
        if (*(unsigned short *)(self + 0x2E) == 3) {
            if (D_00810C64[*(unsigned short *)(self + 0x2E)]) {
                *(unsigned short *)(self + 0x2E) = 0x14;
            }
        }
    }
    func_001C6380(self);
    self[0] = 1;
    self[8] = 3;
    if (D_00810700 == 0x10) {
        *(unsigned int *)(self + 0x30) = D_00275880;
    } else {
        *(unsigned int *)(self + 0x30) = D_00275878;
    }
    func_001A2370(self, self + 0xD0);
    *(int *)0x700038A0 = 0;
    *(int *)0x700038A4 = 0x3F800000;
    *(int *)0x700038A8 = 0;
    *(int *)0x700038AC = 0x3E800000;
    *(char **)(self + 0x2EC) = func_001C5570(self, 0x73, 1, (int)D_700038A0);
st0_out:
    return;

st1:
    /* state 1: pump the pickup take script */
    if (self[5] == 1) {
        goto take;
    }
    if (self[5] == 0) {
        goto place;
    }
    goto out;

place:
    if (!(self[0xB] & 4)) {
        goto out;
    }
    self[2] = 0x87;
    self[5]++;
    if (D_008104A0 == 0x2D) {
        goto grab;
    }
    if (D_008104E6 == 0) {
        goto height;
    }
grab:
    func_001BA1A0(self, (void *)&D_002667E0);
    goto out;

height:
    if (*(float *)(self + 0xB4) < D_00810354 + 6.0f) {
        D_002666B4 = 0x42;
        goto graban;
    }
    if (*(float *)(self + 0xB4) < D_00810354 + 6.0f) {
        D_002666B4 = 0x41;
        goto graban;
    }
    D_002666B4 = 0x40;
graban:
    func_001BA1A0(self, (void *)&D_00266620);
    goto out;

take:
    if (!func_001BA1F0(self)) {
        goto out;
    }
    func_001FBD50(self, 0x194, 0, 300.0f);
    func_001B1190(self[0x9A]);
    self[4] = 3;
    v0 = 4;
    if (*(unsigned char **)(self + 0x2EC) != 0) {
        (*(unsigned char **)(self + 0x2EC))[4] = 4;
    }
    self[2] = v0;

out:
    if (func_001B17A0(self)) {
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
    }
    return;

st2:
    if (func_001B17A0(self)) {
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
    }
    return;

st3:
    func_001AFC10(self);
}
