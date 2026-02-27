#include "App.h"

#include "storage/FsFileRepository.h"
#include "network/StubNetworkClient.h"
#include "infra/settings/Settings.h"
#include "core/usecases/WorkspaceService.h"

namespace app {

App::App() {
    fileRepo_ = std::make_unique<storage::FsFileRepository>();
    netClient_ = std::make_unique<network::StubNetworkClient>();
    settings_  = std::make_unique<infra::Settings>();
    workspaceService_ = std::make_unique<core::WorkspaceService>(*fileRepo_, *netClient_, *settings_);
}

App::~App() = default;

core::IWorkspaceService& App::workspaceService() { return *workspaceService_; }
infra::Settings& App::settings() { return *settings_; }

}
