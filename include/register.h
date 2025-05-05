#ifndef REGISTER_H
#define REGISTER_H

#include <QKeyEvent>
#include <QRegularExpressionValidator>
#include <QSqlQuery>
#include <QWidget>

namespace Ui {
class Register;
}

class Register : public QWidget
{
    Q_OBJECT

public:
    explicit Register(QWidget *parent = nullptr, QFont *fontt = nullptr);
    ~Register();

private slots:
    void on_btn_clear_clicked();
    void on_btn_cancel_clicked();
    void on_btn_confirm_clicked();
    void on_btn_show_p_pressed();
    void on_btn_show_p_released();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    // 模板方法声明
    template <typename T>
    void closePresentPageAndShowNewPage(T * = nullptr)
    {
        this->setAttribute(Qt::WA_DeleteOnClose);
        this->close();
        auto widget = new T(nullptr, &font);
        widget->show();
    }

    // new对象，建在堆上
    QRegularExpressionValidator *IDnumber = nullptr, *reg_account = nullptr;
    QRegularExpression          *reg_accountExp = nullptr, *IDnumberExp = nullptr;
    Ui::Register                *ui;

    QSqlQuery query;
    QFont     font;
};

#endif  // REGISTER_H
