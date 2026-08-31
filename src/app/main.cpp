#include <QApplication>
#include <QIcon>
#include "ui/MainWindow.h"
#include "app/App.h"
#include "app/AppVersion.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName(app::kAppName);
    a.setApplicationVersion(app::kAppVersion);
    a.setWindowIcon(QIcon(":/icons/app.png"));

    QCoreApplication::setOrganizationName("O-O");
    QCoreApplication::setApplicationName("O-Range");

    app::App app;

    ui::MainWindow w(app);
    w.show();

    return a.exec();
}
