// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Entity state-machine tick. p[4] is the state: 0 -> arm (advance to 1);
// 1 -> active, dispatch on the sub-state p[3]; 2/3 -> hand off to
// func_001AFC10 (teardown/next-stage). In state 1:
//   sub 2: poll func_001BA1C0(p, 9); on hit -> state 3. Otherwise, if the
//          global flag D_00810761 == 1, emit 6 effect points from the
//          static position table D_00250F40 (xyz triples), ids 10,12,..,20,
//          via func_001F4E40 (scratchpad vec at 0x700038A0, parm block at
//          0x700038B0: type 0x80, size 0x10, 0, 0x80; radius 30.0).
//   sub 3: poll func_001BA1C0(p, 12); on hit -> state 3. Otherwise emit 2
//          points from D_00250F90, ids 10,14 (same parm block).
//   sub 4: if the linked record *(p+0x18) has a nonzero count at +0x2E,
//          transform the fixed point (-17.457, 18.783, -11.908, 1.0) by the
//          matrix at record+0xD0 (func_001026A0) and emit it as id 15.

extern int func_001BA1C0(unsigned char *p, int idx);
extern void func_001AFC10(unsigned char *p);
extern void func_001026A0(void *dst, void *mtx, void *src);
extern void func_001F4E40(void *pos, void *parms, int id, float radius);

extern unsigned char D_00810761;
extern float D_00250F40[];
extern float D_00250F90[];
extern float D_700038A0;
extern int D_700038B0;

void func_001C4CB0(unsigned char *p)
{
    switch (p[4]) {
    case 0:
        p[4] = 1;
        break;
    case 1:
        switch (p[3]) {
        case 2:
            if (func_001BA1C0(p, 9)) {
                p[4] = 3;
            } else if (D_00810761 == 1) {
                int i;
                float *src;
                int id;

                *(int *)0x700038B0 = 0x80;
                *(int *)0x700038B4 = 0x10;
                *(int *)0x700038B8 = 0;
                *(int *)0x700038BC = 0x80;
                src = D_00250F40;
                id = 10;
                for (i = 0; i < 6; i++) {
                    *(float *)0x700038A0 = src[0];
                    *(float *)0x700038A4 = src[1];
                    *(float *)0x700038A8 = src[2];
                    *(float *)0x700038AC = 1.0f;
                    func_001F4E40(&D_700038A0, &D_700038B0, id, 30.0f);
                    src += 3;
                    id += 2;
                }
            }
            break;
        case 3:
            if (func_001BA1C0(p, 12)) {
                p[4] = 3;
            } else {
                int i;
                float *src;
                int id;

                *(int *)0x700038B0 = 0x80;
                *(int *)0x700038B4 = 0x10;
                *(int *)0x700038B8 = 0;
                *(int *)0x700038BC = 0x80;
                src = D_00250F90;
                id = 10;
                for (i = 0; i < 2; i++) {
                    *(float *)0x700038A0 = src[0];
                    *(float *)0x700038A4 = src[1];
                    *(float *)0x700038A8 = src[2];
                    *(float *)0x700038AC = 1.0f;
                    func_001F4E40(&D_700038A0, &D_700038B0, id, 30.0f);
                    src += 3;
                    id += 4;
                }
            }
            break;
        case 4: {
            unsigned char *q = *(unsigned char **)(p + 0x18);

            if (*(unsigned short *)(q + 0x2E) != 0) {
                *(float *)0x700038A0 = -17.457f;
                *(float *)0x700038A4 = 18.783f;
                *(float *)0x700038A8 = -11.908f;
                *(float *)0x700038AC = 1.0f;
                func_001026A0(&D_700038A0, q + 0xD0, &D_700038A0);
                *(int *)0x700038B0 = 0x80;
                *(int *)0x700038B4 = 0x10;
                *(int *)0x700038B8 = 0;
                *(int *)0x700038BC = 0x80;
                func_001F4E40(&D_700038A0, &D_700038B0, 15, 30.0f);
            }
            break;
        }
        }
        break;
    case 2:
    case 3:
        func_001AFC10(p);
        break;
    }
}
