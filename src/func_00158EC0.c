// NEARMISS func_00158EC0  (vram 0x00158EC0, 0x348 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.67% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 99.67 with mwcc233 -O4,p -sdatathreshold 4 (o991: 86.59). Instruction SEQUENCE is byte-identical everywhere including both dispatches; the whole residual is 9 instructions of pure GPR permutation, no count/order difference: (a) the phase-0 mask test, 7 instrs, expected a2=D_00810700 index / a1=D_...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// SEMANTICS: per-frame update for one scripted-actor object `p`.  p[4] is the
// object's lifecycle phase, p[5] the sub-state of the running phase.
//   phase 0  = one-shot init.  Bails out entirely if func_001B0FD0() reports busy.
//              Otherwise binds the object (func_001C6380), points p+0x30 at the
//              shared descriptor D_00275480, clears p[0xA] and p->0x20, then tests
//              bit (p->0x2E) of the per-slot mask byte D_00810841[D_00810700]:
//                bit set  -> object is suppressed: p[0]=2, p[5]=3, done.
//                bit clear-> pick a timer from the object's kind p[3]
//                            (0x14 -> 4, 0x22 -> 0x10, other -> 0x18) into p->0x34,
//                            go live (p[0]=1), stage the 2x2 identity {1,0,0,1} in
//                            scratchpad at 0x700038A0 and hand it to func_001C5570
//                            (request 0x75); the handle is kept in p->0x20.
//   phase 1  = running.  jtbl_0026D330 dispatches p[5] (0..6); every arm falls
//              through to the common tail func_001B17A0(p) followed by the object's
//              own virtual update at p->0x4C.  p+0x1F0 is the embedded animation
//              sub-object passed to the anim helpers.
//                0: func_00157860(p, anim, 1) -> 0 nothing, 2 -> p[5]=4,
//                   3 -> p[5]++, anything else -> p[5]=5.
//                1: if p[0xA]==0 play clip D_00247DA0 and jump to p[5]=4, else set
//                   the global timer D_00247934=0x15C, play D_002478A0, p[5]++ and
//                   kick func_001BA1F0.
//                2: wait for func_001BA1F0; on completion p[0xB]=0, p[0]=2, p[5]++,
//                   and if a handle is held in p->0x20 mark it finished
//                   (byte 4 = 3) and drop it.
//                3: (empty table slot -- straight to the tail)
//                4: wait for func_001BA1F0; on completion go back to idle
//                   (p[0]=1, p[0xB]=0, p[5]=0).
//                5: wait for func_001BA1F0; on completion p[5]=6.
//                6: same shape as 1, but the "else" branch plays D_002478E0 and
//                   forces p[5]=2.
//   phase 2  = p[4]++ (advance to the teardown phase), nothing else.
//   phase 3  = func_001AFC10() (global teardown), nothing else.
//   any other phase returns immediately.
//
// NOTE (matching): the explicit `(void *)` cast on the second argument of
// func_001BA1A0 is load-bearing -- it reproduces CW's "materialise the address in a
// temp, then move it into $a1" argument setup instead of mwcc's direct
// `addiu a1, v0, %lo(...)`.  Worth 93.05 -> 99.67 on its own.

extern int  func_001B0FD0(void);
extern void func_001C6380(unsigned char *p);
extern int  func_001C5570(unsigned char *p, void *mtx, int id);
extern int  func_00157860(unsigned char *p, unsigned char *anim, int mode);
extern void func_001BA1A0(unsigned char *anim, void *clip);
extern int  func_001BA1F0(unsigned char *p);
extern void func_001B17A0(unsigned char *p);
extern void func_001AFC10(void);

extern int           D_00275480;
extern unsigned char D_00810700[8];
extern unsigned char D_00810841[];
extern int           D_00247934[2];
extern unsigned char D_00247DA0[];
extern unsigned char D_002478A0[];
extern unsigned char D_002478E0[];
extern float         D_700038A0[4];

void func_00158EC0(unsigned char *p)
{
    unsigned char *anim;
    unsigned char *clip;
    unsigned char *h;
    int r;
    int t;

    anim = p + 0x1F0;
    switch (p[4]) {
    case 0:
        if (func_001B0FD0() == 0) {
            func_001C6380(p);
            *(int **)(p + 0x30) = &D_00275480;
            p[0xA] = 0;
            *(int *)(p + 0x20) = 0;
            if (((1 << *(unsigned short *)(p + 0x2E)) & D_00810841[D_00810700[0]]) != 0) {
                p[0] = 2;
                p[5] = 3;
            } else {
                t = p[3];
                if (t == 0x14) {
                    *(short *)(p + 0x34) = 4;
                } else if (t == 0x22) {
                    *(short *)(p + 0x34) = 0x10;
                } else {
                    *(short *)(p + 0x34) = 0x18;
                }
                p[0] = 1;
                *(volatile int *)0x700038A0 = 0x3F800000;
                *(volatile int *)0x700038A4 = 0;
                *(volatile int *)0x700038A8 = 0;
                *(volatile int *)0x700038AC = 0x3F800000;
                *(int *)(p + 0x20) = func_001C5570(p, D_700038A0, 0x75);
            }
        }
        break;
    case 1:
        switch (p[5]) {
        case 0:
            r = func_00157860(p, anim, 1);
            if (r != 0) {
                if (r == 2) {
                    p[5] = 4;
                } else if (r == 3) {
                    p[5] = p[5] + 1;
                } else {
                    p[5] = 5;
                }
            }
            break;
        case 1:
            if (p[0xA] == 0) {
                p[5] = 4;
                clip = D_00247DA0;
                func_001BA1A0(anim, (void *)clip);
            } else {
                D_00247934[0] = 0x15C;
                clip = D_002478A0;
                p[5] = p[5] + 1;
                func_001BA1A0(anim, (void *)clip);
                func_001BA1F0(p);
            }
            break;
        case 2:
            if (func_001BA1F0(p) != 0) {
                p[0xB] = 0;
                p[0] = 2;
                p[5] = p[5] + 1;
                h = *(unsigned char **)(p + 0x20);
                if (h != 0) {
                    h[4] = 3;
                    *(int *)(p + 0x20) = 0;
                }
            }
            break;
        case 3:
            break;
        case 4:
            if (func_001BA1F0(p) != 0) {
                p[0] = 1;
                p[0xB] = 0;
                p[5] = 0;
            }
            break;
        case 5:
            if (func_001BA1F0(p) != 0) {
                p[5] = 6;
            }
            break;
        case 6:
            if (p[0xA] == 0) {
                p[5] = 4;
                clip = D_00247DA0;
                func_001BA1A0(anim, (void *)clip);
            } else {
                D_00247934[0] = 0x15C;
                clip = D_002478E0;
                p[5] = 2;
                func_001BA1A0(anim, (void *)clip);
                func_001BA1F0(p);
            }
            break;
        }
        func_001B17A0(p);
        (*(void (**)(unsigned char *))(p + 0x4C))(p);
        break;
    case 2:
        p[4] = p[4] + 1;
        break;
    case 3:
        func_001AFC10();
        break;
    }
}
