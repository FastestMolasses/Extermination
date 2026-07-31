// COMPILER: eegcc
// CFLAGS: -O2
//
// MPEG (IPU) error recovery. While the IPU is still busy (0x1000B020, DMA ch4
// CHCR) and the IPU control register 0x10002010 has not raised its error/BDEC
// bit, keep feeding the decoder: whenever DMA channel 5 is idle (0x1000B420)
// and no pending transfer is flagged in 0x1000B400, push the next command
// (opcode 1) through func_00109B20.
//
// Afterwards latch the IPU bit-stream position: 0x10002030 (BP register, 64
// bit) low word goes to D_00241B98, and D_00241B9C gets the number of bits
// still buffered (32 when the FIFO is empty, else 32 - (0x10002020 & 0x1F)).
//
// If the error bit IS set, report D_0026B380, push opcodes 2 and 3, then reset
// the IPU: spin with interrupts disabled until COP0 Status.EIE clears, set the
// GIF/VIF path-3 mask bit (0x1000F590 from 0x1000F520 | 0x10000), stop DMA
// channel 0 (0x1000B000), clear the mask bit again, re-enable interrupts and
// clear the IPU DMA channel — returning 0 (failure) instead of 1.
// NOTE on the hardware-register forms: the 32-bit accesses are volatile, which
// keeps ee-gcc from folding the constant address (it emits lui/ori + lw 0(reg));
// the 64-bit BP read at 0x10002030 is deliberately NON-volatile so the address
// stays a plain constant and gas expands `ld` to lui + ld 0x2030(reg), exactly
// as the original. The 0x10002020 read must also be issued BEFORE the BP read
// so the shared `lui 0x1000` is consumed by the ori form, not by the ld.
extern void func_001063E8(void);
extern int func_00109B20(int handle, void *pkt);
extern void func_0010A3A8(char *msg);

extern int D_002412F4;
extern int D_00241B98;
extern int D_00241B9C;
extern char D_0026B380[];

int sub_Error_code_detected_BDEC(void)
{
    int pkt_a[8];
    int pkt_b[8];
    long bp;
    unsigned int fifo;
    unsigned int status;
    int bits;
    int ok;

    ok = 1;
    func_001063E8();
    if (*(volatile unsigned int *)0x1000B020 != 0 &&
        (*(volatile unsigned int *)0x10002010 & 0x4000) == 0) {
        do {
            if (*(volatile unsigned int *)0x1000B420 == 0 &&
                (*(volatile unsigned int *)0x1000B400 & 0x100) == 0) {
                pkt_a[0] = 1;
                func_00109B20(D_002412F4, pkt_a);
            }
            if (*(volatile unsigned int *)0x1000B020 == 0) {
                break;
            }
        } while ((*(volatile unsigned int *)0x10002010 & 0x4000) == 0);
    }

    fifo = *(volatile unsigned int *)0x10002020;
    bp = *(long *)0x10002030;
    D_00241B98 = (int)bp;
    if (bp < 0) {
        bits = (int)(fifo & 0x1F);
        D_00241B9C = (bits != 0) ? (32 - bits) : 0;
    } else {
        D_00241B9C = 32;
    }

    if ((*(volatile unsigned int *)0x10002010 & 0x4000) != 0) {
        func_0010A3A8(D_0026B380);
        pkt_b[0] = 2;
        func_00109B20(D_002412F4, pkt_b);
        *(volatile unsigned int *)0x10002010 = 0x40000000;
        pkt_b[0] = 3;
        func_00109B20(D_002412F4, pkt_b);
        do {
            __asm__ __volatile__("di");
            __asm__ __volatile__("sync.p");
            __asm__ __volatile__("mfc0 %0, $12" : "=r"(status));
        } while ((status & 0x10000) != 0);
        *(volatile unsigned int *)0x1000F590 =
            *(volatile unsigned int *)0x1000F520 | 0x10000;
        *(volatile unsigned int *)0x1000B000 = 0;
        *(volatile unsigned int *)0x1000F590 =
            *(volatile unsigned int *)0x1000F520 & 0xFFFEFFFF;
        __asm__ __volatile__("ei");
        ok = 0;
        *(volatile unsigned int *)0x1000B020 = 0;
    }
    return ok;
}
