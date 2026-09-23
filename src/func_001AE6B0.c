// NEARMISS func_001AE6B0  (vram 0x001AE6B0, 0x130 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.82% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// Only residual: the expected object renders scratchpad 0x70003B68,0x70003B84,0x70003B91,0x70003B92 as literal operands (not in build.py _SPAD_SYMS) while this C uses relocated externs; linked bytes identical. objdiff 100.0% with _SPAD_SYMS += 0x70003B68,0x70003B84,0x70003B91,0x70003B92.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// MATCH NOTE (m1-firstlevel-matching lane): the scratchpad globals at
// 0x70003B68, 0x70003B84, 0x70003B91 and 0x70003B92 are referenced as relocated
// externs (D_7000xxxx), as the original build did (see
// tools/match/spad_symbolize.py). With them, the two counter increments need no
// ordering device. objdiff is 100.0% once build.py _SPAD_SYMS lists these
// addresses (so the expected object carries the same relocations); the linked
// bytes are identical.

//
// Level/frame init routine (sibling of func_001AE5E0). If the scratchpad gate
// byte D_70003B92 is set, increments the halfword D_70003B84. If the scratchpad
// mode byte D_70003B91 == 1 and the sequence globals say ready (D_0028A9A0 == 0 and
// D_00810E74 & 0x900), advances that mode byte to 2. Then bumps the level
// counter D_00810750 and the scratchpad counter D_70003B68 and runs the
// init sequence: func_001D1C50(); func_001C1D00(&D_008101D0); func_001AFD70(1);
// func_001F0360(); func_001CB590(&D_008102B0, 0x320, D_008102B9);
// func_0015BCF0(D_00275B44); func_001CB5A0(); func_001AFD70(2);
// func_0015C160(); func_001CB590(&D_008101E0, 0xD0, 0);
// func_0018B9C0(D_00275B44); func_001CB5A0(); func_001AAD00();
// func_001D1EA0(1).
// -sdatathreshold 4 keeps the int D_00275B44 gp-relative; the far 0x28xxxx /
// 0x810xxx globals are over-declared as arrays to force absolute %hi/%lo
// addressing.
extern int D_70003B68[16];
extern unsigned short D_70003B84[16];
extern unsigned char D_70003B91[16];
extern unsigned char D_70003B92[16];
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

    if (D_70003B92[0] != 0) {
        D_70003B84[0] = D_70003B84[0] + 1;
    }
    if (D_70003B91[0] == 1 && D_0028A9A0[0] == 0 && (D_00810E74[0] & 0x900)) {
        D_70003B91[0] = 2;
    }
    D_00810750[0]++;
    D_70003B68[0]++;
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
