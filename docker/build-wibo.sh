#!/bin/sh
# Build the 32-bit wibo binary (tools/bin/wibo32) used to run the Metrowerks
# CodeWarrior compiler under qemu-i386. See docs/PROGRESS.md (Track A toolchain).
#
# Why a separate 32-bit build: mwccmips.exe is a 32-bit Windows PE. The prebuilt
# wibo releases are all 64-bit ELFs that run 32-bit PEs via a CPU mode-switch
# Rosetta and qemu-x86_64 user-mode cannot emulate. A genuine 32-bit wibo runs
# cleanly under qemu-i386, so we cross-compile one from source.
#
# Run once from anywhere; requires the `exterm-toolchain` image to exist
# (container build -t exterm-toolchain -f docker/Dockerfile docker). The build
# deps are installed into a throwaway container — the runtime image stays lean.
# Output: tools/bin/wibo32 (git-ignored).
set -e
cd "$(dirname "$0")/.."

container run --rm -v "$PWD:/work" -w /work exterm-toolchain sh -c '
set -e
apt-get update -qq
apt-get install -y --no-install-recommends \
  cmake ninja-build git ca-certificates \
  gcc-i686-linux-gnu g++-i686-linux-gnu \
  libclang-dev clang llvm \
  python3-pip python3-venv >/dev/null 2>&1
cat > /tmp/i686-cross.cmake <<TC
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR i686)
set(CMAKE_C_COMPILER i686-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER i686-linux-gnu-g++)
set(CMAKE_ASM_COMPILER i686-linux-gnu-gcc)
TC
git clone --depth 1 --branch 1.1.0 https://github.com/decompals/wibo.git /tmp/wibo
cmake -S /tmp/wibo -B /tmp/wibo/build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=/tmp/i686-cross.cmake \
  -DCMAKE_BUILD_TYPE=Release -DWIBO_VERSION=1.1.0 \
  -DWIBO_ENABLE_FIXTURE_TESTS=OFF -DWIBO_ENABLE_LIBURING=OFF \
  -DWIBO_ENABLE_LTO=OFF -DMI_OPT_ARCH=OFF \
  -DCMAKE_EXE_LINKER_FLAGS=-static
cmake --build /tmp/wibo/build
mkdir -p tools/bin
cp /tmp/wibo/build/wibo tools/bin/wibo32
'
echo "built tools/bin/wibo32"
