// NEARMISS func_001AE5E0  (vram 0x001AE5E0, 0xC4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 89.35% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body fully recovered. Sole residual is a pure list-scheduler permutation of the two independent counter increments at the head of the function: the target stores D_00810750+1 first and sinks the 0x70003B68 store into the func_001CB590 jal delay slot (computing the two +1 values in a3-then-v1 orde...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// NEARMISS (89.3% mwcc 2.3.3; 76.2% 991202) -- body/logic correct, sole residual
// is a two-store instruction-scheduling permutation (see wall note).
//
// Frame/level init routine. Increments the level counter D_00810750 and the
// device/DMA register at 0x70003B68, then memset-clears two work buffers and
// runs the init sequence:
//   func_001CB590(&D_008102B0, 0x320, D_008102B9, oldcount)  -- clear buf A
//   func_0015BCF0(D_00275B44); func_001CB5A0(); func_001D1C50();
//   func_001C1D00(&D_008101D0); func_001AFD70(0); func_0015C160(); func_001F0360();
//   func_001CB590(&D_008101E0, 0xD0, 0, 0)                   -- clear buf B
//   func_0018B9C0(D_00275B44); func_001CB5A0(); func_001AAD00();
//   func_001D1EA0(1)
// -sdatathreshold 4 keeps D_00275B44 gp-relative; the far 0x810xxx buffers are
// over-declared as arrays for absolute %hi/%lo addressing (idiom #20).
extern int func_001CB590(void *buf, int size, unsigned char c, int n);
extern void func_0015BCF0(int a);
extern void func_001CB5A0(void);
extern void func_001D1C50(void);
extern void func_001C1D00(void *p);
extern void func_001AFD70(int a);
extern void func_0015C160(void);
extern void func_001F0360(void);
extern void func_0018B9C0(int a);
extern void func_001AAD00(void);
extern void func_001D1EA0(int a);

extern int D_00275B44;
extern unsigned char D_008101D0[16];
extern unsigned char D_008101E0[16];
extern unsigned char D_008102B0[0x320];
extern unsigned char D_008102B9[16];
extern int D_00810750[2];

void func_001AE5E0(void) {
    int n;

    n = D_00810750[0];
    *(int *)0x70003B68 = *(int *)0x70003B68 + 1;
    D_00810750[0] = n + 1;
    func_001CB590(D_008102B0, 0x320, D_008102B9[0], n);
    func_0015BCF0(D_00275B44);
    func_001CB5A0();
    func_001D1C50();
    func_001C1D00(D_008101D0);
    func_001AFD70(0);
    func_0015C160();
    func_001F0360();
    func_001CB590(D_008101E0, 0xD0, 0, 0);
    func_0018B9C0(D_00275B44);
    func_001CB5A0();
    func_001AAD00();
    func_001D1EA0(1);
}
