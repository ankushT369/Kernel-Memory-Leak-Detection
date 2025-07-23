#!/bin/bash
set -e
echo "[*] Stressing kernfs_node_cache by mounting tmpfs..."
for i in $(seq 1 50); do
    mkdir -p /mnt/tmp$i
    sudo mount -t tmpfs none /mnt/tmp$i &> /dev/null
done
sleep 5

