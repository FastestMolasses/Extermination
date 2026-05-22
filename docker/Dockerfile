# Toolchain container for the Extermination matching decompilation.
#
# The period-correct compiler is Metrowerks CodeWarrior `mwccmips.exe`
# (2.3.1.01) — a *32-bit* Windows PE. It runs on Linux under `wibo`, a minimal
# Win32 syscall shim. The proprietary mwcc/mwld binaries are NOT baked in — the
# user supplies them under tools/mwccps2/ and they are bind-mounted at run time.
#
# IMPORTANT — Apple Silicon: this image is arm64 (native, no Rosetta). The
# CodeWarrior PE is 32-bit x86, and Rosetta 2 translates only 64-bit x86 — it
# cannot run 32-bit x86 code (it faults with "invalid gdt selector"). So wibo
# and the compiler are run through QEMU user-mode emulation (`qemu-x86_64`),
# which emulates a full x86 CPU including the 32-bit code segment wibo needs.
# The MIPS assembler is arm64-native cross-binutils, so only the compiler
# itself pays the emulation cost.
#
# Built/run with Apple's `container` CLI (see CLAUDE.md / docs/PROGRESS.md):
#   container build -t exterm-toolchain -f docker/Dockerfile docker
#   container run --rm -v "$PWD:/work" -w /work exterm-toolchain \
#       qemu-x86_64 /usr/bin/wibo tools/mwccps2/mwccmips.exe ...
FROM debian:13-slim

ARG WIBO_VERSION=1.1.0

RUN apt-get update \
 && apt-get install -y --no-install-recommends \
      python3 \
      binutils-mips-linux-gnu binutils-mipsel-linux-gnu \
      qemu-user \
      wget ca-certificates \
 && rm -rf /var/lib/apt/lists/*

# wibo: the Win32 shim that lets the CodeWarrior .exe run on Linux. It is an
# x86_64 binary — itself run under qemu-x86_64 on this arm64 image.
RUN wget -q "https://github.com/decompals/wibo/releases/download/${WIBO_VERSION}/wibo-x86_64" \
      -O /usr/bin/wibo \
 && chmod +x /usr/bin/wibo

WORKDIR /work
