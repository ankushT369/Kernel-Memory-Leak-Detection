#!/bin/bash
set -e
echo "[*] Growing dentry and inode_cache..."
mkdir -p /tmp/dentry_test
for i in $(seq 1 50000); do
    touch /tmp/dentry_test/file_$i
done
ls -R /tmp/dentry_test > /dev/null
sleep 5

