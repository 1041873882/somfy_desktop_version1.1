#!/bin/sh

./pack/gen_upgrade --partion kernel --version 1.0.0 --sys 316 --limit 777 --file ./kernel/zImage --save ./out/v316_kernel_$(date +%Y_%m_%d).img
