// NEARMISS func_00188430  (vram 0x00188430, 0x114 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 82.04% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Three stacked artifacts: (1) GPR coloring swap arg0=$s1/n=$s0 (mwcc emits $s0/$s1, cascades through all saved-reg refs); (2) switch dispatch lowering (target beq==1 then beqz==0 with per-arm dsll32/dsra32 sign-extend; if/else inverts to bne); (3) D_00275BCC must be gp-rel while D_0028A56C/D_0028A...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 82.04% (mwcc 2.3.3; 991202 reaches 80.23%). Logic fully recovered.
// Bone-system init for an actor (arg0). Picks a clip id + bone-default index from
// the actor type byte at arg0+0xD (type 1 -> n=0xC,id=0x47; type 0 -> n=0xE,id=0x46),
// resolves the clip via func_001C6120(D_0028A56C, id) and binds it with
// func_001CA6E0. Stores the bone count func_001C6150(arg0+0x44) at arg0+0xC; if it
// exceeds the global cap D_00275BCC, returns 1 (failure). Otherwise allocates a
// per-bone slot func_001AF780() into the arg0+0x110[] array for each bone, mirrors
// the count to arg0+9, runs anim_bone_array_setup(count), records the default-set
// pointer D_0028A574 at arg0+0x40, applies bone_init_default_2(self, n), returns 0.
//
// WALL (multiple compiler artifacts, all permuter/orchestrator class):
//  (1) GPR coloring swap -- target puts arg0 in $s1 and n in $s0; mwcc allocates
//      them swapped ($s0/$s1), cascading through every saved-reg reference.
//  (2) switch/if lowering -- target dispatches `beq ==1` then `beqz ==0` with the
//      (short) sign-extend (dsll32/dsra32) recomputed in each arm; an if/else
//      cascade (best of tried shapes) inverts the first test to bne.
//  (3) D_00275BCC is gp-relative ($gp) in the target but a separate small-data
//      section from D_0028A56C/D_0028A574 (which are absolute); a single
//      -sdatathreshold cannot place D_00275BCC gp-rel while keeping the other two
//      absolute -- needs the real section assignment from the orchestrator link.
extern int func_001C6120(int model, short id);
extern int func_001CA6E0(char *p, int v);
extern unsigned char func_001C6150(int v);
extern int func_001AF780(void);
extern void anim_bone_array_setup(int n);
extern void bone_init_default_2(char *p, int n);
extern short D_00275BCC;
extern int D_0028A56C;
extern int D_0028A574;

int func_00188430(char *arg0) {
    int n;
    int id;
    int i;
    char *q;

    if (*(unsigned char *)(arg0 + 0xD) == 1) {
        n = (short)0xC;
        id = 0x47;
    } else if (*(unsigned char *)(arg0 + 0xD) == 0) {
        n = (short)0xE;
        id = 0x46;
    }
    func_001CA6E0(arg0, func_001C6120(D_0028A56C, (short)id));
    *(unsigned char *)(arg0 + 0xC) = func_001C6150(*(int *)(arg0 + 0x44));
    i = 0;
    if (D_00275BCC < (int)*(unsigned char *)(arg0 + 0xC)) {
        return 1;
    }
    q = arg0;
    while (i < (int)*(unsigned char *)(arg0 + 0xC)) {
        *(int *)(q + 0x110) = func_001AF780();
        q += 4;
        i += 1;
    }
    *(unsigned char *)(arg0 + 9) = *(unsigned char *)(arg0 + 0xC);
    anim_bone_array_setup(*(unsigned char *)(arg0 + 0xC));
    *(int *)(arg0 + 0x40) = D_0028A574;
    bone_init_default_2(arg0, n);
    return 0;
}
