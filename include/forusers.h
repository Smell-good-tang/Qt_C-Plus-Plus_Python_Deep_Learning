#ifndef FORUSERS_H
#define FORUSERS_H

#include <QRadioButton>
#include <QSqlQuery>
#include <QWidget>

// Add QObject as base class
class forusers : public QObject
{
    // Add Q_OBJECT macro
    Q_OBJECT

public:
    static void center_screen(QWidget *ui);
    static void messagebox_common(QWidget *ui, const QString &title, const QString &text, const QFont &font);
    static int  messagebox_question_common_2_btn(QWidget *ui, const QString &title, const QString &text, const QFont &font);
};

#endif  // FORUSERS_H
