#include "../include/mainwindow.h"

#include "../include/python_deep_learning.h"
#include "ui_mainwindow.h"

using namespace enum_class_python_deep_learning;

MainWindow::MainWindow(QWidget *parent, QFont *fontt) : QMainWindow(parent), ui(new Ui::MainWindow), font(*fontt) { ui->setupUi(this); }

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_btn_Example_1_clicked()
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->close();
    Python_deep_learning *python_deep_learning = new Python_deep_learning(nullptr, Model_categories::Example_1, &font);
    python_deep_learning->show();
}

void MainWindow::on_btn_Example_2_clicked()
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->close();
    Python_deep_learning *python_deep_learning = new Python_deep_learning(nullptr, Model_categories::Example_2, &font);
    python_deep_learning->show();
}

// 键盘按键，控制按钮
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Return: {
            QWidget     *cur_wid = QApplication::focusWidget();
            QPushButton *btn     = dynamic_cast<QPushButton *>(cur_wid);
            if (btn) {
                if (btn->isEnabled()) {
                    emit btn->clicked();
                }
                break;
            }
            break;
        }
        case Qt::Key_Escape: {
            this->setAttribute(Qt::WA_DeleteOnClose);
            this->close();
            break;
        }
        default:
            QWidget::keyPressEvent(event);
            break;
    }
}
