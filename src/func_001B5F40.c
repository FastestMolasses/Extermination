// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Pad port bring-up and read (libpad). p+4 / p+8 are the port and slot,
// p+0xC the pad state from func_00110B80 (scePadGetState); state 0
// (disconnected) resets the phase bytes +0x10 / +0x11 / +0x12 and calls
// func_001B62A0. Phase +0x10:
//  0: in state 6 (stable) or 2 (find CTP1), read the controller id with
//     func_00110E58 (scePadInfoMode: 1 = current id, 2 = extended id if
//     positive) into +0x14. Id 4 (digital): no mode table (InfoMode 4, -1
//     == 0) goes straight to phase 4, else request analog mode
//     (func_00110F60 = scePadSetMainMode(1, 3)) and go to phase 1. Id 7
//     (DualShock): request analog mode once (+0x11 clear), then set the
//     actuator alignment (func_001110B0 with p+0x1E) and go to phase 2.
//  1: once the request has finished (state not 5 / 7) back to phase 0 with
//     +0x11 = 1.
//  2: once finished, phase 4 with +0x12 = 1.
//  4: copy the id to +0x2A; state 6 or 2 reads the pad through
//     func_001B5940(buf, p, 1 or 0) and returns its result; state 7 resets
//     like a disconnect.
// Returns 0 otherwise.
extern int func_00110B80(int port, int slot);
extern int func_00110E58(int port, int slot, int term, int offs);
extern int func_00110F60(int port, int slot, int offs, int lock);
extern int func_001110B0(int port, int slot, unsigned char *align);
extern void func_001B62A0(unsigned char *p);
extern int func_001B5940(int buf, unsigned char *p, int analog);

int func_001B5F40(int buf, unsigned char *p) {
    int id;
    int ext;

    *(int *)(p + 0xC) = func_00110B80(*(int *)(p + 4), *(int *)(p + 8));
    if (*(int *)(p + 0xC) == 0) {
        p[0x10] = 0;
        p[0x12] = 0;
        p[0x11] = 0;
        func_001B62A0(p);
    }
    switch (p[0x10]) {
    case 0:
        if (*(int *)(p + 0xC) == 6 || *(int *)(p + 0xC) == 2) {
            id = func_00110E58(*(int *)(p + 4), *(int *)(p + 8), 1, 0);
            if (id != 0) {
                ext = func_00110E58(*(int *)(p + 4), *(int *)(p + 8), 2, 0);
                if (ext > 0) {
                    id = ext;
                }
                *(short *)(p + 0x14) = id;
                if (*(unsigned short *)(p + 0x14) == 4) {
                    if (func_00110E58(*(int *)(p + 4), *(int *)(p + 8), 4, -1) == 0) {
                        p[0x10] = 4;
                    } else if (func_00110F60(*(int *)(p + 4), *(int *)(p + 8), 1, 3) == 1) {
                        p[0x10] = 1;
                    }
                } else if (*(unsigned short *)(p + 0x14) == 7) {
                    if (p[0x11] == 0) {
                        if (func_00110F60(*(int *)(p + 4), *(int *)(p + 8), 1, 3) == 1) {
                            p[0x10] = 1;
                        }
                    } else if (func_001110B0(*(int *)(p + 4), *(int *)(p + 8), p + 0x1E) == 1) {
                        p[0x10] = 2;
                    }
                }
            }
        }
        break;
    case 1:
        if (*(int *)(p + 0xC) != 5 && *(int *)(p + 0xC) != 7) {
            p[0x10] = 0;
            p[0x11] = 1;
        }
        break;
    case 2:
        if (*(int *)(p + 0xC) != 5 && *(int *)(p + 0xC) != 7) {
            p[0x10] = 4;
            p[0x12] = 1;
        }
        break;
    case 4:
        *(unsigned short *)(p + 0x2A) = *(unsigned short *)(p + 0x14);
        if (*(int *)(p + 0xC) == 6) {
            return func_001B5940(buf, p, 1);
        } else if (*(int *)(p + 0xC) == 2) {
            return func_001B5940(buf, p, 0);
        } else if (*(int *)(p + 0xC) == 7) {
            p[0x10] = 0;
            p[0x12] = 0;
            p[0x11] = 0;
            func_001B62A0(p);
        }
        break;
    }
    return 0;
}
