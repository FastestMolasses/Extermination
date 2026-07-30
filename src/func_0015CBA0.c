// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: pure state -> action-code mapping for an entity record.
// Reads the entity state byte at +0x1F0 and dispatches it through
// jtbl_0026D4E0 (71 entries, 0..0x46) to select an action code written as a
// word to +0x230.  Most states map to a constant; a few refine the code using
// the sub-state byte at +0x1F1, the flag byte at +0x236 or the mode byte at
// +0x0D.  State 51 leaves +0x230 untouched; state 66 and every out-of-range
// state store 0.  No calls, no side effects other than the +0x230 store.

void func_0015CBA0(unsigned char *p) {
    switch (p[0x1F0]) {
    case 0: case 2: case 3: case 4: case 5: case 65:
        if (p[0x236]) { *(int *)(p + 0x230) = 2; } else { *(int *)(p + 0x230) = 1; }
        break;
    case 1: case 6: case 7: case 15: case 58:
        if (p[0x236]) { *(int *)(p + 0x230) = 4; } else { *(int *)(p + 0x230) = 3; }
        break;
    case 8: case 9: case 11: case 12: case 20:
        *(int *)(p + 0x230) = 5;
        break;
    case 13: case 19:
        *(int *)(p + 0x230) = 0x17;
        break;
    case 14:
        *(int *)(p + 0x230) = 0x18;
        break;
    case 16: case 18:
        *(int *)(p + 0x230) = 0xA;
        break;
    case 17:
        *(int *)(p + 0x230) = 0xB;
        break;
    case 10:
        *(int *)(p + 0x230) = 0x2B;
        break;
    case 21:
        *(int *)(p + 0x230) = 6;
        break;
    case 22:
        *(int *)(p + 0x230) = 7;
        break;
    case 23: case 25: case 26:
        *(int *)(p + 0x230) = 8;
        break;
    case 24:
        *(int *)(p + 0x230) = 9;
        break;
    case 27:
        *(int *)(p + 0x230) = 6;
        break;
    case 28:
        *(int *)(p + 0x230) = 7;
        break;
    case 29: case 31:
        *(int *)(p + 0x230) = 0x19;
        break;
    case 30:
        *(int *)(p + 0x230) = 9;
        break;
    case 32:
        *(int *)(p + 0x230) = 0x1D;
        break;
    case 33:
        if (p[0x1F1] == 0) { *(int *)(p + 0x230) = 0x1F; } else { *(int *)(p + 0x230) = 0x1E; }
        break;
    case 34:
        if (p[0x1F1] == 0) { *(int *)(p + 0x230) = 0x21; } else { *(int *)(p + 0x230) = 0x20; }
        break;
    case 35:
        *(int *)(p + 0x230) = 0x22;
        break;
    case 36:
        *(int *)(p + 0x230) = 0x23;
        break;
    case 37: case 38: case 39:
        *(int *)(p + 0x230) = 0x24;
        break;
    case 40:
        *(int *)(p + 0x230) = 0x25;
        break;
    case 41:
        *(int *)(p + 0x230) = 0x1A;
        break;
    case 42:
        *(int *)(p + 0x230) = 0x1B;
        break;
    case 43:
        *(int *)(p + 0x230) = 0x1C;
        break;
    case 49:
        if (p[0x1F1] == 2) { *(int *)(p + 0x230) = 1; } else { *(int *)(p + 0x230) = 0xD; }
        break;
    case 52:
        if (p[0x1F1] == 2) { *(int *)(p + 0x230) = 0x24; } else { *(int *)(p + 0x230) = 0x2A; }
        break;
    case 50:
        if (p[0x1F1] == 2) { *(int *)(p + 0x230) = 1; } else { *(int *)(p + 0x230) = 0xC; }
        break;
    case 53:
        if (p[0x1F1] == 2) { *(int *)(p + 0x230) = 0x24; } else { *(int *)(p + 0x230) = 0x29; }
        break;
    case 56: case 57:
        if (p[0x1F1] == 2) { *(int *)(p + 0x230) = 1; } else { *(int *)(p + 0x230) = 0x28; }
        break;
    case 70:
        *(int *)(p + 0x230) = 0x2F;
        break;
    case 54: case 55:
        if (p[0x236]) { *(int *)(p + 0x230) = 0xF; } else { *(int *)(p + 0x230) = 0xE; }
        break;
    case 44:
        if (p[0xD] == 0 || p[0xD] == 1) {
            *(int *)(p + 0x230) = 0x10;
        } else if (p[0xD] == 2) {
            if (p[0x1F1] == 0) { *(int *)(p + 0x230) = 0xA; } else { *(int *)(p + 0x230) = 0x10; }
        } else {
            *(int *)(p + 0x230) = 5;
        }
        break;
    case 45:
        *(int *)(p + 0x230) = 0x11;
        break;
    case 46:
        *(int *)(p + 0x230) = 0x12;
        break;
    case 47:
        if (p[0x1F1] == 0) {
            *(int *)(p + 0x230) = 0x14;
        } else if (p[0x1F1] == 1) {
            *(int *)(p + 0x230) = 0x15;
        } else {
            *(int *)(p + 0x230) = 0x16;
        }
        break;
    case 48:
        *(int *)(p + 0x230) = 0x13;
        break;
    case 59: case 60: case 61: case 62:
        if (p[0x236] == 0) { *(int *)(p + 0x230) = 1; } else { *(int *)(p + 0x230) = 2; }
        break;
    case 63:
        *(int *)(p + 0x230) = 0x26;
        break;
    case 64:
        *(int *)(p + 0x230) = 0x27;
        break;
    case 67:
        *(int *)(p + 0x230) = 0x2C;
        break;
    case 68:
        *(int *)(p + 0x230) = 0x2D;
        break;
    case 69:
        *(int *)(p + 0x230) = 0x2E;
        break;
    case 51:
        break;
    default:
        *(int *)(p + 0x230) = 0;
        break;
    }
}
