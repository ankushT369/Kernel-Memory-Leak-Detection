#!/bin/bash
set -e
echo "[*] Stressing kmalloc-* caches with user allocations..."
for i in $(seq 1 1000); do
    dd if=/dev/zero of=/dev/null bs=4096 count=10000 status=none &
done
wait
sleep 5

