/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/ai_chat/core/ai_chat_metrics.h"

#include <memory>

#include "base/memory/raw_ptr.h"
#include "base/test/metrics/histogram_tester.h"
#include "base/time/time.h"
#include "components/prefs/testing_pref_service.h"
#include "content/public/test/browser_task_environment.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace ai_chat {

class AIChatMetricsUnitTest : public testing::Test {
 public:
  AIChatMetricsUnitTest()
      : task_environment_(base::test::TaskEnvironment::TimeSource::MOCK_TIME) {}

  void SetUp() override {
    auto* registry = local_state_.registry();
    AIChatMetrics::RegisterPrefs(registry);
    ai_chat_metrics_ = std::make_unique<AIChatMetrics>(&local_state_);
  }

  void RecordPrompts(bool new_chats, size_t chat_count) {
    for (size_t i = 0; i < chat_count; i++) {
      if (new_chats) {
        ai_chat_metrics_->RecordNewChat();
      }
      ai_chat_metrics_->RecordNewPrompt();
    }
    task_environment_.FastForwardBy(base::Seconds(5));
  }

 protected:
  content::BrowserTaskEnvironment task_environment_;
  TestingPrefServiceSimple local_state_;
  base::HistogramTester histogram_tester_;
  std::unique_ptr<AIChatMetrics> ai_chat_metrics_;
};

TEST_F(AIChatMetricsUnitTest, Enabled) {
  histogram_tester_.ExpectTotalCount(kEnabledHistogramName, 0);
  ai_chat_metrics_->RecordEnabled(false);
  histogram_tester_.ExpectTotalCount(kEnabledHistogramName, 0);
  ai_chat_metrics_->RecordEnabled(true);
  histogram_tester_.ExpectUniqueSample(kEnabledHistogramName, 1, 1);
  ai_chat_metrics_->RecordEnabled(false);
  histogram_tester_.ExpectUniqueSample(kEnabledHistogramName, 1, 1);
}

TEST_F(AIChatMetricsUnitTest, ChatCount) {
  histogram_tester_.ExpectTotalCount(kChatCountHistogramName, 0);

  RecordPrompts(true, 1);
  histogram_tester_.ExpectUniqueSample(kChatCountHistogramName, 0, 1);

  RecordPrompts(true, 3);
  histogram_tester_.ExpectBucketCount(kChatCountHistogramName, 1, 1);

  RecordPrompts(true, 3);
  histogram_tester_.ExpectBucketCount(kChatCountHistogramName, 1, 1);
  histogram_tester_.ExpectBucketCount(kChatCountHistogramName, 2, 1);
  histogram_tester_.ExpectTotalCount(kChatCountHistogramName, 3);

  task_environment_.FastForwardBy(base::Days(4));

  histogram_tester_.ExpectBucketCount(kChatCountHistogramName, 2, 5);
  histogram_tester_.ExpectTotalCount(kChatCountHistogramName, 7);

  task_environment_.FastForwardBy(base::Days(3));

  histogram_tester_.ExpectBucketCount(kChatCountHistogramName, 2, 7);
  histogram_tester_.ExpectTotalCount(kChatCountHistogramName, 9);

  task_environment_.FastForwardBy(base::Days(3));
  histogram_tester_.ExpectTotalCount(kChatCountHistogramName, 9);
}

TEST_F(AIChatMetricsUnitTest, AvgPromptsPerChat) {
  histogram_tester_.ExpectTotalCount(kAvgPromptCountHistogramName, 0);

  RecordPrompts(true, 1);
  histogram_tester_.ExpectUniqueSample(kAvgPromptCountHistogramName, 0, 1);

  RecordPrompts(false, 2);
  histogram_tester_.ExpectBucketCount(kAvgPromptCountHistogramName, 1, 1);

  RecordPrompts(false, 4);
  histogram_tester_.ExpectBucketCount(kAvgPromptCountHistogramName, 2, 1);

  RecordPrompts(true, 1);
  histogram_tester_.ExpectBucketCount(kAvgPromptCountHistogramName, 1, 2);
  histogram_tester_.ExpectTotalCount(kAvgPromptCountHistogramName, 4);

  task_environment_.FastForwardBy(base::Days(4));

  histogram_tester_.ExpectBucketCount(kAvgPromptCountHistogramName, 1, 6);
  histogram_tester_.ExpectTotalCount(kAvgPromptCountHistogramName, 8);

  task_environment_.FastForwardBy(base::Days(3));

  histogram_tester_.ExpectBucketCount(kAvgPromptCountHistogramName, 1, 8);
  histogram_tester_.ExpectTotalCount(kAvgPromptCountHistogramName, 10);

  task_environment_.FastForwardBy(base::Days(3));
  histogram_tester_.ExpectTotalCount(kAvgPromptCountHistogramName, 10);
}

