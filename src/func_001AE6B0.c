// NEARMISS func_001AE6B0  (vram 0x001AE6B0, 0x130 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.25% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// List-scheduler / register-allocation artifact (permuter territory). Body and logic fully recovered. Two residuals, both pure codegen artifacts no source change fixed across ~6 attempts: (1) the two independent counter increments at the head -- D_00810750 (stored first) and the 0x70003B68 device r...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// NEARMISS (94.25% mwcc 2.3.3 and 991202) -- body/logic correct; sole residuals
// are a list-scheduler/register-allocation permutation of the two head counter
// increments plus the first-if empty delay-slot nop (see wall note). Same
// artifact class as the committed sibling NEARMISS func_001AE5E0.
//
// Level/frame init routine (sibling of func_001AE5E0). If the gate byte at
// 0x70003B92 is set, increments the halfword at 0x70003B84. If the mode byte at
// 0x70003B91 == 1 and the sequence globals say ready (D_0028A9A0 == 0 and
// D_00810E74 & 0x900), advances that mode byte to 2. Then bumps the level
// counter D_00810750 and the device/DMA register at 0x70003B68 and runs the
// init sequence: func_001D1C50(); func_001C1D00(&D_008101D0); func_001AFD70(1);
// func_001F0360(); func_001CB590(&D_008102B0, 0x320, D_008102B9);
// func_0015BCF0(D_00275B44); func_001CB5A0(); func_001AFD70(2);
// func_0015C160(); func_001CB590(&D_008101E0, 0xD0, 0);
// func_0018B9C0(D_00275B44); func_001CB5A0(); func_001AAD00();
// func_001D1EA0(1).
// -sdatathreshold 4 keeps the int D_00275B44 gp-relative; the far 0x28xxxx /
// 0x810xxx globals are over-declared as arrays to force absolute %hi/%lo
// addressing.
extern int func_0015BCF0(int);
extern void func_0015C160(void);
extern void func_0018B9C0(int);
extern void func_001AAD00(void);
extern void func_001AFD70(int);
extern void func_001C1D00(void *);
extern void func_001CB590(void *, int, unsigned char);
extern void func_001CB5A0(void);
extern void func_001D1C50(void);
extern void func_001D1EA0(int);
extern void func_001F0360(void);
extern int D_00275B44;
extern short D_0028A9A0[8];
extern unsigned char D_008101D0[16];
extern unsigned char D_008101E0[16];
extern unsigned char D_008102B0[0x320];
extern unsigned char D_008102B9[16];
extern int D_00810750[2];
extern unsigned short D_00810E74[8];

void func_001AE6B0(void) {
    int n;

    if (*(unsigned char *)0x70003B92 != 0) {
        *(unsigned short *)0x70003B84 = *(unsigned short *)0x70003B84 + 1;
    }
    if (*(unsigned char *)0x70003B91 == 1 && D_0028A9A0[0] == 0 && (D_00810E74[0] & 0x900)) {
        *(unsigned char *)0x70003B91 = 2;
    }
    n = D_00810750[0];
    *(int *)0x70003B68 = *(int *)0x70003B68 + 1;
    D_00810750[0] = n + 1;
    func_001D1C50();
    func_001C1D00(D_008101D0);
    func_001AFD70(1);
    func_001F0360();
    func_001CB590(D_008102B0, 0x320, D_008102B9[0]);
    func_0015BCF0(D_00275B44);
    func_001CB5A0();
    func_001AFD70(2);
    func_0015C160();
    func_001CB590(D_008101E0, 0xD0, 0);
    func_0018B9C0(D_00275B44);
    func_001CB5A0();
    func_001AAD00();
    func_001D1EA0(1);
}
