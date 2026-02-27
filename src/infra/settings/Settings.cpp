#include "Settings.h"

namespace infra {

QString Settings::lastWorkspacePath() const { return lastWorkspacePath_; }
void Settings::setLastWorkspacePath(const QString& path) { lastWorkspacePath_ = path; }

}
