#include <QApplication>
#include "ui/MainWindow.h"
#include "app/App.h"
#include "app/AppVersion.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName(app::kAppName);
    a.setApplicationVersion(app::kAppVersion);



    app::App app;

    ui::MainWindow w(app);
    w.show();

    return a.exec();
}
