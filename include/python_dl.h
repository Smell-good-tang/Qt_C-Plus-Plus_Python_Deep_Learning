#ifndef PYTHON_DL_H
#define PYTHON_DL_H

#include <QKeyEvent>
#include <QMainWindow>
#include <QProcess>

struct deepLearningName
{
    QString modelWeight;
    QString model;
    QString dropblock;
    QString dataset;
    QString modules;
    QString heatmapScript;
    QString predictScript;
    QString preGCNScript;
    QString data;
};

struct deepLearningPath
{
    QString modelWeight;
    QString model;
    QString dropblock;
    QString dataset;
    QString modules;
    QString heatmapScript;
    QString predictScript;
    QString preGCNScript;
    QString data;
};

namespace Ui {
class Python_deep_learning;
}

class Python_deep_learning : public QMainWindow
{
    Q_OBJECT

public:
    explicit Python_deep_learning(QWidget *parent = nullptr, int model = -1, QFont *fontt = nullptr);
    ~Python_deep_learning();

signals:
    void processTerminate();

private slots:
    void on_btn_switch_model_clicked();
    void on_btn_select_image_clicked();
    void on_btn_predict_clicked();
    void on_btn_terminate_clicked();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    QString calculateFileHash(const QString &filePath);
    bool    areFilesIdentical(const QString &tempPath, const QString &resourcePath);
    bool    copyFileIfNotExists(const QString &srcDir, const QString &srcName, const QString &dstPath);
    QString executePythonScript(const QStringList &arguments, const QString &workingDir);
    void    calculate_appropriate_image_size(QSize &imageSize, const QSize &labelSize);
    bool    judgeIsRunningPythonScript();
    void    terminateProcess();

    Ui::Python_deep_learning *ui;

    int      deep_learning_model;
    QString  imagePath;
    QFont    font;
    bool     isRunningPythonScript;
    QProcess process;
};

#endif  // PYTHON_DL_H
