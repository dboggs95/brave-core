/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/color/brave_color_mixer.h"

#include "brave/browser/themes/brave_theme_helper_utils.h"
#include "brave/browser/ui/color/color_palette.h"
#include "chrome/browser/ui/color/chrome_color_id.h"
#include "ui/color/color_id.h"
#include "ui/color/color_provider.h"
#include "ui/color/color_recipe.h"
#include "ui/gfx/color_palette.h"
#include "ui/gfx/color_utils.h"

void AddBraveLightThemeColorMixer(ui::ColorProvider* provider,
                                  const ui::ColorProviderManager::Key& key) {
  ui::ColorMixer& mixer = provider->AddMixer();

  mixer[kColorDownloadShelfButtonText] = {gfx::kBraveGrey800};

  mixer[ui::kColorFrameActive] = {kLightFrame};
  mixer[kColorTabBackgroundInactiveFrameActive] = {ui::kColorFrameActive};
  mixer[kColorNewTabButtonBackgroundFrameActive] = {ui::kColorFrameActive};
  mixer[kColorToolbarContentAreaSeparator] = {ui::kColorFrameActive};

  mixer[ui::kColorFrameInactive] = {
      color_utils::HSLShift(kLightFrame, {-1, -1, 0.6})};
  mixer[kColorTabBackgroundInactiveFrameInactive] = {ui::kColorFrameInactive};
  mixer[kColorNewTabButtonBackgroundFrameInactive] = {ui::kColorFrameInactive};

  mixer[kColorToolbar] = {kLightToolbar};
  mixer[kColorToolbarTopSeparatorFrameActive] = {kColorToolbar};
  mixer[kColorToolbarTopSeparatorFrameInactive] = {kColorToolbar};
  mixer[kColorTabBackgroundActiveFrameActive] = {kColorToolbar};
  mixer[kColorTabBackgroundActiveFrameInactive] = {kColorToolbar};

  mixer[kColorTabForegroundActiveFrameActive] = {kLightToolbarIcon};
  mixer[kColorTabForegroundInactiveFrameActive] = {
      kColorTabForegroundActiveFrameActive};
  mixer[kColorBookmarkBarForeground] = {kColorTabForegroundActiveFrameActive};
  mixer[kColorToolbarButtonIcon] = {kColorTabForegroundActiveFrameActive};

  mixer[kColorToolbarButtonIconInactive] = {
      color_utils::AlphaBlend(kLightToolbarIcon, kLightToolbar, 0.3f)};

#if !BUILDFLAG(IS_ANDROID)
  mixer[kColorOmniboxBackground] = {GetLocationBarBackground(
      /*dark*/ false, /*private*/ false, /*hover*/ false)};
  mixer[kColorOmniboxBackgroundHovered] = {GetLocationBarBackground(
      /*dark*/ false, /*private*/ false, /*hover*/ true)};
  mixer[kColorOmniboxText] = {kLightOmniboxText};

  mixer[kColorOmniboxResultsBackground] = {GetOmniboxResultBackground(
      kColorOmniboxResultsBackground, /*dark*/ false, /*incognito*/ false)};
  mixer[kColorOmniboxResultsBackgroundHovered] = {
      GetOmniboxResultBackground(kColorOmniboxResultsBackgroundHovered,
                                 /*dark*/ false, /*incognito*/ false)};
  mixer[kColorOmniboxResultsBackgroundSelected] = {
      GetOmniboxResultBackground(kColorOmniboxResultsBackgroundSelected,
                                 /*dark*/ false, /*incognito*/ false)};
#endif  // #if !BUILDFLAG(IS_ANDROID)
}

void AddBraveDarkThemeColorMixer(ui::ColorProvider* provider,
                                 const ui::ColorProviderManager::Key& key) {
  ui::ColorMixer& mixer = provider->AddMixer();

  mixer[kColorDownloadShelfButtonText] = {SK_ColorWHITE};

  mixer[ui::kColorFrameActive] = {kDarkFrame};
  mixer[kColorTabBackgroundInactiveFrameActive] = {ui::kColorFrameActive};
  mixer[kColorNewTabButtonBackgroundFrameActive] = {ui::kColorFrameActive};

  mixer[ui::kColorFrameInactive] = {
      color_utils::HSLShift(kDarkFrame, {-1, -1, 0.6})};
  mixer[kColorTabBackgroundInactiveFrameInactive] = {ui::kColorFrameInactive};
  mixer[kColorNewTabButtonBackgroundFrameInactive] = {ui::kColorFrameInactive};

  mixer[kColorToolbar] = {kDarkToolbar};
  mixer[kColorToolbarTopSeparatorFrameActive] = {kColorToolbar};
  mixer[kColorToolbarTopSeparatorFrameInactive] = {kColorToolbar};
  mixer[kColorToolbarContentAreaSeparator] = {kColorToolbar};
  mixer[kColorTabStrokeFrameActive] = {kColorToolbar};
  mixer[kColorTabStrokeFrameInactive] = {kColorToolbar};
  mixer[kColorTabBackgroundActiveFrameActive] = {kColorToolbar};
  mixer[kColorTabBackgroundActiveFrameInactive] = {kColorToolbar};

  mixer[kColorTabForegroundActiveFrameActive] = {
      SkColorSetRGB(0xF3, 0xF3, 0xF3)};
  mixer[kColorBookmarkBarForeground] = {kColorTabForegroundActiveFrameActive};
  mixer[kColorTabForegroundInactiveFrameActive] = {
      kColorTabForegroundActiveFrameActive};
  mixer[kColorToolbarButtonIcon] = {kDarkToolbarIcon};

  mixer[kColorToolbarButtonIconInactive] = {
      color_utils::AlphaBlend(kDarkToolbarIcon, kDarkToolbar, 0.3f)};

#if !BUILDFLAG(IS_ANDROID)
  mixer[kColorOmniboxBackground] = {GetLocationBarBackground(
      /*dark*/ true, /*private*/ false, /*hover*/ false)};
  mixer[kColorOmniboxBackgroundHovered] = {GetLocationBarBackground(
      /*dark*/ true, /*private*/ false, /*hover*/ true)};
  mixer[kColorOmniboxText] = {kDarkOmniboxText};

  mixer[kColorOmniboxResultsBackground] = {GetOmniboxResultBackground(
      kColorOmniboxResultsBackground, /*dark*/ true, /*incognito*/ false)};
  mixer[kColorOmniboxResultsBackgroundHovered] = {
      GetOmniboxResultBackground(kColorOmniboxResultsBackgroundHovered,
                                 /*dark*/ true, /*incognito*/ false)};
  mixer[kColorOmniboxResultsBackgroundSelected] = {
      GetOmniboxResultBackground(kColorOmniboxResultsBackgroundSelected,
                                 /*dark*/ true, /*incognito*/ false)};
#endif  // #if !BUILDFLAG(IS_ANDROID)
}

