# PCSX2 live-testing cheats — memory addresses (SCUS-97112)

Decoded + live-verified addresses for editing player state while comparing the
native port against PCSX2. All addresses are EE RAM; poke via the MCP
(`pcsx2_write_memory`) or raw TCP
(`printf '{"cmd":"write_memory","address":0x810CB4,"data":"7800"}\n' | nc 127.0.0.1 21512`
— data is little-endian hex bytes). Sources: FINDINGS s18 (inventory),
s28b (battery), s25 (status screen), s22/s23.

## Vital stats (player actor @ 0x008102B0)
| What | Address | Type | Notes |
|---|---|---|---|
| Health | `0x008104D0` | float | 75.0 = "075/100"; max is 100, or 60 when flag `0x008104E4` set |
| Infection | `0x008104D8` | float | 60.0 = "60%"; 100 shows INFECTED |
| Display copies | `0x00810858/5C` | — | status screen counts toward the real value ±1/frame; poke the floats, the display follows |

## Ammo (SPR4)
| What | Address | Type | Notes |
|---|---|---|---|
| Magazine | `0x00810C62` | u8 | max 30 |
| Mag-equivalent counter | `0x00810C63` | u8 | cap 98; folds to reserve 30:1 |
| Reserve | `0x00810CB4` | s16 | TOTAL pool incl. mag (reload sets mag=min(30,reserve), doesn't subtract; firing decrements both) |

## Items
- Count array: `u8[] @ 0x00810C64`, indexed by **item type id** (catalog in
  FINDINGS s42 message bank group 3): e.g. office ammo boxes 0x0B/0x0C/0x0D →
  `0x810C6F/70/71`; SPR4 mag pack 0x10 → `0x810C74`.
- Secondary-weapon rows: ids `0x810CA4/0x810CA6`, counts `0x810CA8/AA/AC/AE/B0`.

## Battery
| What | Address | Type | Notes |
|---|---|---|---|
| Current | `0x00810CB2` | u16 | HALF-units: displayed value = stored >> 1 ("04/06" = 8/12) |
| Max | `0x00810CB7` | u8 | half-units (0x0C = "06", 0x24 = "18") |
| Overlay gate | `0x00810C7F` | u8 | nonzero enables the battery readout |

## Position / world
- Teleport: write the player position **qwords at `0x00810350` and `0x00810360`**
  (x,y,z floats; both copies) — verified working for repositioning. The
  scratchpad `0x70003B40` mirror is authoritative-read but writes don't stick.
- Area/sub bytes: `0x00810700/701`. Door unlock bits: `0x00810841[area] >> door_id`.
- Transition request: write bytes `0x008106B5..B8` (area, sub|0xFF, entry, mode
  1=inter-area/2=room) to force a transition (s22 lifecycle).

## Gotchas
- Pause the VM before multi-byte pokes (`pcsx2_pause` / resume).
- The status screen reads live values — open it (Triangle) to watch edits land.
- Exec breakpoints don't fire and can degrade performance if armed repeatedly —
  use `watch_change` (raw TCP) instead; restart PCSX2 if the IOP audio wedges.
- Pad injection: `{"cmd":"pad_press","buttons":16384,"frames":12}` (Cross=0x4000,
  Circle=0x2000, Square=0x8000, Triangle=0x1000, R1=0x0800, L3=0x0002,
  Start=0x0008; sticks via pad_set lx/ly 0..255, 128 center).
