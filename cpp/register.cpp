#include "../include/register.h"

#include <QCryptographicHash>
#include <QJsonArray>

#include "../include/forusers.h"
#include "../include/login.h"
#include "ui_register.h"

Register::Register(QWidget *parent, QFont *fontt) : QWidget(parent), ui(new Ui::Register), font(*fontt)
{
    ui->setupUi(this);
    reg_accountExp = new QRegularExpression("[0-9]+$");
    IDnumberExp    = new QRegularExpression("[0-9]{17}(([0-9]|[xX]){1})$");
    IDnumber       = new QRegularExpressionValidator(*IDnumberExp);
    reg_account    = new QRegularExpressionValidator(*reg_accountExp);
    ui->li_ID->setValidator(IDnumber);  // 限制身份证号格式
    ui->li_regista->setValidator(reg_account);
    forusers::center_screen(this);  // 页面居中
}

Register::~Register()
{
    delete IDnumber;
    delete reg_account;
    delete IDnumberExp;
    delete reg_accountExp;
    delete ui;
}

// 返回上一级页面
void Register::on_btn_cancel_clicked()
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->close();
    login *log = new login(nullptr, &font);
    log->show();
    return;
}

// 清除部分输入信息
void Register::on_btn_clear_clicked()
{
    ui->li_regista->clear();
    ui->li_registp->clear();
    ui->li_registxp->clear();
}

// 确认注册
void Register::on_btn_confirm_clicked()
{
    QString user, pwd, xpwd, mb, ID;
    user = ui->li_regista->text();
    pwd  = ui->li_registp->text();
    xpwd = ui->li_registxp->text();
    mb   = ui->li_mb->text();
    ID   = ui->li_ID->text();

    // 注册信息为空
    if (user.isEmpty() && pwd.isEmpty() && xpwd.isEmpty() && mb.isEmpty() && ID.isEmpty()) {
        forusers::messagebox_common(this, "注意", "没有有效的注册信息", font);
        return;
    }

    // 密码不合规
    if (!(pwd.length() > 8 && pwd.length() < 16)) {
        forusers::messagebox_common(this, "注意", "密码长度需要介于8位和16位之间", font);
        ui->li_registp->clear();
        ui->li_registxp->clear();
        return;
    }

    static QRegularExpression regppwd(R"((?=.*[A-Z])(?=.*[a-z])(?=.*[0-9])(?=.*[!-/[:-@[-`{-~]))");
    if (!pwd.contains(regppwd)) {
        forusers::messagebox_common(this, "注意", "密码需要包含大写和小写字母、数字和字符", font);
        ui->li_registp->clear();
        ui->li_registxp->clear();
        return;
    }

    // 密保不合规
    if (mb.length() <= 10) {
        forusers::messagebox_common(this, "注意", "密保过于简单！", font);
        return;
    }

    // 身份证号不合规
    if (ID.length() != 18) {
        forusers::messagebox_common(this, "注意", "身份证号长度不对！", font);
        return;
    }

    // 没有输入账号，或没有重复输入密码
    if (user.isEmpty()) {
        forusers::messagebox_common(this, "注意", "账户不能为空！", font);
        return;
    }
    if (xpwd.isEmpty()) {
        forusers::messagebox_common(this, "注意", "请再次输入密码确认！", font);
        return;
    }

    // 两次输入的密码一致
    if (pwd == xpwd) {
        query.prepare("SELECT * FROM regist WHERE account = ?");
        query.addBindValue(user);
        bool Able_to_regist = query.exec() && query.first();
        query.clear();

        // 账号没有注册
        if (!Able_to_regist) {
            QByteArray md_pwd     = QCryptographicHash::hash(pwd.toLatin1(), QCryptographicHash::Md5);
            QString    str_md_pwd = md_pwd.toHex();

            // 身份证号的（小写）'x'替换成（大写）'X'
            if (ID.back() == 'x') {
                ID.back() = 'X';
            }

            // 向数据库插入注册信息
            query.prepare("INSERT INTO regist VALUES (:account, :password, :secure, :IDnumber)");
            query.bindValue(":account", user);
            query.bindValue(":password", str_md_pwd);
            query.bindValue(":secure", mb);
            query.bindValue(":IDnumber", ID);
            bool Registed = query.exec();
            query.clear();

            if (Registed) {
                forusers::messagebox_common(this, "Congratulations!", "成功注册！！！然后准备登录！！", font);
                // 返回登录页面
                this->setAttribute(Qt::WA_DeleteOnClose);
                this->close();
                login *log = new login(nullptr, &font);
                log->show();
            } else {
                forusers::messagebox_common(this, "注意", "抱歉，注册失败。请重试。", font);
            }
        }

        // 账号注册过了
        else {
            forusers::messagebox_common(this, "注意", "重复账户，请输入新账户！", font);
            ui->li_regista->clear();
        }
    }

    // 两次输入的密码不一致
    else {
        forusers::messagebox_common(this, "注意", "password_enter_twice", font);
        ui->li_registp->clear();
        ui->li_registxp->clear();
    }
}

// 按压显示密码
void Register::on_btn_show_p_pressed()
{
    ui->li_registp->setEchoMode(QLineEdit::Normal);
    ui->li_registxp->setEchoMode(QLineEdit::Normal);
}

// 松开隐藏密码
void Register::on_btn_show_p_released()
{
    ui->li_registp->setEchoMode(QLineEdit::Password);
    ui->li_registxp->setEchoMode(QLineEdit::Password);
}

// 键盘按键，控制按钮
void Register::keyPressEvent(QKeyEvent *event)
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
