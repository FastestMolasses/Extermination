# Assembler prelude for building objdiff target objects.
#
# splat's per-function disassembly writes branch-delay-slot instructions
# literally (the instruction physically after the branch). GNU as defaults to
# reordering mode, which would re-fill delay slots and corrupt the encoding —
# so the MIPS assembler must run in literal mode when assembling that asm.
# Target the EE (R5900): enables 64-bit MIPS-III instructions like sd/ld/daddu
# and the EE-specific extensions (paddub, pmaddh, etc.). Without this the
# assembler runs in mips1 mode and expands sd into two sw, breaking matches.
.set arch=r5900
.set noat
.set noreorder
.set nomacro
