// NEARMISS func_00111680  (vram 0x00111680, 0x194 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 44.41% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// SPLIT-SYMBOL (missing symbol at 0x00111740) -- 100% is structurally impossible, not a compiler wall. The splat symbol func_00111680 is 0x194/416 bytes but contains TWO functions: the thread loop (0x00111680..0x0011173B, ends with an infinite `b`, never returns) + an alignment nop at 0x0011173C + ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

//
// SEMANTICS: SDK exception/alarm service thread + its creator.
//
// func_00111680 is the thread body: an infinite loop that signals its semaphore,
// deletes itself when D_00241D48 == 1, optionally traces, dispatches the
// registered handler D_00241D44(D_00241D4C), runs func_00111AE0 for event 11,
// and clears the "busy" flag D_00241D14 before looping.
//
// func_00111740 (address 0x00111740 -- NO SYMBOL IN symbol_addrs.txt, which is why
// the splat symbol func_00111680 is 0x194 bytes and covers both functions) fills a
// ThreadParam-shaped block at D_00279B38 (+4 entry, +8 stack, +0xC stack size,
// +0x10 $gp, +0x14 priority) with func_00111680 as the entry point, creates the
// thread and starts it; if the thread already exists it just re-dispatches it.
//

extern int D_00241CF8;
extern int D_00241D04;
extern int D_00241D10;
extern volatile int D_00241D14;
extern void (*D_00241D44)();
extern int D_00241D48;
extern int D_00241D4C;
extern char D_0026BB08[];
extern int D_00279B00;
extern int D_00279B08;
extern int D_00279B38[];
extern int D_0027D370;

extern void SignalSema();
extern void DeleteThread();
extern int ReleaseWaitThread();
extern void iReleaseWaitThread();
extern int _iSetAlarm();
extern void CreateThread();
extern void DisableDispatchThread();
extern void func_00122B58();
extern void func_00111AE0();

void func_00111680(void)
{
    for (;;) {
        SignalSema(D_00241D10);
        if (D_00241D48 == 1) {
            D_00241D14 = 0;
            DeleteThread();
        }
        if (D_00241CF8 > 0) {
            func_00122B58(D_0026BB08, D_00241D44, D_00241D4C);
        }
        if (D_00241D44 != 0 && D_00241D4C != 0) {
            D_00241D44(D_00241D4C);
        }
        if (D_00241D4C == 11) {
            func_00111AE0();
        }
        D_00241D14 = 0;
    }
}

int func_00111740(int prio, int stack, int stack_size)
{
    if (D_00241D04 != 0) {
        DisableDispatchThread(D_00241D04, prio);
        return 0;
    }
    D_00279B00 = ReleaseWaitThread(D_00241D04);
    iReleaseWaitThread(D_00279B00, &D_00279B08);
    D_00279B38[1] = (int)func_00111680;
    D_00279B38[2] = stack;
    D_00279B38[3] = stack_size;
    D_00279B38[4] = (int)&D_0027D370;
    D_00279B38[5] = prio;
    D_00241D04 = _iSetAlarm(D_00279B38);
    CreateThread(D_00241D04, 0);
    return 1;
}
