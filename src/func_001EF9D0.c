// NEARMISS func_001EF9D0  (vram 0x001EF9D0, 0x350 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.89% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Integer register-allocation/coloring permutation. Logic, control flow, both switch dispatchers (correct beq dispatch order, ascending source -> mwcc descending), the signed-modulo idiom (n%4 / n%2), all stores, the shared func_001EF940 tail call, and the kind-dispatch are 100% recovered. Residual...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// NEARMISS (mwcc233 = 88.89%, 991202 = 82.52%): logic and control flow fully
// recovered. Both switch dispatchers match the target's beq dispatch shape and
// order, the signed-modulo idiom (% 4 / % 2) matches, all field stores match,
// and the kind-dispatch with its single shared func_001EF940 tail call matches.
// Sole residual is an integer register-allocation/coloring permutation: the
// target uses 6 callee-saved registers (s5=arg1, s4=ent, s3=flagp, s2=off,
// s1=base, s0=out) and keeps the 0x80000000 compare base in a0, while mwcc 2.3.3
// colors my variables into only 4 saved regs and rematerializes the constant,
// producing a 0x70-vs-0x50 frame and a systematic register rename throughout.
// Permuter territory.
//
// Resolve-and-update an entity/effect handle. The handle arg0 selects an entity
// table: if the sign bit is set, base = D_00259C70[0] and the index is the low
// 31 bits; otherwise base = D_00259C74[D_00810700[0]] and the index is arg0.
// The entity is at base + index*0x30; bail (return 0) if its +0xC field is null.
// When the float arg2 == 1.0, dispatch on the handle value: for the three magic
// IDs 0x80000026 / 0x8000002C / 0x80000067 pull a random parity via
// func_00122BB8() (taken mod 4 or mod 2) and write a state code (0x18E..0x193)
// to base+0x24; if arg2 != 1.0, those same three IDs instead write -1 to
// base+0x24. Then allocate/fetch an output record via
// func_001AFA90(*(u8*)ent) (return 0 on failure), copy three fields
// (ent+4 -> out+3, ent+8 -> out+0xD, *(ent+0xC) -> out+0x10), set out+0x38 = 1.
// If arg1 (a target object) is non-null, dispatch on the entity kind at
// ent+0x20: kind 1 -> func_001D80E0, kind 2 -> func_001D8100, kind 4 ->
// throttled func_001D7FA0(arg1, ent+0x10, 1, 0.95, -0.05) gated on a >=13-tick
// interval via the *(0x70003B68) timer vs D_00275C38 and func_0011E860. All
// non-default kinds then fall through to func_001EF940(ent, arg1). Returns out.
extern int D_00275C38;
extern int D_00259C70[2];
extern int D_00259C74[2];
extern unsigned char D_00810700[8];

extern char *func_001AFA90(int);
extern int func_00122BB8(void);
extern void func_001D80E0(int a0, int a1);
extern void func_001D8100(int a0, int a1);
extern void func_001D7FA0(int a0, int a1, int a2, float f12, float f13);
extern int func_0011E860(int a0);
extern void func_001EF940(int a0, int a1);

int func_001EF9D0(int arg0, int arg1, float arg2) {
    int base;
    int idx;
    int off;
    int ent;
    char *flagp;
    char *out;
    int r;
    int kind;
    int t;

    if (arg0 & 0x80000000) {
        base = D_00259C70[0];
        idx = arg0 & 0x7FFFFFFF;
    } else {
        base = D_00259C74[D_00810700[0]];
        idx = arg0;
    }

    off = idx * 0x30;
    flagp = (char *)(base + off + 0xC);
    if (*(int *)flagp == 0) {
        return 0;
    }

    if (arg2 == 1.0f) {
        switch (arg0) {
        case 0x80000026:
            r = func_00122BB8() % 4;
            switch (r) {
            case 0:
                *(int *)(base + 0x24) = 0x18E;
                break;
            case 1:
                *(int *)(base + 0x24) = 0x18F;
                break;
            case 2:
                *(int *)(base + 0x24) = 0x190;
                break;
            case 3:
                *(int *)(base + 0x24) = 0x191;
                break;
            }
            break;
        case 0x8000002C:
            r = func_00122BB8() % 2;
            switch (r) {
            case 0:
                *(int *)(base + 0x24) = 0x18C;
                break;
            case 1:
                *(int *)(base + 0x24) = 0x18D;
                break;
            }
            break;
        case 0x80000067:
            r = func_00122BB8() % 2;
            switch (r) {
            case 0:
                *(int *)(base + 0x24) = 0x192;
                break;
            case 1:
                *(int *)(base + 0x24) = 0x193;
                break;
            }
            break;
        }
    } else {
        switch (arg0) {
        case 0x80000026:
        case 0x8000002C:
        case 0x80000067:
            *(int *)(base + 0x24) = -1;
            break;
        }
    }

    ent = base + off;
    out = func_001AFA90(*(unsigned char *)ent);
    if (out == 0) {
        return 0;
    }
    *(unsigned char *)(out + 3) = *(unsigned char *)(ent + 4);
    *(unsigned char *)(out + 0xD) = *(unsigned char *)(ent + 8);
    *(int *)(out + 0x10) = *(int *)flagp;
    *(int *)(out + 0x38) = 1;
    if (arg1 == 0) {
        return (int)out;
    }

    kind = *(int *)(ent + 0x20);
    switch (kind) {
    case 1:
        func_001D80E0(arg1, ent + 0x10);
        break;
    case 2:
        func_001D8100(arg1, ent + 0x10);
        break;
    case 4:
        t = *(int *)0x70003B68;
        if (func_0011E860(t - D_00275C38) >= 0xD) {
            D_00275C38 = *(int *)0x70003B68;
            func_001D7FA0(arg1, ent + 0x10, 1, 0.95f, -0.05f);
        }
        break;
    }
    func_001EF940(ent, arg1);

    return (int)out;
}
