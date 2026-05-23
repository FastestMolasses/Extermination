typedef struct Node Node;
struct Node {
    int unk[6];
    Node *prev;
    Node *next;
};

extern Node *D_00275BBC; /* tail pointer */
extern Node *D_00275BC0; /* head pointer */

void func_001AFA50(Node *a0) {
    Node **a1 = &D_00275BC0;
    Node **a2 = &D_00275BBC;
    a0->prev = D_00275BBC;
    a0->next = 0;
    if (!D_00275BBC) {
        *a1 = a0;
    } else {
        (*a2)->next = a0;
    }
    *a2 = a0;
}
