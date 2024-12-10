#ifndef PYTHON_deep_learning_H
#define PYTHON_deep_learning_H

#include <QKeyEvent>
#include <QMainWindow>

namespace Ui {
class Python_deep_learning;
}

class Python_deep_learning : public QMainWindow
{
    Q_OBJECT

public:
    explicit Python_deep_learning(QWidget *parent = nullptr, int model = -1, QFont *fontt = nullptr);
    ~Python_deep_learning();

private slots:
    void on_btn_switch_model_clicked();
    void on_btn_select_image_clicked();
    void on_btn_predict_clicked();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QString calculateFileHash(const QString &filePath);
    bool    areFilesIdentical(const QString &tempPath, const QString &resourcePath);
    bool    copyFileIfNotExists(const QString &srcDir, const QString &srcName, const QString &dstPath);
    QString executePythonScript(const QStringList &arguments, const QString &workingDir);

    Ui::Python_deep_learning *ui;

    int     deep_learning_model;
    QString imagePath;
    QFont   font;
};

#endif  // PYTHON_deep_learning_H
