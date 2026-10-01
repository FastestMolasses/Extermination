// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Script-driven actor worker (the opening's script actors; the area script
// spawns two such records). a+0x20 is the actor's current script command,
// a+0x24 its owner. State +4:
//  0: bind the command (func_001BAD40(a, cmd) == 0) and go to state 1;
//  1: once the owner's stop mask (+0x2E) has the actor's bit (+0x2E) the
//     actor goes to state 2. Commands 0x270C / 0x270D do nothing; otherwise
//     the command's mode (+0x0A) picks: 0 starts the clip (func_001BA580(a, id): the
//     callee classifies the command id into a sound category)
//     and plays, 3 waits, 4 only poses (func_001C68C0) and calls the draw
//     callback +0x4C, 5 runs func_001C5C90, 6 plays and also triggers the
//     command's sound (func_001F9660(a, id)); any other mode plays. Playing
//     is anim_advance_time(a, speed +0x0C), func_001C68C0, +1 = 1 and the
//     draw callback.
//  2: state 3; unless the command is 0x270D, mode 0 resets the clip
//     (func_001BA540).
//  3: release (func_001AFC10).
extern int func_001BAD40(unsigned char *a, short *cmd);
// func_001BA580 compares its second argument as the full sign-extended command id
// (its NEARMISS C narrows it to unsigned char; the ids it tests are all below 0x80).
extern void func_001BA580(unsigned char *a, short id);
extern void func_001BA540(unsigned char *a);
extern void anim_advance_time(unsigned char *a, float speed);
extern void func_001C68C0(unsigned char *a);
extern void func_001F9660(unsigned char *a, short id);
extern void func_001C5C90(unsigned char *a);
extern void func_001AFC10(unsigned char *a);

void func_001BB0E0(unsigned char *a) {
    short *cmd = *(short **)(a + 0x20);
    unsigned char *owner = *(unsigned char **)(a + 0x24);

    switch (a[4]) {
    case 0:
        if (func_001BAD40(a, cmd) != 0) {
            break;
        }
        a[4] = 1;
    case 1:
        if (*(unsigned short *)(owner + 0x2E) & (1 << *(unsigned short *)(a + 0x2E))) {
            a[4] = 2;
            break;
        }
        if (cmd[2] == 0x270D || cmd[2] == 0x270C) {
            break;
        }
        switch (cmd[5]) {
        case 5:
            func_001C5C90(a);
            break;
        case 4:
            func_001C68C0(a);
            (*(void (**)(unsigned char *))(a + 0x4C))(a);
            break;
        case 3:
            break;
        case 0:
            func_001BA580(a, cmd[2]);
            anim_advance_time(a, *(float *)(cmd + 6));
            func_001C68C0(a);
            a[1] = 1;
            (*(void (**)(unsigned char *))(a + 0x4C))(a);
            break;
        case 6:
            anim_advance_time(a, *(float *)(cmd + 6));
            func_001C68C0(a);
            func_001F9660(a, cmd[2]);
            a[1] = 1;
            (*(void (**)(unsigned char *))(a + 0x4C))(a);
            break;
        default:
            anim_advance_time(a, *(float *)(cmd + 6));
            func_001C68C0(a);
            a[1] = 1;
            (*(void (**)(unsigned char *))(a + 0x4C))(a);
            break;
        }
        break;
    case 2:
        a[4] = a[4] + 1;
        if (cmd[2] == 0x270D) {
            break;
        }
        if (cmd[5] == 0) {
            func_001BA540(a);
        }
        break;
    case 3:
        func_001AFC10(a);
        break;
    }
}
