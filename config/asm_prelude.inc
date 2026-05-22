# Assembler prelude for building objdiff target objects.
#
# splat's per-function disassembly writes branch-delay-slot instructions
# literally (the instruction physically after the branch). GNU as defaults to
# reordering mode, which would re-fill delay slots and corrupt the encoding —
# so the MIPS assembler must run in literal mode when assembling that asm.
.set noat
.set noreorder
.set nomacro
