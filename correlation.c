#include "slabinfolist.h"
#include "vmstatlist.h"
#include <stdio.h>

void correlate_vmstat_slab()
{
    static unsigned int prev_unreclaimable = 0;
    unsigned int free_pages = get_vmstat("nr_free_pages");
    unsigned int slab_unreclaimable = get_vmstat("nr_slab_unreclaimable");

    if (free_pages < 10000)
    {
        printf("[CORRELATION] Low free pages: %u\n", free_pages);
        list *cur = get_slab_list_head();
        while (cur)
        {
            if (cur->slab->monotonic_count >= 3)
                printf("   -> Slab %s is growing rapidly\n", cur->slab->name);
            cur = cur->next;
        }
    }

    // After printing VMStat info:
    if (free_pages < 50000 && (slab_unreclaimable > prev_unreclaimable)) {
        printf("\033[1;31m[SYSTEM ALERT] Low free memory with increasing unreclaimable slabs!\033[0m\n");
    }

    prev_unreclaimable = slab_unreclaimable;
}