void AddBravePrivateThemeColorMixer(ui::ColorProvider* provider,
                                    const ui::ColorProviderManager::Key& key) {
  AddBraveDarkThemeColorMixer(provider, key);

  ui::ColorMixer& mixer = provider->AddMixer();

  mixer[ui::kColorFrameActive] = {kPrivateFrame};
  mixer[kColorTabBackgroundInactiveFrameActive] = {ui::kColorFrameActive};
  mixer[kColorNewTabButtonBackgroundFrameActive] = {ui::kColorFrameActive};

  mixer[ui::kColorFrameInactive] = {
      color_utils::HSLShift(kPrivateFrame, {-1, -1, 0.55})};
  mixer[kColorTabBackgroundInactiveFrameInactive] = {ui::kColorFrameInactive};
  mixer[kColorNewTabButtonBackgroundFrameInactive] = {ui::kColorFrameInactive};

  mixer[kColorToolbar] = {kPrivateToolbar};
  mixer[kColorToolbarContentAreaSeparator] = {kColorToolbar};
  mixer[kColorTabBackgroundActiveFrameActive] = {kColorToolbar};
  mixer[kColorTabBackgroundActiveFrameInactive] = {kColorToolbar};

  mixer[kColorTabForegroundActiveFrameActive] = {
      SkColorSetRGB(0xF3, 0xF3, 0xF3)};

  mixer[kColorBookmarkBarForeground] = {SkColorSetRGB(0xFF, 0xFF, 0xFF)};
  mixer[kColorTabForegroundInactiveFrameActive] = {
      SkColorSetRGB(0xFF, 0xFF, 0xFF)};

  mixer[kColorToolbarButtonIcon] = {kDarkToolbarIcon};
  mixer[kColorToolbarButtonIconInactive] = {
      color_utils::AlphaBlend(kDarkToolbarIcon, kPrivateToolbar, 0.3f)};

#if !BUILDFLAG(IS_ANDROID)
  mixer[kColorOmniboxBackground] = {GetLocationBarBackground(
      /*dark*/ false, /*private*/ true, /*hover*/ false)};
  mixer[kColorOmniboxBackgroundHovered] = {GetLocationBarBackground(
      /*dark*/ false, /*private*/ true, /*hover*/ true)};
  mixer[kColorOmniboxText] = {kDarkOmniboxText};

  mixer[kColorOmniboxResultsBackground] = {GetOmniboxResultBackground(
      kColorOmniboxResultsBackground, /*dark*/ false, /*incognito*/ true)};
  mixer[kColorOmniboxResultsBackgroundHovered] = {
      GetOmniboxResultBackground(kColorOmniboxResultsBackgroundHovered,
                                 /*dark*/ false, /*incognito*/ true)};
  mixer[kColorOmniboxResultsBackgroundSelected] = {
      GetOmniboxResultBackground(kColorOmniboxResultsBackgroundSelected,
                                 /*dark*/ false, /*incognito*/ true)};
#endif  // #if !BUILDFLAG(IS_ANDROID)
}

void AddBraveTorThemeColorMixer(ui::ColorProvider* provider,
                                const ui::ColorProviderManager::Key& key) {
  AddBravePrivateThemeColorMixer(provider, key);

  ui::ColorMixer& mixer = provider->AddMixer();

  mixer[ui::kColorFrameActive] = {kPrivateTorFrame};
  mixer[kColorTabBackgroundInactiveFrameActive] = {ui::kColorFrameActive};
  mixer[kColorNewTabButtonBackgroundFrameActive] = {ui::kColorFrameActive};

  mixer[ui::kColorFrameInactive] = {
      color_utils::HSLShift(kPrivateTorFrame, {-1, -1, 0.55})};
  mixer[kColorTabBackgroundInactiveFrameInactive] = {ui::kColorFrameInactive};
  mixer[kColorNewTabButtonBackgroundFrameInactive] = {ui::kColorFrameInactive};

  mixer[kColorToolbar] = {kPrivateTorToolbar};
  mixer[kColorToolbarContentAreaSeparator] = {kColorToolbar};
  mixer[kColorTabBackgroundActiveFrameActive] = {kColorToolbar};
  mixer[kColorTabBackgroundActiveFrameInactive] = {kColorToolbar};

  mixer[kColorToolbarButtonIconInactive] = {
      color_utils::AlphaBlend(kDarkToolbarIcon, kPrivateTorToolbar, 0.3f)};
}
