// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 91.26% NEARMISS).
// Fixes read from the original instructions:
//  - Case 5 returns through `if (obj[4] >= 2) return 1; return 0;` (the
//    original's set-0/set-1 branch pair), not a less-than test.
//  - The anim_clip_init start time is an int 0 converted to float (idiom-24):
//    it stays live in a saved float register across func_001C61D0, which is
//    why the frame is 0x60 and saves $f20.
//  - The clip start is an `if (cmd == 4) { ... }` block, not an early return.
//  - The bone-handle loop is a for loop whose `i = 0, slot = obj` init sits
//    after the bone-budget rejection.
//  - `obj[9] > 2` (not `>= 3`) puts the compare result in $at (idiom-28).
//  - A redundant (int) cast on func_001C61D0's first argument restores the
//    original argument-load order.
//  - func_001C6120 takes (table, index); func_001C5C90 takes obj (lane round 8).

// SEMANTICS: message/event handler for one animated actor.  `obj` is the
//   actor block, `ev` is the incoming event record:
//     ev+0x04 (short) = message id / clip selector
//     ev+0x06 (short) = model-table index
//     ev+0x08 (short) = sub-index (bone set / clip)
//     ev+0x0A (short) = command, dispatched through jtbl_0026E170 (0..8)
//     ev+0x28 (int)   = misc parameter, stored as a short
//   Two special messages are handled before the command dispatch:
//     0x270D -> arm the global request block (D_002821B0 = 2,
//               D_002821B4 = 1, D_002821B8 = ev->sub) and reset the actor's
//               bone counters obj[0xC] / obj[9]; returns 0.
//     0x270C -> publish obj->0x18 into D_008106C0; returns 0.
//   Command dispatch (jtbl_0026E170, 9 entries; >=9 falls through to the
//   common tail):
//     0, 4, 6 -> bind the model named by D_0028A490[msg]  (func_001CA6E0)
//     1, 8    -> bind the model resolved from D_0028A59C  (func_001C6120)
//     2       -> same but from D_0028A56C
//     3       -> resolve D_0028A490[ev->modelIdx] with ev->sub into the
//                global slot D_00810250, clear D_00810254, copy the first
//                float of the resolved object into D_00810258, raise the
//                request state D_008101E4 = 3, record ev->0x28 in
//                D_0081024E, and hand the request block D_008101E0 to
//                func_0022EC30.
//     5       -> func_001C5C90() then return "actor already advanced"
//                (obj[4] >= 2); this arm skips the tail entirely.
//   Common tail (skipped when the command is 3, which just clears the bone
//   counters and returns 0):
//     obj->0x40 = D_0028A490[ev->modelIdx];
//     obj[0xC]  = func_001C6150(obj->0x44)   -- the bone count of the model.
//     If the global bone budget D_00275BCC is smaller than that count the
//     actor is rejected: obj[4] = 3 and the handler returns 1.
//     Otherwise obj->0x110[n] is filled with func_001AF780() handles for
//     each of the obj[0xC] bones and obj[9] is set to the count.
//     A second dispatch on the command then kicks the per-command animation:
//       0 -> func_001BA8E0(obj, msg)  then func_001CA6F0(obj, 2 or 0)
//       6 -> func_001D8BF0(obj, 1)    then func_001CA6F0(obj, 2 or 0)
//            (2 when at least 3 bones were bound, otherwise 0)
//       8 -> func_001CA6F0(obj, 1)
//       otherwise -> func_001CA6F0(obj, 0)
//     Finally anim_bone_array_setup(obj[9]) and bone_init_default_2(obj,
//     ev->sub) run, and for command 4 only, anim_clip_init starts the clip
//     at time 0.0f with a length of (func_001C61D0(obj->0x40, ev->sub) - 1)
//     frames.  Returns 0.

extern int    D_002821B0;
extern int    D_002821B4;
extern int    D_002821B8;
extern int    D_008106C0;
extern int    D_0028A490[];
extern int    D_0028A59C;
extern int    D_0028A56C;
extern char  *D_00810250;
extern int    D_00810254;
extern float  D_00810258;
extern char   D_008101E0[];
extern char   D_008101E4[8];
extern short  D_0081024E[4];
extern short  D_00275BCC;

