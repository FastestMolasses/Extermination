// NEARMISS dma_wait_and_submit  (vram 0xait_and_submit, 0x314 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 89.69% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc list-scheduler wall (+ minor GPR coloring). Body/structure fully recovered: all 5 poll loops, the 5 goto-merged timeout blocks and the 11-call register dump match instruction-for-instruction, and the whole query!=0 movn bitmask path is byte-identical. Residual is confined to the dump block:...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

//
// SEMANTICS: DMA / VU0 / GS idle check (Sony SDK "lowmem" region).
//
//   query == 0 : block until every channel of interest has gone idle,
//                giving up after 0x1000000 poll iterations.  On timeout the
//                offending unit is named and a full register dump is printed
//                through func_00122B58 (the SDK printf), and -1 is returned.
//                Returns 0 once everything is idle.
//   query != 0 : do not wait — return a bitmask of the units that are still
//                busy:
//                  0x01 VIF1  (D_STR of 0x10009000)
//                  0x02 GIF   (D_STR of 0x1000A000)
//                  0x04 GS    CSR busy bits (0x1F000003 of 0x10003C00)
//                  0x08 VU0   VPU_STAT (COP2 ctrl 29) VBS1
//                  0x10 VIF1  STAT FQC/FDR (0xC00 of 0x10003020)
//

extern void func_00122B58();

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

int dma_wait_and_submit(int query) {
    unsigned int n;
    unsigned int stat;
    const char *who;
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
        who = D_0026AEB8;
        goto dump;
    timeout_gif:
        who = D_0026AF98;
        goto dump;
    timeout_gs:
        who = D_0026AFC8;
        goto dump;
    timeout_vu:
        who = D_0026AFF8;
        goto dump;
    timeout_vif1_fifo:
        who = D_0026B020;
    dump:
        func_00122B58(who);
        func_00122B58(D_0026AEE8, *(volatile unsigned int *)0x10009000);
        func_00122B58(D_0026AEF8, *(volatile unsigned int *)0x10009030);
        func_00122B58(D_0026AF08, *(volatile unsigned int *)0x10009010);
        func_00122B58(D_0026AF18, *(volatile unsigned int *)0x10009020);
        func_00122B58(D_0026AF28, *(volatile unsigned int *)0x1000A000);
        func_00122B58(D_0026AF38, *(volatile unsigned int *)0x1000A030);
        func_00122B58(D_0026AF48, *(volatile unsigned int *)0x1000A010);
        func_00122B58(D_0026AF58, *(volatile unsigned int *)0x1000A020);
        func_00122B58(D_0026AF68, *(volatile unsigned int *)0x10003C00);
        func_00122B58(D_0026AF80, *(volatile unsigned int *)0x10003020);
        return -1;
    }

    stat = *(volatile unsigned int *)0x10009000 & 0x100;
    busy = (stat != 0);
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
