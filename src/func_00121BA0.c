// COMPILER: eegcc
// CFLAGS: -O2
// Free-list push: slot = (int*)((a1->idx << 2) + a0->table); a1->next = *slot; *slot = a1.
struct node {
    int next;       // 0x0
    int idx;        // 0x4
};
struct owner {
    int pad[19];    // 0x00..0x48
    int table;      // 0x4C (base address as int)
};

void func_00121BA0(struct owner *a0, struct node *a1) {
    if (a1) {
        int *slot = (int *)((a1->idx << 2) + a0->table);
        a1->next = *slot;
        *slot = (int)a1;
    }
}
