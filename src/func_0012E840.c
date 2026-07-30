// NEARMISS func_0012E840  (vram 0x0012E840, 0x318 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.78% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body is fully decoded and the jr-table dispatch matches exactly (jtbl_0026D0D0 reloc included, all 10 case bodies byte-identical). Two residual clusters, 3 instructions of 198: (1) FIRST func_00102948 CALL - ARG SCHEDULING (2 instructions, swapped). Target: `lui v0,%hi(D_700038A0)` / `addiu a0,v0...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: per-frame update for one actor kind - direct sibling of
// func_00151770 and func_00134090 (same "stage two vectors in the EE
// scratchpad, dispatch the behaviour state, run the fixed post-update chain,
// call the actor's virtual hook" shape).  act = the actor/controller block,
// ent = the entity it drives.
//   ent[0x6D] = "actor updated this frame" flag: cleared on entry, set to 1
//               once func_001B2140(act) says the actor is live.  When it is
//               not, the whole update is skipped.
//   0x700038A0 / 0x700038B0 = the two 16-byte scratchpad vectors staged by
//               func_00102948 (from act+0xB0 and from D_00810360); the .y
//               component of each is then cleared and func_001B15D0 reduces
//               the pair to the distance stored in ent[+0x44].
//   act[5]    = behaviour-state id (0..9, 10-entry jump table) selecting one
//               of ten per-state handlers, each called with (act, ent).
//   act[6]    = state sub-step; both act[5] and act[6] are reset to 0 when the
//               scratchpad byte at 0x70003B8D is set and the state is not one
//               of 0/3/4/9 (the "uninterruptible" states).
//   act[0x5D] = animation-blend mode: 0x81 plus a func_001B4810 kick when
//               ent[0x6B] is set, otherwise 1.
//   act[0x5E] / act[0x52] bit0 = "hit/stagger" flags raised when either of
//               ent[0x63] / ent[0x65] is set.
//   act[0]    = state-machine mode written back each frame: 1 when act[0xD]
//               is 3, or when ent[0x6A] is clear and the short at act+0x34 is
//               non-zero; otherwise 3.
//   ent[0x56] / ent[0x5A] = countdown timers decremented while non-zero.
//   act[0xA]  = per-frame scratch byte cleared before func_001328D0.
//   ent[0x58] = anim_advance_time()'s result (stored as a halfword); when its
//               low nibble is 9 the actor fires sound cue 0x7D3 at 300.0f.
//   act[0x4C] = per-actor callback invoked with the actor itself.

extern char D_700038A0[];
extern char D_700038B0[];
extern float D_00810360[4];

extern int func_001B2140(unsigned char *);
extern void func_00102948(char *dst, float *src);
extern float func_001B15D0(char *, char *);
extern void func_0012EB60(unsigned char *, unsigned char *);
extern void func_0012F100(unsigned char *, unsigned char *);
extern void func_0012F6C0(unsigned char *, unsigned char *);
extern void func_0012F980(unsigned char *, unsigned char *);
extern void func_0012FA50(unsigned char *, unsigned char *);
extern void func_0012FC10(unsigned char *, unsigned char *);
extern void func_001305B0(unsigned char *, unsigned char *);
extern void func_00130AB0(unsigned char *, unsigned char *);
extern void func_00131210(unsigned char *, unsigned char *);
extern void func_00131510(unsigned char *, unsigned char *);
extern void func_001B4810(unsigned char *);
extern void func_00133A20(unsigned char *, unsigned char *);
extern void func_00133DB0(unsigned char *, unsigned char *);
extern void func_001328D0(unsigned char *, unsigned char *);
extern int anim_advance_time(unsigned char *, float);
extern void func_001FBD50(unsigned char *, int, int, float);
extern void func_00131ED0(unsigned char *);
extern void func_001C68C0(unsigned char *);
extern void func_001B17A0(unsigned char *);

void func_0012E840(unsigned char *act, unsigned char *ent)
{
    unsigned char st;
    unsigned short t;

    ent[0x6D] = 0;
    if (func_001B2140(act) == 0) {
        return;
    }
    ent[0x6D] = 1;

    func_00102948(D_700038A0, (float *)(act + 0xB0));
    func_00102948(D_700038B0, D_00810360);
    *(volatile int *)0x700038B4 = 0;
    *(volatile int *)0x700038A4 = 0;
    *(float *)(ent + 0x44) = func_001B15D0(D_700038A0, D_700038B0);

    switch (act[5]) {
    case 0:
        func_0012EB60(act, ent);
        break;
    case 1:
        func_0012F100(act, ent);
        break;
    case 2:
        func_0012F6C0(act, ent);
        break;
    case 3:
        func_0012F980(act, ent);
        break;
    case 4:
        func_0012FA50(act, ent);
        break;
    case 5:
        func_0012FC10(act, ent);
        break;
    case 6:
        func_001305B0(act, ent);
        break;
    case 7:
        func_00130AB0(act, ent);
        break;
    case 8:
        func_00131210(act, ent);
        break;
    case 9:
        func_00131510(act, ent);
        break;
    }

    if (*(volatile unsigned char *)0x70003B8D != 0) {
        if (act[5] != 0) {
            st = act[5];
            if (st != 3 && st != 4 && st != 9) {
                act[5] = 0;
                act[6] = 0;
            }
        }
    }

    if (ent[0x6B] != 0) {
        act[0x5D] = 0x81;
        func_001B4810(act);
    } else {
        act[0x5D] = 1;
    }

    func_00133A20(act, ent);
    func_00133DB0(act, ent);

    if (ent[0x63] != 0 || ent[0x65] != 0) {
        act[0x5E] = 0;
        *(unsigned short *)(act + 0x52) |= 1;
    } else {
        act[0x5E] = 1;
    }

    if (act[0xD] != 3 && (ent[0x6A] != 0 || *(short *)(act + 0x34) == 0)) {
        act[0] = 3;
    } else {
        act[0] = 1;
    }

    t = *(unsigned short *)(ent + 0x56);
    if (t != 0) {
        *(unsigned short *)(ent + 0x56) = t - 1;
    }
    t = *(unsigned short *)(ent + 0x5A);
    if (t != 0) {
        *(unsigned short *)(ent + 0x5A) = t - 1;
    }

    act[0xA] = 0;
    func_001328D0(act, ent);

    *(unsigned short *)(ent + 0x58) = anim_advance_time(act, *(float *)(ent + 0x34));
    if ((*(unsigned short *)(ent + 0x58) & 0xF) == 9) {
        func_001FBD50(act, 0x7D3, 0, 300.0f);
    }

    func_00131ED0(act);
    func_001C68C0(act);
    func_001B17A0(act);
    (*(void (**)(unsigned char *))(act + 0x4C))(act);
}
