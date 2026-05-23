typedef struct { int pad0; int pad4; char *buf; char pad_C[0x90]; int field_9C; } State;

extern State *D_00275670;
extern char D_0028F700[];

void func_001D2110(void) {
    State *a1 = D_00275670;
    char *v1 = D_0028F700 + (a1->field_9C << 14);
    a1->buf = v1;
}
