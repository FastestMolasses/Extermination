// NEARMISS func_001B2140  (vram 0x001B2140, 0x9C4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.68% via mwcc 2.3 (mwcps2-2.3-991202) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 97.68 with mwcc 991202 (-O4,p -sdatathreshold 0); 18 diff rows out of 625 instructions. Decode fully verified instruction-by-instruction against the .s (both tables, all 23 outer cases, every inner compare-chain order and every default-chain constant order). NOT a jtbl-reloc problem -- the @NN-vs...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: "is this entity in the player's current room?" predicate.
// The three globals are the live location triple - D_00810700 = stage/level id,
// D_00810701 = sub-area within the stage, D_00810702 = room within the sub-area.
// The entity carries its own pair at ent[0x9D] (sub-area) and ent[0x9E] (room).
// Returns 1 when the entity should be considered present/active, 0 otherwise.
// D_008104E0 == 0x11 is a global "everything off" mode and always returns 0.
//
// Per stage the room comparison is not a plain equality: rooms are grouped into
// equivalence classes (e.g. stage 0 treats rooms 5 and 6 as one place, and rooms
// 8 and 11 as exact-match-only). The shape repeated throughout is
//   switch (room) {
//     <members of a class>: return (entity room is in the same class);
//     default:              return (entity room is NOT in any listed class);
//   }
// so an entity in an unlisted room is visible from any other unlisted room.
// Stages 11, 14, 15, 17, 20 and 21 are always-visible; 5, 9, 10, 12 and any
// stage >= 23 are never-visible.
extern int D_008104E0;
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810702;

