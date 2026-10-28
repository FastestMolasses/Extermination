// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 82.00% NEARMISS).
// Fixes read from the original instructions:
// - Every range keeps a (table, index) pair and one common tail forms the
//   record address: the remapped ranges subtract their base from id first and
//   overwrite id with the remap byte, the fixed ranges subtract and pick the
//   static table.
// - The record pointer reuses the table variable; the area-table temporaries
//   are function-scope locals shared by both remapped ranges.
// - Tail is `if (voice >= 0) { setters } return voice;`, not an early return.
// - Records are 4 signed bytes; D_00281D50 is a [group][0x14] handle table.
//
// Sound-id dispatch: mask id to 15 bits, then bin it: 0..999 index the fixed
// record table D_0025ECA0; 1000..1499 go through the per-area remap tables
// D_00264A70[area][sub] (0xFF = none) into D_00264B30[area][sub]; 1500..1999
// index the fixed table D_00261570; 2000..2499 use D_00264AD0 / D_00264B90 the
// same way (area = D_00810700, sub = D_00810701). Any missing table or id
// outside these ranges returns -1. The record's bank handle
// D_00281D50[group][bank] starts the note via func_00119EA0; on success the
// voice gets func_0011A270(voice, a1) and func_0011A218(voice, a2, a3).
// Returns the voice (negative on failure).

typedef struct SndRec {
    signed char group;
    signed char bank;
    signed char scriptGroup;
    signed char scriptIdx;
} SndRec;

extern SndRec D_0025ECA0[];
extern SndRec D_00261570[];
extern unsigned char **D_00264A70[];
extern SndRec **D_00264B30[];
extern unsigned char **D_00264AD0[];
extern SndRec **D_00264B90[];
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern int D_00281D50[][0x14];

extern int func_00119EA0(int handle, int scriptGroup, int scriptIdx);
extern void func_0011A270(int voice, int a1);
extern void func_0011A218(int voice, int a1, int a2);

int func_001FB9F0(int id, int a1, int a2, int a3) {
    SndRec *tab;
    unsigned char **areaTab;
    unsigned char *remapTab;
    SndRec **recTabs;
    int voice;

    id &= 0x7FFF;

    if (id < 0x3E8) {
        tab = D_0025ECA0;
    } else if (id < 0x5DC) {
        id -= 0x3E8;
        areaTab = D_00264A70[D_00810700];
        if (areaTab == 0)
            return -1;
        remapTab = areaTab[D_00810701];
        if (remapTab == 0)
            return -1;
        id = remapTab[id];
        if (id == 0xFF)
            return -1;
        recTabs = D_00264B30[D_00810700];
        if (recTabs == 0)
            return -1;
        tab = recTabs[D_00810701];
        if (tab == 0)
            return -1;
    } else if (id < 0x7D0) {
        id -= 0x5DC;
        tab = D_00261570;
    } else if (id < 0x9C4) {
        id -= 0x7D0;
        areaTab = D_00264AD0[D_00810700];
        if (areaTab == 0)
            return -1;
        remapTab = areaTab[D_00810701];
        if (remapTab == 0)
            return -1;
        id = remapTab[id];
        if (id == 0xFF)
            return -1;
        recTabs = D_00264B90[D_00810700];
        if (recTabs == 0)
            return -1;
        tab = recTabs[D_00810701];
        if (tab == 0)
            return -1;
    } else {
        return -1;
    }

    tab = &tab[id];
    voice = func_00119EA0(D_00281D50[tab->group][tab->bank], tab->scriptGroup, tab->scriptIdx);
    if (voice >= 0) {
        func_0011A270(voice, a1);
        func_0011A218(voice, a2, a3);
    }
    return voice;
}
