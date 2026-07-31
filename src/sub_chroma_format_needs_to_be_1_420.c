// COMPILER: eegcc
// CFLAGS: -O2
// SEMANTICS: MPEG-2 sequence_extension() parser. func_00106948(n) pulls the next
// n bits out of the video bitstream MSB-first; func_0010A3A8() is the fatal-error
// reporter (D_0026B6B0 / D_0026B6D8 are its two message strings, which is where
// this function's splat name comes from).
//   28 bits: profile_and_level(8) progressive_sequence(1) chroma_format(2)
//            horizontal_size_ext(2) vertical_size_ext(2) bit_rate_ext(12) marker(1)
//   16 bits: vbv_buffer_size_ext(8) low_delay(1) frame_rate_ext_n(2) frame_rate_ext_d(5)
// The two extension fields are then merged into the sequence-header values held in
// D_00241420 (horizontal_size), D_00241424 (vertical_size), D_00241438 (bit_rate)
// and D_0024143C (vbv_buffer_size).

extern void func_00105148(int a0);
extern unsigned int func_00106948(int nbits);
extern void func_0010A3A8(char *msg);

extern char D_0026B6B0[];
extern char D_0026B6D8[];

extern int D_00241BB0;
extern int D_00241420;
extern int D_00241424;
extern int D_00241438;
extern int D_0024143C;
extern int D_00241444;
extern int D_00241448;
extern int D_0024144C;
extern int D_00241450;
extern int D_00241454;
extern int D_00241458;

void sub_chroma_format_needs_to_be_1_420(void)
{
    unsigned int v;
    int hsize_ext;
    int vsize_ext;
    int bitrate_ext;
    int vbv_ext;

    D_00241BB0 = 1;
    func_00105148(0);

    v = func_00106948(0x1C);
    bitrate_ext = (v >> 1) & 0xFFF;
    vsize_ext = (v >> 13) & 3;
    hsize_ext = (v >> 15) & 3;
    D_0024144C = (v >> 17) & 3;
    if (D_0024144C != 1) {
        func_0010A3A8(D_0026B6B0);
    }
    D_00241448 = (v >> 19) & 1;
    D_00241444 = v >> 20;

    v = func_00106948(0x10);
    D_00241458 = v & 0x1F;
    D_00241454 = (v >> 5) & 3;
    D_00241450 = (v >> 7) & 1;
    vbv_ext = v >> 8;
    if (D_00241444 != 0x48 && D_00241444 != 0x58) {
        func_0010A3A8(D_0026B6D8);
    }

    D_00241420 = (hsize_ext << 12) | (D_00241420 & 0xFFF);
    D_00241424 = (vsize_ext << 12) | (D_00241424 & 0xFFF);
    D_00241438 += bitrate_ext << 18;
    D_0024143C += vbv_ext << 10;
}
