// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Walk a table of 0x28-byte placement records (terminated by a negative first
// short) and lazily register each one that still has no handle (-1): build the
// {x,y,z,1.0} position vector on the stack and hand it to func_001D7FA0 together
// with the record's D_0026EB70 template slot, caching the returned handle back
// into the record.
// The `int i = p->kind;` temp is load-bearing: it makes the table index
// materialize ahead of the vector stores (target's `lh v1,4(s0)` first), and the
// `int zi = 0; float z = (float)zi;` staging (idiom-24) is what puts
// `mtc1 zero,$f13` ahead of `mtc1 $a3,$f12` at the call.

struct PlaceRec {
    short live;   /* 0x00 - negative terminates the table */
    short f2;     /* 0x02 */
    short kind;   /* 0x04 - index into the D_0026EB70 template table */
    short f6;     /* 0x06 */
    int f8;       /* 0x08 */
    float x;      /* 0x0C */
    float y;      /* 0x10 */
    float z;      /* 0x14 */
    int f18;      /* 0x18 */
    int f1C;      /* 0x1C */
    int f20;      /* 0x20 */
    int handle;   /* 0x24 */
};

extern int func_001D7FA0(float *pos, char *tmpl, int a2, float f12, float f13);
extern char D_0026EB70[];

void func_001F6640(struct PlaceRec *p) {
    float pos[4];
    int i;
    int zi;
    float z;

    if (p == 0) {
        return;
    }
    while (p->live >= 0) {
        if (p->handle == -1) {
            i = p->kind;
            pos[0] = p->x;
            pos[1] = p->y;
            pos[2] = p->z;
            pos[3] = 1.0f;
            zi = 0;
            z = (float)zi;
            p->handle = func_001D7FA0(pos, &D_0026EB70[i * 16], 1, 1.0f, z);
        }
        p++;
    }
}
