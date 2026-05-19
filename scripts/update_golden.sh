#!/usr/bin/env sh
set -e

BUILD_DIR="${1:-build}"

cmake -S . -B "${BUILD_DIR}"
cmake --build "${BUILD_DIR}"

TV_UPDATE_GOLDEN=1 ctest --test-dir "${BUILD_DIR}" -R TVControllerGoldenTest --output-on-failure

echo "Golden files updated in test/golden/"
