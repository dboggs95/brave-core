/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/tabs/brave_vertical_tab_utils.h"

#define BRAVE_TAB_DRAG_CONTROLLER_INIT_LOC_FOR_VERTICAL_TABS                   \
  if (ShouldShowVerticalTabs()) {                                              \
    start_point_in_screen_ = gfx::Point(mouse_offset.x(), source_view_offset); \
    views::View::ConvertPointToScreen(source_view, &start_point_in_screen_);   \
    last_move_screen_loc_ = start_point_in_screen_.y();                        \
  }

#define BRAVE_TAB_DRAG_CONTROLLER_UPDATE_LOC_FOR_VERTICAL_TABS \
  if (ShouldShowVerticalTabs())                                \
    last_move_screen_loc_ = point_in_screen.y();

#define BRAVE_TAB_DRAG_CONTROLLER_THRESHOLD_FOR_VERTICAL_TABS \
  (ShouldShowVerticalTabs() &&                                \
   abs(point_in_screen.y() - last_move_screen_loc_) > threshold) ||  // NOLINT

#define BRAVE_TAB_DRAG_CONTROLLER_ATTACHED_POINT_FOR_VERTICAL_TABS \
  if (ShouldShowVerticalTabs()) {                                  \
    const int y = tab_loc.y() - mouse_offset_.y();                 \
    return gfx::Point(0, std::max(0, y));                          \
  }

#include "src/chrome/browser/ui/views/tabs/tab_drag_controller.cc"

#undef BRAVE_TAB_DRAG_CONTROLLER_ATTACHED_POINT_FOR_VERTICAL_TABS
#undef BRAVE_TAB_CONTROLLER_THRESHOLD_FOR_VERTICAL_TABS
#undef BRAVE_TAB_DRAG_CONTROLLER_UPDATE_LOC_FOR_VERTICAL_TABS
#undef BRAVE_TAB_DRAG_CONTROLLER_INIT_LOC_FOR_VERTICAL_TABS
