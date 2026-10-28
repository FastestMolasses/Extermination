// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// SDK (libpad, scePadInfoAct shape): actuator information for a pad. Gets the
// pad's current DMA record (func_00110AB8). Returns 0 unless the record's task
// state (0x72) is 1, its mode state (0x64) is at least 2 and actno is below
// the actuator count (0x6A). actno -1 returns the actuator count; otherwise
// term 1..4 returns byte 0..3 of that actuator's 4-byte entry at 0x30, and any
// other term returns 0.

typedef struct {
    char pad_00[0x30];
    unsigned char act[8][4];        /* 0x30: per-actuator info */
    char pad_50[0x14];
    unsigned char state;            /* 0x64 */
    char pad_65[0x05];
    unsigned char nact;             /* 0x6A: number of actuators */
    char pad_6B[0x07];
    unsigned char task;             /* 0x72 */
} PadData;

extern PadData *func_00110AB8(int port, int slot);

int func_00110C80(int port, int slot, int actno, int term)
{
    PadData *p;

    p = func_00110AB8(port, slot);
    if (p->task != 1) {
        return 0;
    }
    if (p->state < 2) {
        return 0;
    }
    if (actno >= p->nact) {
        return 0;
    }
    if (actno == -1) {
        return p->nact;
    }
    switch (term) {
    case 1:
        return p->act[actno][0];
    case 2:
        return p->act[actno][1];
    case 3:
        return p->act[actno][2];
    case 4:
        return p->act[actno][3];
    }
    return 0;
}
