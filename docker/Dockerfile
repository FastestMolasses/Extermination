# Toolchain container for the Extermination matching decompilation.
#
# The period-correct compiler is Metrowerks CodeWarrior `mwccps2` (2.3.1.01) — a
# Windows PE binary. It is run on x86_64 Linux under `wibo`, a minimal Win32
# syscall shim. This image carries wibo and a MIPS assembler/linker; the
# proprietary mwccps2/mwldps2 binaries are NOT baked in — they are supplied by
# the user under tools/mwccps2/ and bind-mounted at run time.
#
# Built/run with Apple's `container` CLI (see CLAUDE.md / docs/PROGRESS.md):
#   container build --arch amd64 -t exterm-toolchain -f docker/Dockerfile .
#   container run --rm --arch amd64 -v "$PWD:/work" -w /work \
#       exterm-toolchain python3 tools/decomp/build.py compile
#
# The image is linux/amd64; on Apple Silicon it runs under emulation. wibo and
# mwccps2 are x86_64-only, so amd64 is mandatory.
FROM --platform=linux/amd64 debian:13-slim

ARG WIBO_VERSION=1.0.0

RUN apt-get update \
 && apt-get install -y --no-install-recommends \
      python3 \
      binutils-mips-linux-gnu \
      wget ca-certificates \
 && rm -rf /var/lib/apt/lists/*

# wibo: the Win32 shim that lets mwccps2.exe run on Linux.
RUN wget -q "https://github.com/decompals/wibo/releases/download/${WIBO_VERSION}/wibo-x86_64" \
      -O /usr/bin/wibo \
 && chmod +x /usr/bin/wibo

WORKDIR /work
