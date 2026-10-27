// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 97.68% NEARMISS).
// Fixes read from the original instructions:
// - Compiler is mwcc 2.3.3, like every neighbour in 0x001B0000-0x001B4FFF: the
//   jump-table address build (upper half, low add, then index scale) is the 2.3.3
//   order; 991202 scales the index first.
// - Every "not present" exit is `goto no` to the one shared `return 0` after the
//   switch (the compare chains all branch to that single block); only the mode
//   guard and the out-of-range stage return 0 directly.
// - Adjacent room pairs (5/6, 4/5, 9/10, 0/1, 2/3) compare ent[0x9E] directly;
//   through the `room` local mwcc 2.3.3 folds them into a subtract-and-range test.
// - The default arms of stage 0, stage 4, stage 19 sub-area 0 and stage 22 are
//   `switch (ent[0x9E])` lists that goto no (the order of the "1" in the branch
//   delay slots follows from that); the other default arms stay && chains.
// - Stage 13's "room 8 and up" test is `room > 7` (`>= 8` puts the compare in a
//   different register).
//
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
            if (ent[0x9E] == 5 || ent[0x9E] == 6) {
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
            switch (ent[0x9E]) {
            case 5:
            case 6:
            case 8:
            case 11:
                goto no;
            }
            return 1;
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
            if (ent[0x9E] == 4 || ent[0x9E] == 5) {
                return 1;
            }
            goto no;
        case 9:
        case 10:
            if (ent[0x9E] == 9 || ent[0x9E] == 10) {
                return 1;
            }
            goto no;
        default:
            switch (ent[0x9E]) {
            case 4:
            case 5:
            case 9:
            case 10:
                goto no;
            }
            return 1;
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
            if (ent[0x9E] == 0 || ent[0x9E] == 1) {
                return 1;
            }
            goto no;
        case 2:
        case 3:
            if (ent[0x9E] == 2 || ent[0x9E] == 3) {
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
            if (room > 7) {
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
                switch (ent[0x9E]) {
                case 4:
                case 5:
                case 7:
                case 8:
                case 9:
                    goto no;
                }
                return 1;
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
            switch (ent[0x9E]) {
            case 0:
            case 2:
            case 4:
                goto no;
            }
            return 1;
        }
    }
no:
    return 0;
}
