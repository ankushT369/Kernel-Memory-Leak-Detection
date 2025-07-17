#include "slabinfolist.h"
#include "vmstatlist.h"
#include <stdio.h>

void correlate_vmstat_slab()
{
    unsigned int free_pages = get_vmstat("nr_free_pages");
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
}
