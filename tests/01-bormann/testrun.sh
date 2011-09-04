#!/bin/sh

GHC=${GHC-../../ghc}

mkdir -p OUTPUT
${GHC} packet1.bin OUTPUT/packet1.out

(
hexdump -C packet1.bin
hexdump -C OUTPUT/packet1.out
) | tee OUTPUT/generated.txt | diff - expected.txt

