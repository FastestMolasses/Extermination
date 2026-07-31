#!/bin/bash
# Autonomous permuter sweep: for each func with a base.c, run permuter with reseeding,
# harvest any score-0 into tools/permuter/found/. Runs inside the exterm-permuter container.
cd /work
mkdir -p tools/permuter/found
log(){ echo "[sweep $(date -u +%H:%M:%S)] $*"; }
for D in tools/permuter/work/*/; do
  FN=$(basename "$D")
  [ -f "$D/base.c" ] || continue
  [ -f "$D/settings.toml" ] || continue
  SDT=$(grep -oE 'sdatathreshold [0-9]+' "$D/compile.sh" 2>/dev/null | awk '{print $2}'); [ -z "$SDT" ] && SDT=0
  for pass in 1 2 3; do
    if ls -d "$D"/output-0-* >/dev/null 2>&1; then break; fi
    log "$FN pass $pass (sdt=$SDT)"
    cd tools/permuter
    timeout 700 python3 permuter.py "work/$FN" --stop-on-zero --best-only -j 4 >/dev/null 2>&1
    cd /work
    if ls -d "$D"/output-0-* >/dev/null 2>&1; then
      cp "$(ls -d "$D"/output-0-* | head -1)/source.c" "tools/permuter/found/$FN.c"
      log "$FN *** SCORE 0 -> tools/permuter/found/$FN.c ***"; break
    fi
    BEST=$(ls -d "$D"/output-* 2>/dev/null | sed 's#.*/output-##' | cut -d- -f1 | sort -n | head -1)
    log "$FN pass $pass best=$BEST"
    # reseed from best for next pass
    BESTDIR=$(ls -d "$D"/output-* 2>/dev/null | sort -t- -k2 -n | head -1)
    [ -n "$BESTDIR" ] && [ -f "$BESTDIR/source.c" ] && cp "$BESTDIR/source.c" "$D/base.c"
  done
done
log "SWEEP DONE. matches: $(ls tools/permuter/found/ 2>/dev/null | wc -l)"
ls tools/permuter/found/ 2>/dev/null
