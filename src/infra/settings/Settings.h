#pragma once
#include <QString>
#include <QSettings>
#include <QDir>

namespace infra {

class Settings final : public QDir {
public:
    Settings() = default;
    ~Settings() = default;

    QString lastWorkspacePath() const;
    void setPath(const QString& path);

private:
    QString lastPath = QDir::path();
};

}
