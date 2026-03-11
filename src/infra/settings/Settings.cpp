#include "Settings.h"

namespace infra {

QString Settings::lastWorkspacePath() const { return lastPath; }
void Settings::setPath(const QString& path) { lastPath = path; }

}
