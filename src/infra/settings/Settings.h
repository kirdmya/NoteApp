#pragma once
#include <QString>

namespace infra {

class Settings final {
public:
    Settings() = default;

    QString lastWorkspacePath() const;
    void setLastWorkspacePath(const QString& path);

private:
    QString lastWorkspacePath_;
};

}
