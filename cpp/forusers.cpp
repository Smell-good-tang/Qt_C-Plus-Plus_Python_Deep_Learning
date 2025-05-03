#include "../include/forusers.h"

#include <QMessageBox>
#include <QScreen>
#include <QStyle>
#include <QWindow>

// 公共普通消息框
void forusers::messagebox_common(QWidget *ui, const QString &title, const QString &text, const QFont &font)
{
    QMessageBox box(QMessageBox::Information, title, text, QMessageBox::NoButton, ui);
    box.setFont(font);
    box.exec();
}

// 公共询问消息框_Yes&&No
int forusers::messagebox_question_common_2_btn(QWidget *ui, const QString &title, const QString &text, const QFont &font)
{
    QMessageBox box(QMessageBox::Question, title, text, QMessageBox::Yes | QMessageBox::No, ui);
    box.setFont(font);
    int result = box.exec();
    return result;
}

//  页面居中
void forusers::center_screen(QWidget *ui)
{
    // 适应屏幕实际大小
    QRect screenRect      = QGuiApplication::primaryScreen()->availableGeometry();  // 获取工作区域的几何信息（不包括任务栏）
    int   titleBarHeight  = ui->style()->pixelMetric(QStyle::PM_TitleBarHeight);    // 获取标题栏高度
    int   screenW         = screenRect.width();
    int   screenH         = screenRect.height();
    int   screen_W_Widget = ui->width();
    int   screen_H_Widget = ui->height() + titleBarHeight;
    bool  need_resize     = false;
    if (screen_W_Widget > screenW) {
        screen_W_Widget = screenW;
        need_resize     = true;
    }
    if (screen_H_Widget > screenH) {
        screen_H_Widget = screenH - titleBarHeight;
        need_resize     = true;
    }
    if (need_resize) {
        ui->setMinimumSize(screen_W_Widget / 5, screen_H_Widget / 5);
        ui->resize(screen_W_Widget, screen_H_Widget);
    }

    // 页面居中
    int screenW_Widget = ui->width();
    int screenH_Widget = ui->height() + titleBarHeight;
    ui->move((screenW - screenW_Widget) / 2, (screenH - screenH_Widget) / 2);
}
