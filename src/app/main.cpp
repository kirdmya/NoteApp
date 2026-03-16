#include <QApplication>
#include <QDebug>
#include "infra/settings/settings.cpp"
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
    settings.settingsPtr->setValue("Path", settings.settingsPtr->fileName());
    qDebug() << settings.settingsPtr->value("Path").toString();

    app::App app;

    ui::MainWindow w(app);
    w.show();

    return a.exec();
}
