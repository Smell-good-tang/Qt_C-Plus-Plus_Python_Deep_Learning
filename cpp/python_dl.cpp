#include "../include/python_dl.h"

#include <QCryptographicHash>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QThread>

#include "../include/forusers.h"
#include "../include/mainwindow.h"
#include "ui_python_dl.h"

using namespace enum_class_python_dl;

Python_deep_learning::Python_deep_learning(QWidget *parent, int model, QFont *fontt)
    : QMainWindow(parent), ui(new Ui::Python_deep_learning), deep_learning_model(model), font(*fontt)
{
    ui->setupUi(this);
    if (model == Model_categories::Self_FAGCFN) {
        this->setWindowTitle("基于 Self-FAGCFN 模型的单标签肺部疾病分类系统");
    } else if (model == Model_categories::ALCFA_Net) {
        this->setWindowTitle("基于 ALCFA-Net 模型的多标签肺（胸）部疾病分类系统");
    }

    isRunningPythonScript = false;

    ui->btn_terminate->setEnabled(false);
}

Python_deep_learning::~Python_deep_learning() { delete ui; }

void Python_deep_learning::on_btn_switch_model_clicked()
{
    if (judgeIsRunningPythonScript()) {
        return;
    }

    if (deep_learning_model == Model_categories::Self_FAGCFN) {
        this->setWindowTitle("基于 ALCFA-Net 模型的多标签肺（胸）部疾病分类系统");
        deep_learning_model = Model_categories::ALCFA_Net;
    } else if (deep_learning_model == Model_categories::ALCFA_Net) {
        this->setWindowTitle("基于 Self-FAGCFN 模型的单标签肺部疾病分类系统");
        deep_learning_model = Model_categories::Self_FAGCFN;
    }
    ui->upload_image->clear();
    ui->heatmap->clear();
    ui->predict_label->clear();
    ui->upload_image->setText("上传图片");
    ui->heatmap->setText("热图");
    ui->predict_label->setText("预测");
}

void Python_deep_learning::on_btn_select_image_clicked()
{
    if (judgeIsRunningPythonScript()) {
        return;
    }

    auto    fileDialog = std::make_unique<QFileDialog>(this);
    QString filePath   = fileDialog->getOpenFileName(this, "选择图片", "./", "Image (*.jpg *.jpeg *.png)");
    if (!filePath.isEmpty()) {
        // 获取QLabel的当前大小
        QSize labelSize = ui->upload_image->size();

        // 使用scaled()方法调整图像的大小
        QPixmap pixmap(filePath);

        // 确保图像按比例缩放来适应 QLabel 的尺寸
        pixmap = pixmap.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // 将缩放后的图像设置到 QLabel
        ui->upload_image->setPixmap(pixmap);

        imagePath = filePath;

        ui->heatmap->clear();
        ui->predict_label->clear();
        ui->heatmap->setText("热图");
        ui->predict_label->setText("预测");
    }
}

// 计算文件的哈希值
QString Python_deep_learning::calculateFileHash(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QString();
    }

    QCryptographicHash hash(QCryptographicHash::Sha256);  // 使用 SHA-256 哈希算法
    if (hash.addData(&file)) {
        return hash.result().toHex();
    }

    return QString();
}

// 比较 tempPath 和 resourcePath 对应的文件是否相同
bool Python_deep_learning::areFilesIdentical(const QString &tempPath, const QString &resourcePath)
{
    // 获取 tempPath 文件的哈希值
    QString tempFileHash = calculateFileHash(tempPath);
    if (tempFileHash.isEmpty()) {
        return false;  // 文件无法读取或哈希计算失败
    }

    // 获取 resourcePath 文件的哈希值
    QFile resourceFile(resourcePath);
    if (!resourceFile.open(QIODevice::ReadOnly)) {
        return false;  // 资源文件无法读取
    }

    // 获取资源文件的哈希值
    QByteArray         resourceData = resourceFile.readAll();
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(resourceData);
    QString resourceFileHash = hash.result().toHex();

    // 比较两个哈希值
    return tempFileHash == resourceFileHash;
}

// 提取文件复制函数
bool Python_deep_learning::copyFileIfNotExists(const QString &srcDir, const QString &srcName, const QString &dstPath)
{
    if (srcName.isEmpty()) {
        return false;
    }

    QString srcPath = srcDir + srcName;

    if (QFile::exists(dstPath)) {
        bool result = areFilesIdentical(srcPath, dstPath);
        if (!result) {
            int     counts   = 1;
            QString tempPath = "Backup_" + QString::number(counts) + "_" + srcName;
            while (QFile::exists(tempPath)) {
                ++counts;
                tempPath = "Backup_" + QString::number(counts) + "_" + srcName;
            }
            QFile::copy(dstPath, tempPath);
        } else {
            return true;
        }
    }

    QFile file(srcPath);
    if (file.open(QIODevice::ReadOnly)) {
        QFile localFile(dstPath);
        if (localFile.open(QIODevice::WriteOnly)) {
            localFile.write(file.readAll());
            qDebug() << srcPath << "copied to" << dstPath;
            return true;
        }
    }
    return false;
}

