// NEARMISS init_vtable_a0_at_0011FE90  (vram 0xvtable_a0_at_0011FE90, 0x58 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.27% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc list-scheduler wall (register-pressure variant). All 22 instructions are correct; only the position of `sw a0,0x1c(a0)` differs — target emits it last (in the jr $ra delay slot), ee-gcc emits it right after the four vtable-pointer stores. Mechanism confirmed: ee-gcc's pre-reload scheduler (...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

// NEARMISS 99.27% — body/logic fully recovered. Sole residual: ee-gcc's
// pre-reload list scheduler hoists the self-pointer store (sw a0,0x1c(a0))
// ahead of the five zero stores; the target emits it last, in the jr delay
// slot. See wall note (register-pressure ranking, INSN_REG_WEIGHT).

extern void vtable_a0_at_0011FE90_off20(void);
extern void vtable_a0_at_0011FE90_off24(void);
extern void vtable_a0_at_0011FE90_off28(void);
extern void vtable_a0_at_0011FE90_off2C(void);

void init_vtable_a0_at_0011FE90(void *obj, short a, short b, int c)
{
    unsigned char *p = (unsigned char *)obj;

    *(int *)(p + 0x54) = c;
    *(short *)(p + 0xC) = a;
    *(short *)(p + 0xE) = b;
    *(void **)(p + 0x20) = (void *)vtable_a0_at_0011FE90_off20;
    *(void **)(p + 0x24) = (void *)vtable_a0_at_0011FE90_off24;
    *(void **)(p + 0x28) = (void *)vtable_a0_at_0011FE90_off28;
    *(void **)(p + 0x2C) = (void *)vtable_a0_at_0011FE90_off2C;
    *(int *)(p + 0x0) = 0;
    *(int *)(p + 0x4) = 0;
    *(int *)(p + 0x8) = 0;
    *(int *)(p + 0x10) = 0;
    *(int *)(p + 0x18) = 0;
    *(void **)(p + 0x1C) = p;
}
