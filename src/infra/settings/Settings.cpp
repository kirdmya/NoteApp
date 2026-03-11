#include "Settings.h"

namespace infra {


QString Settings::lastWorkspacePath() const { return Settings::settingsPtr->value("Path").toString(); }
void Settings::setLastWorkspacePath(const QString& path) { Settings::settingsPtr->setValue("Path", path); }

}
