#!/usr/bin/env bash
set -euo pipefail

# --------------------------------------------------------------------
# Minimal CMSIS fetcher for STM32F446 (no submodules, no leftover .git)
# Enhanced with version tracking, verification, startup sync, and idempotency
# --------------------------------------------------------------------

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

CMSIS_CORE_DST="${ROOT_DIR}/cmsis/core"
CMSIS_DEV_INC_DST="${ROOT_DIR}/cmsis/device/include"
CMSIS_DEV_SRC_DST="${ROOT_DIR}/cmsis/device/source"
CMSIS_DEV_STARTUP_DST="${ROOT_DIR}/cmsis/device/startup"
VERSION_FILE="${ROOT_DIR}/cmsis/.version"
MANIFEST_FILE="${ROOT_DIR}/cmsis/CMSIS_MANIFEST.txt"

mkdir -p "${CMSIS_CORE_DST}" "${CMSIS_DEV_INC_DST}" "${CMSIS_DEV_SRC_DST}" "${CMSIS_DEV_STARTUP_DST}"

# --------------------------------------------------------------------
# Pinned versions (REPRODUCIBLE BUILDS)
# --------------------------------------------------------------------

CMSIS5_REMOTE="https://github.com/ARM-software/CMSIS_5.git"
CMSIS5_REF="5.9.0"

CMSISF4_REMOTE="https://github.com/STMicroelectronics/cmsis-device-f4.git"
CMSISF4_REF="v2.6.11"

CURRENT_VERSION="${CMSIS5_REF}-${CMSISF4_REF}"

# --------------------------------------------------------------------
# Idempotency check
# --------------------------------------------------------------------

if [[ -f "${VERSION_FILE}" ]] && [[ "$(cat "${VERSION_FILE}")" == "${CURRENT_VERSION}" ]]; then
  echo "[cmsis] Already up to date (${CURRENT_VERSION})"
  echo "[cmsis] Use --force to re-fetch."
  if [[ "${1:-}" != "--force" ]]; then
    exit 0
  fi
  echo "[cmsis] Force re-fetching..."
fi

# --------------------------------------------------------------------
# Helper: verify files exist
# --------------------------------------------------------------------

verify_files() {
  local dst_dir=$1
  shift
  for file in "$@"; do
    if [[ ! -f "${dst_dir}/${file}" ]]; then
      echo "ERROR: Missing file: ${dst_dir}/${file}" >&2
      exit 1
    fi
  done
}

# ====================================================================
#   Fetch CMSIS CORE (ARM)
# ====================================================================

echo "[cmsis] Fetching CMSIS core headers from ARM (${CMSIS5_REF})..."

(
  TMP_DIR="$(mktemp -d)"
  trap 'rm -rf "${TMP_DIR}"' EXIT
  cd "${TMP_DIR}"

  git init -q
  git remote add origin "${CMSIS5_REMOTE}"
  git config --local core.sparseCheckout true

  cat > .git/info/sparse-checkout <<EOF
CMSIS/Core/Include/cmsis_compiler.h
CMSIS/Core/Include/cmsis_gcc.h
CMSIS/Core/Include/cmsis_version.h
CMSIS/Core/Include/core_cm4.h
CMSIS/Core/Include/mpu_armv7.h
EOF

  git fetch origin "${CMSIS5_REF}" --depth=1 -q
  git checkout -q FETCH_HEAD

  cp -f CMSIS/Core/Include/cmsis_compiler.h "${CMSIS_CORE_DST}/"
  cp -f CMSIS/Core/Include/cmsis_gcc.h       "${CMSIS_CORE_DST}/"
  cp -f CMSIS/Core/Include/cmsis_version.h   "${CMSIS_CORE_DST}/"
  cp -f CMSIS/Core/Include/core_cm4.h        "${CMSIS_CORE_DST}/"
  cp -f CMSIS/Core/Include/mpu_armv7.h       "${CMSIS_CORE_DST}/"

  verify_files "${CMSIS_CORE_DST}" \
    cmsis_compiler.h cmsis_gcc.h cmsis_version.h core_cm4.h mpu_armv7.h
)

# ====================================================================
#   Fetch STM32F4 CMSIS Device (Headers + SystemInit + GCC Startup)
# ====================================================================

echo "[cmsis] Fetching STM32F4 device headers from ST (${CMSISF4_REF})..."

(
  TMP_DIR="$(mktemp -d)"
  trap 'rm -rf "${TMP_DIR}"' EXIT
  cd "${TMP_DIR}"

  git init -q
  git remote add origin "${CMSISF4_REMOTE}"
  git config --local core.sparseCheckout true

  cat > .git/info/sparse-checkout <<EOF
Include/stm32f4xx.h
Include/stm32f446xx.h
Include/system_stm32f4xx.h
Source/Templates/system_stm32f4xx.c
Source/Templates/gcc/startup_stm32f446xx.s
EOF

  git fetch origin "${CMSISF4_REF}" --depth=1 -q
  git checkout -q FETCH_HEAD

  # Header files
  cp -f Include/stm32f4xx.h        "${CMSIS_DEV_INC_DST}/"
  cp -f Include/stm32f446xx.h      "${CMSIS_DEV_INC_DST}/"
  cp -f Include/system_stm32f4xx.h "${CMSIS_DEV_INC_DST}/"

  # SystemInit source
  cp -f Source/Templates/system_stm32f4xx.c "${CMSIS_DEV_SRC_DST}/"

  # GCC startup assembly
  cp -f Source/Templates/gcc/startup_stm32f446xx.s "${CMSIS_DEV_STARTUP_DST}/"

  verify_files "${CMSIS_DEV_INC_DST}"       stm32f4xx.h stm32f446xx.h system_stm32f4xx.h
  verify_files "${CMSIS_DEV_SRC_DST}"       system_stm32f4xx.c
  verify_files "${CMSIS_DEV_STARTUP_DST}"   startup_stm32f446xx.s
)

# --------------------------------------------------------------------
# Write version and manifest
# --------------------------------------------------------------------

echo "${CURRENT_VERSION}" > "${VERSION_FILE}"

cat > "${MANIFEST_FILE}" <<EOF
# CMSIS Dependencies Manifest
# Generated: $(date -u +"%Y-%m-%d %H:%M:%S UTC")
# Version: ${CURRENT_VERSION}

[ARM CMSIS Core ${CMSIS5_REF}]
Repository: ${CMSIS5_REMOTE}
Files:
  - cmsis_compiler.h
  - cmsis_gcc.h
  - cmsis_version.h
  - core_cm4.h
  - mpu_armv7.h

[STM32F4 Device Package ${CMSISF4_REF}]
Repository: ${CMSISF4_REMOTE}
Headers:
  - stm32f4xx.h
  - stm32f446xx.h
  - system_stm32f4xx.h
Source:
  - system_stm32f4xx.c
Startup:
  - startup_stm32f446xx.s
EOF

# --------------------------------------------------------------------

echo "[cmsis] Done (${CURRENT_VERSION})"
echo "  - Core headers:   ${CMSIS_CORE_DST}"
echo "  - Device headers: ${CMSIS_DEV_INC_DST}"
echo "  - Device source:  ${CMSIS_DEV_SRC_DST}"
echo "  - Startup (GCC):  ${CMSIS_DEV_STARTUP_DST}"
echo "  - Manifest:       ${MANIFEST_FILE}"
