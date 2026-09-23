#!/usr/bin/env bash
# =====================================================================
# Bomb Lab Autograder  (works both locally and in GitHub Actions)
#
# Usage:
#   bash tools/autograde.sh              # grade solution.txt
#   bash tools/autograde.sh other.txt    # grade another file
#
# What it does:
#   1) verifies that `bomb` has not been modified (sha256)
#   2) sanity-checks the format of your answers (CRLF, trailing newline, line count)
#   3) grades EVERY PHASE INDEPENDENTLY via `./bomb --phase N`,
#      so failing one phase does not cost you the others
#   4) writes a Markdown report to autograde-report.md
#      (the CI appends it to the GitHub Actions summary)
# =====================================================================
set -u
cd "$(dirname "$0")/.." || exit 1

SOL=${1:-solution.txt}
MAX_SCORE=100
PHASES=5
PER=$((MAX_SCORE / PHASES))

echo "=============================================="
echo "  Bomb Lab Autograder (per-phase, independent)"
echo "=============================================="
echo

# ---------- 1) binary integrity ----------
if [ -f bomb.sha256 ]; then
  if sha256sum -c --status bomb.sha256 2>/dev/null; then
    echo "[ok] bomb integrity verified (unmodified)"
  else
    echo "[FAIL] bomb does not match bomb.sha256: the binary has been modified."
    echo "       Grading only accepts the original binary. Restore it with: git checkout -- bomb"
    exit 1
  fi
else
  echo "[warn] bomb.sha256 not found - skipping integrity check"
fi

# ---------- 2) answer file format ----------
if [ ! -f "$SOL" ]; then
  echo "[FAIL] answer file not found: $SOL"
  exit 1
fi

if grep -q $'\r' "$SOL"; then
  echo "[FAIL] CRLF line endings detected (Windows-style)."
  echo "       The bomb treats \\r as part of your answer, so phase 1 will explode."
  echo "       Fix it with: sed -i 's/\\r$//' $SOL   (or switch your editor to LF)"
  exit 1
fi

if [ -s "$SOL" ]; then
  lastbyte=$(tail -c 1 "$SOL" | od -An -tx1 | tr -d ' \n')
  if [ "$lastbyte" != "0a" ]; then
    echo "[FAIL] the last line of $SOL has no trailing newline."
    echo "       The bomb's read_line() treats the last character of a line as the newline,"
    echo "       so the final phase would silently lose its last character."
    echo "       Fix it by appending a newline to the file."
    exit 1
  fi
fi

lines=$(wc -l < "$SOL" | tr -d ' ')
if [ "$lines" -lt "$PHASES" ]; then
  echo "[warn] only $lines line(s) in the answer file (one line per phase, $PHASES needed);"
  echo "       missing lines are graded as not passed."
else
  echo "[ok] format check passed ($lines lines)"
fi
echo

# ---------- 3) grade each phase independently ----------
chmod +x ./bomb 2>/dev/null || true
tmp=$(mktemp)
trap 'rm -f "$tmp"' EXIT

passed=0
details=""

for k in $(seq 1 "$PHASES"); do
  line=$(sed -n "${k}p" "$SOL")
  printf '%s\n' "$line" > "$tmp"
  out=$(./bomb --phase "$k" "$tmp" 2>&1) && rc=0 || rc=$?

  if echo "$out" | grep -qF "[phase $k] PASS"; then
    passed=$((passed + 1))
    mark="PASS"
    note=""
  else
    mark="FAIL"
    if [ -z "$line" ]; then
      note=" (this line is empty)"
    elif [ "$line" = "???" ]; then
      note=" (still the placeholder)"
    elif [ "$rc" -eq 8 ] && echo "$out" | grep -q "Premature EOF"; then
      note=" (could not read the line - is it blank?)"
    else
      note=" (wrong answer, or wrong format for this phase)"
    fi
  fi

  details="$details
| Phase $k | $mark$note | \`$(printf '%s' "$line" | cut -c1-40)\` |"
done

score=$(( passed * PER ))

# ---------- 4) report ----------
{
  echo "## Bomb Lab Grading Report"
  echo
  echo "| Phase | Result | Your answer |"
  echo "| --- | --- | --- |$details"
  echo
  echo "### Score: $score / $MAX_SCORE  ($passed / $PHASES phases passed, $PER points each)"
  echo
  echo "> Grading is **per-phase and independent**: a failed phase does not affect the others."
  echo
  echo "Local debugging: \`./bomb solution.txt\` (all five phases in sequence), \`./bomb --phase 3\` (one phase), \`gdb -q ./bomb\`."
} > autograde-report.md

cat autograde-report.md
echo

if [ "$passed" -ne "$PHASES" ]; then
  exit 1
fi
exit 0
