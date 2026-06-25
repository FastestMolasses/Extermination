// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Lookup: key = (D_00810700<<8)+D_00810701. ~35 keys explicitly return NULL,
// 10 keys return a fixed table pointer, everything else returns NULL. The
// blind-NULL keys are written as a leading if(key==X) goto ret0; chain (mwcc
// 2.3.3 emits the target's beql-first + beq->ret0 dispatch from this shape);
// the pointer keys are a trailing switch (mwcc keeps beq->block per case).
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern char D_0025CAD0;
extern char D_0025CB20;
extern char D_0025CBA0;
extern char D_0025CC40;
extern char D_0025CCE0;
extern char D_0025CD30;
extern char D_0025CD80;
extern char D_0025CDD0;
extern char D_0025CE20;
extern char D_0025CEC0;

void *func_001F5CA0(void) {
    void *result;
    int key = (D_00810700 << 8) + D_00810701;
    if (key == 0x1600) goto ret0;
    if (key == 0x1301) goto ret0;
    if (key == 0x1200) goto ret0;
    if (key == 0x1000) goto ret0;
    if (key == 0xC00) goto ret0;
    if (key == 0x900) goto ret0;
    if (key == 0x806) goto ret0;
    if (key == 0x805) goto ret0;
    if (key == 0x804) goto ret0;
    if (key == 0x802) goto ret0;
    if (key == 0x801) goto ret0;
    if (key == 0x704) goto ret0;
    if (key == 0x702) goto ret0;
    if (key == 0x701) goto ret0;
    if (key == 0x601) goto ret0;
    if (key == 0x500) goto ret0;
    if (key == 0x300) goto ret0;
    if (key == 0x202) goto ret0;
    if (key == 0x201) goto ret0;
    if (key == 0x200) goto ret0;
    if (key == 0x101) goto ret0;
    if (key == 0x2) goto ret0;
    if (key == 0xF01) goto ret0;
    if (key == 0xE00) goto ret0;
    if (key == 0x703) goto ret0;
    if (key == 0x1400) goto ret0;
    if (key == 0x1300) goto ret0;
    if (key == 0x1100) goto ret0;
    if (key == 0xB00) goto ret0;
    if (key == 0x1) goto ret0;
    if (key == 0x0) goto ret0;
    if (key == 0x600) goto ret0;
    if (key == 0xA00) goto ret0;
    if (key == 0x1001) goto ret0;
    if (key == 0x100) goto ret0;
    switch (key) {
    case 0x301: result = &D_0025CAD0; return result;
    case 0x302: result = &D_0025CB20; return result;
    case 0x400: result = &D_0025CBA0; return result;
    case 0x401: result = &D_0025CC40; return result;
    case 0x700: result = &D_0025CCE0; return result;
    case 0x800: result = &D_0025CD30; return result;
    case 0x803: result = &D_0025CD80; return result;
    case 0xD00: result = &D_0025CDD0; return result;
    case 0xF00: result = &D_0025CE20; return result;
    case 0x1500: result = &D_0025CEC0; return result;
    }
ret0:
    return 0;
}
