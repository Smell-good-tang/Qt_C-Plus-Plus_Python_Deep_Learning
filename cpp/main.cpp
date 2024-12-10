#include <QApplication>

#include "./include/mainwindow.h"

int main(int argc, char *argv[])
{
    qputenv("QT_QPA_PLATFORM", "windows:darkmode=0");  // 控件取消自适应Win系统深色模式

    QApplication a(argc, argv);
    a.setStyle("windowsvista");  // 标题栏取消自适应Win系统深色模式

    QFont   fontt("Microsoft YaHei", 14);
    MainWindow *w = new MainWindow(nullptr, &fontt);
    w->show();

    return a.exec();
}
