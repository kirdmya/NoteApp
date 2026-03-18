#include "Settings.h"

namespace infra {

namespace {
const char* lastWorkspacePathKey = "Path";
}

Settings::Settings()
    : settingsPtr(std::make_unique<QSettings>())
{
}

QString Settings::lastWorkspacePath() const { return settingsPtr->value(lastWorkspacePathKey).toString(); }
void Settings::setLastWorkspacePath(const QString& path) { settingsPtr->setValue(lastWorkspacePathKey, path); }
QString Settings::settingsFilePath() const { return settingsPtr->fileName(); }

}
