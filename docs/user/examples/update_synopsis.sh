#!/bin/bash

subdirs=("box-2d" "box-3d" "reverse-2d" "strikeslip-2d" "crustal-strikeslip-2d" "crustal-strikeslip-3d" "subduction-2d" "subduction-3d" "magma-2d")

for d in ${subdirs[*]}; do
    pushd $d && pylith_cfgsearch --display=features --output-format=markdown --path=../../../../examples/$d && popd
done

# Remove superfluous metadata files (not used).
rm strikeslip-2d/step01_slip_cubit-synopsis.md
rm crustal-strikeslip-2d/step01_slip_cubit-synopsis.md
rm crustal-strikeslip-3d/step01_slip_cubit-synopsis.md
