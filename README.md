# Kernel-Memory-Leak-Detection
User-Space tool to detect kernel memory leaks

## 1. INTRODUCTION
The Linux kernel uses slab allocators and virtual memory mechanisms to manage memory efficiently. However, uncontrolled growth in slab caches or virtual memory counters may indicate memory leaks or performance issues.
This project introduces a "user-space Kernel Memory Leak Detector" that monitors kernel memory behavior by periodically reading two key files:
- '/proc/slabinfo' – provides statistics on slab caches.
- '/proc/vmstat – reports virtual memory activity.
The tool captures and compares snapshots over time to detect abnormal growth in slab allocations or VM counters. It runs entirely in user space, requires no root privileges, and offers a command-line interface (CLI) with options to view all stats, differences between snapshots, or the top-N growing slab caches.

## 2. OBJECTIVES
The core objectives of this project are:
-  Monitor Linux kernel memory usage from user space without root access.
-  Periodically read and parse data from '/proc/slabinfo' and '/proc/vmstat'.
-  Detect growing slab caches and increasing VM counters that may indicate memory leaks.
-  Capture and compare snapshots of memory statistics over time.
-  Alert users when memory growth crosses defined thresholds.
-  Provide a CLI interface with modes to:
  - Show all entries ('-all')
  - Show only changes ('-diff')
  - Display top-N growing slab caches ('-top N')

## 3. IMPLEMENTATION
  ### 3.1 Architecture and Data Flow
  The program begins by creating a pipe and using fork() to split into two processes:
  - The child process reads memory data (/proc/slabinfo) and writes it to the pipe.
  - The parent process reads data from the pipe, handles /proc/vmstat directly, and performs all snapshot comparisons and CLI handling.

 ### 3.2 Slab Cache Monitoring (/proc/slabinfo)
  The child process:
  - Opens /proc/slabinfo and skips header lines.
  - Continuously reads each line, parses slab details (name, active and total objects, size, etc.).
  - Sends each parsed line to the parent via the write-end of the pipe.
  The parent process:
  - Uses a custom function to read complete lines from the pipe.
  - Parses each line into a structured format.
  - Stores the parsed data in a list for snapshot handling.

 ### 3.3 Snapshot Handling and Comparison
  - The program maintains two snapshot phases:
    - INIT_SNAPSHOT: Captures and stores the initial memory state.
    - CHECK_SNAPSHOT: Compares the current state with the previous snapshot.
 - On each iteration:
   - If a slab entry already exists, it checks for growth using a diff structure.
   - If a new slab appears, it is added to the snapshot.
   - Only increased values are flagged and displayed.

 ### 3.4 Virtual Memory Monitoring (/proc/vmstat)
  - The parent process (or a separate child) periodically reads /proc/vmstat.
  - Each line is parsed into key-value pairs.
  - A function updates or compares these values with the previous snapshot.
  - If a counter has increased and the phase is CHECK_SNAPSHOT, the difference is printed.

  ### 3.5 Command-Line Interface and Modes
  The tool supports the following modes via command-line arguments:
    -all --> Show all slab and memory statistics.
    -diff --> Show only changed entries (compared to last snapshot).
    -top N --> Show top-N slab caches with the highest growth in objects.
  Arguments are validated at runtime, and incorrect usage results in a help message.

 ###3.6 Output Generation
  After snapshot comparison, results are printed based on the selected mode:
  - In -diff mode: Only changed slab and VMStat entries are shown.
  - In -top N mode: Slab caches are sorted by growth and the top N are displayed.
  - In -all mode: All current entries are printed regardless of changes.
   
