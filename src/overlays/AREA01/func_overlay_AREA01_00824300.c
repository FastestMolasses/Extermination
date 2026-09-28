// NEARMISS func_overlay_AREA01_00824300  (runtime 0x00824340, 0x42C bytes) — readable decompilation, NOT linked from C.
//
// objdiff 99.99% via mwccps2 2.3.3 (tools/overlay/overlay_match.py check AREA01 <this file>;
// the only unresolved operands are the jump-table %hi/%lo). Not compiled by
// tools/overlay/compile_overlay_src.py; the overlay links this function from its
// splat pieces. Splat names overlay code 0x40 below its runtime address (the MWo3
// header is loaded first).
// Role: called from 0x823CD0 for +5 in 2..5 with (self, self + 0x1F0).
//  func_001C2770(self, block, 2) first; +6 (jump table, 9 entries) steps a
//  sequence of func_00128830 / func_001287F0 calls and the 0x825040,
//  0x824FE0 and 0x824F70 helpers; func_001C3D60 runs when the func_001C2770
//  result was 0.
// The compiled instructions are byte-identical to the original, and so is the
// jump table once it is placed where the original's lui/addiu pair points and
// its entries are resolved at runtime addresses (link + 0x40); checked with a
// scratch resolver on top of tools/overlay/overlay_match.py. It stays NEARMISS
// only because the overlay link cannot do that yet: link_overlay.py places a
// compiled .rodata after the data section and resolves the table entries and
// the table address at link addresses, 0x40 below the runtime values the
// original stores (docs/PROGRESS.md, AREA01 overlay entry).
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern int func_001C2770(unsigned char *self, unsigned char *talk, int mode);
extern void func_001C3D60(unsigned char *self, unsigned char *talk);
extern void func_0012D580(unsigned char *self, unsigned char *talk, int held);
extern void func_00128830(unsigned char *self, float x, float y, float z);
extern void func_001287F0(unsigned char *self, unsigned char *talk, short anim, float t);
extern void func_overlay_AREA01_00825040(unsigned char *self, unsigned char *talk);
extern void func_overlay_AREA01_00824FE0(unsigned char *self);
extern int func_overlay_AREA01_00824F70(unsigned char *self);
extern float func_0011E2A8(float a);
extern float func_001B1470(float a);

void func_overlay_AREA01_00824300(unsigned char *self, unsigned char *talk) {
    int held = func_001C2770(self, talk, 2);
    int st;
    int e4;

    switch (st = self[6]) {
    case 0:
        *(float *)(talk + 0xD8) = 0.0f;
        self[7] = 0;
        if (held == 0) {
            self[6] = 1;
        }
        break;
    case 1:
        func_0012D580(self, talk, held);
        break;
    case 2:
        { int zi = 0; float z = (float)zi; func_00128830(self, 0.0f, z, -2.5f); }
        func_001287F0(self, talk, 0x13, 0.0f);
        self[6]++;
        self[7] = 0;
        *(float *)(talk + 0xD8) = 0.0f;
        break;
    case 3:
        if (*(short *)(talk + 0xF4) & 0x1000) {
            self[6] = st + 1;
            self[7] = 0;
            func_00128830(self, 0.0f, 2.5f, 0.0f);
            func_001287F0(self, talk, 0x14, 0.0f);
        } else {
            func_overlay_AREA01_00825040(self, talk);
        }
        break;
    case 4:
        if (*(short *)(talk + 0xF4) & 0x5000) {
            self[6] = st + 1;
            func_00128830(self, 0.0f, 1.5f, 3.0f);
            func_001287F0(self, talk, 0x15, 0.0f);
            *(short *)(talk + 0xD0) = 0x78;
            *(int *)(talk + 0xE4) = 0;
            *(float *)(talk + 0xF0) = 0.8f;
            *(float *)(self + 0xC0) = -0.7853982f;
            *(float *)(talk + 0xD8) = 0.6f;
        }
        break;
    case 5:
        func_overlay_AREA01_00824FE0(self);
        if (*(int *)(talk + 0xE4) & 0xF) {
            self[6] = 0;
            self[7] = 0;
            break;
        }
        *(float *)(self + 0xC0) = 3.1415927f * (56.25f * -*(float *)(talk + 0xF0)) / 180.0f;
        if ((*(float *)(talk + 0xF0) -= 0.04f) < 0.0f) {
            *(float *)(self + 0xC0) = 0.0f;
            self[6]++;
            *(short *)(talk + 0xD0) = 0;
        }
        *(float *)(self + 0xB4) += *(float *)(talk + 0xF0);
        if (func_overlay_AREA01_00824F70(self)) {
            *(float *)(self + 0xC0) = 0.0f;
            self[5] = 6;
            self[6] = 0;
        }
        break;
    case 6:
        func_overlay_AREA01_00824FE0(self);
        *(float *)(self + 0xB4) += 0.1f * func_0011E2A8(func_001B1470((float)*(short *)(talk + 0xD0)));
        *(short *)(talk + 0xD0) += 0x18;
        if (*(int *)(talk + 0xE4) & 0xF) {
            self[6] = 0;
            self[7] = 0;
        } else if (func_overlay_AREA01_00824F70(self)) {
            *(float *)(self + 0xC0) = 0.0f;
            self[5] = 6;
            self[6] = 0;
        } else if (*(short *)(talk + 0xD0) > 0x870) {
            *(int *)(talk + 0xE4) = 0x400;
            self[6]++;
        }
        break;
    case 7:
        e4 = *(int *)(talk + 0xE4);
        if (e4 == 0x100) {
            func_001287F0(self, talk, 0x11, 0.0f);
            *(float *)(self + 0xC0) = 0.0f;
            self[6]++;
            *(float *)(talk + 0xD8) = 0.0f;
            *(short *)(talk + 0xF4) = 0;
        } else if (e4 & 0xF) {
            self[6] = 0;
            self[7] = 0;
        }
        break;
    case 8:
        if (*(short *)(talk + 0xF4) & 0x1000) {
            func_00128830(self, 0.0f, 0.0f, 1.0f);
            func_001287F0(self, talk, 0, 0.0f);
            func_001287F0(self, talk, 1, 6.0f);
            *(float *)(self + 0xC4) = func_001B1470(3.1415927f + *(float *)(self + 0xC4));
            self[6] = 0;
            self[7] = 0;
        }
        break;
    }
    if (held == 0) {
        func_001C3D60(self, talk);
    }
}
