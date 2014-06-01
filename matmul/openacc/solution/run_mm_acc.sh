#!/bin/bash
export PGI_ACC_TIME=1
./mmaccv0.out
# ./mmaccv1.out nra nca ncb check eps
./mmaccv1.out 2048 2048 2048 1 0.001
