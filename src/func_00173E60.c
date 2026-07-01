// NEARMISS func_00173E60  (vram 0x00173E60, 0x368 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.95% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Sole residual is a genuine $at-vs-GPR register-coloring artifact: the `if((u8)(arg0+0x23F) >= 2)` compare feeding the state 0x63/0x64 dispatch materializes into $at in the target (`slti at,v0,2; bnez at,...`) but into $v0 in mwcc in every source shape tried (int local, inverted condition, direct ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Per-entity state machine (dispatch on *(arg0+6)): state 0 spawns via
// func_001749A0(anim clip from D_002754A8[gait]), clears the linked-list-node
// flag at *(arg0+0x18)+0xA; state 1 waits for the 0x8000 flag in arg0+0x200;
// states 2/3 run func_00173DD0 then compare arg0+0x3C against threshold
// tables D_00248700/D_00248704 (indexed by arg0+0x236, stride 8 bytes) to
// advance, arm a queued clip via func_001FBD50, and set up node fields; state
// 3's "node busy" branch instead resolves the queued clip index via
// func_0011A070 and advances to state 0x50; states 0x50/0x51 run a 4-tick
// countdown then fire an anim-clip cue (id depends on arg0+0x236) via
// func_001749A0; states 0x63/0x64 drive per-tick aim update via
// func_00174AC0 and either arm a rewind (func_0017C440) or reset
// (func_0017C540) depending on arg0+0x23F. Always: func_001764E0(arg0);
// decay arg0+0xB4 by 0.2; func_00175900(arg0,1); func_001796C0(arg0).
extern int func_001749A0(unsigned char *e, short clip, int flags, float blend);
extern void func_00173DD0(unsigned char *self);
extern unsigned char func_00174AC0(unsigned char *arg0, int arg1);
extern void func_0017C440(unsigned char *p, int unused);
extern void func_0017C540(unsigned char *a0);
extern void func_0011A070(int arg0);
extern int func_001FBD50(void *a0, int a1, int a2, float f12);
extern void func_00175900(unsigned char *a0, int a1);
extern void func_001764E0(unsigned char *a0);
extern void func_00178B90(unsigned char *a0, int a1);
extern void func_001796C0(unsigned char *a0);
extern float D_00248700[256][2];
extern float D_00248704[256][2];
extern short D_002754A8[2];

void func_00173E60(unsigned char *arg0) {
    unsigned char st;

    st = arg0[6];
    switch (st) {
    case 0:
        arg0[6] = st + 1;
        arg0[7] = 0;
        *(int *)(arg0 + 0x38) = 0;
        func_001749A0(arg0, D_002754A8[*(unsigned char *)(arg0 + 0x236)], 0, *(float *)(arg0 + 0x1FC));
        *(unsigned char *)(*(char **)(arg0 + 0x18) + 0xA) = 0;
        *(unsigned char *)(arg0 + 0x302) = 0xFF;
        break;
    case 1:
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            arg0[6] = st + 1;
        }
        break;
    case 2:
        func_00173DD0(arg0);
        if (*(float *)(arg0 + 0x3C) <= D_00248700[*(unsigned char *)(arg0 + 0x236)][0]) {
            arg0[6] = arg0[6] + 1;
            *(char *)(*(char **)(arg0 + 0x18) + 0x0) = 1;
            *(short *)(*(char **)(arg0 + 0x18) + 0x36) = 0xF;
            *(unsigned char *)(arg0 + 0x302) = func_001FBD50(arg0, 0x17F, 0, 300.0f);
            *(unsigned char *)(arg0 + 0x25E) = 0x83;
        }
        break;
    case 3:
        if (*(unsigned char *)(*(char **)(arg0 + 0x18) + 0xA) != 0) {
            arg0[6] = 0x50;
            {
                unsigned char idx = *(unsigned char *)(arg0 + 0x302);
                if ((int)idx != -1) {
                    func_0011A070(idx);
                    *(unsigned char *)(arg0 + 0x302) = 0xFF;
                }
            }
        } else {
            func_00173DD0(arg0);
            if (*(float *)(arg0 + 0x3C) <= D_00248704[*(unsigned char *)(arg0 + 0x236)][0]) {
                arg0[6] = arg0[6] + 1;
                *(char *)(*(char **)(arg0 + 0x18) + 0x0) = 2;
            }
        }
        break;
    case 4:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            arg0[6] = 0x63;
        }
        break;
    case 0x50:
        arg0[6] = st + 1;
        *(short *)(arg0 + 0x28) = 4;
        /* fallthrough */
    case 0x51:
        {
            short v = *(short *)(arg0 + 0x28);
            *(short *)(arg0 + 0x28) = v - 1;
            if (v == 0) {
                arg0[6] = arg0[6] + 1;
                if (*(unsigned char *)(arg0 + 0x236) == 0) {
                    func_001749A0(arg0, 0x10F, 0.0f, 4.0f);
                } else {
                    func_001749A0(arg0, 0x1C1, 0.0f, 4.0f);
                }
            } else {
                *(int *)(arg0 + 0x204) = 0;
            }
        }
        break;
    case 0x52:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            arg0[6] = 0x63;
        }
        break;
    case 0x63:
        func_00174AC0(arg0, 1);
        if ((int)*(unsigned char *)(arg0 + 0x23F) >= 2) {
            arg0[6] = arg0[6] + 1;
            func_0017C440(arg0, 0);
        } else {
            *(char *)(arg0 + 0x25C) = 0;
            func_0017C540(arg0);
        }
        break;
    case 0x64:
        func_00174AC0(arg0, 1);
        func_00178B90(arg0, 0);
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            func_0017C540(arg0);
        }
        break;
    }
    func_001764E0(arg0);
    *(float *)(arg0 + 0xB4) += -0.2f;
    func_00175900(arg0, 1);
    func_001796C0(arg0);
}
