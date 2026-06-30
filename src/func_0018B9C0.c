// NEARMISS func_0018B9C0  (vram 0x0018B9C0, 0x25C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 84.56% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring + branch-likely-lowering scheduling permuter class. Logic fully recovered; the outer switch's case-0 entry wants beql v1,zero with the `st+1` store in the delay slot (target emits branch-likely, mwcc 2.3.3 emits plain beq), plus the input-bits OR operand-evaluation order and two...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS (best: 84.6% mwcc 2.3.3, 74.6% mwcc 991202). Logic fully recovered.
// Per-frame state driver dispatched on the state byte at arg0+0. First decays the
// global cooldown D_008106EF and ORs the input-bits byte *0x700031F0 into arg0+0x8B.
// State 0 (one-shot init): advances state to 1, zeroes the camera/aim block
// D_008105F0..FC (0,-1.0f,0,1.0f), copies the float at arg0+0x64 into arg0+0xC,
// clears several flags/fields, seeds defaults (0x54=1000.0f, 0x50=-200.0f,
// 0x5C=2.0f, 0x8C=6.0f as raw bit patterns), recomputes the aim scalar via
// func_001B1240(&D_008105D0, ...) into arg0+0x44, then func_0018CE60(self, base+0xB0,
// 5). If field5==1 or field6 in {0xA,0xF,0xD} it bails; otherwise sets field6=8
// (or 0 when D_00810700==0x12 && D_00810702==0), runs func_0018C0C0/D7B0/C0D0 and
// recomputes the aim scalar again. State 1: sub-dispatch on field4 -- 3 ->
// func_0022EEF0+C0D0(0); 0 -> func_00191390+0018BC20(both with base=&D_008102B0)+C0D0(1);
// 1/2/default -> C0D0(1). base = &D_008102B0 is hoisted to a callee-saved reg.
//
// Residual wall (NOT the clean-store nop): the outer switch's case-0 entry wants a
// branch-likely (beql v1,0) filling its delay slot with the `st+1` store, and several
// stores/the input-OR operand differ only in mwcc instruction scheduling / operand
// evaluation order. Register-coloring + branch-likely-lowering permuter class.
extern void func_0018BC20(char *p, void *q);
extern void func_0018C0C0(char *p);
extern void func_0018C0D0(char *p, int a);
extern void func_0018CE60(char *p, void *q, int n);
extern void func_0018D7B0(char *p, int a);
extern void func_00191390(char *p, void *q);
extern float func_001B1240(void *p, float a, float b);
extern void func_0022EEF0(char *p, int a);
extern char D_008102B0;
extern char D_008105D0;
extern float D_008105E0;
extern float D_008105E8;
extern int D_008105F0;
extern int D_008105F4;
extern int D_008105F8;
extern int D_008105FC;
extern unsigned char D_008106EF;
extern unsigned char D_00810700;
extern unsigned char D_00810702;

void func_0018B9C0(char *arg0) {
    char *base = &D_008102B0;
    unsigned char st;
    unsigned char sub;

    if (D_008106EF != 0) {
        D_008106EF -= 1;
    }
    *(unsigned char *)(arg0 + 0x8B) = *(unsigned char *)(arg0 + 0x8B) | *(unsigned char *)0x700031F0;
    st = *(unsigned char *)(arg0 + 0);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 0) = st + 1;
        D_008105F0 = 0;
        D_008105F4 = 0xBF800000;
        D_008105F8 = 0;
        D_008105FC = 0x3F800000;
        *(float *)(arg0 + 0xC) = *(float *)(arg0 + 0x64);
        *(unsigned char *)(arg0 + 4) = 0;
        *(char *)(arg0 + 7) = 0;
        *(int *)(arg0 + 0x90) = 0;
        *(int *)(arg0 + 0x54) = 0x447A0000;
        *(int *)(arg0 + 0x50) = 0xC3480000;
        *(int *)(arg0 + 0x5C) = 0x40000000;
        *(int *)(arg0 + 0x8C) = 0x40C00000;
        *(short *)(arg0 + 0x5A) = 0;
        *(char *)(arg0 + 0x6D) = 0;
        *(char *)(arg0 + 0x6C) = 0;
        *(short *)(arg0 + 0xA0) = 0;
        *(float *)(arg0 + 0x44) = func_001B1240(&D_008105D0, D_008105E0, D_008105E8);
        func_0018CE60(arg0, base + 0xB0, 5);
        sub = *(unsigned char *)(arg0 + 5);
        if (sub == 1) {
            return;
        }
        sub = *(unsigned char *)(arg0 + 6);
        if (sub == 0xA || sub == 0xF || sub == 0xD) {
            return;
        }
        *(unsigned char *)(arg0 + 6) = 8;
        if (D_00810700 == 0x12) {
            if (D_00810702 == 0) {
                *(unsigned char *)(arg0 + 6) = 0;
            }
        }
        func_0018C0C0(arg0);
        func_0018D7B0(arg0, 1);
        func_0018C0D0(arg0, 1);
        *(float *)(arg0 + 0x44) = func_001B1240(&D_008105D0, D_008105E0, D_008105E8);
        break;
    case 1:
        sub = *(unsigned char *)(arg0 + 4);
        switch (sub) {
        case 3:
            func_0022EEF0(arg0, 1);
            func_0018C0D0(arg0, 0);
            break;
        case 0:
            func_00191390(arg0, base);
            func_0018BC20(arg0, base);
            func_0018C0D0(arg0, 1);
            break;
        case 1:
        case 2:
        default:
            func_0018C0D0(arg0, 1);
            break;
        }
        break;
    }
}
