// NEARMISS func_00159210  (vram 0x00159210, 0x408 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.77% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Pure register-allocation-ORDER permutation, 6 of 258 instructions, confined to the D_00810841[D_00810700] bit test at 0xD4..0xF4. Instruction SEQUENCE is identical; only the 4 temporaries are permuted: target {index=a2, table-base=a1, shift-amount=a0, const-1=v1}, mwcc233 {index=a0, table-base=v1...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// SEMANTICS: Per-frame tick for a scripted actor; `q = p + 0x1F0` is its animation/
//   sub-object. Outer state is the byte p[4]:
//     0 - spawn/enter. func_001B0FD0(p,q,p[4],2) gates it; on 0 it runs func_001C6380(p),
//         clears the pending-request pointer p+0x20 and p[0xA], then branches on the
//         actor type byte p[3]:
//           type 0x2C -> vtable p+0x30 = &D_00275478, p[5]=0, p[0]=1, load the 2x2
//                        transform at 0x700038A0 with {0, 1.0f, 0, 1.0f} (raw bit
//                        patterns 0x00000000 / 0x3F800000) and
//                        p+0x20 = func_001C5570(p, D_700038A0, 0x74, 1).
//           otherwise  -> vtable p+0x30 = &D_00275480; if bit (p[0x2E]) of the
//                        per-stage mask byte D_00810841[D_00810700] is set, park the
//                        actor (p[0]=2, p[5]=3); else pick the variant count
//                        p[0x34] = 2 / 4 / 6 for type 0x24 / 0x25 / other, set p[0]=1,
//                        load the transform with {1.0f, 0, 0, 1.0f} and
//                        p+0x20 = func_001C5570(p, D_700038A0, 0x75, 1).
//     1 - running. Sub-state byte p[5] dispatches through jtbl_0026D350 (7 entries;
//         case 3 is empty, >=7 skips straight to the tail):
//           0 - r = func_00157860(p,q,0); r==2 -> p[5]=4, r==3 -> p[5]++, r!=0 other
//               -> p[5]=5, r==0 -> stay.
//           1 - if p[0xA]==0: p[5]=4 and play D_002470E0 (type 0x2C) or D_00247DA0;
//               else p[5]=sub+1, play D_00247BA0 and poke func_001BA1F0(p).
//           2 - when func_001BA1F0(p) reports done: p[0xB]=0; type 0x2C -> p[0]=1,
//               p[5]=0; otherwise p[0]=2, p[5]++, and if a request object is pending at
//               p+0x20 mark it finished (obj[4]=3) and clear the slot.
//           4 - when done: p[0]=1, p[0xB]=0, p[5]=0.
//           5 - when done: p[5]=6.
//           6 - as case 1 but the "busy" arm sets p[5]=2 and plays D_00247BE0.
//         The tail always runs func_001B17A0(p) then the virtual tick (*(p+0x4C))(p).
//     2 - p[4]++ (advance to state 3).
//     3 - func_001AFC10(p, q, p[4]); no tail work.
//     other - nothing.
//   All animation plays go through func_001BA1A0(q, clip, sub, 2).

extern unsigned char D_00810700[16];
extern unsigned char D_00810841[256];
extern int D_00275478;
extern int D_00275480;
extern int D_002470E0[4];
extern int D_00247BA0[4];
extern int D_00247BE0[4];
extern int D_00247DA0[4];
extern int D_700038A0[4];

extern int func_00157860(char *, char *, int);
extern void func_001AFC10(char *, char *, int);
extern int func_001B0FD0(char *, char *, int, int);
extern void func_001B17A0(char *);
extern void func_001BA1A0(char *, int, int, int);
extern int func_001BA1F0(char *);
extern char *func_001C5570(char *, int *, int, int);
extern void func_001C6380(char *);

