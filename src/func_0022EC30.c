// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// SEMANTICS: cutscene/demo scene setup -- the companion of the per-frame
// timeline driver func_0022EEF0 (see its header for the record layout).
// Called once when a scene starts, with the playback record d:
//   d[+0x6E] = scene id (signed short); ids outside 0..0x25 set up nothing
//   d[+0x7C] = audio-ducking event track      (float *, state byte d[+0x88])
//   d[+0x80] = subtitle/portrait event track  (float *, state byte d[+0x89])
//   d[+0x84] = letterbox-toggle event track   (float *, state byte d[+0x8A])
// It latches the scene start time from func_0021BAB0() into the 64-bit
// global D_00275C98, clears all three track cursors and their state bytes,
// then a 38-entry jump table binds the per-scene event tracks. Scene ids
// 0, 1, 3, 4, 9, 11, 14, 18, 22, 29, 35 and 36 bind nothing (they have no
// scripted audio/subtitle/letterbox events), so they leave all three tracks
// null. The D_0026Axxx blobs are the static float event tables in rodata:
// D_0026AAE0..D_0026AC00 are letterbox tracks, D_0026AC10..D_0026AC70 audio
// tracks and D_0026AC80..D_0026AE00 subtitle tracks.

extern long long func_0021BAB0(void);
extern long long D_00275C98;
extern float D_0026AAE0[];
extern float D_0026AB00[];
extern float D_0026AB18[];
extern float D_0026AB28[];
extern float D_0026AB38[];
extern float D_0026AB48[];
extern float D_0026AB60[];
extern float D_0026AB78[];
extern float D_0026AB88[];
extern float D_0026AB98[];
extern float D_0026ABA8[];
extern float D_0026ABB8[];
extern float D_0026ABC8[];
extern float D_0026ABE0[];
extern float D_0026AC00[];
extern float D_0026AC10[];
extern float D_0026AC30[];
extern float D_0026AC40[];
extern float D_0026AC50[];
extern float D_0026AC60[];
extern float D_0026AC70[];
extern float D_0026AC80[];
extern float D_0026ACA0[];
extern float D_0026ACB0[];
extern float D_0026ACC0[];
extern float D_0026ACD0[];
extern float D_0026ACF0[];
extern float D_0026AD10[];
extern float D_0026AD20[];
extern float D_0026AD30[];
extern float D_0026AD40[];
extern float D_0026AD60[];
extern float D_0026AD70[];
extern float D_0026AD80[];
extern float D_0026AD90[];
extern float D_0026ADA0[];
extern float D_0026ADB0[];
extern float D_0026ADD0[];
extern float D_0026ADF0[];
extern float D_0026AE00[];

void func_0022EC30(unsigned char *d)
{
    D_00275C98 = func_0021BAB0();
    *(float **)(d + 0x7C) = 0;
    *(float **)(d + 0x80) = 0;
    *(float **)(d + 0x84) = 0;
    d[0x88] = 0;
    d[0x8A] = 0;
    d[0x89] = 0;

    switch (*(short *)(d + 0x6E)) {
    case 2:
        *(float **)(d + 0x84) = D_0026AAE0;
        *(float **)(d + 0x80) = D_0026AC80;
        break;
    case 5:
        *(float **)(d + 0x80) = D_0026ACA0;
        break;
    case 6:
        *(float **)(d + 0x84) = D_0026AB00;
        *(float **)(d + 0x80) = D_0026AC80;
        break;
    case 7:
        *(float **)(d + 0x80) = D_0026ACB0;
        break;
    case 8:
        *(float **)(d + 0x80) = D_0026ACC0;
        break;
    case 10:
        *(float **)(d + 0x80) = D_0026ACD0;
        break;
    case 12:
        *(float **)(d + 0x80) = D_0026ACF0;
        *(float **)(d + 0x84) = D_0026AB18;
        break;
    case 13:
        *(float **)(d + 0x80) = D_0026AD10;
        break;
    case 15:
        *(float **)(d + 0x80) = D_0026AD20;
        break;
    case 16:
        *(float **)(d + 0x84) = D_0026AB28;
        break;
    case 17:
        *(float **)(d + 0x84) = D_0026AB38;
        break;
    case 19:
        *(float **)(d + 0x80) = D_0026AD30;
        *(float **)(d + 0x7C) = D_0026AC10;
        break;
    case 20:
        *(float **)(d + 0x80) = D_0026AD40;
        *(float **)(d + 0x7C) = D_0026AC30;
        *(float **)(d + 0x84) = D_0026AB48;
        break;
    case 21:
        *(float **)(d + 0x84) = D_0026AB60;
        break;
    case 23:
        *(float **)(d + 0x80) = D_0026AD70;
        break;
    case 24:
        *(float **)(d + 0x80) = D_0026AD80;
        *(float **)(d + 0x84) = D_0026AB78;
        break;
    case 25:
        *(float **)(d + 0x80) = D_0026AD90;
        *(float **)(d + 0x84) = D_0026AB88;
        break;
    case 26:
        *(float **)(d + 0x80) = D_0026ADA0;
        *(float **)(d + 0x84) = D_0026AB98;
        break;
    case 27:
        *(float **)(d + 0x84) = D_0026ABA8;
        break;
    case 28:
        *(float **)(d + 0x80) = D_0026ADB0;
        *(float **)(d + 0x7C) = D_0026AC40;
        *(float **)(d + 0x84) = D_0026ABB8;
        break;
    case 30:
        *(float **)(d + 0x84) = D_0026ABC8;
        *(float **)(d + 0x7C) = D_0026AC50;
        break;
    case 31:
        *(float **)(d + 0x80) = D_0026ADD0;
        *(float **)(d + 0x84) = D_0026ABE0;
        break;
    case 32:
        *(float **)(d + 0x80) = D_0026ADF0;
        *(float **)(d + 0x7C) = D_0026AC70;
        break;
    case 33:
        *(float **)(d + 0x7C) = D_0026AC60;
        *(float **)(d + 0x84) = D_0026AC00;
        break;
    case 34:
        *(float **)(d + 0x80) = D_0026AE00;
        break;
    case 37:
        *(float **)(d + 0x80) = D_0026AD60;
        break;
    }
}
