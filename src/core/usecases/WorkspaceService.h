#pragma once
#include "core/usecases/IWorkspaceService.h"

namespace storage { class IFileRepository; }
namespace network { class INetworkClient; }
namespace infra { class Settings; }

namespace core {

class WorkspaceService final : public IWorkspaceService {
public:
    WorkspaceService(storage::IFileRepository& repo,
                     network::INetworkClient& net,
                     infra::Settings& settings);

    Workspace current() const override;
    void setCurrent(const Workspace& ws) override;

private:
    storage::IFileRepository& repo_;
    network::INetworkClient& net_;
    infra::Settings& settings_;
    Workspace current_;
};

}
