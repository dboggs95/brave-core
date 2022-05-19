/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "base/path_service.h"
#include "brave/browser/brave_content_browser_client.h"
#include "brave/components/brave_shields/browser/brave_shields_util.h"
#include "brave/components/constants/brave_paths.h"
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "chrome/browser/extensions/extension_browsertest.h"
#include "chrome/browser/ui/browser_window.h"
#include "chrome/common/chrome_content_client.h"
#include "chrome/test/base/ui_test_utils.h"
#include "content/public/test/browser_test.h"
#include "net/dns/mock_host_resolver.h"
#include "net/test/embedded_test_server/embedded_test_server.h"
#include "net/test/embedded_test_server/http_request.h"

using brave_shields::ControlType;

class BraveScreenFarblingBrowserTest : public InProcessBrowserTest {
 public:
  void SetUpOnMainThread() override {
    InProcessBrowserTest::SetUpOnMainThread();

    content_client_.reset(new ChromeContentClient);
    content::SetContentClient(content_client_.get());
    browser_content_client_.reset(new BraveContentBrowserClient());
    content::SetBrowserClientForTesting(browser_content_client_.get());

    host_resolver()->AddRule("*", "127.0.0.1");
    content::SetupCrossSiteRedirector(embedded_test_server());

    brave::RegisterPathProvider();
    base::FilePath test_data_dir;
    base::PathService::Get(brave::DIR_TEST_DATA, &test_data_dir);
    embedded_test_server()->ServeFilesFromDirectory(test_data_dir);

    ASSERT_TRUE(embedded_test_server()->Start());

    top_level_page_url_ = embedded_test_server()->GetURL("a.com", "/");
    farbling_url_ = embedded_test_server()->GetURL("a.com", "/simple.html");
  }

  void TearDown() override {
    browser_content_client_.reset();
    content_client_.reset();
  }

  HostContentSettingsMap* content_settings() {
    return HostContentSettingsMapFactory::GetForProfile(browser()->profile());
  }

  void AllowFingerprinting() {
    brave_shields::SetFingerprintingControlType(
        content_settings(), ControlType::ALLOW, top_level_page_url_);
  }

  void SetFingerprintingDefault() {
    brave_shields::SetFingerprintingControlType(
        content_settings(), ControlType::DEFAULT, top_level_page_url_);
  }

  content::WebContents* contents() const {
    return browser()->tab_strip_model()->GetActiveWebContents();
  }

  bool NavigateToURLUntilLoadStop(const GURL& url) {
    EXPECT_TRUE(ui_test_utils::NavigateToURL(browser(), url));
    return WaitForLoadStop(contents());
  }

  Browser* OpenPopup(const std::string& script) const {
    content::ExecuteScriptAsync(contents(), script);
    Browser* popup = ui_test_utils::WaitForBrowserToOpen();
    EXPECT_NE(popup, browser());
    auto* popup_contents = popup->tab_strip_model()->GetActiveWebContents();
    EXPECT_TRUE(WaitForRenderFrameReady(popup_contents->GetMainFrame()));
    return popup;
  }

  const GURL& farbling_url() { return farbling_url_; }

 private:
  GURL top_level_page_url_;
  GURL farbling_url_;
  std::unique_ptr<ChromeContentClient> content_client_;
  std::unique_ptr<BraveContentBrowserClient> browser_content_client_;
};

#define PREPARE_TEST_EVENT                                   \
  "let fakeScreenX = 100, fakeScreenY = 200; "               \
  "let fakeClientX = 300, fakeClientY = 400; "               \
  "let testEvent = document.createEvent('MouseEvent'); "     \
  "testEvent.initMouseEvent('click', true, true, window, 1," \
  "fakeScreenX + devicePixelRatio * fakeClientX,"            \
  "fakeScreenY + devicePixelRatio * fakeClientY,"            \
  "fakeClientX, fakeClientY, false, false, false, false, 0, null); "

const char* testScript[] = {
    "window.screenX",
    "window.screenY",
    "window.screen.availLeft",
    "window.screen.availTop",
    "window.outerWidth - window.innerWidth",
    "window.outerHeight - window.innerHeight",
    "window.screen.availWidth - window.innerWidth",
    "window.screen.availHeight - window.innerHeight",
    "window.screen.width - window.innerWidth",
    "window.screen.height - window.innerHeight",
    PREPARE_TEST_EVENT
    "testEvent.screenX - devicePixelRatio * testEvent.clientX",
    PREPARE_TEST_EVENT
    "testEvent.screenY - devicePixelRatio * testEvent.clientY"};

IN_PROC_BROWSER_TEST_F(BraveScreenFarblingBrowserTest, FarbleScreenSize) {
  AllowFingerprinting();
  NavigateToURLUntilLoadStop(farbling_url());
  for (int i = 0; i < static_cast<int>(std::size(testScript)); ++i) {
    EXPECT_LE(0, EvalJs(contents(), testScript[i]));
  }
  SetFingerprintingDefault();
  NavigateToURLUntilLoadStop(farbling_url());
  for (int i = 0; i < static_cast<int>(std::size(testScript)); ++i) {
    EXPECT_GE(8, EvalJs(contents(), testScript[i]));
  }
}

const char* mediaQueryTestScripts[] = {
    "matchMedia(`(max-device-width: ${innerWidth + 8}px) and "
    "(min-device-width: ${innerWidth}px)`).matches",
    "matchMedia(`(max-device-height: ${innerHeight + 8}px) and "
    "(min-device-height: ${innerHeight}px)`).matches"};

IN_PROC_BROWSER_TEST_F(BraveScreenFarblingBrowserTest, FarbleScreenMediaQuery) {
  SetFingerprintingDefault();
  NavigateToURLUntilLoadStop(farbling_url());
  for (int i = 0; i < static_cast<int>(std::size(mediaQueryTestScripts)); ++i) {
    EXPECT_EQ(true, EvalJs(contents(), mediaQueryTestScripts[i]));
  }
  AllowFingerprinting();
  NavigateToURLUntilLoadStop(farbling_url());
  for (int i = 0; i < static_cast<int>(std::size(mediaQueryTestScripts)); ++i) {
    printf("%d\n", i);
    EXPECT_EQ(false, EvalJs(contents(), mediaQueryTestScripts[i]));
  }
}

IN_PROC_BROWSER_TEST_F(BraveScreenFarblingBrowserTest,
                       FarbleScreenPopupPosition) {
  SetFingerprintingDefault();
  NavigateToURLUntilLoadStop(farbling_url());
  gfx::Rect parentBounds = browser()->window()->GetBounds();
  const char* script =
      "open('http://d.test/', '', `left=${screen.availLeft + "
      "50},top=${screen.availTop + 50},width=70,height=70`);";
  Browser* popup = OpenPopup(script);
  gfx::Rect childBounds = popup->window()->GetBounds();
  printf("%d %d %d %d\n", childBounds.x(), parentBounds.x(), childBounds.y(),
         parentBounds.y());
  EXPECT_GE(childBounds.x(), 50 + parentBounds.x());
  EXPECT_GE(childBounds.y(), 50 + parentBounds.y());
}
