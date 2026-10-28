// COMPILER: eegcc
// EEGCC_AS: 2.96
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 97.55% NEARMISS).
// Fixes read from the original instructions:
//  - Each timeout site prints its message and then repeats the full register
//    dump and return -1 (not a shared `who` variable + one goto-merged dump);
//    the original's single shared print call comes from the compiler's
//    cross-jumping, which also puts each message address straight into the
//    argument register.
//  - func_00122B58 is the int-returning SDK printf (a void prototype changes
//    the dump block's register choice).
//  - query != 0 path: busy starts at 0 and the VIF1 test sets it to 1 (not
//    busy = (stat != 0)), which gives the original's register choice.
//  - The 2.96 assembler fills the call / return delay slots as the original does.

//
// SEMANTICS: DMA / VU0 / GS idle check (Sony SDK "lowmem" region).
//
//   query == 0 : block until every channel of interest has gone idle,
//                giving up after 0x1000000 poll iterations.  On timeout the
//                offending unit is named and a full register dump is printed
//                through func_00122B58 (the SDK printf), and -1 is returned.
//                Each timeout site prints its own message and then the
//                whole register dump (DUMP_REGS); the compiler's
//                cross-jumping folds the five copies into one.
//                Returns 0 once everything is idle.
//   query != 0 : do not wait — return a bitmask of the units that are still
//                busy:
//                  0x01 VIF1  (D_STR of 0x10009000)
//                  0x02 GIF   (D_STR of 0x1000A000)
//                  0x04 GS    CSR busy bits (0x1F000003 of 0x10003C00)
//                  0x08 VU0   VPU_STAT (COP2 ctrl 29) VBS1
//                  0x10 VIF1  STAT FQC/FDR (0xC00 of 0x10003020)
//
//   timeout    : every caller passes a second argument (0); the body never
//                reads it.
//

extern int func_00122B58(const char *fmt, ...);

extern const char D_0026AEB8[];
extern const char D_0026AEE8[];
extern const char D_0026AEF8[];
extern const char D_0026AF08[];
extern const char D_0026AF18[];
extern const char D_0026AF28[];
extern const char D_0026AF38[];
extern const char D_0026AF48[];
extern const char D_0026AF58[];
extern const char D_0026AF68[];
extern const char D_0026AF80[];
extern const char D_0026AF98[];
extern const char D_0026AFC8[];
extern const char D_0026AFF8[];
extern const char D_0026B020[];

#define DUMP_REGS() \
    func_00122B58(D_0026AEE8, *(volatile unsigned int *)0x10009000); \
    func_00122B58(D_0026AEF8, *(volatile unsigned int *)0x10009030); \
    func_00122B58(D_0026AF08, *(volatile unsigned int *)0x10009010); \
    func_00122B58(D_0026AF18, *(volatile unsigned int *)0x10009020); \
    func_00122B58(D_0026AF28, *(volatile unsigned int *)0x1000A000); \
    func_00122B58(D_0026AF38, *(volatile unsigned int *)0x1000A030); \
    func_00122B58(D_0026AF48, *(volatile unsigned int *)0x1000A010); \
    func_00122B58(D_0026AF58, *(volatile unsigned int *)0x1000A020); \
    func_00122B58(D_0026AF68, *(volatile unsigned int *)0x10003C00); \
    func_00122B58(D_0026AF80, *(volatile unsigned int *)0x10003020); \
    return -1

int dma_wait_and_submit(int query, int timeout) {
    unsigned int n;
    unsigned int stat;
    int busy;

    n = 0;
    if (query == 0) {
        /* VIF1 channel */
        while (*(volatile unsigned int *)0x10009000 & 0x100) {
            if (n++ > 0x1000000) {
                goto timeout_vif1;
            }
        }
        /* GIF channel */
        while (*(volatile unsigned int *)0x1000A000 & 0x100) {
            if (n++ > 0x1000000) {
                goto timeout_gif;
            }
        }
        /* GS CSR */
        while (*(volatile unsigned int *)0x10003C00 & 0x1F000003) {
            if (n++ > 0x1000000) {
                goto timeout_gs;
            }
        }
        /* VU0 VPU_STAT */
        asm volatile("cfc2.ni %0,$vi29" : "=r"(stat));
        if (stat & 0x100) {
            do {
                if (n++ > 0x1000000) {
                    goto timeout_vu;
                }
                asm volatile("cfc2.ni %0,$vi29" : "=r"(stat));
            } while (stat & 0x100);
        }
        /* VIF1 STAT FIFO */
        while (*(volatile unsigned int *)0x10003020 & 0xC00) {
            if (n++ > 0x1000000) {
                goto timeout_vif1_fifo;
            }
        }
        return 0;

    timeout_vif1:
        func_00122B58(D_0026AEB8);
        DUMP_REGS();
    timeout_gif:
        func_00122B58(D_0026AF98);
        DUMP_REGS();
    timeout_gs:
        func_00122B58(D_0026AFC8);
        DUMP_REGS();
    timeout_vu:
        func_00122B58(D_0026AFF8);
        DUMP_REGS();
    timeout_vif1_fifo:
        func_00122B58(D_0026B020);
        DUMP_REGS();
    }

    busy = 0;
    if (*(volatile unsigned int *)0x10009000 & 0x100) {
        busy = 1;
    }
    if (*(volatile unsigned int *)0x1000A000 & 0x100) {
        busy |= 2;
    }
    if (*(volatile unsigned int *)0x10003C00 & 0x1F000003) {
        busy |= 4;
    }
    asm volatile("cfc2.ni %0,$vi29" : "=r"(stat));
    if (stat & 0x100) {
        busy |= 8;
    }
    if (*(volatile unsigned int *)0x10003020 & 0xC00) {
        busy |= 0x10;
    }
    return busy;
}
