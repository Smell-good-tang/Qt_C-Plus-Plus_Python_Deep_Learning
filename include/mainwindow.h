#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

namespace enum_class_python_dl {
enum Model_categories
{
    Self_FAGCFN = 0,
    ALCFA_Net   = 1
};
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, QFont *fontt = nullptr);
    ~MainWindow();

private slots:
    void on_btn_Self_FAGCFN_clicked();
    void on_btn_ALCFA_Net_clicked();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    // 模板方法声明
    template <typename T>
    void closePresentPageAndShowNewPage(enum_class_python_dl::Model_categories modelCategories, T * = nullptr)
    {
        this->setAttribute(Qt::WA_DeleteOnClose);
        this->close();
        auto widget = new T(nullptr, modelCategories, &font);
        widget->show();
    }

    Ui::MainWindow *ui;
    QFont           font;
};
#endif  // MAINWINDOW_H
