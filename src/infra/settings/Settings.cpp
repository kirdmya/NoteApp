#include "Settings.h"

namespace infra {

QString Settings::lastWorkspacePath() const { return settingsPtr.value("Workspace_Path").toString(); }
void Settings::setLastWorkspacePath(const QString& path) { settingsPtr.setValue("Workspace_Path", path); }

}
