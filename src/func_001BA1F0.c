// NEARMISS func_001BA1F0  (vram 0x001BA1F0, 0x31C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 71.33% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Script-interpreter pump loop (opcode dispatch via ftab_0024D880). Body/control-flow fully recovered per the documented interpreter contract (docs/FINDINGS.md 'Interpreter contract - func_001BA1F0'): loop over blk[0]>0, call handler(actor,blk,rec), switch on return code {0=stay,1=advance+yield,2=a...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int (*ftab_0024D880[])();
extern unsigned char D_70003B91;

int func_001BA1F0(char *arg0) {
    char *e = arg0 + 0x1F0;
    char *n;
    int r;
    int f;

loop_1:
    if (*(int *)(e + 0) <= 0) {
        return 1;
    }
    n = *(char **)(e + 8);
    r = ftab_0024D880[*(int *)(n + 0) & 0xFFF](arg0, e, n);
    if (r == 3) {
        *(int *)(e + 0) = -1;
        *(int *)(e + 4) = 0;
        return 3;
    }
    if (r == 2) {
        goto adv;
    }
    if (r == 1) {
        goto adv;
    }
    if (r == 0) {
        goto stay;
    }
    r = 2;
    goto tail;

stay:
    /* r == 0 : stay/wait */
    if (D_70003B91 != 2) {
        return 0;
    }
    if (*(signed char *)(e + 0xC) < 2) {
        return 0;
    }
    *(char *)(e + 0xC) = 2;

    /* fast-forward loop copy #1 (reached from the stay path) */
    for (;;) {
        f = *(int *)(n + 0);
        if (f & 0x40000000) {
            *(char **)(e + 8) = *(char **)(n + 4);
        } else {
            *(char **)(e + 8) = *(char **)(e + 8) + 0x40;
        }
        n = *(char **)(e + 8);
        f = *(int *)(n + 0);
        if ((f & 0xFFF) == 0x18) {
            *(int *)(e + 4) = 0;
            return 0;
        }
        if (!(f & 0x80000000)) {
            goto ff1_checkstop;
        }
        *(int *)(e + 4) = 0;
        do {
            r = ftab_0024D880[f & 0xFFF](arg0, e, n);
            if (r != 0) {
                break;
            }
            n = *(char **)(e + 8);
            f = *(int *)(n + 0);
        } while (1);

    ff1_checkstop:
        n = *(char **)(e + 8);
        f = *(int *)(n + 0);
        if (!(f & 0x80000000)) {
            continue;
        }
        *(int *)(e + 0) = -1;
        *(int *)(e + 4) = 0;
        return 3;
    }

adv:
    f = *(int *)(n + 0);
    if (f & 0x80000000) {
        *(int *)(e + 0) = -1;
        *(int *)(e + 4) = 0;
        return 1;
    }
    if (f & 0x40000000) {
        *(char **)(e + 8) = *(char **)(n + 4);
    } else if (f & 0x20000000) {
        *(char **)(e + 8) = *(char **)(e + 8) + 0x40;
        r = 2;
    } else {
        *(char **)(e + 8) = *(char **)(e + 8) + 0x40;
    }
    *(int *)(e + 4) = 0;

    if (D_70003B91 != 2) {
        goto tail;
    }
    if (*(signed char *)(e + 0xC) < 2) {
        goto tail;
    }
    *(char *)(e + 0xC) = 2;

    /* fast-forward loop copy #2 (reached from the advance path) */
    for (;;) {
        n = *(char **)(e + 8);
        f = *(int *)(n + 0);
        if (f & 0x40000000) {
            *(char **)(e + 8) = *(char **)(n + 4);
        } else {
            *(char **)(e + 8) = *(char **)(e + 8) + 0x40;
        }
        n = *(char **)(e + 8);
        f = *(int *)(n + 0);
        if (f & 0x80000000) {
            *(int *)(e + 0) = -1;
            *(int *)(e + 4) = 0;
            return 3;
        }
        do {
            r = ftab_0024D880[f & 0xFFF](arg0, e, n);
            if (r != 0) {
                break;
            }
            n = *(char **)(e + 8);
            f = *(int *)(n + 0);
        } while (1);

        n = *(char **)(e + 8);
        f = *(int *)(n + 0);
        if (f & 0x80000000) {
            break;
        }
    }

    *(int *)(e + 0) = -1;
    *(int *)(e + 4) = 0;
    return 3;

tail:
    if (r == 2) {
        goto loop_1;
    }
    return 0;
}
