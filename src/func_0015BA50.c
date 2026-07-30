// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// SEMANTICS: per-frame actor animation/update entry. Seeds the frame time
// step p+0x34 from the per-anim rate table D_00248C98 (12-byte stride, indexed
// by the anim id at p+0x20C), scales it by the one-shot multiplier p+0x204
// (then resets that to 1.0), latches/clears several per-frame fields, and
// dispatches on the actor mode byte p[4] via a 7-entry jump table. Table entry
// 3 points at the default label, i.e. mode 3 is a deliberate no-op (that is why
// there is no `case 3:` below). Finally recomputes the global "actor is busy"
// flag D_008106B3.

extern float D_00248C98[];
extern unsigned char D_008106B3;
extern unsigned char D_008106F1;
extern unsigned char D_00810CB6;

extern int anim_advance_time(unsigned char *, float);
extern int func_0015B130(unsigned char *);
extern int func_0015B530(unsigned char *);
extern int func_0015B610(unsigned char *);
extern int func_0015B770(unsigned char *);
extern int func_0015C420(unsigned char *);
extern int func_0015D460(unsigned char *);
extern int func_00183090(unsigned char *);

void func_0015BA50(unsigned char *p) {
    int idx;
    int v;
    int a;

    idx = *(short *)(p + 0x20C);
    *(float *)(p + 0x34) = D_00248C98[idx * 3];
    *(float *)(p + 0x34) = *(float *)(p + 0x34) * *(float *)(p + 0x204);
    *(float *)(p + 0x204) = 1.0f;
    p[0x303] = 0;
    p[0x25D] = 0;
    p[1] = 1;
    p[0x319] = p[0xA];
    p[0xA] = 0;
    *(int *)(p + 0x308) = *(int *)(p + 0x214);
    *(int *)(p + 0x214) = 0;
    p[0x318] = 0;
    if (*(unsigned char *)0x70003B8F != 2) {
        *(short *)(p + 0x94) = -1;
    }
    D_008106B3 = 0;
    switch (p[4]) {
    case 0:
        func_0015C420(p);
        break;
    case 1:
        *(int *)(p + 0x200) = anim_advance_time(p, *(float *)(p + 0x34));
        func_0015B130(p);
        break;
    case 2:
        *(int *)(p + 0x200) = anim_advance_time(p, *(float *)(p + 0x34));
        func_0015B770(p);
        break;
    case 4:
        v = p[5];
        if (v == 0 || v == 0x17) {
            if (func_00183090(p) != 0) {
                *(int *)(p + 0x200) = anim_advance_time(p, *(float *)(p + 0x1F4));
            }
        } else {
            *(int *)(p + 0x200) = anim_advance_time(p, *(float *)(p + 0x34));
        }
        func_0015B530(p);
        break;
    case 5:
        *(int *)(p + 0x200) = anim_advance_time(p, *(float *)(p + 0x34));
        func_0015B610(p);
        break;
    case 6:
        func_0015D460(p);
        break;
    }
    p[0xB] = 0;
    if (p[4] != 1) {
        *(short *)(p + 0x276) = 0;
        p[0x274] = 0;
    }
    a = p[0x1F0];
    if (a != 0x31 && a != 0x32 && !(a == 0x34 || a == 0x35)) {
        *(short *)(p + 0x276) = 0;
        p[0x274] = 0;
    }
    if (D_008106F1 != 0 || *(short *)(p + 0x276) != 0 || p[0x1F0] == 0x33 || D_00810CB6 != 0 ||
        (p[4] == 2 &&
         (p[5] == 0xD || p[5] == 0xE || p[5] == 0xF || p[5] == 0xB || p[5] == 0xC) &&
         p[0x1F1] == 1)) {
        D_008106B3 = 1;
    }
}
