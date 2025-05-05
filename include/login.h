#ifndef LOGIN_H
#define LOGIN_H

#include <QKeyEvent>
#include <QSqlQuery>
#include <QWidget>

namespace Ui {
class login;
}

class login : public QWidget
{
    Q_OBJECT

    struct user
    {
        QString name;
        QString password;
    };

public:
    explicit login(QWidget *parent = nullptr, QFont *fontt = nullptr);
    ~login();

private slots:
    void on_btn_cancel_clicked();   // 取消登录
    void on_btn_regist_clicked();   // 注册
    void on_btn_log_clicked();      // 登录
    void on_btn_clear_clicked();    // 清空
    void on_btn_show_p_pressed();   // 按压显示密码
    void on_btn_show_p_released();  // 松开隐藏密码

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    const bool matchAccountWithPassword(const user &user_info);  // 账号是否与密码匹配

    // 模板方法声明
    template <typename T>
    void closePresentPageAndShowNewPage(T * = nullptr)
    {
        this->setAttribute(Qt::WA_DeleteOnClose);
        this->close();
        auto widget = new T(nullptr, &font);
        widget->show();
    }

    Ui::login *ui;  // new对象，建在堆上
    QSqlQuery  query;
    QFont      font;
};

#endif  // LOGIN_H
