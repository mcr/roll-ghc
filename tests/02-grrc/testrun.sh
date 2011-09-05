#!/bin/sh

GHC=${GHC-../../ghc}

mkdir -p OUTPUT
${GHC} packet2.bin OUTPUT/packet2.out

(
hexdump -C packet2.bin
hexdump -C OUTPUT/packet2.out
) | tee OUTPUT/generated.txt | diff - expected.txt

