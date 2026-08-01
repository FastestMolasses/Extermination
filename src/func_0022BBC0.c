// NEARMISS func_0022BBC0  (vram 0x0022BBC0, 0x1794 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.76% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 88.728% on mwcc 2.3.3 (-O4,p -sdatathreshold 0), 1509 instructions. Body/structure fully recovered (all five jump tables decode cleanly and every case body is present); the residual is dominated by ONE class. (1) WHOLE-FUNCTION SAVED-REGISTER PERMUTATION — this is ~80% of the diff rows and it is ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: the ending / "staff-roll cinematic" director tick.  It runs a
// small timeline script over the credits scene, drives the fade / colour /
// scroll interpolators, spawns the trailing particle sprites for every actor
// in the scene, and finally re-renders the whole particle ring.
//
//   seq  = the director object
//     seq[4]     = director state: 0 = first tick (load), 1 = running,
//                  2/3 = finished (hand off to func_001AFC10).
//     seq[0xD]   = which cinematic variant is playing (0..9) - selects the
//                  particle offsets, the per-actor trail period and the
//                  sprite tables below.
//     seq+0x24   = pointer to the credits scene
//     seq+0x80   = vec4 target colour, copied from the scene on load
//     seq+0x1F0  = the work block `w` (see below)
//   scn  = the scene
//     scn[4]     = scene state (3 = the scene asked to stop)
//     scn[0xC]   = actor count
//     scn+0x60   = vec3 scene tint,  scn+0x80 = vec4 fade colour
//     scn+0x110  = array of actor pointers; actor+0x90 = orientation quat,
//                  actor+0 = position, actor+0x88..0x8C = RGB scroll
//   w    = the per-run work block
//     w[i]       = per-actor trail phase counter (i < actor count)
//     w[0x38+i]  = per-actor trail sprite kind
//     w+0x70..7C = per-frame delta added to scn+0x80 (vec4 fade)
//     w+0x80..88 = per-frame delta added to scn+0x60 (vec3 tint)
//     w+0x90     = timeline script base (8-byte entries:
//                    +0 short frame, +2 short opcode, +4 short arg,
//                    +6 short value)
//     w+0x94     = LCG state for the particle jitter
//     w+0x98     = timeline entry count
//     w+0x9C     = fade countdown,  w+0xA0 = tint countdown,
//     w+0xA4     = scroll countdown, w+0xA8 = scroll step
//     w+0xAC     = frame counter (the timeline clock)
//     w+0xB0     = write index into the 0x41-entry particle ring
//
// The three EE-scratchpad staging areas used here are 0x700036A0 (the three
// billboard basis vectors + the world position at 0x700036D0), 0x700038A0
// (the per-actor emit position / offset pair) and 0x70003A20 (four scalar
// particle parameters: scale, life, phase, alpha).
//

extern int D_0028A490[];
extern char D_00268090[];
extern char D_00268120[];
extern char D_002681B0[];
extern char D_00268240[];
extern char D_002682D0[];
extern char D_00268360[];
extern char D_002683F0[];
extern char D_00268480[];
extern char D_00268900[];
extern float D_00268910;
extern float D_00268918;
extern char D_00268990[];
extern float D_002689A0;
extern float D_002689A8;
extern char D_00268A20[];
extern char D_00268AB0[];
extern float D_700036A0[4];
extern float D_700036B0[4];
extern float D_700036C0[4];
extern float D_700036D0[4];
extern float D_700038A0[4];
extern float D_700038C0[4];

extern void copy_qw4(float *dst, float *src);
extern void func_001026A0(float *dst, float *quat, float *vec);
extern void func_00102760(float *dst, float *src);
extern void func_001028B8(float *dst, float *a, float *b);
extern void func_00102948(char *dst, char *src);
extern void func_001029C0(float *dst, int seed);
extern void func_00103230(float *dst, float *src, float scale);
extern int func_00122BB8();
extern void func_001AFC10();   /* K&R: the caller leaves the state byte in a1 */
extern void func_001CA6E0(char *scn, int id);
extern int func_001CCF70(float *pos);
extern int func_001CD070(float *pos, int flags);
extern float func_001CD2B0(float a, float b, float c, float d);
extern void func_001CFB50(char *out, int mode, float *basis, float f0, float f1,
                          float f2, float f3, float f4);
