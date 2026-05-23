typedef struct Node Node;
struct Node { int unk[6]; Node *prev; Node *next; };

extern Node *D_00275BC0; /* head */
extern Node *D_00275BBC; /* tail */

void func_001AFBC0(Node *a0) {
    Node *a3;
    Node **a1;
    Node **a2;
    Node *a1n;
    a3 = a0->prev;
    a1 = &D_00275BC0;
    a2 = &D_00275BBC;
    if (!a3) {
        *a1 = a0->next;
    } else {
        a3->next = a0->next;
    }
    a1n = a0->next;
    if (a1n) {
        a1n->prev = a0->prev;
    } else {
        *a2 = a0->prev;
    }
}
