#pragma once
#include <memory>
#include <QSettings>
#include <QString>

namespace infra {

class Settings final {
public:
    Settings();
    ~Settings() = default;

    QString lastWorkspacePath() const;
    void setLastWorkspacePath(const QString& path);
    QString settingsFilePath() const;

private:
    std::unique_ptr<QSettings> settingsPtr;
};
}
