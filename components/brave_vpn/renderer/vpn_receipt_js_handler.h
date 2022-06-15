/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_VPN_RENDERER_VPN_RECEIPT_JS_HANDLER_H_
#define BRAVE_COMPONENTS_BRAVE_VPN_RENDERER_VPN_RECEIPT_JS_HANDLER_H_

#include <memory>
#include <string>
#include <vector>

#include "brave/components/brave_vpn/mojom/brave_vpn.mojom.h"
#include "content/public/renderer/render_frame.h"
#include "content/public/renderer/render_frame_observer.h"
#include "gin/wrappable.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "url/gurl.h"
#include "v8/include/v8.h"

namespace brave_vpn {

// If present, this will inject a few methods (used by Brave accounts website)
// into window.chrome.braveVpn.*
//
// This is only intended to be used on account.brave.com and the dev / staging
// counterparts. The accounts website will use this if present which allows a
// safe way for the browser to share existing purchase details which are used
// to obtain a desktop credential.
//
// This code in particular is meant to only be run on Android. This will expose
// a purchase token for the In-App-Purchase of Brave VPN if one exists.
class VpnReceiptJSHandler : public gin::Wrappable<VpnReceiptJSHandler> {
 public:
  static gin::WrapperInfo kWrapperInfo;

  explicit VpnReceiptJSHandler(content::RenderFrame* render_frame);
  VpnReceiptJSHandler(const VpnReceiptJSHandler&) = delete;
  VpnReceiptJSHandler& operator=(const VpnReceiptJSHandler&) = delete;
  ~VpnReceiptJSHandler() override;

  void AddJavaScriptObjectToFrame(v8::Local<v8::Context> context);
  void ResetRemote(content::RenderFrame* render_frame);

 private:
  bool EnsureConnected();

  // gin::WrappableBase
  gin::ObjectTemplateBuilder GetObjectTemplateBuilder(
      v8::Isolate* isolate) override;

  // window.chrome.braveVpn.receipt
  v8::Local<v8::Promise> GetPurchaseToken(v8::Isolate* isolate);
  void OnGetPurchaseToken(v8::Global<v8::Promise::Resolver> promise_resolver,
                          v8::Isolate* isolate,
                          v8::Global<v8::Context> context_old,
                          const std::string& token);

  content::RenderFrame* render_frame_;
  mojo::Remote<brave_vpn::mojom::ServiceHandler> vpn_service_;
};

}  // namespace brave_vpn

#endif  // BRAVE_COMPONENTS_BRAVE_VPN_RENDERER_VPN_RECEIPT_JS_HANDLER_H_

