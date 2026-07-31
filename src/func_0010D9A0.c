// NEARMISS func_0010D9A0  (vram 0x0010D9A0, ?) — readable decompilation, NOT byte-identical.
//
// objdiff 94.11% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc GPR-coloring + list-scheduler wall. Body/structure FULLY recovered: .text size is byte-exact (668 == 668, 171 instrs vs 171), the whole frame (0x60, s0-s4 saved at the exact offsets), both di/sync.p/mfc0 spin sections, both zero loops (incl. the unfolded lui/addiu/addiu-0x7c strength-reduce...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

//
// SEMANTICS: SIF (EE<->IOP) subsystem one-shot initialisation (Sony SDK
// "lowmem" region, vram 0x0010D9A0).  Idempotent: guarded by D_00241C78.
//
//   * Spins with interrupts disabled until COP0 Status.EIE(0x10000) is clear,
//     then takes the "already initialised" exit if D_00241C78 != 0.
//   * Publishes the SIF command queue descriptor at D_00277218:
//       f00 = D_00277140 | 0x20000000   (uncached mirror, receive buffer)
//       f04 = D_002771C0 | 0x20000000   (uncached mirror, packet buffer)
//       f0C = D_00277240                (32-entry command handler table)
//       f10 = 0x20                      (table capacity)
//       f1C = D_00277340                (32-word scratch/status array)
//   * Clears the 32-entry handler table (2 words each) and the 32-word array.
//   * Installs the two built-in handlers func_0010D948 / func_0010D928 in
//     slots 0 and 1, both with D_00277218 as their user argument.
//   * Re-enables interrupts, releases DMAC channel 0, force-clears the
//     0x20 bit of D_STAT (0x1000E010) if set, installs the PGIF handler when
//     the SBUS control word (0x1000C000) bit 8 is clear, swaps the INTC cause-5
//     (VBLANK-start) handler for func_0010DEB8 and re-arms it via func_0010C3C8.
//   * Asks the IOP for its SIF handshake word (RFU116(0x80000000)).  If the
//     IOP is already up, hands the "EE ready" packet straight to func_0010DE38.
//     Otherwise it waits for RFU116(4) bit 17, registers the two VSync flags
//     and sends the 0x80000002 variant of the packet.
//
// Both exits are tail calls (`j func_0010DE38`), which ee-gcc 2.9 emits on its
// own for a discarded final call.
//

typedef struct SifQ {
    void *f00;
    void *f04;
    int f08;
    void *f0C;
    int f10;
    int f14;
    int f18;
    void *f1C;
} SifQ;

typedef struct Chan {
    int f00;
    int f04;
    int f08;
    int f0C;
    void *f10;
} Chan;

extern int D_00241C78;
extern char D_00277140[];
extern char D_002771C0[];
extern Chan D_00277200;
extern int D_00277214;
extern SifQ D_00277218;
extern int D_00277240[64];
extern int D_00277340[32];

extern void func_0010D928(void);
extern void func_0010D948(void);
extern void func_0010DEB8(void);
extern int DisableDmacHandler(int chan);
extern void SetPgifHandler(void);
extern int RemoveIntcHandler(int cause, void *handler, int arg);
extern int func_0010C3C8(int cause);
extern unsigned int RFU116(unsigned int arg);
extern void SetVSyncFlag(unsigned int cmd, void *p);
extern int func_0010DE38(unsigned int cmd, void *pkt, int size, void *src, void *dest, int len);

void func_0010D9A0(void)
{
    int *p;
    int i;
    unsigned int st;
    int r;

    do {
        __asm__ __volatile__("di");
        __asm__ __volatile__("sync.p");
        __asm__ __volatile__("mfc0 %0, $12" : "=r" (st));
    } while (st & 0x10000);

    if (D_00241C78 != 0) {
        __asm__ __volatile__("ei");
        return;
    }

    D_00241C78 = 1;
    D_00277218.f00 = (void *)((unsigned int)D_00277140 | 0x20000000);
    D_00277218.f1C = D_00277340;
    D_00277218.f04 = (void *)((unsigned int)D_002771C0 | 0x20000000);
    D_00277218.f10 = 0x20;
    D_00277218.f08 = 0;
    D_00277218.f0C = D_00277240;
    D_00277218.f14 = 0;
    D_00277218.f18 = 0;

    p = D_00277240;
    for (i = 31; i >= 0; i--) {
        p[0] = 0;
        p[1] = 0;
        p += 2;
    }

    for (i = 31; i >= 0; i--) {
        D_00277340[i] = 0;
    }

    D_00277240[0] = (int)func_0010D948;
    i = 0x20;
    D_00277240[2] = (int)func_0010D928;
    D_00277240[1] = (int)&D_00277218;
    D_00277240[3] = (int)&D_00277218;

    __asm__ __volatile__("ei");

    DisableDmacHandler(0);

    if (*(volatile unsigned int *)0x1000E010 & 0x20) {
        *(volatile unsigned int *)0x1000E010 = i;
    }
    if (!(*(volatile unsigned int *)0x1000C000 & 0x100)) {
        SetPgifHandler();
    }
    D_00277214 = RemoveIntcHandler(5, (void *)func_0010DEB8, 0);
    func_0010C3C8(5);

    r = RFU116(0x80000000);
    D_00277218.f08 = r;
    if (r != 0) {
        D_00277200.f10 = D_00277140;
        func_0010DE38(0x80000000, &D_00277200, 0x14, 0, 0, 0);
        return;
    }

    while (!(RFU116(4) & 0x20000)) {
        ;
    }
    r = RFU116(2);
    D_00277218.f08 = r;
    SetVSyncFlag(0x80000000, (void *)r);
    SetVSyncFlag(0x80000001, &D_00277218);
    D_00277200.f10 = D_00277140;
    D_00277200.f0C = 0;
    func_0010DE38(0x80000002, &D_00277200, 0x14, 0, 0, 0);
}
