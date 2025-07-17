#include "slabinfolist.h"
#include <stdio.h>
#include <math.h>

#define EMA_ALPHA 0.30
#define GROWTH_THRESHOLD 20.0
#define MONO_LIMIT 3

void init_trend_tracking()
{
    list *cur = get_slab_list_head();
    while (cur)
    {
        cur->slab->ema = cur->slab->active_objs;
        cur->slab->prev_active_objs = cur->slab->active_objs;
        cur->slab->monotonic_count = 0;
        cur = cur->next;
    }
}

void update_ema_for_slabs()
{
    list *cur = get_slab_list_head();
    while (cur)
    {
        slabinfo *s = cur->slab;
        s->ema = EMA_ALPHA * s->active_objs + (1 - EMA_ALPHA) * s->ema;
        cur = cur->next;
    }
}

void compute_growth_for_slabs()
{
    list *cur = get_slab_list_head();
    while (cur)
    {
        slabinfo *s = cur->slab;
        double growth = 0.0;
        if (s->prev_active_objs > 0)
            growth = ((double)(s->active_objs - s->prev_active_objs) /
                      s->prev_active_objs) *
                     100.0;

        if (growth > GROWTH_THRESHOLD)
            printf("[ALERT] %s grew %.2f%%\n", s->name, growth);

        s->prev_active_objs = s->active_objs;
        cur = cur->next;
    }
}

void update_monotonic_for_slabs()
{
    list *cur = get_slab_list_head();
    while (cur)
    {
        slabinfo *s = cur->slab;
        if (s->active_objs > s->prev_active_objs)
            s->monotonic_count++;
        else
            s->monotonic_count = 0;

        if (s->monotonic_count >= MONO_LIMIT)
            printf("[ALERT] Slab %s sustained growth\n", s->name);

        cur = cur->next;
    }
}

void show_topN_slabs(int N)
{
    // Sort slabs by ema and print top N (same as orchestration code)
}
