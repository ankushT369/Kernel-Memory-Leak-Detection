#ifndef TREND_H
#define TREND_H

void update_ema_for_slabs();
void compute_growth_for_slabs();
void update_monotonic_for_slabs();
void init_trend_tracking();
void show_topN_slabs(int N);

#endif
