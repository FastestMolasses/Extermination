// NEARMISS func_00151770  (vram 0x00151770, 0x1CC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.91% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// jr-table external-dispatch wall (proven s84) — SOLE residual, 2 of 230 rows: `lui v1,%hi(jtbl_0026D2F0)` / `addiu v1,v1,%lo(jtbl_0026D2F0)` vs mwcc's local `@19` table (ARG_MISMATCH reloc). Every other instruction is byte-identical, including the full dispatch sequence order, the bc1t LOD-flag se...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: per-frame update for a character/actor. Dispatches the actor's
// behaviour state (self[5], 0..9) to one of seven per-state handlers, plays a
// sound cue for two of the animation states, then runs the fixed post-update
// chain (physics/collision/anim advance/etc.) and finally the actor's virtual
// update hook at self+0x4C.
//   self[5]        = behaviour-state id (switch selector, 10-entry jump table)
//   st[0]          = anim/sound state byte (also written as a word by
//                    anim_advance_time's return value)
//   st[0x28]       = delta time fed to anim_advance_time
//   st[0x58]       = "far from camera / LOD" flag set from D_00810364 >= 75.0f
//   D_00810364     = camera-or-player distance scalar

extern float D_00810364;

extern int func_00122BB8();
extern void func_00151940(unsigned char *self, unsigned char *st);
extern void func_00151E50(unsigned char *self, unsigned char *st);
extern void func_00152040(unsigned char *self, unsigned char *st);
extern void func_00152330(unsigned char *self, unsigned char *st);
extern void func_00152660(unsigned char *self, unsigned char *st);
extern void func_00152930(unsigned char *self, unsigned char *st);
extern void func_00152B60(unsigned char *self, unsigned char *st);
extern void func_001FBD50(unsigned char *self, int id, int flags, float dist);
extern void func_00153770(unsigned char *self, unsigned char *st);
extern void func_001B4810(unsigned char *self);
extern void func_00152F20(unsigned char *self, unsigned char *st);
extern void func_00153180(unsigned char *self, unsigned char *st);
extern int anim_advance_time(unsigned char *self, float dt);
extern void func_00152EB0(unsigned char *self);
extern void func_001C68C0(unsigned char *self);
extern void func_001B17A0(unsigned char *self);

void func_00151770(unsigned char *self, unsigned char *st)
{
    unsigned char snd;

    if (D_00810364 >= 75.0f) {
        st[0x58] = 1;
    } else {
        st[0x58] = 0;
    }

    switch (self[5]) {
    case 0:
        func_00151940(self, st);
        break;
    case 1:
        func_00151E50(self, st);
        break;
    case 2:
        func_00152040(self, st);
        break;
    case 3:
        func_00152330(self, st);
        break;
    case 4:
        func_00152660(self, st);
        break;
    case 5:
        func_00152930(self, st);
        break;
    case 9:
        func_00152B60(self, st);
        break;
    }

    snd = st[0];
    if (snd == 9) {
        func_001FBD50(self, ((func_00122BB8() >> 13) & 1) + 0x883, 0, 500.0f);
    } else if (snd == 8) {
        func_001FBD50(self, 0x885, 0, 500.0f);
    }

    func_00153770(self, st);
    func_001B4810(self);
    func_00152F20(self, st);
    func_00153180(self, st);
    *(int *)st = anim_advance_time(self, *(float *)(st + 0x28));
    func_00152EB0(self);
    func_001C68C0(self);
    func_001B17A0(self);
    (*(void (**)(unsigned char *))(self + 0x4C))(self);
}
