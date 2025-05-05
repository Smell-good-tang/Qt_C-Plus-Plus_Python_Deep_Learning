#include "../include/login.h"

#include <QCryptographicHash>
#include <QJsonArray>
#include <QProcess>

#include "../include/forusers.h"
#include "../include/mainwindow.h"
#include "../include/register.h"
#include "ui_login.h"

login::login(QWidget *parent, QFont *fontt) : QWidget(parent), ui(new Ui::login), font(*fontt)
{
    ui->setupUi(this);
    forusers::center_screen(this);  // 页面居中
}

login::~login() { delete ui; }

// 返回主页面
void login::on_btn_cancel_clicked()
{
    QCoreApplication::quit();  // 关闭所有页面
}

// 打开“注册”页面
void login::on_btn_regist_clicked() { closePresentPageAndShowNewPage<Register>(); }

// 账号匹配密码
const bool login::matchAccountWithPassword(const user &user_info)
{
    query.prepare("SELECT * FROM regist WHERE account = ? AND password = ?");
    query.addBindValue(user_info.name);
    query.addBindValue(user_info.password);
    bool result = query.exec() && query.first();
    query.clear();
    return result;
}

void login::on_btn_log_clicked()
{
    QString userName = ui->li_account->text();
    user    user_info;
    user_info.name   = userName;
    QString password = ui->li_password->text();

    if (userName.isEmpty()) {
        forusers::messagebox_common(this, "注意", "账户不能为空！", font);
        return;
    }

    if (password.isEmpty()) {
        forusers::messagebox_common(this, "注意", "密码不能为空！", font);
        return;
    }

    query.prepare("SELECT * FROM regist WHERE account = ?");
    query.addBindValue(userName);
    bool validate_account = query.exec() && query.first();
    query.clear();

    if (!validate_account) {
        forusers::messagebox_common(this, "注意", "账户不存在！", font);
        ui->li_account->clear();
        ui->li_password->clear();
    } else {
        QByteArray md_pwd  = QCryptographicHash::hash(password.toLatin1(), QCryptographicHash::Md5);
        user_info.password = md_pwd.toHex();
        bool able_to_login = matchAccountWithPassword(user_info);
        if (able_to_login) {
            closePresentPageAndShowNewPage<MainWindow>();
        } else {
            forusers::messagebox_common(this, "注意", "登录失败，因为帐号或密码错误！！！", font);
            forusers::messagebox_common(this, "注意", "如果您还没有任何账户，请尽快注册！", font);
            ui->li_account->clear();
            ui->li_password->clear();
        }
    }
}

// 清除输入信息
void login::on_btn_clear_clicked()
{
    ui->li_password->clear();
    ui->li_account->clear();
}

// 点击显示密码
void login::on_btn_show_p_pressed() { ui->li_password->setEchoMode(QLineEdit::Normal); }

// 松开隐藏密码
void login::on_btn_show_p_released() { ui->li_password->setEchoMode(QLineEdit::Password); }

// 键盘按键，控制按钮
void login::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Return: {
            QWidget *cur_wid = QApplication::focusWidget();
            if (cur_wid == ui->btn_show_p) {
                if (ui->btn_show_p->isEnabled()) {
                    emit ui->btn_show_p->pressed();
                }
                break;
            }
            QPushButton *btn = dynamic_cast<QPushButton *>(cur_wid);
            if (btn) {
                if (btn->isEnabled()) {
                    emit btn->clicked();
                }
                break;
            }
            break;
        }
        case Qt::Key_Escape: {
            if (ui->btn_cancel->isEnabled()) {
                emit ui->btn_cancel->clicked();
            }
            break;
        }
        default:
            QWidget::keyPressEvent(event);
            break;
    }
}

// 松开回车键
void login::keyReleaseEvent(QKeyEvent *event)
{
    (void)event;
    QWidget *cur_wid = QApplication::focusWidget();
    if (cur_wid == ui->btn_show_p) {
        if (ui->btn_show_p->isEnabled()) {
            emit ui->btn_show_p->released();
        }
    }
}
