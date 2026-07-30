#!/bin/bash
# ee-compile.sh <input.c> <output.o> [opt flags e.g. -O2]
# ee-gcc 2.9-991111-01 (Sony PS2 Tool-Chain DTL-S13010, decompme rebuild) — the
# compiler Sony built the PS2 SDK with (16-byte callee-save stride). cc1+as via qemu-i386.
set -e
IN="$1"; OUT="$2"; shift 2 || true; EXTRA="$*"; [ -z "$EXTRA" ] && EXTRA="-O2"
export QEMU_LD_PREFIX=/
P=tools/eegcc/ee-gcc2.9-991111-01
CC1="$P/lib/gcc-lib/ee/2.9-ee-991111-01/cc1"
AS="$P/ee/bin/as"
TS="${OUT%.o}.s"
qemu-i386 "$CC1" -lang-c -D__GNUC__=2 -Dmips -DMIPSEL -DR5900 -D__mips__ -D__MIPSEL__ -D__R5900__ \
  -D_MIPSEL -D_R5900 -D__mips -D__MIPSEL -D__R5900 -D__LANGUAGE_C -D_LANGUAGE_C -DLANGUAGE_C \
  "-D__SIZE_TYPE__=unsigned int" "-D__PTRDIFF_TYPE__=int" -D__LONG_MAX__=9223372036854775807L \
  -D__mips=3 -D__mips64 -D__mips_eabi -D__mips_single_float \
  "$IN" -G0 -quiet $EXTRA -o "$TS"
# -G0 must reach the ASSEMBLER too, not just cc1: cc1 emits `li.s $fN,<const>` for SFmode
# float constants, and without -G0 this gas routes li.s through a gp-relative .lit4 pool
# (lwc1) instead of building the value with lui/ori/mtc1 via $at — which is what the target
# does. Verified safe: all 255 already-byte-matched eegcc units rebuild byte-identical with
# and without the flag (0/255 differ); it only affects float-constant materialization.
qemu-i386 "$AS" -EL -mabi=eabi -G0 -o "$OUT" "$TS"
rm -f "$TS"
