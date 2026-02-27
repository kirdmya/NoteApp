#pragma once
#include "core/domain/Workspace.h"

namespace core {

class IWorkspaceService {
public:
    virtual ~IWorkspaceService() = default;

    virtual Workspace current() const = 0;
    virtual void setCurrent(const Workspace& ws) = 0;
};

}
