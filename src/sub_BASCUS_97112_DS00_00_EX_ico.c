// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: memory-card save step that writes the save's ICON file
// ("BASCUS-97112DS00-00.EX/ico", descriptor D_002739B0).
//
// `e` is the save-task record shared by the whole sub_BASCUS_97112_DS00_00_EX_*
// family:
//   e[+0x48], e[+0x4C] = card port / slot, forwarded to every async step
//   e[+0x64]           = the async sub-state word driven by func_001FE6B0
//
// D_0028A4A0 points at the loaded icon resource header. The bytes actually sent
// to the card start at  header + header[0x00] + header[0x10]  and are
// header[0x18] bytes long, so the two header words are the payload's offset
// within the resource and its length lives at +0x18.
//
// This is a pure tail call (`j func_001FE6B0`, no frame): the async transfer
// state machine's 3-way result (1 = done, 0 = busy, -1 = error) is returned to
// the caller unchanged.
extern int func_001FE6B0(unsigned char *, int, int, unsigned char *, unsigned char *, int);
extern unsigned char *D_0028A4A0;
extern unsigned char D_002739B0[];

int sub_BASCUS_97112_DS00_00_EX_ico(int *e)
{
    unsigned char *p = D_0028A4A0;

    return func_001FE6B0((unsigned char *)e + 0x64, e[0x12], e[0x13],
                         D_002739B0,
                         p + *(int *)p + *(int *)(p + 0x10),
                         *(int *)(p + 0x18));
}
