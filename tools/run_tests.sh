#!/bin/bash
# Full validation suite for CZboy: PD waveforms, envelopes, 16 presets, CPU benchmark, and official host_sim.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
MPC_SDK="${MPC_SDK:-}"
TMP="$ROOT/build_tests"
mkdir -p "$TMP"

echo "== 1/3: Phase Distortion Tuning, Waveforms, Space FX & 32-Preset Suite =="
clang++ -std=c++17 -O3 -Wall -Wextra "$ROOT/tests/test_czboy.cpp" -o "$TMP/test_czboy"
"$TMP/test_czboy"

echo ""
echo "== 2/3: Desktop CPU Benchmark =="
clang++ -std=c++17 -O3 -Wall -Wextra "$ROOT/tests/bench_czboy.cpp" -o "$TMP/bench_czboy"
"$TMP/bench_czboy"

if [ -n "$MPC_SDK" ] && [ -x "$MPC_SDK/.sdk-tools/host_sim" ]; then
  echo ""
  echo "== 3/3: Official Akai SDK host_sim Verification =="
  "$MPC_SDK/.sdk-tools/host_sim" "$ROOT/plugins/czboy/build/native/czboy.dylib" test \
    --wav "$TMP/czboy_test.wav"
fi