TEST_F(AIChatMetricsUnitTest, UsageDaily) {
  histogram_tester_.ExpectTotalCount(kUsageDailyHistogramName, 0);

  RecordPrompts(true, 1);
  histogram_tester_.ExpectUniqueSample(kUsageDailyHistogramName, 1, 1);
}

TEST_F(AIChatMetricsUnitTest, AcquisitionSource) {
  histogram_tester_.ExpectTotalCount(kAcquisitionSourceHistogramName, 0);

  ai_chat_metrics_->HandleOpenViaSidebar();
  histogram_tester_.ExpectTotalCount(kAcquisitionSourceHistogramName, 0);

  ai_chat_metrics_->RecordEnabled(true);
  histogram_tester_.ExpectUniqueSample(kAcquisitionSourceHistogramName, 1, 1);

  ai_chat_metrics_->RecordOmniboxOpen();
  histogram_tester_.ExpectUniqueSample(kAcquisitionSourceHistogramName, 1, 1);

  ai_chat_metrics_->RecordEnabled(true);
  histogram_tester_.ExpectTotalCount(kAcquisitionSourceHistogramName, 2);
  histogram_tester_.ExpectBucketCount(kAcquisitionSourceHistogramName, 0, 1);
}

TEST_F(AIChatMetricsUnitTest, OmniboxOpens) {
  histogram_tester_.ExpectTotalCount(kOmniboxOpensHistogramName, 0);

  for (size_t i = 0; i < 297; i++) {
    ai_chat_metrics_->RecordOmniboxSearchQuery();
  }
  histogram_tester_.ExpectTotalCount(kOmniboxOpensHistogramName, 0);

  ai_chat_metrics_->RecordEnabled(true);
  histogram_tester_.ExpectUniqueSample(kOmniboxOpensHistogramName, 0, 1);

  ai_chat_metrics_->RecordOmniboxOpen();
  histogram_tester_.ExpectBucketCount(kOmniboxOpensHistogramName, 1, 1);

  for (size_t i = 0; i < 2; i++) {
    ai_chat_metrics_->RecordOmniboxOpen();
  }
  histogram_tester_.ExpectBucketCount(kOmniboxOpensHistogramName, 1, 2);
  histogram_tester_.ExpectBucketCount(kOmniboxOpensHistogramName, 2, 1);

  task_environment_.FastForwardBy(base::Days(3));
  histogram_tester_.ExpectBucketCount(kOmniboxOpensHistogramName, 2, 4);

  for (size_t i = 0; i < 12; i++) {
    ai_chat_metrics_->RecordOmniboxOpen();
  }
  histogram_tester_.ExpectBucketCount(kOmniboxOpensHistogramName, 2, 13);
  histogram_tester_.ExpectBucketCount(kOmniboxOpensHistogramName, 3, 3);

  histogram_tester_.ExpectBucketCount(kOmniboxOpensHistogramName, 0, 1);

  task_environment_.FastForwardBy(base::Days(7));
  histogram_tester_.ExpectBucketCount(kOmniboxOpensHistogramName, 0, 2);
}

TEST_F(AIChatMetricsUnitTest, OmniboxWeekCompare) {
  histogram_tester_.ExpectTotalCount(kOmniboxWeekCompareHistogramName, 0);

  for (size_t i = 0; i < 10; i++) {
    ai_chat_metrics_->RecordOmniboxSearchQuery();
  }
  histogram_tester_.ExpectTotalCount(kOmniboxWeekCompareHistogramName, 0);

  ai_chat_metrics_->RecordEnabled(true);
  for (size_t i = 0; i < 2; i++) {
    ai_chat_metrics_->RecordOmniboxOpen();
  }
  histogram_tester_.ExpectTotalCount(kOmniboxWeekCompareHistogramName, 0);

  task_environment_.FastForwardBy(base::Days(7));

  histogram_tester_.ExpectUniqueSample(kOmniboxWeekCompareHistogramName, 0, 1);

  for (size_t i = 0; i < 10; i++) {
    ai_chat_metrics_->RecordOmniboxSearchQuery();
  }
  for (size_t i = 0; i < 3; i++) {
    ai_chat_metrics_->RecordOmniboxOpen();
  }
  histogram_tester_.ExpectBucketCount(kOmniboxWeekCompareHistogramName, 0, 13);
  histogram_tester_.ExpectBucketCount(kOmniboxWeekCompareHistogramName, 1, 1);
  histogram_tester_.ExpectTotalCount(kOmniboxWeekCompareHistogramName, 14);
}

}  // namespace ai_chat
