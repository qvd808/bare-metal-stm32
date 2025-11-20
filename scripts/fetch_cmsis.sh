#!/usr/bin/env bash
set -euo pipefail

# --------------------------------------------------------------------
# Minimal CMSIS fetcher for STM32F446 (no submodules, no leftover .git)
# Enhanced with version tracking, verification, and idempotency
# --------------------------------------------------------------------

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

CMSIS_CORE_DST="${ROOT_DIR}/cmsis/core"
CMSIS_DEV_INC_DST="${ROOT_DIR}/cmsis/device/include"
CMSIS_DEV_SRC_DST="${ROOT_DIR}/cmsis/device/source"
VERSION_FILE="${ROOT_DIR}/cmsis/.version"
MANIFEST_FILE="${ROOT_DIR}/cmsis/CMSIS_MANIFEST.txt"

mkdir -p "${CMSIS_CORE_DST}" "${CMSIS_DEV_INC_DST}" "${CMSIS_DEV_SRC_DST}"

# Pin to specific versions for reproducible builds
CMSIS5_REMOTE="https://github.com/ARM-software/CMSIS_5.git"
CMSIS5_REF="5.9.0"  # Pin to release tag instead of 'develop'

CMSISF4_REMOTE="https://github.com/STMicroelectronics/cmsis-device-f4.git"
CMSISF4_REF="v2.6.11"  # Pin to release tag instead of 'master'

CURRENT_VERSION="${CMSIS5_REF}-${CMSISF4_REF}"

# Check if already up to date
if [[ -f "${VERSION_FILE}" ]] && [[ "$(cat "${VERSION_FILE}" 2>/dev/null || echo '')" == "${CURRENT_VERSION}" ]]; then
  echo "[cmsis] Already up to date (${CURRENT_VERSION})"
  echo "[cmsis] Use --force to re-fetch"
  if [[ "${1:-}" != "--force" ]]; then
    exit 0
  fi
  echo "[cmsis] Force re-fetching..."
fi

# Function to verify files were copied
verify_files() {
  local dst_dir=$1
  shift
  local files=("$@")
  
  for file in "${files[@]}"; do
    if [[ ! -f "${dst_dir}/${file}" ]]; then
      echo "ERROR: Failed to fetch ${file}" >&2
      return 1
    fi
  done
}

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
EOF

  git fetch origin "${CMSISF4_REF}" --depth=1 -q
  git checkout -q FETCH_HEAD

  # Headers → include
  cp -f Include/stm32f4xx.h        "${CMSIS_DEV_INC_DST}/"
  cp -f Include/stm32f446xx.h      "${CMSIS_DEV_INC_DST}/"
  cp -f Include/system_stm32f4xx.h "${CMSIS_DEV_INC_DST}/"

  # Source file → source
  cp -f Source/Templates/system_stm32f4xx.c "${CMSIS_DEV_SRC_DST}/"
  
  verify_files "${CMSIS_DEV_INC_DST}" \
    stm32f4xx.h stm32f446xx.h system_stm32f4xx.h
  verify_files "${CMSIS_DEV_SRC_DST}" \
    system_stm32f4xx.c
)

# Write version tracking file
echo "${CURRENT_VERSION}" > "${VERSION_FILE}"

# Generate manifest
cat > "${MANIFEST_FILE}" <<EOF
# CMSIS Dependencies Manifest
# Generated: $(date -u +"%Y-%m-%d %H:%M:%S UTC")
# Version: ${CURRENT_VERSION}

[ARM CMSIS Core ${CMSIS5_REF}]
Repository: ${CMSIS5_REMOTE}
Reference: ${CMSIS5_REF}
Destination: ${CMSIS_CORE_DST}
Files:
  - cmsis_compiler.h
  - cmsis_gcc.h
  - cmsis_version.h
  - core_cm4.h
  - mpu_armv7.h

[STM32F4 Device Support ${CMSISF4_REF}]
Repository: ${CMSISF4_REMOTE}
Reference: ${CMSISF4_REF}
Destinations:
  Headers: ${CMSIS_DEV_INC_DST}
  Source: ${CMSIS_DEV_SRC_DST}
Files:
  - stm32f4xx.h
  - stm32f446xx.h
  - system_stm32f4xx.h
  - system_stm32f4xx.c
EOF

echo "[cmsis] Done (${CURRENT_VERSION})"
echo "  - Core:           ${CMSIS_CORE_DST}"
echo "  - Device headers: ${CMSIS_DEV_INC_DST}"
echo "  - Device source:  ${CMSIS_DEV_SRC_DST}"
echo "  - Manifest:       ${MANIFEST_FILE}"
