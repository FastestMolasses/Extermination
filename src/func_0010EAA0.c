// COMPILER: eegcc
// CFLAGS: -O2
// EEGCC_AS: 2.96

//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 95.56% NEARMISS).
// The C was already right: the original ends with `ei` in the return's delay
// slot, which the 2.9 gas never fills. Assembling this file with the 2.96 gas
// (`// EEGCC_AS: 2.96`, tools/eegcc/ee-compile.sh) reproduces it.
//

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
