// NEARMISS func_00196CE0  (vram 0x00196CE0, 0x6B0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.32% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation / rematerialization + call-arg-register scheduling (permuter class), NOT the clean-store nop. CW holds self[2] in $a0 across uses and materializes &D_0024A6BC once; mwcc reloads lbu + re-lui's. func_001916C0(0) arg goes to $a2 (CW) vs $a0 (mwcc). FPU-MAC 2-term distance checks...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 95.3% (mwcc 2.3.3; 991202 caps 90.6%). Body/logic FULLY recovered
// and verified; the FPU-MAC distance checks (dx*dx+dy*dy -> mula.s/madd.s) DO
// match. AI state-machine tick for an entity: dispatch on self[1] (state 0..4);
// state 0 inits anim/heading, snaps position from the D_0024A6B0/B4/B8 table,
// and transitions to 4/3/2 based on two squared-distance gates against the
// player (other[0xA0]/[0xA8]) plus other[0xA4] window checks; states 1/2 track
// or steer via func_001B12B0 and re-evaluate; states 3/4 set fixed positions.
// A trailing dispatch on other[0x230] resets the state (self[5]/[6]/[1]=0) for
// all values except {6,7,8,9,44,45}. Ends with func_0018C0C0(self).
//
// RESIDUAL WALLS (register-allocation / scheduling -> permuter class, NOT the
// clean-store nop; mwcc 2.3.3 does not fix):
//  (a) idx/self[2] reload + &D_0024A6BC base rematerialization: CW holds self[2]
//      in a non-saved reg ($a0) across the threshold compare and the <3 test and
//      materializes the table base once; mwcc reloads lbu + re-lui's the base.
//  (b) call-arg register: CW zeros $a2 in the func_001916C0(0) delay slot; mwcc
//      puts the 0 in $a0. Scheduling/ABI artifact.
// All non-offset deltas are register coloring / rematerialization; the remaining
// large diff count is branch-target offset shift cascading from (a)/(b).
extern void func_001916C0(int);
extern void func_00196970(unsigned char *, unsigned char *);
extern void func_0018C0C0(unsigned char *);
extern void func_0018C4B0(void *, float, float);
extern void func_0018C6A0(void *, void *, float);
extern void func_0018D7B0(unsigned char *, int);
extern void func_00192010(unsigned char *, float, float, float);
extern void func_00102948(void *, void *, void *);
extern float func_0011DE90(float);
extern float func_0011E2A8(float);
extern float func_001B1240(void *, float, float);
extern float func_001B12B0(float, float, float);

extern float D_0024A6B0;
extern float D_0024A6B4;
extern float D_0024A6B8;
extern float D_0024A6BC;
extern float D_008105D0;
extern float D_008105E0;
extern float D_008105E8;

