// NEARMISS func_00134090  (vram 0x00134090, 0x270 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.51% via mwcc24 (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 13 residual instructions of 156 (mwcc24 -O4,p -sdatathreshold 4). NOT a jr-table wall: the 9-entry jtbl_0026D140 dispatch matches exactly (reloc included, entry 7 = the fall-through/default label), all eight case bodies match, both of CodeWarrior's dead duplicated `addiu v0,zero,1` branch-fill co...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc24
// CFLAGS: -O4,p -sdatathreshold 4

//
// SEMANTICS: per-frame update for one actor kind (act = actor/controller block,
// ent = the entity it drives).  It first stages two 16-byte vectors in the EE
// scratchpad (0x700038A0 and 0x700038B0): 0x700038A0 gets the four floats at
// D_00810360..D_0081036C with its .y component then cleared, and 0x700038B0 gets
// { 340.0f, 0.0f, 270.0f, 1.0f }.  func_001B15D0 reduces that pair to a single
// float; ent[+0x5C] is set to 0 when the result is <= 94.0f and to 1 otherwise
// (a near/far flag).
//   act[+0x05] = state id (0..8); a 9-entry jump table selects the per-state
//                handler.  State 7 has no handler (falls straight through).
//   act[+0x00] = state-machine mode written back each frame: 3 while
//                ent[+0x52] (a signed short "busy/aborted" counter) is non-zero,
//                otherwise 1.
//   ent[+0x00] = byte tag on entry; when it is 9 the actor fires func_001FBD50
//                with id 0x841 + ((func_00122BB8(act) >> 16) % 10) and 450.0f,
//                i.e. one of ten variants picked from a rolling counter.
//                The same field is overwritten below with anim_advance_time()'s
//                result (a 32-bit store).
//   ent[+0x20] = per-frame time delta handed to anim_advance_time().
//   act[+0x4C] = per-actor callback invoked with the actor itself.
// D_00275B44 is the global render/actor context passed to func_001DA6A0 last.

extern char D_700038A0[];
extern char D_700038B0[];
extern float D_00810360[2];
extern float D_00810364[2];
extern float D_00810368[2];
extern float D_0081036C[2];
extern int D_00275B44;

extern float func_001B15D0(char *, char *);
extern void func_00134300(unsigned char *, unsigned char *);
extern void func_00134500(unsigned char *, unsigned char *);
extern void func_001348E0(unsigned char *, unsigned char *);
extern void func_00134DC0(unsigned char *, unsigned char *);
extern void func_00134FF0(unsigned char *, unsigned char *);
extern void func_001352E0(unsigned char *, unsigned char *);
extern void func_00135870(unsigned char *, unsigned char *);
extern void func_00135D00(unsigned char *, unsigned char *);
extern int func_00122BB8(unsigned char *);
extern void func_001FBD50(unsigned char *, int, int, float);
extern void func_00136CB0(unsigned char *, unsigned char *);
extern void func_001368D0(unsigned char *, unsigned char *);
extern void func_00136630(unsigned char *, unsigned char *);
extern void func_001366E0(unsigned char *, unsigned char *);
extern int anim_advance_time(unsigned char *, float);
extern void func_00131ED0(unsigned char *);
extern void func_001C68C0(unsigned char *);
extern void func_001B17A0(unsigned char *);
extern void func_001DA6A0(int);

void func_00134090(unsigned char *act, unsigned char *ent)
{
    short flag;
    float f0, f1, f2, f3;

    f3 = D_00810360[0];
    f2 = D_00810364[0];
    f1 = D_00810368[0];
    f0 = D_0081036C[0];

    *(volatile int *)0x700038B0 = 0x43AA0000;
    *(volatile float *)0x700038A0 = f3;
    *(volatile float *)0x700038A4 = f2;
    *(volatile float *)0x700038A8 = f1;
    *(volatile float *)0x700038AC = f0;
    *(volatile int *)0x700038B4 = 0;
    *(volatile int *)0x700038A4 = 0;
    *(volatile int *)0x700038B8 = 0x43870000;
    *(volatile int *)0x700038BC = 0x3F800000;

    if (func_001B15D0(D_700038A0, D_700038B0) <= 94.0f) {
        *(char *)(ent + 0x5C) = 0;
    } else {
        *(char *)(ent + 0x5C) = 1;
    }

    switch (act[5]) {
    case 0:
        func_00134300(act, ent);
        break;
    case 1:
        func_00134500(act, ent);
        break;
    case 2:
        func_001348E0(act, ent);
        break;
    case 3:
        func_00134DC0(act, ent);
        break;
    case 4:
        func_00134FF0(act, ent);
        break;
    case 5:
        func_001352E0(act, ent);
        break;
    case 6:
        func_00135870(act, ent);
        break;
    case 8:
        func_00135D00(act, ent);
        break;
    }

    flag = *(short *)(ent + 0x52);
    if (flag != 0) {
        act[0] = 3;
    } else {
        act[0] = 1;
    }

    if (ent[0] == 9) {
        func_001FBD50(act, ((func_00122BB8(act) >> 16) % 10) + 0x841, 0, 450.0f);
    }

    func_00136CB0(act, ent);
    func_001368D0(act, ent);
    func_00136630(act, ent);
    func_001366E0(act, ent);
    *(int *)ent = anim_advance_time(act, *(float *)(ent + 0x20));
    func_00131ED0(act);
    func_001C68C0(act);
    func_001B17A0(act);
    (*(void (**)(unsigned char *))(act + 0x4C))(act);
    func_001DA6A0(D_00275B44);
}
