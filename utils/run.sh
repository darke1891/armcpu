#!/bin/bash -e

./memtrans/controller.py flash erase 0 5m
./memtrans/controller.py flash write 0 ~/bachelor_thesis/LiangDun/ucore/obj/ucore-kernel-initrd
./terminal.py
