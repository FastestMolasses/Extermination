// NEARMISS func_001ACA20  (vram 0x001ACA20, 0x450 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.67% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring wall: the target keeps slot in a1 and the precomputed &slot[9] pointer in a0 across the whole function (never promoting either to a callee-saved reg), while mwcc233 assigns them the other way round in one internal spot causing a v0/v1 swap in the mode==1 (D_00275BD4) global-seed...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// Top-level game-task state machine dispatched on the byte at slot[9]
// (slot = *(void**)0x70003B6C, the current frame-task slot pointer -- same
// slot object used by the sibling task functions func_001AD1A0/
// func_001ACEC0/func_001ADF50/func_001B5BC0). All paths that fall through
// return 0 via the shared tail; only the "still loading" (D_00810E70 flag)
// and "done" gates return 2/3/4 directly.
//
// state 0: tear down (func_001D1EF0); if the loading-flags word has any of
//   bits 0x9F0 set, bail with 2. Else bump slot[9], reset the module
//   (func_001AF2C0), set gp-rel flags D_00275BE0=2 / D_00275BD8=1, and kick
//   func_001FF080(0, 3).
// state 1: tear down (func_001D1EF0); once D_00275BD8 clears and the
//   loading flags are clear, resolve the level (func_00200830(D_0028A564),
//   func_001D19D0()), bump slot[9], clear slot[0xA]/slot[0xB], set
//   D_00810C61[0]=1, then branch on the game-mode selector D_00275BD4 to seed
//   a batch of D_008107xx/D_00810Cxx globals for mode 0 / 1 / other.
// state 2: once func_001ADF50() signals done, bump slot[9], clear
//   slot[0xA], set D_00275BD8=1, kick func_001FF080(0, 0x36).
// state 3: once D_00275BD8 clears and the loading flags are clear, bump
//   slot[9] and clear slot[0xA]/slot[0xB].
// state 4: sub-dispatch on slot[0xB] --
//   sub 0: gated on the loading flags; primes the HUD/session
//     (func_00122BA8(0x45)), clears the two scratch words 0x70003B68/64,
//     bumps slot[0xB], clears slot[0x1A]/slot[0x16], and kicks the whole
//     "enter play" fanout (func_001AFCA0/CF0/func_001B07C0(0)/func_001B6990/
//     func_001D19E0/func_001C1DC0/func_00199C50/func_001AEE10(4,0)/
//     func_001C5C50/func_001D1EF0()).
//   sub 1: once the frame counter slot[0x16] hits 0xE10 return 3 (done);
//     else, gated on the loading flags, advance one config record
//     (func_001B5BC0(slot[0x16])), bump slot[0x16]; if func_001ACE70()
//     signals return 2; else dispatch func_001AE5E0()/func_001AE6B0() by
//     the byte at 0x70003B8D and, once D_008106B9[0] (or D_008106B8[0]) is set
//     with D_0028A9A0==2, return 3.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build
// fills the shared-tail/clean-store delay slots differently.
extern void func_00122BA8(int a0);
extern void func_00199C50(void);
extern int func_001ACE70(void);
extern int func_001ADF50(void);
extern void func_001AE5E0(void);
extern void func_001AE6B0(void);
extern void func_001AEE10(short a0, unsigned char a1);
extern void func_001AF2C0(void);
extern void func_001AFCA0(void);
extern void func_001AFCF0(void);
extern void func_001B07C0(int arg0);
extern void func_001B5BC0(int a0);
extern void func_001B6990(void);
extern void func_001C1DC0(void);
extern void func_001C5C50(void);
extern void func_001D19D0(void);
extern void func_001D19E0(void);
extern void func_001D1EF0(void);
extern void func_001FF080(int a0, int a1);
extern void func_00200830(int a0);
extern int D_00275BD4;
extern unsigned char D_00275BD8;
extern signed char D_00275BE0;
extern int D_0028A564[8];
extern short D_0028A9A0[64];
extern unsigned char D_008106B8[16];
extern unsigned char D_008106B9[16];
extern signed char D_00810700[16];
extern signed char D_00810701[16];
extern signed char D_00810702[16];
extern signed char D_00810774[16];
extern signed char D_0081079A[16];
extern signed char D_008107F4[16];
extern signed char D_00810C61[16];
extern signed char D_00810C65[16];
extern signed char D_00810C68[16];
extern signed char D_00810C7E[16];
extern signed char D_00810CA6[16];
extern short D_00810CA8[16];
extern short D_00810CAC[16];
extern unsigned short D_00810E70[8];

