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
    printf("\n--- Top %d Growing Slabs ---\n", N);
    
    // Create temporary array for sorting
    typedef struct {
        slabinfo *slab;
        double growth_score;
    } slab_ranking;
    
    int count = list_cnt();
    if (count == 0) return;
    
    slab_ranking rankings[count];
    int idx = 0;
    
    // Fill array with slab pointers and calculate growth score
    list *cur = get_slab_list_head();
    while (cur) {
        rankings[idx].slab = cur->slab;
        // Score based on EMA and monotonic count
        rankings[idx].growth_score = cur->slab->ema * (1.0 + (0.1 * cur->slab->monotonic_count));
        idx++;
        cur = cur->next;
    }
    
    // Simple bubble sort by growth score (descending)
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (rankings[j].growth_score < rankings[j+1].growth_score) {
                slab_ranking temp = rankings[j];
                rankings[j] = rankings[j+1];
                rankings[j+1] = temp;
            }
        }
    }
    
    // Print top N slabs
    int display_count = (N < count) ? N : count;
    for (int i = 0; i < display_count; i++) {
        printf("%2d. %-20s Active: %-6u EMA: %.1f Growth: %d\n", 
               i+1, rankings[i].slab->name, 
               rankings[i].slab->active_objs, 
               rankings[i].slab->ema,
               rankings[i].slab->monotonic_count);
    }
    printf("\n");
}