extern void func_001CA6E0(unsigned char *, int);
extern char *func_001C6120(char *a0, int a1);
extern void func_0022EC30(char *);
extern void func_001C5C90(char *arg0);
extern int  func_001AF780(void);
extern int  func_001C6150(int);
extern void func_001BA8E0(unsigned char *, int);
extern void func_001D8BF0(unsigned char *, int);
extern void func_001CA6F0(unsigned char *, int);
extern int  func_001C61D0(int, int);
extern void anim_bone_array_setup(int);
extern void bone_init_default_2(unsigned char *, int);
extern void anim_clip_init(unsigned char *, int, float, float);

int func_001BAD40(unsigned char *obj, unsigned char *ev)
{
    int msg;
    int i;
    int n;
    unsigned char *slot;
    float z;

    msg = *(short *)(ev + 4);
    if (msg == 0x270D) {
        D_002821B0 = 2;
        D_002821B4 = 1;
        D_002821B8 = *(short *)(ev + 8);
        obj[0xC] = 0;
        obj[9] = 0;
        return 0;
    }
    if (msg == 0x270C) {
        D_008106C0 = *(int *)(obj + 0x18);
        return 0;
    }

    switch (*(short *)(ev + 0xA)) {
    case 0:
    case 4:
    case 6:
        func_001CA6E0(obj, D_0028A490[msg]);
        break;
    case 1:
    case 8:
        func_001CA6E0(obj, (int)func_001C6120((char *)D_0028A59C, msg));
        break;
    case 2:
        func_001CA6E0(obj, (int)func_001C6120((char *)D_0028A56C, msg));
        break;
    case 3:
        D_00810250 = func_001C6120((char *)D_0028A490[*(short *)(ev + 6)],
                                           *(short *)(ev + 8));
        D_00810254 = 0;
        D_00810258 = *(float *)D_00810250;
        D_008101E4[0] = 3;
        n = *(int *)(ev + 0x28);
        D_0081024E[0] = n;
        func_0022EC30(D_008101E0);
        break;
    case 5:
        func_001C5C90((char *)obj);
        if (obj[4] >= 2) {
            return 1;
        }
        return 0;
    }

    if (*(short *)(ev + 0xA) != 3) {
        *(int *)(obj + 0x40) = D_0028A490[*(short *)(ev + 6)];
        obj[0xC] = func_001C6150(*(int *)(obj + 0x44));
        if (D_00275BCC < obj[0xC]) {
            obj[4] = 3;
            return 1;
        }
        for (i = 0, slot = obj; i < obj[0xC]; i++) {
            *(int *)(slot + 0x110) = func_001AF780();
            slot += 4;
        }
        obj[9] = obj[0xC];

        switch (*(short *)(ev + 0xA)) {
        case 0:
            func_001BA8E0(obj, *(short *)(ev + 4));
            if (obj[9] > 2) {
                func_001CA6F0(obj, 2);
            } else {
                func_001CA6F0(obj, 0);
            }
            break;
        case 6:
            func_001D8BF0(obj, 1);
            if (obj[9] > 2) {
                func_001CA6F0(obj, 2);
            } else {
                func_001CA6F0(obj, 0);
            }
            break;
        case 8:
            func_001CA6F0(obj, 1);
            break;
        default:
            func_001CA6F0(obj, 0);
            break;
        }
        anim_bone_array_setup(obj[9]);
        bone_init_default_2(obj, *(short *)(ev + 8));
        if (*(short *)(ev + 0xA) == 4) {
        n = 0;
        z = (float)n;
        anim_clip_init(obj, *(short *)(ev + 8), z,
                       (float)(func_001C61D0((int)*(int *)(obj + 0x40), *(short *)(ev + 8)) - 1));
        }
    } else {
        obj[0xC] = 0;
        obj[9] = 0;
    }
    return 0;
}
