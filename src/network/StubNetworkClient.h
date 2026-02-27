#pragma once
#include "network/INetworkClient.h"

namespace network {

class StubNetworkClient final : public INetworkClient {
public:
    bool isAvailable() const override { return false; }
};

}
