#!/bin/sh

PATH=$(pwd):$PATH export PATH
TOP=../..
GHC=${GHC-../../ghc}
export TOP
export GHC

for d in ??-*; do 
    if [ -d $d ]; then 
        rm -f $d/core
        printf "RUNNING %-20s..." $d; 
        if (cd $d && GHC=${GHC} . ./testrun.sh ); then 
            if [ -f $d/core ]; then 
                echo ...FAILED-CORE found; 
                else 
                echo ...SUCCEEDED.; 
            fi 
        else 
            echo ...FAILED.; 
        fi; 
    fi;
done

