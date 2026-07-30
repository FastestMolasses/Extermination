// COMPILER: eegcc
// CFLAGS: -O2
typedef struct {
    unsigned long val;
    unsigned long mask;
} Ent;

extern Ent D_00241238[];

int func_001088A0(int *pidx, int *pval, unsigned long key) {
    Ent *p;
    unsigned int i;
    int found;
    unsigned long masked;
    unsigned long bits;

    p = D_00241238;
    found = 0;
    i = 0;
    do {
        switch (p->mask) {
        case 0xFFFFFFFFFFL:
            masked = key & 0xFFFFFF0000L;
            if (masked == p->val) {
                *pidx = i;
                *pval = (int)(key & 0xFFFF);
                found = 1;
            }
            break;
        case 0xFFFF000000L:
            switch (p->val) {
            case 0xBD20000000L:
                masked = key & 0xFFE0000000L;
                bits = 0x1F;
                break;
            case 0xBD80000000L:
            case 0xBD88000000L:
            case 0xBD90000000L:
            case 0xBDA0000000L:
                masked = key & 0xFFF8000000L;
                bits = 7;
                break;
            default:
                masked = key & 0xFFFF000000L;
                bits = 0;
                break;
            }
            if (masked == p->val) {
                *pidx = i;
                *pval = (int)((key >> 24) & bits);
                found = 1;
            }
            break;
        case 0xFF00000000L:
            if (p->val == 0xE000000000L) {
                masked = key & 0xF000000000L;
                bits = 0xF;
            } else if (p->val == 0xC000000000L) {
                masked = key & 0xE000000000L;
                bits = 0x1F;
            } else {
                masked = key & 0xFF00000000L;
                bits = 0;
            }
            if (masked == p->val) {
                *pidx = i;
                *pval = (int)((key >> 32) & bits);
                found = 1;
            }
            break;
        }
        i++;
        p++;
        if (i >= 10) break;
    } while (!found);
    return found;
}