// 提取Python脚本执行函数
QString Python_deep_learning::executePythonScript(const QStringList &arguments, const QString &workingDir)
{
    QString python_env_path = "D:/Python_envs/python_3.9.13_2_advanced_pytorch/Scripts/python.exe";

    process.setProgram(python_env_path);
    process.setArguments(arguments);
    process.setWorkingDirectory(workingDir);
    process.start();

    if (!process.waitForFinished()) {
        qDebug() << "Python script execution failed!";
        return QString();
    }

    return process.readAllStandardOutput().trimmed();
}

void Python_deep_learning::on_btn_predict_clicked()
{
    if (judgeIsRunningPythonScript()) {
        return;
    }

    if (ui->upload_image->pixmap().isNull()) {
        return;
    }

    isRunningPythonScript = true;

    ui->btn_predict->setEnabled(false);
    ui->btn_terminate->setEnabled(true);

    auto waitForExecutePythonScriptThread = QThread::create([this]() {
        // 设置模型目录和资源文件
        QString          targetDir_name;
        deepLearningName DL_Name;
        deepLearningPath DL_Path;

        if (deep_learning_model == Model_categories::Self_FAGCFN) {
            DL_Name.modelWeight   = "GCN_lr_de_ori_f_schedu-3.pth";
            DL_Name.model         = "model_Self_FAGCFN.py";
            DL_Name.dropblock     = "dropblock.py";
            DL_Name.modules       = "modules_Self_FAGCFN.py";
            DL_Name.heatmapScript = "heatmap_Self_FAGCFN.py";
            DL_Name.predictScript = "predict_Self_FAGCFN.py";
            DL_Name.preGCNScript  = "Pre_GCN_Self_FAGCFN.py";

            targetDir_name = "./data/Self_FAGCFN/";
        } else if (deep_learning_model == Model_categories::ALCFA_Net) {
            DL_Name.modelWeight   = "GCN_28a0296c0f154a36905dd107bf9fe067.pth";
            DL_Name.model         = "model_ALCFA_Net.py";
            DL_Name.dataset       = "dataset.py";
            DL_Name.modules       = "modules_ALCFA_Net.py";
            DL_Name.heatmapScript = "heatmap_ALCFA_Net.py";
            DL_Name.predictScript = "predict_ALCFA_Net.py";
            DL_Name.preGCNScript  = "Pre_GCN_ALCFA_Net.py";
            DL_Name.data          = "Data_Entry_2017_v2020.csv";

            targetDir_name = "./data/ALCFA_Net/";
        }

        DL_Path.modelWeight   = targetDir_name + DL_Name.modelWeight;
        DL_Path.model         = targetDir_name + DL_Name.model;
        DL_Path.dropblock     = targetDir_name + DL_Name.dropblock;
        DL_Path.dataset       = targetDir_name + DL_Name.dataset;
        DL_Path.modules       = targetDir_name + DL_Name.modules;
        DL_Path.heatmapScript = targetDir_name + DL_Name.heatmapScript;
        DL_Path.predictScript = targetDir_name + DL_Name.predictScript;
        DL_Path.preGCNScript  = targetDir_name + DL_Name.preGCNScript;
        DL_Path.data          = targetDir_name + DL_Name.data;

        // 创建目标目录
        QDir dir;
        if (!dir.exists(targetDir_name)) {
            dir.mkpath(targetDir_name);
        }

        static QString modelDataPath    = ":/D:/OneDrive/QT/Qt_projects/For WinAndLinux/Paper/";
        static QString pythonScriptPath = ":/D:/OneDrive/QT/Qt_projects/For WinAndLinux/Paper/resources/";

        // 复制资源文件
        copyFileIfNotExists(modelDataPath, DL_Name.modelWeight, DL_Path.modelWeight);
        copyFileIfNotExists(pythonScriptPath, DL_Name.model, DL_Path.model);
        copyFileIfNotExists(pythonScriptPath, DL_Name.dropblock, DL_Path.dropblock);
        copyFileIfNotExists(pythonScriptPath, DL_Name.dataset, DL_Path.dataset);
        copyFileIfNotExists(pythonScriptPath, DL_Name.modules, DL_Path.modules);
        copyFileIfNotExists(pythonScriptPath, DL_Name.heatmapScript, DL_Path.heatmapScript);
        copyFileIfNotExists(pythonScriptPath, DL_Name.predictScript, DL_Path.predictScript);
        copyFileIfNotExists(pythonScriptPath, DL_Name.preGCNScript, DL_Path.preGCNScript);
        copyFileIfNotExists(modelDataPath, DL_Name.data, DL_Path.data);

        // 执行 heatmap 脚本
        QString output = executePythonScript({DL_Name.heatmapScript, "--r_heatmap", imagePath}, targetDir_name);
        if (output.isEmpty()) return;

        // 显示 heatmap 图像
        QPixmap heatmapPixmap(targetDir_name + output);
        QSize   labelSize = ui->heatmap->size();
        QSize   imageSize = ui->upload_image->pixmap().size();

        calculate_appropriate_image_size(imageSize, labelSize);
        heatmapPixmap = heatmapPixmap.scaled(imageSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        // 确保在主线程中执行以下操作
        QMetaObject::invokeMethod(
            this, [this, heatmapPixmap]() { ui->heatmap->setPixmap(heatmapPixmap); }, Qt::QueuedConnection);

        // 执行 predict 脚本
        output = executePythonScript({DL_Name.predictScript, "--r", imagePath}, targetDir_name);
        if (output.isEmpty()) return;

        // 显示预测结果图像
        QPixmap predictPixmap(targetDir_name + output);
        imageSize = ui->upload_image->pixmap().size();
        calculate_appropriate_image_size(imageSize, labelSize);
        predictPixmap = predictPixmap.scaled(imageSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        // 确保在主线程中执行以下操作
        QMetaObject::invokeMethod(
            this, [this, predictPixmap]() { ui->predict_label->setPixmap(predictPixmap); }, Qt::QueuedConnection);
    });

    connect(waitForExecutePythonScriptThread, &QThread::finished, this, [waitForExecutePythonScriptThread, this]() {
        waitForExecutePythonScriptThread->deleteLater();

        isRunningPythonScript = false;

        ui->btn_predict->setEnabled(true);
        ui->btn_terminate->setEnabled(false);
    });

    disconnect(this, &Python_deep_learning::processTerminate, nullptr, nullptr);
    connect(this, &Python_deep_learning::processTerminate, waitForExecutePythonScriptThread, [=]() {
        process.terminate();
        waitForExecutePythonScriptThread->terminate();
    });

    waitForExecutePythonScriptThread->start();
}

void Python_deep_learning::calculate_appropriate_image_size(QSize &imageSize, const QSize &labelSize)
{
    double imageRatio = (double)imageSize.width() / imageSize.height();
    imageSize.width() < labelSize.width() ? imageSize.setWidth(labelSize.width()) : imageSize.setWidth(imageSize.width());
    imageSize.height() < labelSize.height() ? imageSize.setHeight(labelSize.height()) : imageSize.setHeight(imageSize.height());
    double new_imageRatio = (double)imageSize.width() / imageSize.height();
    if (new_imageRatio > imageRatio) {
        double ratioAspectImage = imageRatio / new_imageRatio;
        imageSize.setWidth(imageSize.width() + imageSize.width() * (ratioAspectImage - 1));
    } else {
        double ratioAspectImage = new_imageRatio / imageRatio;
        imageSize.setHeight(imageSize.height() + imageSize.height() * (ratioAspectImage - 1));
    }
}

// 键盘按键，控制按钮
void Python_deep_learning::keyPressEvent(QKeyEvent *event)
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
            if (!isRunningPythonScript) {
                MainWindow *w = new MainWindow(nullptr, &font);
                w->show();
            }
            break;
        }
        default:
            QWidget::keyPressEvent(event);
            break;
    }
}

void Python_deep_learning::closeEvent(QCloseEvent *event)
{
    if (judgeIsRunningPythonScript()) {
        event->ignore();
    }
}

bool Python_deep_learning::judgeIsRunningPythonScript()
{
    if (isRunningPythonScript) {
        int result = forusers::messagebox_question_common_2_btn(this, "注意", "Python脚本正在运行。是否中断Python进程？", font);
        if (result == QMessageBox::No) {
            return true;
        } else {
            terminateProcess();
        }
    }

    return false;
}

void Python_deep_learning::on_btn_terminate_clicked() { terminateProcess(); }

void Python_deep_learning::terminateProcess()
{
    if (isRunningPythonScript) {
        emit processTerminate();
        isRunningPythonScript = false;

        ui->btn_predict->setEnabled(true);
        ui->btn_terminate->setEnabled(false);
    }
}
