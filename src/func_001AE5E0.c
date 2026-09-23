// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// MATCH NOTE (m2-matching lane, 89.35% -> 100.0%): (1) the scratchpad frame
// counter 0x70003B68 is a relocated extern (idiom-32), over-declared as an
// array so -sdatathreshold 4 keeps it absolute; (2) func_001CB590 is
// UNPROTOTYPED because the second call passes three arguments: the target sets
// $a0-$a2 for it and leaves $a3 as whatever func_001F0360 returned with. With a
// four-argument prototype mwcc emitted an extra a3 = 0 and scheduled
// the two counter stores differently.
//
// THE GAMEPLAY FRAME (FINDINGS "ENGINE FRAME ANATOMY"), run once per frame by
// anim_frame_top_b state 1 when scratchpad selector 0x70003B8D == 0. NOT a
// level init and it clears no buffers. Increments the gameplay frame counters
// D_00810750 and scratchpad 0x70003B68, then:
//   func_001CB590(&D_008102B0, 0x320, D_008102B9, oldcount) -- actor-context
//       begin: func_001CB590 only stores its first argument to D_00275B44 /
//       D_00275B48 and tail-calls anim_bone_array_setup (D_00275B40 =
//       base + 0x110); the other arguments are unused
//   func_0015BCF0(D_00275B44) player actor update; func_001CB5A0() (empty);
//   func_001D1C50(); func_001C1D00(&D_008101D0); func_001AFD70(0);
//   func_0015C160(); func_001F0360();
//   func_001CB590(&D_008101E0, 0xD0, 0) -- context = the camera block ($a3 unset)
//   func_0018B9C0(D_00275B44) camera state machine; func_001CB5A0();
//   func_001AAD00(); func_001D1EA0(1)
// -sdatathreshold 4 keeps D_00275B44 gp-relative; the far 0x810xxx buffers are
// over-declared as arrays for absolute %hi/%lo addressing (idiom #20).
/* Unprototyped: the two calls pass four and three arguments (see below). */
extern int func_001CB590();
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
extern int D_70003B68[4];

void func_001AE5E0(void) {
    int n;

    n = D_00810750[0];
    D_00810750[0] = n + 1;
    D_70003B68[0] = D_70003B68[0] + 1;
    func_001CB590(D_008102B0, 0x320, D_008102B9[0], n);
    func_0015BCF0(D_00275B44);
    func_001CB5A0();
    func_001D1C50();
    func_001C1D00(D_008101D0);
    func_001AFD70(0);
    func_0015C160();
    func_001F0360();
    func_001CB590(D_008101E0, 0xD0, 0);
    func_0018B9C0(D_00275B44);
    func_001CB5A0();
    func_001AAD00();
    func_001D1EA0(1);
}
