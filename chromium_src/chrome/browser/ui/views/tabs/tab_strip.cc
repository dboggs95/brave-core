/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/tabs/brave_tab_container.h"
#include "brave/browser/ui/views/tabs/brave_vertical_tab_utils.h"

#define TabContainer BraveTabContainer
#define BRAVE_TAB_STRIP_CALCULATE_INDEX_FOR_VERTICAL_TABS                      \
  if (ShouldShowVerticalTabs()) {                                              \
    const int ideal_y =                                                        \
        candidate_index == 0 ? 0 : ideal_bounds(candidate_index - 1).bottom(); \
    const int distance = std::abs(dragged_bounds.y() - ideal_y);               \
    if (distance < min_distance) {                                             \
      min_distance = distance;                                                 \
      min_distance_index = candidate_index;                                    \
    }                                                                          \
    continue;                                                                  \
  }

#define BRAVE_TAB_STRIP_INIT_DRAG_CONTROLLER_FOR_VERTICAL_TABS             \
  if (ShouldShowVerticalTabs())                                            \
    drag_controller_->Init(this, source, dragging_views, gfx::Point(x, y), \
                           event.y(), std::move(selection_model),          \
                           EventSourceFromEvent(event));                   \
  else  // NOLINT(readability/braces)

#define BRAVE_TAB_STRIP_CALCULATE_DRAGGED_VIEW_BOUNDS_FOR_VERTICAL_TABS \
  if (ShouldShowVerticalTabs()) {                                       \
    std::vector<gfx::Rect> bounds;                                      \
    int y = 0;                                                          \
    for (const TabSlotView* view : views) {                             \
      const int height = view->height();                                \
      bounds.push_back(gfx::Rect(0, y, view->width(), view->height())); \
      y += height;                                                      \
    }                                                                   \
    return bounds;                                                      \
  }

#include "src/chrome/browser/ui/views/tabs/tab_strip.cc"
#undef BRAVE_TAB_STRIP_CALCULATE_DRAGGED_VIEW_BOUNDS_FOR_VERTICAL_TABS
#undef BRAVE_TAB_STRIP_INIT_DRAG_CONTROLLER_FOR_VERTICAL_TABS
#undef BRAVE_TAB_STRIP_CALCULATE_INDEX_FOR_VERTICAL_TABS
#undef TabContainer
