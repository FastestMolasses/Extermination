// NEARMISS func_0010EAA0  (vram 0x0010EAA0, ?) — readable decompilation, NOT byte-identical.
//
// objdiff 95.56% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc ASSEMBLER delay-slot-swap wall — PIPELINE BUG, fixable, and this C is then a TRUE 100.0. Sole residual: expected ends `jr $ra` with `ei` in the delay slot; we emit `ei; jr $ra; nop` (+1 instr, size 148 vs 144). Root cause: tools/eegcc/ee-compile.sh assembles with gas 2.9-ee (tools/eegcc/ee-...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

typedef struct Node {
    /* 0x00 */ int         f0;
    /* 0x04 */ int         f4;
    /* 0x08 */ int         f8;
    /* 0x0C */ int         fC;
    /* 0x10 */ int         f10;
    /* 0x14 */ struct Node *next;
} Node;

typedef struct Ctx {
    /* 0x00 */ char  pad00[0x28];
    /* 0x28 */ Node *list;
} Ctx;

extern Ctx D_00278BC0;

/* Clear `node`, then append it to the tail of the singly-linked list
   rooted at D_00278BC0.list, with interrupts disabled. */
void func_0010EAA0(Node *node, int arg)
{
    Node *p;
    unsigned int stat;

    do {
        __asm__ __volatile__("di");
        __asm__ __volatile__("sync.p");
        __asm__ __volatile__("mfc0 %0, $12" : "=r"(stat));
    } while (stat & 0x10000);

    node->f0 = arg;
    node->f4 = 0;
    node->f8 = 0;
    node->fC = 0;
    node->f10 = 0;
    node->next = 0;

    if (D_00278BC0.list == 0) {
        D_00278BC0.list = node;
    } else {
        p = D_00278BC0.list;
        while (p->next != 0) {
            p = p->next;
        }
        p->next = node;
    }

    __asm__ __volatile__("ei");
}
