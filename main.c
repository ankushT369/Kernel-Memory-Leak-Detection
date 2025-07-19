#include <stdio.h>
#include <unistd.h>

#include <vmstatlist.h>
#include <slabinfolist.h>

#include "trend.h"
#include "correlation.h"

#define INTERVAL 5
#define TOP_N 10

int main()
{
    printf("Starting Kernel Memory Leak Detector...\n");

    init_vmstat_list();
    init_slab_list();

    // Initial snapshots for both proc dirs
    parse_vmstat();
    parse_slabinfo();

    
    init_trend_tracking();

    while (1)
    {
        sleep(INTERVAL);

        parse_vmstat();
        parse_slabinfo();

        // Trend updates
        update_ema_for_slabs();
        compute_growth_for_slabs();
        update_monotonic_for_slabs();

        // Correlate VMStat & slab growth
        correlate_vmstat_slab();

        // Display alerts & rankings
        show_topN_slabs(TOP_N);
        show_vmstat_summary();
    }
    return 0;
}