int func_001B2140(unsigned char *ent) {
    int room;
    int area;

    if (D_008104E0 == 0x11) {
        return 0;
    }
    switch (D_00810700) {
    case 0:
        switch (D_00810702) {
        case 5:
        case 6:
            room = ent[0x9E];
            if (room == 5 || room == 6) {
                return 1;
            }
            goto no;
        case 8:
        case 11:
            if (ent[0x9E] == D_00810702) {
                return 1;
            }
            goto no;
        default:
            room = ent[0x9E];
            if (room != 11 && room != 8 && room != 6 && room != 5) {
                return 1;
            }
            goto no;
        }
    case 1:
        switch (D_00810702) {
        case 1:
        case 8:
            if (ent[0x9E] == D_00810702) {
                return 1;
            }
            goto no;
        default:
            room = ent[0x9E];
            if (room != 8 && room != 1) {
                return 1;
            }
            goto no;
        }
    case 2:
        if (D_00810701 == 1) {
            goto no;
        }
        return 1;
    case 3:
        area = D_00810701;
        if (area != ent[0x9D]) {
            goto no;
        }
        if (area == 0) {
            switch (D_00810702) {
            case 0:
            case 2:
                room = ent[0x9E];
                if (room == 0 || room == 2) {
                    return 1;
                }
                goto no;
            default:
                room = ent[0x9E];
                if (room != 0 && room != 2) {
                    return 1;
                }
                goto no;
            }
        } else {
            switch (D_00810702) {
            case 2:
            case 4:
            case 7:
                if (ent[0x9E] == D_00810702) {
                    return 1;
                }
                goto no;
            default:
                room = ent[0x9E];
                if (room != 2 && room != 4 && room != 7) {
                    return 1;
                }
                goto no;
            }
        }
    case 4:
        switch (D_00810702) {
        case 4:
        case 5:
            room = ent[0x9E];
            if (room == 4 || room == 5) {
                return 1;
            }
            goto no;
        case 9:
        case 10:
            room = ent[0x9E];
            if (room == 9 || room == 10) {
                return 1;
            }
            goto no;
        default:
            room = ent[0x9E];
            if (room != 10 && room != 9 && room != 5 && room != 4) {
                return 1;
            }
            goto no;
        }
    case 6:
        switch (D_00810702) {
        case 1:
            if (ent[0x9E] == 1) {
                return 1;
            }
            goto no;
        default:
            if (ent[0x9E] == 1) {
                goto no;
            }
            return 1;
        }
    case 7:
        switch (D_00810701) {
        case 0:
            switch (D_00810702) {
            case 1:
            case 8:
            case 10:
            case 11:
                if (ent[0x9E] == D_00810702) {
                    return 1;
                }
                goto no;
            case 3:
            case 7:
            case 9:
            case 12:
                room = ent[0x9E];
                if (room == 3 || room == 7 || room == 9 || room == 12) {
                    return 1;
                }
                goto no;
            default:
                room = ent[0x9E];
                if (room != 12 && room != 9 && room != 7 && room != 3 &&
                    room != 11 && room != 10 && room != 8 && room != 1) {
                    return 1;
                }
                goto no;
            }
        case 2:
            return 1;
        case 3:
            switch (D_00810702) {
            case 2:
                if (ent[0x9E] == 2) {
                    return 1;
                }
                goto no;
            default:
                if (ent[0x9E] == 2) {
                    goto no;
                }
                return 1;
            }
        }
        break;
    case 8:
        switch (D_00810701) {
        case 0:
            switch (D_00810702) {
            case 1:
            case 6:
                if (ent[0x9E] == D_00810702) {
                    return 1;
                }
                goto no;
            case 3:
            case 7:
                room = ent[0x9E];
                if (room == 3 || room == 7) {
                    return 1;
                }
                goto no;
            default:
                room = ent[0x9E];
                if (room != 7 && room != 6 && room != 3 && room != 1) {
                    return 1;
                }
                goto no;
            }
        case 2:
            return 1;
        case 3:
            switch (D_00810702) {
            case 2:
                if (ent[0x9E] == 2) {
                    return 1;
                }
                goto no;
            default:
                if (ent[0x9E] == 2) {
                    goto no;
                }
                return 1;
            }
        }
        break;
    case 11:
        return 1;
    case 13:
        switch (D_00810702) {
        case 0:
        case 1:
            room = ent[0x9E];
            if (room == 0 || room == 1) {
                return 1;
            }
            goto no;
        case 2:
        case 3:
            room = ent[0x9E];
            if (room == 2 || room == 3) {
                return 1;
            }
            goto no;
        case 4:
        case 6:
            room = ent[0x9E];
            if (room == 4 || room == 6) {
                return 1;
            }
            goto no;
        case 5:
        case 7:
            room = ent[0x9E];
            if (room == 5 || room == 7) {
                return 1;
            }
            goto no;
        default:
            room = ent[0x9E];
            if (room >= 8) {
                return 1;
            }
            goto no;
        }
    case 14:
        return 1;
    case 15:
        return 1;
    case 16:
        switch (D_00810701) {
        case 0:
            switch (D_00810702) {
            case 2:
                if (ent[0x9E] == 2) {
                    return 1;
                }
                goto no;
            default:
                if (ent[0x9E] == 2) {
                    goto no;
                }
                return 1;
            }
        case 1:
            switch (D_00810702) {
            case 5:
                if (ent[0x9E] == 5) {
                    return 1;
                }
                goto no;
            case 2:
            case 4:
            case 6:
                room = ent[0x9E];
                if (room == 2 || room == 4 || room == 6) {
                    return 1;
                }
                goto no;
            default:
                room = ent[0x9E];
                if (room != 6 && room != 5 && room != 4 && room != 2) {
                    return 1;
                }
                goto no;
            }
        }
        break;
    case 17:
        return 1;
    case 18:
        switch (D_00810702) {
        case 1:
            if (ent[0x9E] == 1) {
                return 1;
            }
            goto no;
        default:
            if (ent[0x9E] == 1) {
                goto no;
            }
            return 1;
        }
    case 19:
        if (D_00810701 == 0) {
            switch (D_00810702) {
            case 4:
                if (ent[0x9E] == 4) {
                    return 1;
                }
                goto no;
            case 5:
            case 7:
            case 8:
            case 9:
                room = ent[0x9E];
                if (room == 5 || room == 7 || room == 8 || room == 9) {
                    return 1;
                }
                goto no;
            default:
                room = ent[0x9E];
                if (room != 9 && room != 8 && room != 7 && room != 5 && room != 4) {
                    return 1;
                }
                goto no;
            }
        } else {
            switch (D_00810702) {
            case 0:
            case 7:
                room = ent[0x9E];
                if (room == 0 || room == 7) {
                    return 1;
                }
                goto no;
            case 1:
            case 3:
            case 6:
                room = ent[0x9E];
                if (room == 1 || room == 3 || room == 6) {
                    return 1;
                }
                goto no;
            default:
                room = ent[0x9E];
                if (room != 7 && room != 6 && room != 3 && room != 1 && room != 0) {
                    return 1;
                }
                goto no;
            }
        }
    case 20:
        return 1;
    case 21:
        return 1;
    case 22:
        switch (D_00810702) {
        case 4:
            if (ent[0x9E] == 4) {
                return 1;
            }
            goto no;
        case 0:
        case 2:
            room = ent[0x9E];
            if (room == 0 || room == 2) {
                return 1;
            }
            goto no;
        default:
            room = ent[0x9E];
            if (room != 4 && room != 2 && room != 0) {
                return 1;
            }
            goto no;
        }
    }
no:
    return 0;
}
