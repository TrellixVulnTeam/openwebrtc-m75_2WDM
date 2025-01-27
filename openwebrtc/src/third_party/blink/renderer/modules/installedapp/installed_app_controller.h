// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_INSTALLEDAPP_INSTALLED_APP_CONTROLLER_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_INSTALLEDAPP_INSTALLED_APP_CONTROLLER_H_

#include <memory>

#include "base/macros.h"
#include "base/memory/scoped_refptr.h"
#include "third_party/blink/public/mojom/installedapp/installed_app_provider.mojom-blink.h"
#include "third_party/blink/public/mojom/installedapp/related_application.mojom-blink.h"
#include "third_party/blink/public/platform/modules/installedapp/web_related_application.h"
#include "third_party/blink/public/platform/modules/installedapp/web_related_apps_fetcher.h"
#include "third_party/blink/public/platform/web_callbacks.h"
#include "third_party/blink/public/platform/web_vector.h"
#include "third_party/blink/renderer/core/execution_context/context_lifecycle_observer.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/platform/supplementable.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

using AppInstalledCallbacks =
    WebCallbacks<const WebVector<WebRelatedApplication>&, void>;

class MODULES_EXPORT InstalledAppController final
    : public GarbageCollectedFinalized<InstalledAppController>,
      public Supplement<LocalFrame>,
      public ContextLifecycleObserver {
  USING_GARBAGE_COLLECTED_MIXIN(InstalledAppController);

 public:
  static const char kSupplementName[];

  InstalledAppController(LocalFrame&, WebRelatedAppsFetcher*);
  virtual ~InstalledAppController();

  // Gets a list of related apps from the current page's manifest that belong
  // to the current underlying platform, and are installed.
  void GetInstalledRelatedApps(std::unique_ptr<AppInstalledCallbacks>);

  static void ProvideTo(LocalFrame&, WebRelatedAppsFetcher*);
  static InstalledAppController* From(LocalFrame&);

  void Trace(blink::Visitor*) override;

 private:
  class GetRelatedAppsCallbacks;

  // Inherited from ContextLifecycleObserver.
  void ContextDestroyed(ExecutionContext*) override;

  // Callback for the result of
  // WebRelatedAppsFetcher::getManifestRelatedApplications. Calls
  // filterByInstalledApps upon receiving the list of related applications.
  void OnGetRelatedAppsCallback(std::unique_ptr<AppInstalledCallbacks>,
                                const WebVector<WebRelatedApplication>&);

  // Takes a set of related applications and filters them by those which belong
  // to the current underlying platform, and are actually installed and related
  // to the current page's origin. Passes the filtered list to the callback.
  void FilterByInstalledApps(const WebVector<WebRelatedApplication>&,
                             std::unique_ptr<AppInstalledCallbacks>);

  // Callback from the InstalledAppProvider mojo service.
  void OnFilterInstalledApps(std::unique_ptr<blink::AppInstalledCallbacks>,
                             WTF::Vector<mojom::blink::RelatedApplicationPtr>);

  // Handle to the InstalledApp mojo service.
  mojom::blink::InstalledAppProviderPtr provider_;

  WebRelatedAppsFetcher* related_apps_fetcher_;

  DISALLOW_COPY_AND_ASSIGN(InstalledAppController);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_INSTALLEDAPP_INSTALLED_APP_CONTROLLER_H_
