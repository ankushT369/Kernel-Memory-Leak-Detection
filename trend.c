#include "slabinfolist.h"
#include <stdio.h>
#include <math.h>
#include <string.h> // For strcpy

#define EMA_ALPHA 0.30
#define GROWTH_THRESHOLD 20.0
#define MONO_LIMIT 3

void init_trend_tracking()
{
    // In init_trend_tracking():
    list *cur = get_slab_list_head();
    while (cur)
    {
        cur->slab->baseline_active_objs = cur->slab->active_objs;
        cur->slab->ema = cur->slab->active_objs;
        cur->slab->prev_active_objs = cur->slab->active_objs;
        cur->slab->monotonic_count = 0;
        cur->slab->growth = 0.0f;  // Initialize growth
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
        // Calculate percentage growth
        if (cur->slab->prev_active_objs > 10) {
            cur->slab->growth = ((float)cur->slab->active_objs - cur->slab->prev_active_objs) / 
                               (float)cur->slab->prev_active_objs * 100.0f;
        } else {
            // For small values, use absolute difference
            cur->slab->growth = (float)(cur->slab->active_objs - cur->slab->prev_active_objs);
        }
        
        // Add clear threshold alerts
        if (cur->slab->growth > 5.0f) {
            printf("\033[1;31m[ALERT] %s growing at %.1f%%\033[0m\n", 
                   cur->slab->name, cur->slab->growth);
        }
        
        cur = cur->next;
    }
}

void update_monotonic_for_slabs()
{
    list *cur = get_slab_list_head();
    while (cur)
    {
        slabinfo *s = cur->slab;
        if (s->active_objs > s->prev_active_objs) {
            s->monotonic_count++;
            // Persistent growth detection
            if (s->monotonic_count >= MONO_LIMIT) {
                printf("\033[1;33m[LEAK WARNING] %s has grown %d consecutive times\033[0m\n", 
                       s->name, s->monotonic_count);
            }
        } else {
            s->monotonic_count = 0;
        }
        
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
        // Trend indicator
        char trend_indicator[10] = "→";  // Default: stable
        if (rankings[i].slab->growth > 1.0f) {
            strcpy(trend_indicator, "↑");  // Growing
        } else if (rankings[i].slab->growth < -1.0f) {
            strcpy(trend_indicator, "↓");  // Shrinking
        }
        
        // Color code based on monotonic count
        char *color_code = "\033[0m";  // Default: normal
        if (rankings[i].slab->monotonic_count >= MONO_LIMIT) {
            color_code = "\033[1;31m";  // Red for potential leaks
        } else if (rankings[i].slab->growth > 5.0f) {
            color_code = "\033[1;33m";  // Yellow for high growth
        }
        
        printf("%s%2d. %-20s %s Active: %-6u EMA: %.1f Growth: %.1f%%\033[0m\n", 
               color_code, i+1, rankings[i].slab->name, trend_indicator,
               rankings[i].slab->active_objs, 
               rankings[i].slab->ema,
               rankings[i].slab->growth);
    }
    printf("\n");
}
