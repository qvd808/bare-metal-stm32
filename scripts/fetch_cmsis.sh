#!/usr/bin/env bash
set -euo pipefail

# --------------------------------------------------------------------
# Minimal CMSIS fetcher for STM32F446 (no submodules, no leftover .git)
# --------------------------------------------------------------------

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

CMSIS_CORE_DST="${ROOT_DIR}/cmsis/core"
CMSIS_DEV_INC_DST="${ROOT_DIR}/cmsis/device/include"
CMSIS_DEV_SRC_DST="${ROOT_DIR}/cmsis/device/source"

mkdir -p "${CMSIS_CORE_DST}" "${CMSIS_DEV_INC_DST}" "${CMSIS_DEV_SRC_DST}"

# You can pin these later
CMSIS5_REMOTE="https://github.com/ARM-software/CMSIS_5.git"
CMSIS5_REF="develop"

CMSISF4_REMOTE="https://github.com/STMicroelectronics/cmsis-device-f4.git"
CMSISF4_REF="master"

echo "[cmsis] Fetching minimal CMSIS core headers from ARM (${CMSIS5_REF})..."

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

  git fetch origin "${CMSIS5_REF}" --depth=1
  git checkout -q FETCH_HEAD

  cp -f CMSIS/Core/Include/cmsis_compiler.h "${CMSIS_CORE_DST}/"
  cp -f CMSIS/Core/Include/cmsis_gcc.h       "${CMSIS_CORE_DST}/"
  cp -f CMSIS/Core/Include/cmsis_version.h   "${CMSIS_CORE_DST}/"
  cp -f CMSIS/Core/Include/core_cm4.h        "${CMSIS_CORE_DST}/"
  cp -f CMSIS/Core/Include/mpu_armv7.h       "${CMSIS_CORE_DST}/"
)

echo "[cmsis] Fetching minimal STM32F4 device headers from ST (${CMSISF4_REF})..."

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

  git fetch origin "${CMSISF4_REF}" --depth=1
  git checkout -q FETCH_HEAD

  # Headers → include
  mv Include/stm32f4xx.h        "${CMSIS_DEV_INC_DST}/"
  mv Include/stm32f446xx.h      "${CMSIS_DEV_INC_DST}/"
  mv Include/system_stm32f4xx.h "${CMSIS_DEV_INC_DST}/"

  # Source file → source
  mv Source/Templates/system_stm32f4xx.c "${CMSIS_DEV_SRC_DST}/"
)

echo "[cmsis] Done."
echo "  - Core:    ${CMSIS_CORE_DST}"
echo "  - Device headers: ${CMSIS_DEV_INC_DST}"
echo "  - Device source:  ${CMSIS_DEV_SRC_DST}"
