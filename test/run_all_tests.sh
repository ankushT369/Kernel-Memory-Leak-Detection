#!/bin/bash
set -e

# Colors using tput
YELLOW=$(tput setaf 3)
GREEN=$(tput setaf 2)
RED=$(tput setaf 1)
RESET=$(tput sgr0)

mkdir -p logs

log() { printf "%s[*] %s%s\n" "$YELLOW" "$1" "$RESET"; }
success() { printf "%s[+] %s%s\n" "$GREEN" "$1" "$RESET"; }
error() { printf "%s[!] %s%s\n" "$RED" "$1" "$RESET"; }

# Cleanup on Ctrl+C
cleanup() {
    error "Caught Ctrl+C, cleaning up..."
    pkill -f stress_shm || true
    pkill -f stress_kmalloc_user.sh || true
    pkill -f stress_dentry.sh || true
    pkill -f stress_tmpfs_mounts.sh || true
    for i in $(seq 1 50); do
        sudo umount /mnt/tmp$i 2>/dev/null || true
    done
    sudo rm -rf /tmp/dentry_test
    exit 1
}
trap cleanup INT

# Ensure the detector is running
if ! pgrep -f "kernel_leak_detector" > /dev/null; then
    error "Kernel leak detector is NOT running. Start it first!"
    exit 1
fi

log "Compiling stress_shm.c..."
gcc -o stress_shm stress_shm.c &> logs/compile_shm.log
success "Compiled stress_shm successfully."

log "Running dentry stressor..."
sudo ./stress_dentry.sh > logs/dentry.log 2>&1
success "Dentry stressor done."

log "Running kmalloc stressor..."
sudo ./stress_kmalloc_user.sh > logs/kmalloc.log 2>&1
success "Kmalloc stressor done."

log "Running tmpfs stressor..."
sudo ./stress_tmpfs_mounts.sh > logs/tmpfs.log 2>&1
success "Tmpfs stressor done."

log "Running shared memory stressor..."
sudo ./stress_shm > logs/shm.log 2>&1
success "Shared memory stressor done."

success "All stress tests completed. Check logs/ for details."

