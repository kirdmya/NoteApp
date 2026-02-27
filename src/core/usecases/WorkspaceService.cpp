#include "WorkspaceService.h"
#include "storage/IFileRepository.h"
#include "network/INetworkClient.h"
#include "infra/settings/Settings.h"

namespace core {

WorkspaceService::WorkspaceService(storage::IFileRepository& repo,
                                   network::INetworkClient& net,
                                   infra::Settings& settings)
    : repo_(repo), net_(net), settings_(settings)
{
    current_ = Workspace{ "", "No workspace" };
}

Workspace WorkspaceService::current() const { return current_; }

void WorkspaceService::setCurrent(const Workspace& ws)
{
    current_ = ws;
    settings_.setLastWorkspacePath(ws.rootPath);
    (void)repo_;
    (void)net_;
}

}
