// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Per-tick driver on the state byte self+4. 0: func_00154040(self,
// self+0x1F0). 1: when the scratchpad byte 0x70003B8D is 0 or 4,
// func_00154120(self, self+0x1F0). 2: sub-step byte self+5 -- step 0 fires
// func_001FBD50(self, 0x434, 0, 300.0f), func_001EFE00(0x80000052, self) and
// the hook at self+0x4C, then advances; step 1 advances; step 2 sets the state
// to 3 and clears the step. 3: func_001AFC10(self) (despawn).
extern void func_00154040(char *arg0, short *arg1);
extern void func_00154120(char *arg0, char *arg1);
extern void func_001AFC10(unsigned char *self);
extern char *func_001EFE00(int kind, char *src);
extern int func_001FBD50(void *a0, int a1, int a2, float f12);
extern unsigned char D_70003B8D;

void func_00153F10(char *self) {
    char *st = self + 0x1F0;
    unsigned char m;

    switch (*(unsigned char *)(self + 4)) {
    case 0:
        func_00154040(self, (short *)st);
        break;
    case 1:
        m = D_70003B8D;
        if (m == 0 || m == 4) {
            func_00154120(self, st);
        }
        break;
    case 2:
        switch (*(unsigned char *)(self + 5)) {
        case 0:
            func_001FBD50(self, 0x434, 0, 300.0f);
            func_001EFE00(0x80000052, self);
            (*(void (**)(char *))(self + 0x4C))(self);
            *(unsigned char *)(self + 5) += 1;
            break;
        case 1:
            *(unsigned char *)(self + 5) += 1;
            break;
        case 2:
            *(unsigned char *)(self + 4) = 3;
            *(unsigned char *)(self + 5) = 0;
            break;
        }
        break;
    case 3:
        func_001AFC10((unsigned char *)self);
        break;
    }
}
