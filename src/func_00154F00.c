// All-word: everything as .word except jal/j-external (for R_MIPS_26 relocs)
//
// SEMANTICS (resolved s33 — docs/FINDINGS.md "KIND-0xE COMPANION
// RESOLVED"): tendril-field renderer — 12 re-posed draws of the one
// spike mesh. Per VALID record (+0x84): bob integrator on {phase
// +0x7C, vel +0x7E} (parent pad open >0.5: gravity -8 + kicks
// +28..41 below phase 128 = thrash; closed: gravity -1, kicks +3..7,
// vel halved at >=8; floor clamp phase 100 with fresh vel 3..7);
// scale X/Z +0x60/+0x68 = girth(+0x82)/256, scale Y +0x64 =
// phase*ramp(+0x80)/65536, alpha +0x8C = scratch+0xC faded over ramp
// 0..16; pos.xz = record X/Z; func_001C6380 TRS + the +0x4C draw
// method (func_001CB1F0). Own pos/matrix saved+restored around the
// loop (func_00102948 qword copy).
extern void func_00102948(int, int, int, int);
extern void func_00122BB8(int, int, int, int);
extern void float_to_int(int, int, int, int);
extern void func_001C6380(int, int, int, int);

asm void func_00154F00(void) {
    .word 0x27bdff80
    .word 0x7fbf0060
    .word 0x7fb50050
    .word 0x7fb40040
    .word 0x7fb30030
    .word 0x7fb20020
    .word 0x7080ae28
    .word 0x7fb10010
    .word 0x7fb00000
    .word 0x27a40070
    .word 0x26a500b0
    jal       func_00102948
    .word 0x26b001f0
    .word 0x8ea20020
    .word 0x1040000f
    .word 0x70009628
    .word 0xc4410080
    .word 0x3c023f00
    .word 0x44820000
    .word 0x00000000
    .word 0x46000836
    .word 0x00000000
    .word 0x45010004
    .word 0x70009628
    .word 0x10000005
    .word 0x24120001
    .word 0x70009628
    .word 0x10000003
    .word 0x70008e28
    .word 0x70009628
    .word 0x70008e28
    .word 0x72009e28
    .word 0x7200a628
    .word 0x86620084
    .word 0x50400074
    .word 0x26310001
    .word 0x8663007c
    .word 0x8662007e
    .word 0x00621021
    .word 0x12400005
    .word 0xa662007c
    .word 0x8662007e
    .word 0x2442fff8
    .word 0x10000004
    .word 0xa662007e
    .word 0x8662007e
    .word 0x2442ffff
    .word 0xa662007e
    .word 0x8662007c
    .word 0x28410064
    .word 0x1020000c
    .word 0x28410080
    .word 0x24020064
    jal       func_00122BB8
    .word 0xa662007c
    .word 0x00021c03
    .word 0x00031080
    .word 0x00431821
    .word 0x00031bc3
    .word 0x24620003
    .word 0x1000001d
    .word 0xa662007e
    .word 0x28410080
    .word 0x1020001a
    .word 0x00000000
    .word 0x1240000d
    .word 0x00000000
    jal       func_00122BB8
    .word 0x00000000
    .word 0x00021c03
    .word 0x000310c0
    .word 0x00431023
    .word 0x00021840
    .word 0x8662007e
    .word 0x00031bc3
    .word 0x2463001c
    .word 0x00431021
    .word 0x1000000c
    .word 0xa662007e
    jal       func_00122BB8
    .word 0x00000000
    .word 0x00021c03
    .word 0x00031080
    .word 0x00431821
    .word 0x8662007e
    .word 0x00031bc3
    .word 0x24630003
    .word 0x00431021
    .word 0xa662007e
    .word 0x00000000
    .word 0x1640000d
    .word 0x00000000
    .word 0x8663007e
    .word 0x28620008
    .word 0x14400009
    .word 0x00000000
    .word 0x44830000
    .word 0x3c023f00
    .word 0x46800060
    .word 0x44820000
    jal       float_to_int
    .word 0x46000b02
    .word 0xa662007e
    .word 0x00000000
    .word 0x86630080
    .word 0x8664007c
    .word 0x3c023780
    .word 0x44820000
    .word 0x28610010
    .word 0x00831018
    .word 0x44820800
    .word 0x00000000
    .word 0x468008e0
    .word 0x1020000b
    .word 0x460018c2
    .word 0x44830000
    .word 0x3c024180
    .word 0x468000a0
    .word 0x44820800
    .word 0xc600000c
    .word 0x46011043
    .word 0x00000000
    .word 0x46000802
    .word 0x10000004
    .word 0xe6a0008c
    .word 0xc600000c
    .word 0xe6a0008c
    .word 0x00000000
    .word 0x86630082
    .word 0x3c023b80
    .word 0x26a400b0
    .word 0x44820000
    .word 0x70802e28
    .word 0x44830800
    .word 0x00000000
    .word 0x46800860
    .word 0x46010002
    .word 0xe6a00068
    .word 0xe6a00060
    .word 0xe6a30064
    .word 0xc680001c
    .word 0xe6a000b0
    .word 0xc6800020
    jal       func_00102948
    .word 0xe6a000b8
    jal       func_001C6380
    .word 0x72a02628
    .word 0x8ea2004c
    .word 0x0040f809
    .word 0x72a02628
    .word 0x26310001
    .word 0x2a22000c
    .word 0x2673000a
    .word 0x1440ff87
    .word 0x26940008
    .word 0x26a400b0
    jal       func_00102948
    .word 0x27a50070
    jal       func_001C6380
    .word 0x72a02628
    .word 0x7bbf0060
    .word 0x7bb50050
    .word 0x7bb40040
    .word 0x7bb30030
    .word 0x7bb20020
    .word 0x7bb10010
    .word 0x7bb00000
    .word 0x03e00008
    .word 0x27bd0080
}