void func_00196CE0(unsigned char *self, unsigned char *other) {
    float dx, dy;
    int idx;

    func_001916C0(0);
    switch (self[1]) {
    case 0:
        *(short *)(self + 8) = 0;
        self[1] = self[1] + 1;
        self[2] = 0;
        *(float *)(self + 0x44) = func_001B1240(&D_008105D0, D_008105E0, D_008105E8);
        func_00196970(self, other);
        if (*(int *)(other + 0x230) == 0x2D) {
            *(float *)(self + 0x10) = (&D_0024A6B0)[self[2] * 4];
            *(float *)(self + 0x14) = (&D_0024A6B4)[self[2] * 4];
            *(float *)(self + 0x18) = (&D_0024A6B8)[self[2] * 4];
            func_00102948(&D_008105D0, self + 0x10, &D_0024A6B4);
        } else if (self[2] == 3 || self[2] == 4) {
            *(float *)(self + 0x10) = (&D_0024A6B0)[self[2] * 4];
            *(float *)(self + 0x14) = (&D_0024A6B4)[self[2] * 4];
            *(float *)(self + 0x18) = (&D_0024A6B8)[self[2] * 4];
        }
        dx = *(float *)(other + 0xA0) - 848.6f;
        dy = *(float *)(other + 0xA8) - 882.1f;
        if (dx * dx + dy * dy < 225.0f &&
            *(float *)(other + 0xA4) < 294.0f && *(float *)(other + 0xA4) > 241.0f) {
            self[1] = 4;
            break;
        }
        dx = *(float *)(other + 0xA0) - 915.0f;
        dy = *(float *)(other + 0xA8) - 939.1f;
        if (dx * dx + dy * dy < 225.0f &&
            *(float *)(other + 0xA4) < 266.0f && *(float *)(other + 0xA4) > 229.0f) {
            self[1] = 3;
            break;
        }
        if (dx * dx + dy * dy >= 225.0f && self[2] == 5) {
            self[1] = 2;
            break;
        }
        /* fallthrough into shared idle/track branch */
    case 1:
        if (*(int *)(other + 0x230) == 0x2D) {
            func_00196970(self, other);
        } else {
            func_00196970(self, other);
            if (*(float *)(other + 0xB4) < (&D_0024A6BC)[self[2] * 4]) {
                if (self[2] < 3) {
                    self[1] = self[1] + 1;
                }
                func_0018D7B0(self, 3);
            }
            func_0018C6A0(self + 0x10, &D_008105D0, 2.0f);
            func_0018C4B0(&D_008105D0, *(float *)(self + 0x14), 2.0f);
        }
        break;
    case 2:
        if (*(int *)(other + 0x230) == 7) {
            *(float *)(self + 0x44) = func_001B12B0(3.1415927f + *(float *)(other + 0xC4), *(float *)(self + 0x44), 0.034906585f);
            self[0x6C] = 0;
            *(float *)(self + 0x10) = D_008105E0 + *(float *)(self + 0xC) * func_0011E2A8(*(float *)(self + 0x44));
            *(float *)(self + 0x18) = D_008105E8 + *(float *)(self + 0xC) * func_0011DE90(*(float *)(self + 0x44));
            func_0018D7B0(self, 3);
            func_0018C6A0(self + 0x10, &D_008105D0, 1.0f);
            func_0018C4B0(&D_008105D0, *(float *)(self + 0x14), 1.0f);
        } else {
            *(float *)(self + 0x44) = func_001B12B0(*(float *)(other + 0xC4), *(float *)(self + 0x44), 0.034906585f);
            *(float *)(self + 0x10) = D_008105E0 + *(float *)(self + 0xC) * func_0011E2A8(*(float *)(self + 0x44));
            *(float *)(self + 0x18) = D_008105E8 + *(float *)(self + 0xC) * func_0011DE90(*(float *)(self + 0x44));
            func_00192010(self, *(float *)(self + 0x8C) + (*(float *)(self + 0x5C) + *(float *)(other + 0xB4)), 20.0f, 15.0f);
            func_00196970(self, other);
            idx = self[2];
            if (idx != 5 && *(float *)(other + 0xB4) >= (&D_0024A6BC)[idx * 4]) {
                self[1] = self[1] - 1;
            }
            func_0018D7B0(self, 3);
            func_0018C6A0(self + 0x10, &D_008105D0, 2.0f);
            func_0018C4B0(&D_008105D0, *(float *)(self + 0x14), 2.0f);
        }
        break;
    case 3:
        *(float *)(self + 0x10) = 915.8f;
        *(float *)(self + 0x18) = 900.0f;
        func_00192010(self, *(float *)(self + 0x8C) + (*(float *)(self + 0x5C) + *(float *)(other + 0xB4)), 15.0f, 10.0f);
        func_0018D7B0(self, 5);
        func_0018C6A0(self + 0x10, &D_008105D0, 1.0f);
        func_0018C4B0(&D_008105D0, *(float *)(self + 0x14), 2.0f);
        break;
    case 4:
        *(float *)(self + 0x10) = 801.8f;
        *(float *)(self + 0x18) = 882.1f;
        func_00192010(self, *(float *)(self + 0x8C) + (*(float *)(self + 0x5C) + *(float *)(other + 0xB4)), 15.0f, 10.0f);
        func_0018D7B0(self, 5);
        func_0018C6A0(self + 0x10, &D_008105D0, 1.0f);
        func_0018C4B0(&D_008105D0, *(float *)(self + 0x14), 2.0f);
        break;
    }
    switch (*(int *)(other + 0x230)) {
    case 6:
    case 7:
    case 8:
    case 9:
    case 44:
    case 45:
        break;
    default:
        self[5] = 0;
        self[6] = 0;
        self[1] = 0;
        break;
    }
    func_0018C0C0(self);
}
