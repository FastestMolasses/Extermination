// NEARMISS func_00137EB0  (vram 0x00137EB0, 0x31C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.89% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a compiler artifact that the source levers tried so far (matching wave 2026-10-28) do not fix:
// Remaining differences: the 20.0f argument of the D_0081070A-set func_001B3C30 call is materialised (into f13) before the self copy and the 200.0f argument in the original, after them here; and one address add (idx*4 + other) has its operands in the other order. Improved 97.62 -> 99.89 by idiom-32 scratchpad externs (volatile, one per address, arrays so they stay out of small data at -sdatathreshold 4) with float-literal stores, and by computing the bone-row index before loading the linked actor pointer.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// SPAD: 0x700036A0 0x700036D0 0x700036D4 0x700036D8 0x700036DC 0x700038A0 0x700038A4 0x700038A8 0x700038AC 0x700038B0 0x700038B4 0x700038B8 0x700038BC

//
// 4-state skeletal-rig owner step, dispatched on the state byte at arg0+4;
// sub-block at arg0+0x1F0. State 0: advances the state, zeroes the sub
// counter (+0x1F4), loads the model D_0028A490[self[0xD] ? 0xA7 : 0xA8] via
// func_001CA6E0, gets the bone count from func_001C6150(*(arg0+0x44)) into
// arg0+0xC; if the free-slot watermark D_00275BCC can't fit it, bails to
// state 3; otherwise allocates one record per bone via func_001AF780 into the
// pointer table at arg0+0x110, mirrors the count to arg0+9, and runs
// anim_bone_array_setup(count) + bone_init_default_1(self). State 1: copies
// the 4-quadword transform of the linked actor's bone row
// (*(other+0x110+4*(self[0xD] ? 0xE : 4)) + 0x90, other = *(arg0+0x20)) over
// the current row **D_00275B40+0x90; ticks down the sub counter; when this
// object is the primary (self[0xD]==0), the parent is live (other[0xD]!=0)
// and the counter just hit 0: re-arms it to 6, snapshots the current row
// transform to spad 0x700036A0, transforms the point {5.62,0.98,0,1}
// (0x700038A0 block) through it, stores the result to 0x700036D0..DC, sets
// the scale vector {0.8,0.8,0.8,1.0} at 0x700038B0, and fires the burst
// func_001B3C30(self, 0x700036A0, 0x700038B0, 200.0f, D_0081070A ? 20.0f :
// 15.0f) plus sound func_001FBD50(self, 0x164, 0, 450.0f). Then propagates
// the parent's state-3, ticks func_001B17A0(self) and the virtual at
// arg0+0x4C. States 2/3: func_001AFC10(self).
//
// NEARMISS: 99.89 (matching wave 2026-10-28). The scratchpad words are
// relocated volatile externs (idiom-32): this fixed the 0.8f/1.0f
// materialisation order. Computing idx before loading `other` fixed the
// copy_qw4 argument-chain colouring. Reading the four transformed words
// into t3..t0 before storing them reproduces the original's load-all-then-
// store-all copy block.
extern void copy_qw4(void *dst, void *src);
extern void func_001026A0(void *dst, void *src, void *mtx);
extern void func_001AFC10(char *p);
extern int func_001AF780(void);
extern void func_001B17A0(char *p);
extern void func_001B3C30(char *p, void *a, void *b, float fa, float fb);
extern unsigned char func_001C6150(int v);
extern void func_001CA6E0(char *p, int model);
extern void func_001FBD50(char *p, int a, int b, float f);
extern void anim_bone_array_setup(unsigned char n);
extern void bone_init_default_1(char *p);
extern char **D_00275B40;
extern short D_00275BCC;
extern int D_0028A490[];
extern unsigned char D_0081070A[16];
extern volatile float D_700036A0[4];
extern volatile float D_700036D0[];
extern volatile float D_700036D4[];
extern volatile float D_700036D8[];
extern volatile float D_700036DC[];
extern volatile float D_700038A0[4];
extern volatile float D_700038A4[];
extern volatile float D_700038A8[];
extern volatile float D_700038AC[];
extern volatile float D_700038B0[4];
extern volatile float D_700038B4[];
extern volatile float D_700038B8[];
extern volatile float D_700038BC[];

void func_00137EB0(char *arg0) {
    unsigned char st;
    char *other;
    int i;
    char *sub;
    int idx;
    float t0;
    float t1;
    float t2;
    float t3;

    st = *(unsigned char *)(arg0 + 4);
    sub = arg0 + 0x1F0;
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 4) = st + 1;
        *(int *)(sub + 4) = 0;
        func_001CA6E0(arg0, D_0028A490[*(unsigned char *)(arg0 + 0xD) ? 0xA7 : 0xA8]);
        *(unsigned char *)(arg0 + 0xC) = func_001C6150(*(int *)(arg0 + 0x44));
        if (D_00275BCC < *(unsigned char *)(arg0 + 0xC)) {
            *(unsigned char *)(arg0 + 4) = 3;
            return;
        }
        for (i = 0; i < *(unsigned char *)(arg0 + 0xC); i++) {
            *(int *)(arg0 + 0x110 + i * 4) = func_001AF780();
        }
        *(unsigned char *)(arg0 + 9) = *(unsigned char *)(arg0 + 0xC);
        anim_bone_array_setup(*(unsigned char *)(arg0 + 0xC));
        bone_init_default_1(arg0);
        break;
    case 1:
        idx = *(unsigned char *)(arg0 + 0xD) ? 0xE : 4;
        other = *(char **)(arg0 + 0x20);
        copy_qw4(*D_00275B40 + 0x90, *(char **)(idx * 4 + (int)other + 0x110) + 0x90);
        if (*(int *)(sub + 4) != 0) {
            *(int *)(sub + 4) = *(int *)(sub + 4) - 1;
        }
        if (*(unsigned char *)(arg0 + 0xD) == 0 && *(unsigned char *)(other + 0xD) != 0 &&
            *(int *)(sub + 4) == 0) {
            *(int *)(sub + 4) = 6;
            copy_qw4(D_700036A0, *D_00275B40 + 0x90);
            D_700038A0[0] = 5.62f;
            D_700038A4[0] = 0.98f;
            D_700038A8[0] = 0.0f;
            D_700038AC[0] = 1.0f;
            func_001026A0(D_700038A0, D_700036A0, D_700038A0);
            t3 = D_700038A0[0];
            t2 = D_700038A4[0];
            t1 = D_700038A8[0];
            t0 = D_700038AC[0];
            D_700036D0[0] = t3;
            D_700036D4[0] = t2;
            D_700036D8[0] = t1;
            D_700036DC[0] = t0;
            D_700038B0[0] = 0.8f;
            D_700038B4[0] = 0.8f;
            D_700038B8[0] = 0.8f;
            D_700038BC[0] = 1.0f;
            if (D_0081070A[0]) {
                func_001B3C30(arg0, D_700036A0, D_700038B0, 200.0f, 20.0f);
            } else {
                func_001B3C30(arg0, D_700036A0, D_700038B0, 200.0f, 15.0f);
            }
            func_001FBD50(arg0, 0x164, 0, 450.0f);
        }
        if (*(unsigned char *)(other + 4) == 3) {
            *(unsigned char *)(arg0 + 4) = 3;
        }
        func_001B17A0(arg0);
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        break;
    case 2:
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
