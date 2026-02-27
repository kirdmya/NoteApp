#pragma once

namespace network {

class INetworkClient {
public:
    virtual ~INetworkClient() = default;
    virtual bool isAvailable() const = 0;
};

}
