// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// SEMANTICS: walk a spawn/placement script and instantiate the entities it asks for.
// `p` points at a table of 0x2C-byte records terminated by a -1 halfword id. The id
// (halfword +0x00) selects a spawn CONDITION via a 7-entry jump table; ids 0 and >= 7
// spawn unconditionally, ids 1..6 test the actor id at +0x02 and/or the per-actor
// status table D_00810700 (byte +0x58 = "slot in use" marker, 0xFF meaning free;
// byte +0xD8 = a secondary per-actor flag) and skip the record when the test fails:
//   1: skip if func_001B11E0(p[2]) reports the actor is already live.
//   2: skip if the actor's slot is free.
//   3: require the slot free AND the actor not live.
//   4: require the secondary flag set AND the actor not live.
//   5: require the slot NOT free AND the secondary flag == 1.
//   6: require the slot free, then require bit 7 of halfword +0x08 to agree with the
//      global mode byte D_00810778 (set when it is 0xFF, clear otherwise), and the
//      actor not live.
// A record that passes creates the entity with func_001AFA90(p[4]) and, if that
// succeeds, copies the record's payload into it: type byte +0x02 -> e[0x9A], +0x06 ->
// e[0x03] and its high byte -> e[0x2E], +0x08 -> e[0x0D]; then, when the sub-type at
// +0x04 masked with ~0xE0 is 2, the global D_00810701 -> e[0x9D] and +0x0A -> e[0x9E],
// otherwise the halfword at +0x0A -> e[0x0E]; then +0x0C/+0x0E -> e[0x54]/e[0x56], the
// six floats at +0x10..+0x24 -> e[0xB0..0xB8] and e[0xC0..0xC8], and the word at +0x28
// -> e[0x10]. Returns the last entity created (0 if none).

extern unsigned char D_00810700[];
extern unsigned char D_00810701;
extern unsigned char D_00810778;

extern int func_001B11E0(int id);
extern unsigned char *func_001AFA90(int id);

unsigned char *func_001B6660(unsigned char *p)
{
    unsigned char *e;
    int id;
    int i;
    int v;

    e = 0;
    for (; (id = *(short *)(p + 0)) != -1; p += 0x2C) {
        switch (id) {
        case 0:
            break;
        case 1:
            if (func_001B11E0(p[2]) != 0) {
                continue;
            }
            break;
        case 2:
            if (D_00810700[((*(short *)(p + 2) >> 8) & 0xFF) + 0x58] == 0xFF) {
                continue;
            }
            break;
        case 3:
            v = *(short *)(p + 2);
            if (D_00810700[((v >> 8) & 0xFF) + 0x58] != 0xFF) {
                continue;
            }
            if (func_001B11E0(v & 0xFF) != 0) {
                continue;
            }
            break;
        case 4:
            v = *(short *)(p + 2);
            if (D_00810700[(v >> 8) + 0xD8] == 0) {
                continue;
            }
            if (func_001B11E0(v & 0xFF) != 0) {
                continue;
            }
            break;
        case 5:
            i = (*(short *)(p + 2) >> 8) & 0xFF;
            if (D_00810700[i + 0x58] == 0xFF) {
                continue;
            }
            if (D_00810700[i + 0xD8] != 1) {
                continue;
            }
            break;
        case 6:
            v = *(short *)(p + 2);
            if (D_00810700[((v >> 8) & 0xFF) + 0x58] != 0xFF) {
                continue;
            }
            if (D_00810778 == 0xFF) {
                if ((*(short *)(p + 8) & 0x80) == 0) {
                    continue;
                }
            } else {
                if ((*(short *)(p + 8) & 0x80) != 0) {
                    continue;
                }
            }
            if (func_001B11E0(v & 0xFF) != 0) {
                continue;
            }
            break;
        }
        e = func_001AFA90(p[4]);
        if (e == 0) {
            continue;
        }
        e[0x9A] = p[2];
        e[3] = p[6];
        *(short *)(e + 0x2E) = (*(short *)(p + 6) >> 8) & 0xFF;
        e[0xD] = p[8];
        if ((*(short *)(p + 4) & ~0xE0) == 2) {
            e[0x9D] = D_00810701;
            e[0x9E] = p[0xA];
        } else {
            *(unsigned short *)(e + 0xE) = *(unsigned short *)(p + 0xA);
        }
        *(short *)(e + 0x54) = *(short *)(p + 0xC);
        *(short *)(e + 0x56) = *(short *)(p + 0xE);
        *(float *)(e + 0xB0) = *(float *)(p + 0x10);
        *(float *)(e + 0xB4) = *(float *)(p + 0x14);
        *(float *)(e + 0xB8) = *(float *)(p + 0x18);
        *(float *)(e + 0xC0) = *(float *)(p + 0x1C);
        *(float *)(e + 0xC4) = *(float *)(p + 0x20);
        *(float *)(e + 0xC8) = *(float *)(p + 0x24);
        *(int *)(e + 0x10) = *(int *)(p + 0x28);
    }
    return e;
}
