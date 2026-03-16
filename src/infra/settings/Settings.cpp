#include "Settings.h"

namespace infra {

Settings::Settings()
    : settingsPtr(std::make_unique<QSettings>())
{
}

QString Settings::lastWorkspacePath() const { return settingsPtr->value("Path").toString(); }
void Settings::setLastWorkspacePath(const QString& path) { settingsPtr->setValue("Path", path); }
QString Settings::settingsFilePath() const { return settingsPtr->fileName(); }
}