void func_00159210(char *p) {
    char *q;
    char *r;
    int tn;
    unsigned char st;
    unsigned char sub;
    int v;

    q = p + 0x1F0;
    st = *(unsigned char *)(p + 4);
    switch (st) {
    case 0:
        if (func_001B0FD0(p, q, st, 2) == 0) {
            func_001C6380(p);
            *(int *)(p + 0x20) = 0;
            *(char *)(p + 0xA) = 0;
            if (*(unsigned char *)(p + 3) == 0x2C) {
                *(int *)(p + 0x30) = (int)&D_00275478;
                *(char *)(p + 5) = 0;
                *(char *)(p + 0) = 1;
                *(volatile int *)0x700038A0 = 0;
                *(volatile int *)0x700038A4 = 0x3F800000;
                *(volatile int *)0x700038A8 = 0;
                *(volatile int *)0x700038AC = 0x3F800000;
                *(char **)(p + 0x20) = func_001C5570(p, D_700038A0, 0x74, 1);
            } else {
                *(int *)(p + 0x30) = (int)&D_00275480;
                if ((1 << *(unsigned short *)(p + 0x2E)) & D_00810841[D_00810700[0]]) {
                    *(char *)(p + 0) = 2;
                    *(char *)(p + 5) = 3;
                } else {
                    if (*(unsigned char *)(p + 3) == 0x24) {
                        *(short *)(p + 0x34) = 2;
                    } else if (*(unsigned char *)(p + 3) == 0x25) {
                        *(short *)(p + 0x34) = 4;
                    } else {
                        *(short *)(p + 0x34) = 6;
                    }
                    *(char *)(p + 0) = 1;
                    *(volatile int *)0x700038A0 = 0x3F800000;
                    *(volatile int *)0x700038A4 = 0;
                    *(volatile int *)0x700038A8 = 0;
                    *(volatile int *)0x700038AC = 0x3F800000;
                    *(char **)(p + 0x20) = func_001C5570(p, D_700038A0, 0x75, 1);
                }
            }
        }
        break;
    case 1:
        sub = *(unsigned char *)(p + 5);
        switch (sub) {
        case 0:
            v = func_00157860(p, q, 0);
            if (v != 0) {
                if (v == 2) {
                    *(char *)(p + 5) = 4;
                } else if (v == 3) {
                    *(unsigned char *)(p + 5) = *(unsigned char *)(p + 5) + 1;
                } else {
                    *(char *)(p + 5) = 5;
                }
            }
            break;
        case 1:
            if (*(unsigned char *)(p + 0xA) == 0) {
                *(char *)(p + 5) = 4;
                if (*(unsigned char *)(p + 3) == 0x2C) {
                    tn = (int)D_002470E0;
                    func_001BA1A0(q, tn, sub, 2);
                } else {
                    tn = (int)D_00247DA0;
                    func_001BA1A0(q, tn, sub, 2);
                }
            } else {
                *(unsigned char *)(p + 5) = sub + 1;
                tn = (int)D_00247BA0;
                func_001BA1A0(q, tn, sub, 2);
                func_001BA1F0(p);
            }
            break;
        case 2:
            if (func_001BA1F0(p) != 0) {
                *(char *)(p + 0xB) = 0;
                if (*(unsigned char *)(p + 3) == 0x2C) {
                    *(char *)(p + 0) = 1;
                    *(char *)(p + 5) = 0;
                } else {
                    *(char *)(p + 0) = 2;
                    *(unsigned char *)(p + 5) = *(unsigned char *)(p + 5) + 1;
                    r = *(char **)(p + 0x20);
                    if (r != 0) {
                        *(char *)(r + 4) = 3;
                        *(char **)(p + 0x20) = 0;
                    }
                }
            }
            break;
        case 3:
            break;
        case 4:
            if (func_001BA1F0(p) != 0) {
                *(char *)(p + 0) = 1;
                *(char *)(p + 0xB) = 0;
                *(char *)(p + 5) = 0;
            }
            break;
        case 5:
            if (func_001BA1F0(p) != 0) {
                *(char *)(p + 5) = 6;
            }
            break;
        case 6:
            if (*(unsigned char *)(p + 0xA) == 0) {
                *(char *)(p + 5) = 4;
                if (*(unsigned char *)(p + 3) == 0x2C) {
                    tn = (int)D_002470E0;
                    func_001BA1A0(q, tn, sub, 2);
                } else {
                    tn = (int)D_00247DA0;
                    func_001BA1A0(q, tn, sub, 2);
                }
            } else {
                *(unsigned char *)(p + 5) = 2;
                tn = (int)D_00247BE0;
                func_001BA1A0(q, tn, sub, 2);
                func_001BA1F0(p);
            }
            break;
        }
        func_001B17A0(p);
        (*(void (**)(char *))(p + 0x4C))(p);
        break;
    case 2:
        *(char *)(p + 4) = st + 1;
        break;
    case 3:
        func_001AFC10(p, q, st);
        break;
    }
}
