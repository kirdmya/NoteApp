#include <QApplication>
#include "core/usecases/WorkspaceService.h"
#include "infra/settings/Settings.h"
#include "ui/MainWindow.h"
#include "app/App.h"
#include "app/AppVersion.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName(app::kAppName);
    a.setApplicationVersion(app::kAppVersion);

    QCoreApplication::setOrganizationName("O-O");
    QCoreApplication::setApplicationName("O-Range");

    infra::Settings settings;

    settings.setLastWorkspacePath(settings.settingsFilePath());
    qDebug() << settings.lastWorkspacePath();

    app::App app;
    if (settings.lastWorkspacePath().isEmpty()) {
        app.workspaceService().setCurrent(app.workspaceService().current());
    }

    ui::MainWindow w(app);
    w.show();

    return a.exec();
}
