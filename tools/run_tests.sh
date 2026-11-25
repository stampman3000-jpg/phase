#!/bin/bash
# Full validation suite for Phase: PD waveforms, envelopes, 32 presets, CPU benchmark, and official host_sim.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
MPC_SDK="${MPC_SDK:-}"
TMP="$ROOT/build_tests"
mkdir -p "$TMP"

echo "== 1/3: Phase Distortion Tuning, Waveforms, Space FX & 32-Preset Suite =="
clang++ -std=c++17 -O3 -Wall -Wextra "$ROOT/tests/test_phase.cpp" -o "$TMP/test_phase"
"$TMP/test_phase"

echo ""
echo "== 2/3: Desktop CPU Benchmark =="
clang++ -std=c++17 -O3 -Wall -Wextra "$ROOT/tests/bench_phase.cpp" -o "$TMP/bench_phase"
"$TMP/bench_phase"

if [ -n "$MPC_SDK" ] && [ -x "$MPC_SDK/.sdk-tools/host_sim" ]; then
  echo ""
  echo "== 3/3: Official Akai SDK host_sim Verification =="
  "$MPC_SDK/.sdk-tools/host_sim" "$ROOT/plugins/phase/build/native/phase.dylib" test \
    --wav "$TMP/phase_test.wav"
fi
