/* Copyright 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "base/files/file_path.h"
#include "base/strings/utf_string_conversions.h"
#include "base/threading/scoped_blocking_call.h"
#include "base/values.h"
#include "brave/common/importer/chrome_importer_utils.h"
#include "chrome/browser/importer/importer_list.h"
#include "chrome/grit/generated_resources.h"
#include "ui/base/l10n/l10n_util.h"

namespace {
void AddChromeToProfiles(std::vector<importer::SourceProfile>* profiles,
                         base::Value::List chrome_profiles,
                         const base::FilePath& user_data_folder,
                         const std::string& brand) {
  for (const auto& value : chrome_profiles) {
    const auto* dict = value.GetIfDict();
    if (!dict)
      continue;
    uint16_t items = importer::NONE;
    auto* profile = dict->FindString("id");
    auto* name = dict->FindString("name");
    DCHECK(profile);
    DCHECK(name);
    base::FilePath path = user_data_folder;
    if (!ChromeImporterCanImport(path.Append(base::FilePath::StringType(
                                     profile->begin(), profile->end())),
                                 &items))
      continue;
    importer::SourceProfile chrome;
    std::string importer_name(brand);
    importer_name.append(*name);
    chrome.importer_name = base::UTF8ToUTF16(importer_name);
    chrome.importer_type = importer::TYPE_CHROME;
    chrome.services_supported = items;
    chrome.source_path = user_data_folder.Append(
        base::FilePath::StringType(profile->begin(), profile->end()));
    profiles->push_back(chrome);
  }
}

void DetectChromeProfiles(std::vector<importer::SourceProfile>* profiles) {
  base::ScopedBlockingCall scoped_blocking_call(FROM_HERE,
                                                base::BlockingType::WILL_BLOCK);
  const base::FilePath chrome_user_data_folder = GetChromeUserDataFolder();
  auto chrome_profiles = GetChromeSourceProfiles(chrome_user_data_folder);
  const std::string brand_chrome("Chrome ");
  AddChromeToProfiles(profiles, std::move(chrome_profiles),
                      chrome_user_data_folder, brand_chrome);

#if !BUILDFLAG(IS_LINUX)
  const base::FilePath canary_user_data_folder = GetCanaryUserDataFolder();
  auto canary_profiles = GetChromeSourceProfiles(canary_user_data_folder);
  const std::string brandCanary("Chrome Canary ");
  AddChromeToProfiles(profiles, std::move(canary_profiles),
                      canary_user_data_folder, brandCanary);
#endif

  const base::FilePath chromium_user_data_folder = GetChromiumUserDataFolder();
  auto chromium_profiles = GetChromeSourceProfiles(chromium_user_data_folder);
  const std::string brandChromium("Chromium ");
  AddChromeToProfiles(profiles, std::move(chromium_profiles),
                      chromium_user_data_folder, brandChromium);
}

}  // namespace

#include "src/chrome/browser/importer/importer_list.cc"
