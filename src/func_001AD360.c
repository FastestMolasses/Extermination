// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// SPAD: 0x70003B6C
//
// Byte-matched (objdiff 100%) from the former NEARMISS body. The scratchpad pointer 0x70003B6C is a relocated extern at every access (idiom-32).

//
// SEMANTICS: game-task bring-up poll (func_001AD250 case 0 -> HERE). Dispatches
// on byte +0xA of the current task slot (slot pointer in scratchpad 0x70003B6C)
// via the 6-entry jr-table at 0x0026DCD0. Steps 0..4 each do their work and then
// advance the step byte (+0xA); step 5 does the final teardown poll and returns 4
// to tell the caller the bring-up sequence finished. Every other step (and any
// out-of-range step) returns 0 = "still working".
//   0: func_001D1EF0() (teardown) + func_001FABB0(); advance
//   1: func_001D1EF0(); gated on D_00282157 == 0 -> clear gp-rel flag D_00275C78,
//      set D_00821058 = 1, advance (otherwise stay on step 1)
//   2: func_001D1EF0(); advance
//   3: advance, clear slot+0x18 (short) and slot+0x10 (byte), FALL THROUGH into 4
//   4: func_001D2830(3, 1); D_00810700 = 0xB, D_00810701 = D_00810702 = 0,
//      D_00810730[D_00810700] = D_00810701; advance
//   5: func_001D1EF0(); return 4 (done)

extern unsigned char *volatile D_70003B6C[16];      /* PS2 scratchpad @ 0x70003B6C */

extern void func_001D1EF0(void);
extern void func_001FABB0(void);
extern void func_001D2830(int, int);
extern char D_00282157[16];
extern volatile unsigned char D_00275C78;
extern volatile unsigned char D_00821058[16];
extern volatile unsigned char D_00810700[16];
extern volatile unsigned char D_00810701[16];
extern volatile unsigned char D_00810702[16];
extern unsigned char D_00810730[64];

int func_001AD360(void) {
    unsigned char *p;
    unsigned char *slot;
    unsigned char *q;

    p = D_70003B6C[0];
    slot = p + 0xA;
    switch (p[0xA]) {
    case 0:
        func_001D1EF0();
        func_001FABB0();
        q = D_70003B6C[0];
        q[0xA] = q[0xA] + 1;
        break;
    case 1:
        func_001D1EF0();
        if (D_00282157[0] == 0) {
            q = D_70003B6C[0];
            D_00275C78 = 0;
            D_00821058[0] = 1;
            q[0xA] = q[0xA] + 1;
        }
        break;
    case 2:
        func_001D1EF0();
        q = D_70003B6C[0];
        q[0xA] = q[0xA] + 1;
        break;
    case 3:
        *slot = *slot + 1;
        *(short *)(D_70003B6C[0] + 0x18) = 0;
        *(char *)(D_70003B6C[0] + 0x10) = 0;
        /* fallthrough */
    case 4:
        func_001D2830(3, 1);
        D_00810700[0] = 0xB;
        D_00810701[0] = 0;
        D_00810702[0] = 0;
        D_00810730[D_00810700[0]] = D_00810701[0];
        q = D_70003B6C[0];
        q[0xA] = q[0xA] + 1;
        break;
    case 5:
        func_001D1EF0();
        return 4;
    }
    return 0;
}
