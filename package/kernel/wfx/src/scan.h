/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Scan related functions.
 *
 * Copyright (c) 2017-2020, Silicon Laboratories, Inc.
 * Copyright (c) 2010, ST-Ericsson
 */
#ifndef WFX_SCAN_H
#define WFX_SCAN_H

#include <net/mac80211.h>

struct wfx_dev;
struct wfx_vif;

void wfx_hw_scan_work(struct work_struct *work);
int wfx_hw_scan(struct ieee80211_hw *hw, struct ieee80211_vif *vif,
		struct ieee80211_scan_request *req);
void wfx_cancel_hw_scan(struct ieee80211_hw *hw, struct ieee80211_vif *vif);
void wfx_scan_complete(struct wfx_vif *wvif, int nb_chan_done);

void wfx_remain_on_channel_work(struct work_struct *work);
int wfx_remain_on_channel(struct ieee80211_hw *hw, struct ieee80211_vif *vif,
			  struct ieee80211_channel *chan, int duration,
			  enum ieee80211_roc_type type);
#if (KERNEL_VERSION(5, 4, 0) > LINUX_VERSION_CODE)
int wfx_cancel_remain_on_channel(struct ieee80211_hw *hw);
#else
int wfx_cancel_remain_on_channel(struct ieee80211_hw *hw, struct ieee80211_vif *vif);
#endif

#endif