int func_001ACA20(void) {
    unsigned char *statep;
    unsigned char *slot;

    slot = *(unsigned char **)0x70003B6C;
    statep = slot + 9;
    switch (*statep) {
    case 0:
        func_001D1EF0();
        if (D_00810E70[0] & 0x9F0) {
            return 2;
        }
        (*(unsigned char **)0x70003B6C)[9] = (*(unsigned char **)0x70003B6C)[9] + 1;
        func_001AF2C0();
        D_00275BE0 = 2;
        D_00275BD8 = 1;
        func_001FF080(0, 3);
        break;
    case 1:
        func_001D1EF0();
        if (D_00275BD8 == 0) {
            if (D_00810E70[0] & 0x9F0) {
                return 2;
            }
            func_00200830(D_0028A564[0]);
            func_001D19D0();
            (*(unsigned char **)0x70003B6C)[9] = (*(unsigned char **)0x70003B6C)[9] + 1;
            (*(unsigned char **)0x70003B6C)[0xA] = 0;
            (*(unsigned char **)0x70003B6C)[0xB] = 0;
            D_00810C61[0] = 1;
            if (D_00275BD4 == 0) {
                D_00810700[0] = 6;
            } else if (D_00275BD4 == 1) {
                D_00810700[0] = 0xD;
                D_00810702[0] = 8;
                D_00810C7E[0] = 1;
                D_00810C68[0] = 1;
                D_00810CAC[0] = 0xA;
                D_00810CA6[0] = 4;
                D_00810774[0] = -1;
                D_008107F4[0] = -1;
                D_0081079A[0] = -1;
            } else {
                D_00810700[0] = 3;
                D_00810701[0] = 1;
                D_00810C65[0] = 1;
                D_00810CA6[0] = 1;
                D_00810CA8[0] = 0x28;
            }
        }
        break;
    case 2:
        if (func_001ADF50()) {
            (*(unsigned char **)0x70003B6C)[9] = (*(unsigned char **)0x70003B6C)[9] + 1;
            (*(unsigned char **)0x70003B6C)[0xA] = 0;
            D_00275BD8 = 1;
            func_001FF080(0, 0x36);
        }
        break;
    case 3:
        if (D_00275BD8 == 0) {
            if (D_00810E70[0] & 0x9F0) {
                return 2;
            }
            *statep = *statep + 1;
            (*(unsigned char **)0x70003B6C)[0xA] = 0;
            (*(unsigned char **)0x70003B6C)[0xB] = 0;
        }
        break;
    case 4:
        switch (slot[0xB]) {
        case 0:
            if (D_00810E70[0] & 0x9F0) {
                return 2;
            }
            func_00122BA8(0x45);
            *(int *)0x70003B68 = 0;
            *(int *)0x70003B64 = 0;
            slot = *(unsigned char **)0x70003B6C;
            slot[0xB] = slot[0xB] + 1;
            *(short *)(slot + 0x1A) = 0;
            *(unsigned short *)(slot + 0x16) = 0;
            func_001AFCA0();
            func_001AFCF0();
            func_001B07C0(0);
            func_001B6990();
            func_001D19E0();
            func_001C1DC0();
            func_00199C50();
            func_001AEE10(4, 0);
            func_001C5C50();
            func_001D1EF0();
            break;
        case 1:
            if ((int)*(unsigned short *)(slot + 0x16) >= 0xE10) {
                return 3;
            }
            if (D_00810E70[0] & 0x9F0) {
                return 2;
            }
            func_001B5BC0(*(unsigned short *)(slot + 0x16));
            slot = *(unsigned char **)0x70003B6C;
            *(unsigned short *)(slot + 0x16) = *(unsigned short *)(slot + 0x16) + 1;
            if (func_001ACE70()) {
                return 2;
            }
            if (*(unsigned char *)0x70003B8D == 0) {
                func_001AE5E0();
            } else {
                func_001AE6B0();
            }
            if (D_008106B9[0] != 0) {
                if (D_0028A9A0[0] == 2) {
                    return 3;
                }
            } else if (D_008106B8[0] != 0 && D_0028A9A0[0] == 2) {
                return 3;
            }
            break;
        }
        break;
    }
    return 0;
}
