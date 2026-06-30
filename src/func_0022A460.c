// NEARMISS func_0022A460  (vram 0x0022A460, 0x124 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 82.25% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring + scheduling permutation on a recovered nested state-machine switch. Rewriting as switch(o[0]) with the descending-dispatch fallthrough (case0 falls into default/case2 return-0; case3 return-1 placed at function tail) recovered the full dispatch shape and lbu loads (o made unsig...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern unsigned char D_00810040[];

int func_0022A460(int arg0) {
    unsigned char *o = D_00810040;

    switch (o[0]) {
    case 0:
        o[0] = 1;
        func_00225CF0(o, 1, 7);
        o[9] = 0;
        o[0xA] = 0;
        *(int *)(o + 0x24) = 0;
        *(int *)(o + 0x10) = 0;
        func_001FE8D0();
    default:
    case 2:
        return 0;
    case 1:
        switch (o[1]) {
        case 0:
            o[0x16] = func_00229770(o, 1);
            break;
        case 1:
            o[0x16] = func_00229C00(o, 1);
            break;
        case 2:
            o[0x16] = func_00229C90(o, 1);
            break;
        }
        if (arg0 != 0 && o[0x16] == 2) {
            return 2;
        }
        return 0;
    case 3:
        return 1;
    }
}
