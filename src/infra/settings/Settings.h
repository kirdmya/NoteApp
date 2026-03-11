#pragma once
#include <QDebug>
#include <QString>
#include <QSettings>

namespace infra {

class Settings final {
public:
    Settings() = default;
    ~Settings() = default;

    QString lastWorkspacePath() const;
    void setLastWorkspacePath(const QString& path);

    std::unique_ptr<QSettings> settingsPtr;
};
}
