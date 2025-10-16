#!/usr/bin/env bash
set -euo pipefail

# 專案根路徑（由 external/build_physx.sh 反推上去）
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PHYSX_ROOT="$ROOT/external/physx/physx"
PHYSX_BUILD="$ROOT/external/physx_build"
BASH_BIN="$(command -v bash)"
PY_BIN="$(command -v python3 || true)"

if [[ -z "${BASH_BIN}" ]]; then
  echo "找不到 bash，可在 Nix shell 或系統安裝 bash 後再試。"
  exit 1
fi
if [[ -z "${PY_BIN:-}" ]]; then
  echo "找不到 python3；請先安裝或在 nix-shell 中提供。" >&2
  exit 1
fi
if [[ ! -d "$PHYSX_ROOT" ]]; then
  echo "PhysX repo 不存在：$PHYSX_ROOT"
  exit 1
fi

# --- 修補 NixOS 上常見的 /bin/bash 問題 ---
# 將 PhysX 相關 .sh 的 shebang 改為 /usr/bin/env bash，並補可執行權限
fix_shebang() {
  local dir="$1"
  while IFS= read -r -d '' f; do
    # 只改第一行的 shebang
    sed -i '1s|^#! */bin/bash.*$|#!/usr/bin/env bash|' "$f" || true
    chmod +x "$f" || true
  done < <(find "$dir" -type f -name "*.sh" -print0)
}
fix_shebang "$PHYSX_ROOT"

rm -rf "$PHYSX_BUILD"
mkdir -p "$PHYSX_BUILD"

# PhysX 的建置腳本依賴這個環境變數
export PHYSX_ROOT_DIR="$PHYSX_ROOT"
export CFLAGS="${CFLAGS:-} -Wformat -Wno-error=format-security"
export CXXFLAGS="${CXXFLAGS:-} -Wformat -Wno-error=format-security"

"$PY_BIN" "$PHYSX_ROOT/buildtools/cmake_generate_projects.py" linux-gcc-cpu-only \
  --compiler gcc \
  --build-dir "$PHYSX_BUILD" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_FLAGS="-Wformat -Wno-error=format-security" \
  -DCMAKE_CXX_FLAGS="-Wformat -Wno-error=format-security" \
  -DCMAKE_C_FLAGS_RELEASE="-Wformat -Wno-error=format-security" \
  -DCMAKE_CXX_FLAGS_RELEASE="-Wformat -Wno-error=format-security" \
  -DCMAKE_C_FLAGS_CHECKED="-Wformat -Wno-error=format-security" \
  -DCMAKE_CXX_FLAGS_CHECKED="-Wformat -Wno-error=format-security" \
  -DCMAKE_C_FLAGS_PROFILE="-Wformat -Wno-error=format-security" \
  -DCMAKE_CXX_FLAGS_PROFILE="-Wformat -Wno-error=format-security" \
  -DPX_GENERATE_STATIC_LIBRARIES=ON \
  -DPX_WARNINGS_AS_ERRORS=OFF \
  -DPX_BUILDPUBLICSAMPLES=OFF \
  -DPX_COMPILE_SNIPPETS=OFF \
  -DPX_EXTRAS=OFF \
  -DPX_BUILDPVDSDK=OFF \
  -DPX_BUILD_CUDA=OFF \
  -DPX_SUPPORT_GPU_PHYSX=OFF

PHYSX_GEN_BUILD_DIR="$PHYSX_ROOT/compiler/linux-gcc-cpu-only-release"
cmake --build "$PHYSX_GEN_BUILD_DIR" --target PhysXFoundation PhysXCommon PhysX -j"$(nproc || echo 4)"
