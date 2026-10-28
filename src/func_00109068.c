// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 96.18% NEARMISS).
// Fixes read from the original instructions:
//  - the PTS and DTS fields are read into their own locals (a/b/c and a2/b2/c2);
//    sharing one set made the "colouring wall" (pointer and field temps swapped).
//  - hdrlen is declared after the flag locals (its stack slot follows escr_flag's).
//  - the DTS default (-1) is stored before the PTS default.
//  - the PES-header path has no return of its own: it falls through to the common
//    `return 1`, so its skip tail is not merged with the padding-stream tail.
//
// MPEG-2 PES packet header parser: reads start code, stream_id and packet length,
// then for normal streams the optional header (PTS/DTS, ESCR, extension fields),
// skips the rest of the header and the payload; special streams only skip.

extern int func_001086F8(char *bs, int nbits);      /* get_bits            */
extern void func_00108748(char *bs);                /* marker_bit          */
extern void func_00108790(char *bs, int nbytes);    /* skip_bytes          */
extern void func_0010A3A8(char *msg);               /* error report        */

extern unsigned char D_002412D8[];                  /* optional-field skip-length table */
extern char D_0026B590[];                           /* "pack header" message            */

int func_00109068(char *bs, char *pes)
{
    int saved;
    int n, n2, rem, rem2;
    int pts_dts_flags, escr_flag, optflags, ext_flag;
    int hdrlen;
    unsigned int a;
    int b, c;
    unsigned int a2;
    int b2, c2;
    int f1, f2, f3, f4, f5;
    unsigned int i, len;

    *(int *)(pes + 0x28) = *(long *)(bs + 0x18);
    func_001086F8(bs, 24);                                        /* start code prefix */
    *(long *)(pes + 0x00) = (long)func_001086F8(bs, 8) << 32;     /* stream_id         */
    *(int *)(pes + 0x08) = func_001086F8(bs, 16);                 /* PES_packet_length */
    *(long *)(pes + 0x18) = -1;
    *(long *)(pes + 0x10) = -1;

    if (*(long *)(pes + 0x00) != 0xBC00000000L &&
        *(long *)(pes + 0x00) != 0xBE00000000L &&
        *(long *)(pes + 0x00) != 0xBF00000000L &&
        *(long *)(pes + 0x00) != 0xF000000000L &&
        *(long *)(pes + 0x00) != 0xF100000000L &&
        *(long *)(pes + 0x00) != 0xFF00000000L &&
        *(long *)(pes + 0x00) != 0xF200000000L &&
        *(long *)(pes + 0x00) != 0xF800000000L) {

        func_001086F8(bs, 2);                                     /* '10'              */
        *(int *)(pes + 0x0C) = func_001086F8(bs, 2);              /* scrambling ctrl   */
        func_001086F8(bs, 4);                                     /* prio/align/copy   */
        pts_dts_flags = func_001086F8(bs, 2);
        escr_flag = func_001086F8(bs, 1);
        optflags = func_001086F8(bs, 4);
        ext_flag = func_001086F8(bs, 1);
        hdrlen = func_001086F8(bs, 8);

        saved = (int)*(long *)(bs + 0x18);

        if ((pts_dts_flags & 2) != 0) {
            func_001086F8(bs, 4);
            a = func_001086F8(bs, 3);
            func_00108748(bs);
            b = func_001086F8(bs, 15);
            func_00108748(bs);
            c = func_001086F8(bs, 15);
            func_00108748(bs);
            *(long *)(pes + 0x10) = ((long)((a >> 2) & 1) << 32) |
                                    (unsigned int)((a << 30) | (b << 15) | c);
        }
        if (pts_dts_flags == 3) {
            func_001086F8(bs, 4);
            a2 = func_001086F8(bs, 3);
            func_00108748(bs);
            b2 = func_001086F8(bs, 15);
            func_00108748(bs);
            c2 = func_001086F8(bs, 15);
            func_00108748(bs);
            *(long *)(pes + 0x18) = ((long)((a2 >> 2) & 1) << 32) |
                                    (unsigned int)((a2 << 30) | (b2 << 15) | c2);
        }
        if (escr_flag == 1)
            func_001086F8(bs, 0x30);
        if (optflags != 0)
            func_001086F8(bs, D_002412D8[optflags]);

        if (ext_flag == 1) {
            f1 = func_001086F8(bs, 1);
            f2 = func_001086F8(bs, 1);
            f3 = func_001086F8(bs, 1);
            f4 = func_001086F8(bs, 1);
            func_001086F8(bs, 3);
            f5 = func_001086F8(bs, 1);
            if (f1 == 1) {
                func_001086F8(bs, 0x30);
                func_001086F8(bs, 0x30);
                func_001086F8(bs, 0x20);
            }
            if (f2 == 1) {
                func_0010A3A8(D_0026B590);
                return 0;
            }
            if (f3 == 1)
                func_001086F8(bs, 0x10);
            if (f4 == 1)
                func_001086F8(bs, 0x10);
            if (f5 == 1) {
                func_00108748(bs);
                len = func_001086F8(bs, 7);
                for (i = 0; i < len; i++)
                    func_001086F8(bs, 8);
            }
        }

        rem = hdrlen - (int)((*(long *)(bs + 0x18) - saved) >> 3);
        if (rem != 0)
            func_00108790(bs, rem);

        n = *(int *)(pes + 0x08) - hdrlen;
        rem2 = n - 3;
        *(int *)(pes + 0x24) = rem2;
        *(int *)(pes + 0x20) = *(long *)(bs + 0x18);
        if (*(long *)(pes + 0x00) == 0xBD00000000L) {
            *(long *)(pes + 0x00) |= (unsigned int)func_001086F8(bs, 32);
            rem2 = n - 7;
        }
        if (rem2 != 0)
            func_00108790(bs, rem2);
    } else if (*(long *)(pes + 0x00) == 0xBC00000000L ||
               *(long *)(pes + 0x00) == 0xBF00000000L ||
               *(long *)(pes + 0x00) == 0xF000000000L ||
               *(long *)(pes + 0x00) == 0xF100000000L ||
               *(long *)(pes + 0x00) == 0xFF00000000L ||
               *(long *)(pes + 0x00) == 0xF200000000L ||
               *(long *)(pes + 0x00) == 0xF800000000L) {
        n = *(int *)(pes + 0x08);
        if (*(long *)(pes + 0x00) == 0xBF00000000L) {
            *(long *)(pes + 0x00) |= (unsigned int)func_001086F8(bs, 32);
            n -= 4;
        }
        if (n != 0)
            func_00108790(bs, n);
    } else if (*(long *)(pes + 0x00) == 0xBE00000000L) {
        n2 = *(int *)(pes + 0x08);
        if (n2 != 0)
            func_00108790(bs, n2);
    }
    return 1;
}
