#include "../include/mainwindow.h"

#include "../include/login.h"
#include "../include/python_dl.h"
#include "ui_mainwindow.h"

using namespace enum_class_python_dl;

MainWindow::MainWindow(QWidget *parent, QFont *fontt) : QMainWindow(parent), ui(new Ui::MainWindow), font(*fontt) { ui->setupUi(this); }

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_btn_Self_FAGCFN_clicked() { closePresentPageAndShowNewPage<Python_deep_learning>(Model_categories::Self_FAGCFN); }

void MainWindow::on_btn_ALCFA_Net_clicked() { closePresentPageAndShowNewPage<Python_deep_learning>(Model_categories::ALCFA_Net); }

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
            login *log = new login(nullptr, &font);
            log->show();
            break;
        }
        default:
            QWidget::keyPressEvent(event);
            break;
    }
}
