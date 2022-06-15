/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_vpn/renderer/vpn_receipt_js_handler.h"

#include <tuple>
#include <utility>

#include "base/base64.h"
#include "base/json/json_reader.h"
#include "base/no_destructor.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/renderer/v8_value_converter.h"
#include "gin/arguments.h"
#include "gin/function_template.h"
#include "gin/handle.h"
#include "gin/object_template_builder.h"
#include "third_party/blink/public/common/browser_interface_broker_proxy.h"
#include "third_party/blink/public/mojom/devtools/console_message.mojom.h"
#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/public/web/blink.h"
#include "third_party/blink/public/web/web_console_message.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "third_party/blink/public/web/web_script_source.h"

namespace {
std::string FormatReceipt(std::string purchase_token) {
  std::string type = "android";
  std::string response =
      base::StringPrintf("{\"type\": \"%s\", \"raw_receipt\": \"%s\"}",
                         type.c_str(), purchase_token.c_str());
  base::Base64Encode(response, &response);
  return response;
}
}  // namespace

namespace brave_vpn {

gin::WrapperInfo VpnReceiptJSHandler::kWrapperInfo = {gin::kEmbedderNativeGin};

VpnReceiptJSHandler::VpnReceiptJSHandler(content::RenderFrame* render_frame)
    : render_frame_(render_frame) {}

VpnReceiptJSHandler::~VpnReceiptJSHandler() = default;

bool VpnReceiptJSHandler::EnsureConnected() {
  if (!vpn_service_.is_bound()) {
    render_frame_->GetBrowserInterfaceBroker()->GetInterface(
        vpn_service_.BindNewPipeAndPassReceiver());
  }

  return vpn_service_.is_bound();
}

void VpnReceiptJSHandler::ResetRemote(content::RenderFrame* render_frame) {
  render_frame_ = render_frame;
  vpn_service_.reset();
  EnsureConnected();
}

void VpnReceiptJSHandler::AddJavaScriptObjectToFrame(
    v8::Local<v8::Context> context) {
  v8::Isolate* isolate = blink::MainThreadIsolate();
  v8::HandleScope handle_scope(isolate);
  if (context.IsEmpty())
    return;

  v8::Context::Scope context_scope(context);

  v8::Local<v8::Object> global = context->Global();

  // window.chrome
  v8::Local<v8::Object> chrome_obj;
  v8::Local<v8::Value> chrome_value;
  if (!global->Get(context, gin::StringToV8(isolate, "chrome"))
           .ToLocal(&chrome_value) ||
      !chrome_value->IsObject()) {
    chrome_obj = v8::Object::New(isolate);
    global->Set(context, gin::StringToSymbol(isolate, "chrome"), chrome_obj)
        .Check();
  } else {
    chrome_obj = chrome_value->ToObject(context).ToLocalChecked();
  }

  // window.chrome.braveVpn
  gin::Handle<VpnReceiptJSHandler> handler = gin::CreateHandle(isolate, this);
  CHECK(!handler.IsEmpty());
  v8::PropertyDescriptor vpn_desc(handler.ToV8(), false);
  vpn_desc.set_configurable(false);

  chrome_obj
      ->DefineProperty(isolate->GetCurrentContext(),
                       gin::StringToV8(isolate, "braveVpn"), vpn_desc)
      .Check();
}

v8::Local<v8::Promise> VpnReceiptJSHandler::GetPurchaseToken(
    v8::Isolate* isolate) {
  if (!EnsureConnected()) {
    return v8::Local<v8::Promise>();
  }

  v8::MaybeLocal<v8::Promise::Resolver> resolver =
      v8::Promise::Resolver::New(isolate->GetCurrentContext());
  if (resolver.IsEmpty()) {
    return v8::Local<v8::Promise>();
  }

  auto promise_resolver(
      v8::Global<v8::Promise::Resolver>(isolate, resolver.ToLocalChecked()));
  auto context_old(
      v8::Global<v8::Context>(isolate, isolate->GetCurrentContext()));

  vpn_service_->GetPurchaseToken(base::BindOnce(
      &VpnReceiptJSHandler::OnGetPurchaseToken, base::Unretained(this),
      std::move(promise_resolver), isolate, std::move(context_old)));

  return resolver.ToLocalChecked()->GetPromise();
}

void VpnReceiptJSHandler::OnGetPurchaseToken(
    v8::Global<v8::Promise::Resolver> promise_resolver,
    v8::Isolate* isolate,
    v8::Global<v8::Context> context_old,
    const std::string& token) {
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Context> context = context_old.Get(isolate);
  v8::Context::Scope context_scope(context);
  v8::MicrotasksScope microtasks(isolate,
                                 v8::MicrotasksScope::kDoNotRunMicrotasks);

  v8::Local<v8::Promise::Resolver> resolver = promise_resolver.Get(isolate);
  v8::Local<v8::String> result;
  result = v8::String::NewFromUtf8(isolate, FormatReceipt(token).c_str())
               .ToLocalChecked();

  std::ignore = resolver->Resolve(context, result);
}

gin::ObjectTemplateBuilder VpnReceiptJSHandler::GetObjectTemplateBuilder(
    v8::Isolate* isolate) {
  return gin::Wrappable<VpnReceiptJSHandler>::GetObjectTemplateBuilder(isolate)
      .SetMethod("get_receipt", &VpnReceiptJSHandler::GetPurchaseToken);
}

}  // namespace brave_vpn