extern void func_001CFBE0(int handle, int count, char *tbl, char *desc, int flag);
extern void func_001F0190(float a, float b);
extern void func_001F0290();
extern void func_0021B9A0(int chan, float a, float b);
extern int func_0022B700(char *seq, int id);
extern int func_0022B7A0();    /* K&R: reads the state byte the caller left in a1 */
extern char *func_0022BB70(int slot);

void func_0022BBC0(char *seq)
{
    char spA0[0x60];
    char *scn;
    char *w;
    char *ev;
    char *act;
    char *p;
    int state;
    int kind;
    int i;
    int off;
    int period;
    int burst;
    int handle;
    int seed;
    int seedB;
    int seedC;
    int n;
    float alpha;
    float scale;
    float t;

    state = (unsigned char)seq[4];
    scn = *(char **)(seq + 0x24);
    w = seq + 0x1F0;

    switch (state) {
    case 0:
        if (func_0022B7A0() == 0) {
            seq[4] = 3;
            return;
        }
        for (i = 0; i < (int)(unsigned char)scn[0xC]; i++) {
            w[i] = 0;
            w[i + 0x38] = 0;
        }
        if (func_0022B700(seq, 5) == 0) {
            return;
        }
        for (i = 0; i < 0x41; i++) {
            *(short *)(func_0022BB70(i) + 0xE) = -1;
        }
        *(int *)(w + 0x94) = func_00122BB8();
        *(int *)(w + 0x9C) = 0;
        *(int *)(w + 0xA0) = 0;
        *(int *)(w + 0xA4) = 0;
        *(int *)(w + 0xB0) = 0;
        *(int *)(w + 0xAC) = 0;
        func_00102948(seq + 0x80, scn + 0x80);
        /* fallthrough */

    case 1:
        if ((unsigned char)scn[4] == 3) {
            seq[4] = 3;
            return;
        }

        /* ---- run every timeline entry that is due this frame ---- */
        off = 0;
        for (n = 0; n < *(int *)(w + 0x98); n++) {
            ev = *(char **)(w + 0x90) + off;
            if (*(int *)(w + 0xAC) == *(short *)ev) {
                switch (*(short *)(ev + 2)) {
                case 0:
                    if (*(unsigned char *)(seq + 0xD) != 0) {
                    } else {
                        *(int *)(w + 0xA0) = *(short *)(ev + 6);
                        *(float *)(w + 0x80) = (1.0f - *(float *)(scn + 0x60)) /
                            (float)*(short *)(*(char **)(w + 0x90) + off + 6);
                        *(float *)(w + 0x84) = (0.1f - *(float *)(scn + 0x64)) /
                            (float)*(short *)(*(char **)(w + 0x90) + off + 6);
                        *(float *)(w + 0x88) = (1.0f - *(float *)(scn + 0x68)) /
                            (float)*(short *)(*(char **)(w + 0x90) + off + 6);
                    }
                    break;
                case 1:
                    *(int *)(w + 0xA4) = *(short *)(ev + 6);
                    *(int *)(w + 0xA8) = 0x1000 / *(short *)(*(char **)(w + 0x90) + off + 6);
                    *(int *)(w + 0xA8) = *(int *)(w + 0xA8) + 4;
                    break;
                case 2:
                    w[*(short *)(ev + 4)] = 1;
                    ev = *(char **)(w + 0x90) + off;
                    w[*(short *)(ev + 4) + 0x38] = (char)*(short *)(ev + 6);
                    break;
                case 3:
                    if (*(unsigned char *)(seq + 0xD) != 0) {
                    } else {
                        *(short *)(*(char **)(scn + 0x110 + *(short *)(ev + 4) * 4) + 0x88) = 0;
                        *(short *)(*(char **)(scn + 0x110 +
                            *(short *)(*(char **)(w + 0x90) + off + 4) * 4) + 0x8A) = 0;
                        *(short *)(*(char **)(scn + 0x110 +
                            *(short *)(*(char **)(w + 0x90) + off + 4) * 4) + 0x8C) = 0;
                    }
                    break;
                case 4:
                    if (*(unsigned char *)(seq + 0xD) != 0) {
                    } else {
                        scn[4] = 3;
                    }
                    break;
                case 5:
                    kind = *(unsigned char *)(seq + 0xD);
                    switch (kind) {
                    case 0:
                        *(int *)(w + 0x9C) = *(short *)(ev + 6);
                        *(float *)(w + 0x70) = (0.2f - *(float *)(scn + 0x80)) /
                            (float)*(short *)(*(char **)(w + 0x90) + off + 6);
                        *(float *)(w + 0x74) = (0.8f - *(float *)(scn + 0x84)) /
                            (float)*(short *)(*(char **)(w + 0x90) + off + 6);
                        *(float *)(w + 0x78) = (0.2f - *(float *)(scn + 0x88)) /
                            (float)*(short *)(*(char **)(w + 0x90) + off + 6);
                        *(float *)(w + 0x7C) = (1.0f - *(float *)(scn + 0x8C)) /
                            (float)*(short *)(*(char **)(w + 0x90) + off + 6);
                        break;
                    case 1:
                        *(int *)(w + 0x9C) = *(short *)(ev + 6);
                        *(float *)(w + 0x70) = (0.1f - *(float *)(scn + 0x80)) /
                            (float)*(short *)(*(char **)(w + 0x90) + off + 6);
                        *(float *)(w + 0x74) = (0.1f - *(float *)(scn + 0x84)) /
                            (float)*(short *)(*(char **)(w + 0x90) + off + 6);
                        *(float *)(w + 0x78) = (0.1f - *(float *)(scn + 0x88)) /
                            (float)*(short *)(*(char **)(w + 0x90) + off + 6);
                        *(float *)(w + 0x7C) = (1.0f - *(float *)(scn + 0x8C)) /
                            (float)*(short *)(*(char **)(w + 0x90) + off + 6);
                        break;
                    case 3:
                        *(int *)(w + 0x9C) = *(short *)(ev + 6);
                        *(float *)(w + 0x70) = (0.1f - *(float *)(scn + 0x80)) /
                            (float)*(short *)(*(char **)(w + 0x90) + off + 6);
                        *(float *)(w + 0x74) = (0.1f - *(float *)(scn + 0x84)) /
                            (float)*(short *)(*(char **)(w + 0x90) + off + 6);
                        *(float *)(w + 0x78) = (0.1f - *(float *)(scn + 0x88)) /
                            (float)*(short *)(*(char **)(w + 0x90) + off + 6);
                        *(float *)(w + 0x7C) = (1.0f - *(float *)(scn + 0x8C)) /
                            (float)*(short *)(*(char **)(w + 0x90) + off + 6);
                        break;
                    }
                    break;
                case 6:
                    kind = *(unsigned char *)(seq + 0xD);
                    if (kind == 9 || kind == 3 || kind == 1) {
                        *(int *)(w + 0x9C) = *(short *)(ev + 6);
                        *(float *)(w + 0x70) = (*(float *)(seq + 0x80) - *(float *)(scn + 0x80)) /
                            (float)*(short *)(*(char **)(w + 0x90) + off + 6);
                        *(float *)(w + 0x74) = (*(float *)(seq + 0x84) - *(float *)(scn + 0x84)) /
                            (float)*(short *)(*(char **)(w + 0x90) + off + 6);
                        *(float *)(w + 0x78) = (*(float *)(seq + 0x88) - *(float *)(scn + 0x88)) /
                            (float)*(short *)(*(char **)(w + 0x90) + off + 6);
                        *(float *)(w + 0x7C) = (*(float *)(seq + 0x8C) - *(float *)(scn + 0x8C)) /
                            (float)*(short *)(*(char **)(w + 0x90) + off + 6);
                    }
                    break;
                case 7:
                    if (*(unsigned char *)(seq + 0xD) != 1) {
                    } else {
                        func_001CA6E0(scn, D_0028A490[*(short *)(ev + 6)]);
                    }
                    break;
                case 8:
                    seq[4] = 3;
                    break;
                }
            }
            off += 8;
        }

        /* ---- advance the three interpolators ---- */
        if (*(int *)(w + 0x9C) != 0) {
            *(int *)(w + 0x9C) = *(int *)(w + 0x9C) - 1;
            *(float *)(scn + 0x80) += *(float *)(w + 0x70);
            *(float *)(scn + 0x84) += *(float *)(w + 0x74);
            *(float *)(scn + 0x88) += *(float *)(w + 0x78);
            *(float *)(scn + 0x8C) += *(float *)(w + 0x7C);
        }
        if (*(int *)(w + 0xA0) != 0) {
            *(int *)(w + 0xA0) = *(int *)(w + 0xA0) - 1;
            *(float *)(scn + 0x60) += *(float *)(w + 0x80);
            *(float *)(scn + 0x64) += *(float *)(w + 0x84);
            *(float *)(scn + 0x68) += *(float *)(w + 0x88);
        }
        if (*(int *)(w + 0xA4) != 0) {
            *(int *)(w + 0xA4) = *(int *)(w + 0xA4) - 1;
            p = scn;
            for (i = 0; i < (int)(unsigned char)scn[0xC]; i++) {
                act = *(char **)(p + 0x110);
                *(short *)(act + 0x8A) = (short)(*(short *)(act + 0x8A) - *(int *)(w + 0xA8));
                act = *(char **)(p + 0x110);
                if (*(short *)(act + 0x8A) < 0) {
                    *(short *)(act + 0x8A) = 0;
                }
                p += 4;
            }
        }

        /* ---- per-actor trail emitters ---- */
        p = scn;
        period = 0;
        burst = 0;
        for (i = 0; i < (int)(unsigned char)scn[0xC]; i++) {
            if (w[i] != 0) {
                switch (*(unsigned char *)(seq + 0xD)) {
                case 0:
                    *(volatile float *)0x700038B0 = 0.0f;
                    *(volatile float *)0x700038C0 = 0.0f;
                    *(volatile float *)0x700038C4 = 0.0f;
                    *(volatile float *)0x700038C8 = 0.0f;
                    *(volatile float *)0x700038CC = 0.0f;
                    *(volatile float *)0x70003A20 = 0.95f;
                    *(volatile float *)0x700038B4 = -2.0f;
                    *(volatile float *)0x700038B8 = 0.0f;
                    period = 0x28;
                    *(volatile float *)0x700038BC = 0.0f;
                    break;
                case 1:
                    *(volatile float *)0x70003A20 = 0.0f;
                    *(volatile float *)0x700038B0 = 0.0f;
                    *(volatile float *)0x700038C0 = 0.0f;
                    *(volatile float *)0x700038B4 = -2.0f;
                    *(volatile float *)0x700038B8 = 0.0f;
                    *(volatile float *)0x700038BC = 0.0f;
                    *(volatile float *)0x700038C4 = -4.0f;
                    *(volatile float *)0x700038C8 = 0.0f;
                    period = 0xC;
                    *(volatile float *)0x700038CC = 0.0f;
                    break;
                case 6:
                case 7:
                case 8:
                    *(volatile float *)0x700038B0 = 0.0f;
                    *(volatile float *)0x700038C0 = 0.0f;
                    *(volatile float *)0x700038C4 = 0.0f;
                    *(volatile float *)0x700038C8 = 0.0f;
                    *(volatile float *)0x700038CC = 0.0f;
                    *(volatile float *)0x70003A20 = 0.95f;
                    *(volatile float *)0x700038B4 = -2.0f;
                    *(volatile float *)0x700038B8 = 0.0f;
                    period = 4;
                    *(volatile float *)0x700038BC = 0.0f;
                    break;
                case 4:
                    *(volatile float *)0x70003A20 = 0.0f;
                    *(volatile float *)0x700038B0 = 0.0f;
                    *(volatile float *)0x700038C0 = 0.0f;
                    *(volatile float *)0x700038C4 = 0.0f;
                    *(volatile float *)0x700038C8 = 0.0f;
                    *(volatile float *)0x700038CC = 0.0f;
                    *(volatile float *)0x700038B4 = 3.0f;
                    *(volatile float *)0x700038B8 = 0.0f;
                    period = 0xC;
                    *(volatile float *)0x700038BC = 0.0f;
                    break;
                case 5:
                    *(volatile float *)0x70003A20 = 0.0f;
                    *(volatile float *)0x700038B0 = 0.0f;
                    *(volatile float *)0x700038C0 = 0.0f;
                    *(volatile float *)0x700038C4 = 0.0f;
                    *(volatile float *)0x700038C8 = 0.0f;
                    *(volatile float *)0x700038CC = 0.0f;
                    *(volatile float *)0x700038B4 = -5.0f;
                    *(volatile float *)0x700038B8 = 0.0f;
                    period = 0xC;
                    *(volatile float *)0x700038BC = 0.0f;
                    break;
                case 2:
                    *(volatile float *)0x70003A20 = 0.0f;
                    *(volatile float *)0x700038B0 = 0.0f;
                    *(volatile float *)0x700038B4 = 0.0f;
                    *(volatile float *)0x700038B8 = 0.0f;
                    *(volatile float *)0x700038BC = 0.0f;
                    *(volatile float *)0x700038C0 = 0.0f;
                    *(volatile float *)0x700038C4 = -1.2f;
                    *(volatile float *)0x700038C8 = 0.0f;
                    period = 0x14;
                    *(volatile float *)0x700038CC = 0.0f;
                    break;
                case 3:
                    *(volatile float *)0x700038B0 = 0.0f;
                    *(volatile float *)0x700038B4 = 0.0f;
                    *(volatile float *)0x700038B8 = 0.0f;
                    *(volatile float *)0x700038BC = 0.0f;
                    *(volatile float *)0x700038C0 = 0.0f;
                    *(volatile float *)0x70003A20 = 0.95f;
                    *(volatile float *)0x700038C4 = -2.0f;
                    *(volatile float *)0x700038C8 = 0.0f;
                    period = 0xC;
                    *(volatile float *)0x700038CC = 0.0f;
                    break;
                case 9:
                    *(volatile float *)0x70003A20 = 0.0f;
                    *(volatile float *)0x700038B0 = 0.0f;
                    *(volatile float *)0x700038C0 = 0.0f;
                    *(volatile float *)0x700038C4 = 0.0f;
                    *(volatile float *)0x700038C8 = 0.0f;
                    *(volatile float *)0x700038CC = 0.0f;
                    *(volatile float *)0x700038B4 = -2.0f;
                    *(volatile float *)0x700038B8 = 0.0f;
                    period = 0x14;
                    *(volatile float *)0x700038BC = 0.0f;
                    break;
                }

                if ((w[i] % period) == 1) {
                    *(volatile float *)0x700038A0 = *(float *)(*(char **)(p + 0x110));
                    *(volatile float *)0x700038A4 = *(float *)(*(char **)(p + 0x110) + 4);
                    *(volatile float *)0x700038A8 = *(float *)(*(char **)(p + 0x110) + 8);
                    *(volatile float *)0x700038AC = 1.0f;
                    func_00103230(D_700038A0, D_700038A0, *(volatile float *)0x70003A20);
                    func_001028B8(D_700038A0, D_700038A0, D_700038C0);
                    func_001026A0(D_700038A0, (float *)(*(char **)(p + 0x110) + 0x90), D_700038A0);
                    act = func_0022BB70(*(int *)(w + 0xB0));
                    *(float *)act = *(volatile float *)0x700038A0 + *(volatile float *)0x700038B0;
                    *(float *)(act + 4) = *(volatile float *)0x700038A4 + *(volatile float *)0x700038B4;
                    *(float *)(act + 8) = *(volatile float *)0x700038A8 + *(volatile float *)0x700038B8;
                    *(short *)(act + 0xC) = (short)w[i + 0x38];
                    *(short *)(act + 0xE) = 0;
                    *(int *)(w + 0xB0) = *(int *)(w + 0xB0) + 1;
                    if (*(int *)(w + 0xB0) >= 0x41) {
                        *(int *)(w + 0xB0) = 0;
                    }
                }

                w[i] = (char)(w[i] + 1);

                switch (*(unsigned char *)(seq + 0xD)) {
                case 0:
                case 6:
                case 7:
                case 8:
                    act = *(char **)(p + 0x110);
                    *(short *)(act + 0x88) = (short)(*(short *)(act + 0x88) - 0x66);
                    act = *(char **)(p + 0x110);
                    if (*(short *)(act + 0x88) < 0) {
                        *(short *)(act + 0x88) = 0;
                        *(short *)(*(char **)(p + 0x110) + 0x8A) = 0;
                        *(short *)(*(char **)(p + 0x110) + 0x8C) = 0;
                        w[i] = 0;
                    }
                    break;
                case 1:
                    if (w[i] >= 0x51) {
                        w[i] = 0;
                    }
                    break;
                case 2:
                case 3:
                case 4:
                case 5:
                    if (w[i] >= 0x79) {
                        w[i] = 0;
                    }
                    break;
                case 9:
                    if (w[i] >= 0x3D) {
                        w[i] = 0;
                        break;
                    }
                    switch (w[i + 0x38]) {
                    case 0:
                        *(volatile float *)0x70003A20 = 1.0f;
                        *(volatile float *)0x70003A24 = 5.0f;
                        burst = 0;
                        break;
                    case 1:
                        *(volatile float *)0x70003A20 = 0.5f;
                        burst = 2;
                        *(volatile float *)0x70003A24 = 5.0f;
                        break;
                    case 2:
                        *(volatile float *)0x70003A20 = 1.0f;
                        burst = 2;
                        *(volatile float *)0x70003A24 = 15.0f;
                        break;
                    case 3:
                        *(volatile float *)0x70003A20 = 1.0f;
                        burst = 4;
                        *(volatile float *)0x70003A24 = 20.0f;
                        break;
                    case 4:
                        *(volatile float *)0x70003A20 = 1.0f;
                        burst = 6;
                        *(volatile float *)0x70003A24 = 20.0f;
                        break;
                    case 5:
                        *(volatile float *)0x70003A20 = 1.0f;
                        *(volatile float *)0x70003A24 = 5.0f;
                        burst = 0;
                        break;
                    }
                    *(volatile float *)0x70003A28 = 2.0f * ((float)w[i] / 60.0f);
                    copy_qw4(D_700036A0, (float *)(*(char **)(p + 0x110) + 0x90));
                    func_00102760(D_700036A0, D_700036A0);
                    func_00102760(D_700036B0, D_700036B0);
                    func_00102760(D_700036C0, D_700036C0);
                    func_00103230(D_700036A0, D_700036A0, *(volatile float *)0x70003A20);
                    func_00103230(D_700036B0, D_700036B0, *(volatile float *)0x70003A20);
                    func_00103230(D_700036C0, D_700036C0, *(volatile float *)0x70003A20);
                    handle = func_001CCF70(D_700036D0);
                    func_001F0190(-145.0f, 450.0f);
                    if (w[i + 0x38] == 5) {
                        for (n = 1; n < 4; n++) {
                            t = 3.0f * (float)n;
                            D_00268910 = t;
                            D_00268918 = t;
                            D_002689A0 = t;
                            D_002689A8 = t;
                            func_001CFB50(spA0, 0, D_700036A0,
                                          *(volatile float *)0x70003A28,
                                          0.123456f * (float)n, 1.0f, 0.000001f,
                                          *(volatile float *)0x70003A24);
                            func_001CFBE0(handle, 1, D_00268990, spA0, 1);
                            func_001CFBE0(handle, 1, D_00268900, spA0, 1);
                        }
                    } else {
                        func_001CFB50(spA0, 0, D_700036A0,
                                      *(volatile float *)0x70003A28, 0.123456f, 1.0f,
                                      0.000001f, *(volatile float *)0x70003A24);
                        func_001CFBE0(handle, 1, &D_00268480[(burst + 1) * 0x90], spA0, 0);
                        func_001CFBE0(handle, 1, &D_00268480[burst * 0x90], spA0, 0);
                    }
                    func_001F0290();
                    break;
                }
            }
            p += 4;
        }

        /* ---- re-render the particle ring ---- */
        func_0021B9A0(2, 1.0f, 20.0f);
        func_0021B9A0(3, 1.0f, 20.0f);
        seed = *(int *)(w + 0x94);
        for (i = 0; i < 0x41; i++) {
            act = func_0022BB70(i);
            seedB = (seed * 0x25) + 0xB;
            seedC = (seedB * 0x25) + 0xB;
            *(volatile float *)0x70003A24 = 0.0001f * (float)*(short *)(act + 0xE);
            *(volatile float *)0x700038D0 =
                ((float)((seed >> 0x10) & 0xFFFF) / 65535.0f) + 0.0001f;
            *(volatile float *)0x700038D4 =
                ((float)((seedB >> 0x10) & 0xFFFF) / 65535.0f) + 0.0001f;
            *(volatile float *)0x700038D8 =
                ((float)((seedC >> 0x10) & 0xFFFF) / 65535.0f) + 0.0001f;
            seed = (seedC * 0x25) + 0xB;
            if (!(*(volatile float *)0x70003A24 < 0.0f)) {
                scale = 1.0f;
                switch (*(short *)(act + 0xC)) {
                case 0:
                case 3:
                case 4:
                    scale = 1.0f;
                    break;
                case 1:
                    scale = 1.5f;
                    break;
                case 2:
                    scale = 2.0f;
                    break;
                default:
                    goto no_scale;
                }
                *(volatile float *)0x70003A28 = scale;
no_scale:
                func_001029C0(D_700036A0, seedB);
                func_00103230(D_700036A0, D_700036A0, *(volatile float *)0x70003A28);
                func_00103230(D_700036B0, D_700036B0, *(volatile float *)0x70003A28);
                func_00103230(D_700036C0, D_700036C0, *(volatile float *)0x70003A28);
                *(volatile float *)0x700036D0 = *(float *)act;
                *(volatile float *)0x700036D4 = *(float *)(act + 4);
                *(volatile float *)0x700036D8 = *(float *)(act + 8);
                handle = func_001CD070(D_700036D0, 0x30);
                if (handle != 0xFFFFFF) {
                    switch (*(unsigned char *)(seq + 0xD)) {
                    case 0:
                        *(volatile float *)0x70003A2C =
                            func_001CD2B0(15.0f, 20.0f, 320.0f, 320.0f);
                        alpha = *(volatile float *)0x70003A2C;
                        if (alpha != 0.0f) {
                            func_001CFB50(spA0, 0, D_700036A0,
                                          *(volatile float *)0x70003A24,
                                          *(volatile float *)0x700038D0, alpha,
                                          0.000001f, 8.0f);
                            func_001CFBE0(handle, 1, D_00268090, spA0, 0);
                            func_001CFB50(spA0, 0, D_700036A0,
                                          *(volatile float *)0x70003A24,
                                          *(volatile float *)0x700038D4,
                                          *(volatile float *)0x70003A2C,
                                          0.000001f, 8.0f);
                            func_001CFBE0(handle, 1, D_00268120, spA0, 0);
                            func_001CFB50(spA0, 0, D_700036A0,
                                          *(volatile float *)0x70003A24,
                                          *(volatile float *)0x700038D8,
                                          *(volatile float *)0x70003A2C,
                                          0.000001f, 8.0f);
                            func_001CFBE0(handle, 1, D_002681B0, spA0, 0);
                        }
                        n = *(short *)(act + 0xE) + 0xC8;
                        break;
                    case 6:
                    case 7:
                    case 8:
                        func_001CFB50(spA0, 0, D_700036A0,
                                      *(volatile float *)0x70003A24,
                                      *(volatile float *)0x700038D0, 1.0f,
                                      0.000001f, 8.0f);
                        func_001CFBE0(handle, 1, D_00268240, spA0, 0);
                        func_001CFBE0(handle, 1, D_002682D0, spA0, 0);
                        n = *(short *)(act + 0xE) + 0xFA;
                        break;
                    case 4:
                        func_001CFB50(spA0, 0, D_700036A0,
                                      *(volatile float *)0x70003A24,
                                      *(volatile float *)0x700038D0, 1.0f,
                                      0.1f, 0.0f);
                        func_001CFBE0(handle, 1, D_00268360, spA0, 0);
                        n = *(short *)(act + 0xE) + 0xC8;
                        break;
                    case 5:
                        func_001CFB50(spA0, 0, D_700036A0,
                                      *(volatile float *)0x70003A24,
                                      *(volatile float *)0x700038D0, 1.0f,
                                      0.1f, 0.0f);
                        func_001CFBE0(handle, 1, D_002683F0, spA0, 0);
                        n = *(short *)(act + 0xE) + 0xC8;
                        break;
                    case 2:
                    case 3:
                        func_001CFB50(spA0, 0, D_700036A0,
                                      *(volatile float *)0x70003A24,
                                      *(volatile float *)0x700038D0, 1.0f,
                                      0.000001f, 0.0f);
                        func_001CFBE0(handle, 1, D_00268AB0, spA0, 0);
                        n = *(short *)(act + 0xE) + 0xC8;
                        break;
                    case 1:
                        func_001CFB50(spA0, 0, D_700036A0,
                                      *(volatile float *)0x70003A24,
                                      *(volatile float *)0x700038D0, 1.0f,
                                      0.000001f, 5.0f);
                        func_001CFBE0(handle, 1, D_00268A20, spA0, 0);
                        n = *(short *)(act + 0xE) + 0xC8;
                        break;
                    default:
                        n = *(short *)(act + 0xE) + 0x3A98;
                        break;
                    }
                    *(short *)(act + 0xE) = (short)n;
                }
                if (*(short *)(act + 0xE) >= 0x3A99) {
                    *(short *)(act + 0xE) = -1;
                }
            }
        }
        func_0021B9A0(1, 0.0f, 0.0f);
        *(int *)(w + 0xAC) = *(int *)(w + 0xAC) + 1;
        return;

    case 2:
    case 3:
        func_001AFC10(seq);
        break;
    }
}
