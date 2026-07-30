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

/* Unlink `node` from the singly-linked list rooted at D_00278BC0.list.
   Returns the predecessor node (or the node itself when it was the head,
   or NULL when it was not found). */
Node *func_0010EC40(Node *node)
{
    Node *p;
    unsigned int stat;

    do {
        __asm__ __volatile__("di");
        __asm__ __volatile__("sync.p");
        __asm__ __volatile__("mfc0 %0, $12" : "=r"(stat));
    } while (stat & 0x10000);

    p = D_00278BC0.list;
    if (p == node) {
        D_00278BC0.list = node->next;
    } else {
        while (p != 0) {
            if (p->next == node) {
                p->next = node->next;
                break;
            }
            p = p->next;
        }
    }

    __asm__ __volatile__("ei");
    return p;
}
