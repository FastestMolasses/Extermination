// NEARMISS func_001BA1F0  (vram 0x001BA1F0, 0x31C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 73.65% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// Corrected both signed skip gates, opcode18 scan stops, and pre-advance record retention; compiled original-instruction oracle passes 43 cases. Residual control-flow layout and instruction differences remain unclassified (744-byte candidate vs 796-byte original).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Corrected 2026-09-21 against the original instructions: both skip gates
// accept signed phase < 2 (001BA298/001BA404), not >= 2. Both scans check
// STOP before advancing and stop at opcode18. Only a newly encountered
// STOP record is executed during scanning. The former C had inverted
// gates and a different second scan. Original assembly remains linked.
// The compiled candidate and native sequencer both pass 43 synthetic
// original-instruction cases in tools/test_script_reference.py (port repo).
// These tests cover sequencing, not every command handler implementation.

extern int (*ftab_0024D880[])();
extern unsigned char D_70003B91;

int func_001BA1F0(char *actor) {
    char *script = actor + 0x1F0;
    char *record;
    int result;
    int flags;

run:
    if (*(int *)script <= 0) return 1;
    record = *(char **)(script + 8);
    result = ftab_0024D880[*(int *)record & 0xFFF](actor, script, record);
    if (result == 3) goto abort;
    if (result == 2) goto advance;
    if (result == 1) goto advance;
    if (result == 0) goto stay;
    goto tail;
stay:
    if (D_70003B91 != 2 || *(signed char *)(script + 0xC) >= 2)
        return 0;
    *(char *)(script + 0xC) = 2;
    /* record intentionally remains the pre-advance pointer on entry
     * from the advance path: 001BA410 jumps to the load at001BA4A8. */
    for (;;) {
        flags = *(int *)record;
        if (flags & 0x80000000) goto abort;
        if (flags & 0x40000000) {
            *(char **)(script + 8) = *(char **)(record + 4);
        } else {
            *(char **)(script + 8) += 0x40;
        }
        record = *(char **)(script + 8);
        flags = *(int *)record;
        if ((flags & 0xFFF) == 0x18) {
            *(int *)(script + 4) = 0;
            return 0;
        }
        if (flags & 0x80000000) {
            *(int *)(script + 4) = 0;
            do {
                result = ftab_0024D880[*(int *)record & 0xFFF](actor, script, record);
            } while (result == 0);
        }
    }

advance:
    flags = *(int *)record;
    if (flags & 0x80000000) {
        *(int *)script = -1;
        *(int *)(script + 4) = 0;
        return 1;
    }
    if (flags & 0x40000000) {
        *(char **)(script + 8) = *(char **)(record + 4);
    } else {
        *(char **)(script + 8) += 0x40;
        if (flags & 0x20000000) result = 2;
    }
    *(int *)(script + 4) = 0;
    if (D_70003B91 == 2 && *(signed char *)(script + 0xC) < 2) {
        *(char *)(script + 0xC) = 2;
        /* record intentionally remains the pre-advance pointer on entry
         * from the advance path: 001BA410 jumps to the load at001BA4A8. */
        for (;;) {
            flags = *(int *)record;
            if (flags & 0x80000000) goto abort;
            if (flags & 0x40000000) {
                *(char **)(script + 8) = *(char **)(record + 4);
            } else {
                *(char **)(script + 8) += 0x40;
            }
            record = *(char **)(script + 8);
            flags = *(int *)record;
            if ((flags & 0xFFF) == 0x18) {
                *(int *)(script + 4) = 0;
                return 0;
            }
            if (flags & 0x80000000) {
                *(int *)(script + 4) = 0;
                do {
                    result = ftab_0024D880[*(int *)record & 0xFFF](actor, script, record);
                } while (result == 0);
            }
        }
    }

tail:
    if (result == 2) goto run;
    return 0;
abort:
    *(int *)script = -1;
    *(int *)(script + 4) = 0;
    return 3;
}
