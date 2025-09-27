#!/bin/bash
# run_many.sh — loop your app and summarize correctness

APP="./build/release/myapp"
ITER=10000
LEN=100
MODE="both"   # both | int | string

usage() {
  echo "Usage: $0 [-i iterations] [-l length] [-m both|int|string]"
  exit 0
}

while getopts "i:l:m:h" opt; do
  case $opt in
    i) ITER="$OPTARG" ;;
    l) LEN="$OPTARG" ;;
    m) MODE="$OPTARG" ;;
    h) usage ;;
    *) usage ;;
  esac
done
shift $((OPTIND-1))

# Ensure the binary exists
make release >/dev/null || { echo "Build failed"; exit 1; }

run_suite () {
  local TYPE="$1"
  local iter="$2"
  local len="$3"

  local pass=0
  local fail=0
  local sp='/-\|'
  local ii=0

  echo "Running ${iter} iterations: ${TYPE} ${len}"
  # spinner start
  printf " "

  for ((k=1; k<=iter; k++)); do
    # Discard stdout; capture stderr (where your app prints the ERROR line)
    err="$("$APP" "$TYPE" "$len" 1>/dev/null 2>&1)"
    if echo "$err" | grep -q "ERROR: final list not sorted"; then
      ((fail++))
    else
      ((pass++))
    fi
    # spinner
    printf "\b${sp:ii++%${#sp}:1}"
  done

  printf "\n"
  # Percentages
  awk -v p="$pass" -v f="$fail" 'BEGIN{
    t=p+f; pc=t?100*p/t:0; fc=t?100*f/t:0;
    printf "Result for %-6s: pass=%d (%.2f%%), fail=%d (%.2f%%), total=%d\n",
           "'"$TYPE"'", p, pc, f, fc, t
  }'
}

case "$MODE" in
  both)
    run_suite int "$ITER" "$LEN"
    run_suite string "$ITER" "$LEN"
    ;;
  int|string)
    run_suite "$MODE" "$ITER" "$LEN"
    ;;
  *)
    echo "Invalid mode: $MODE (use both|int|string)"; exit 1;;
esac
