#!/bin/sh

mksquashfs rootfs rootfs_squashfs.fex -noappend -comp xz
./pack/gen_upgrade --partion rootfs --version 1.0.0 --sys 316 --limit 0 --file rootfs_squashfs.fex --save ./out/316M_SF4_t1.0.3_$(date +%Y_%m_%d).img
rm -f rootfs_squashfs.fex
