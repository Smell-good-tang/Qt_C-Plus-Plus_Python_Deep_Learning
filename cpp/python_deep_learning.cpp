#include "../include/python_deep_learning.h"

#include <QCryptographicHash>
#include <QFileDialog>
#include <QProcess>

#include "../include/mainwindow.h"
#include "ui_python_deep_learning.h"

using namespace enum_class_python_deep_learning;

Python_deep_learning::Python_deep_learning(QWidget *parent, int model, QFont *fontt)
    : QMainWindow(parent), ui(new Ui::Python_deep_learning), deep_learning_model(model), font(*fontt)
{
    ui->setupUi(this);
    if (model == Model_categories::Example_1) {
        this->setWindowTitle("Example_1");
    } else if (model == Model_categories::Example_2) {
        this->setWindowTitle("Example_2");
    }
}

Python_deep_learning::~Python_deep_learning() { delete ui; }

void Python_deep_learning::on_btn_switch_model_clicked()
{
    if (deep_learning_model == Model_categories::Example_1) {
        this->setWindowTitle("Example_2");
        deep_learning_model = Model_categories::Example_2;
    } else if (deep_learning_model == Model_categories::Example_2) {
        this->setWindowTitle("Example_1");
        deep_learning_model = Model_categories::Example_1;
    }
    ui->upload_image->clear();
    ui->heatmap->clear();
    ui->predict_label->clear();
    ui->upload_image->setText("上传图片");
}

void Python_deep_learning::on_btn_select_image_clicked()
{
    QString directory = QFileDialog::getOpenFileName(this, "选择图片", "./", "Image (*.jpg *.jpeg *.png)");
    if (!directory.isEmpty()) {
        // 获取QLabel的当前大小
        QSize labelSize = ui->upload_image->size();

        // 使用scaled()方法调整图像的大小
        QPixmap pixmap(directory);

        // 确保图像按比例缩放来适应 QLabel 的尺寸
        pixmap = pixmap.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // 将缩放后的图像设置到 QLabel
        ui->upload_image->setPixmap(pixmap);

        imagePath = directory;

        ui->heatmap->clear();
        ui->predict_label->clear();
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
            QString tempPath = "Backup_" + QString::number(counts) + dstPath;
            while (QFile::exists(tempPath)) {
                ++counts;
                tempPath = "Backup_" + QString::number(counts) + dstPath;
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
    QProcess process;
    process.setProgram("D:/python.exe");
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
    if (ui->upload_image->pixmap().isNull()) {
        return;
    }

    // 设置模型目录和资源文件
    QString targetDir_name;
    QString modelWeightName, modelName, dropblockName, datasetName, modulesName, heatmapScriptName, predictScriptName, preGCNScriptName, dataName;
    QString modelWeightPath, modelPath, dropblockPath, datasetPath, modulesPath, heatmapScriptPath, predictScriptPath, preGCNScriptPath, dataPath;

    if (deep_learning_model == Model_categories::Example_1) {
        modelWeightName   = ".pth";
        modelName         = ".py";
        dropblockName     = ".py";
        modulesName       = ".py";
        heatmapScriptName = ".py";
        predictScriptName = ".py";
        preGCNScriptName  = ".py";

        targetDir_name = "./data/Example_1/";
    } else if (deep_learning_model == Model_categories::Example_2) {
        modelWeightName   = ".pth";
        modelName         = ".py";
        datasetName       = ".py";
        modulesName       = ".py";
        heatmapScriptName = ".py";
        predictScriptName = ".py";
        preGCNScriptName  = ".py";
        dataName          = ".csv";

        targetDir_name = "./data/Example_2/";
    }

    modelWeightPath   = targetDir_name + modelWeightName;
    modelPath         = targetDir_name + modelName;
    dropblockPath     = targetDir_name + dropblockName;
    datasetPath       = targetDir_name + datasetName;
    modulesPath       = targetDir_name + modulesName;
    heatmapScriptPath = targetDir_name + heatmapScriptName;
    predictScriptPath = targetDir_name + predictScriptName;
    preGCNScriptPath  = targetDir_name + preGCNScriptName;
    dataPath          = targetDir_name + dataName;

    // 创建目标目录
    QDir dir;
    if (!dir.exists(targetDir_name)) {
        dir.mkpath(targetDir_name);
    }

    // 复制资源文件
    copyFileIfNotExists(":/", modelWeightName, modelWeightPath);
    copyFileIfNotExists(":/resources/", modelName, modelPath);
    copyFileIfNotExists(":/resources/", dropblockName, dropblockPath);
    copyFileIfNotExists(":/resources/", datasetName, datasetPath);
    copyFileIfNotExists(":/resources/", modulesName, modulesPath);
    copyFileIfNotExists(":/resources/", heatmapScriptName, heatmapScriptPath);
    copyFileIfNotExists(":/resources/", predictScriptName, predictScriptPath);
    copyFileIfNotExists(":/resources/", preGCNScriptName, preGCNScriptPath);
    copyFileIfNotExists(":/", dataName, dataPath);

    // 执行 heatmap 脚本
    QString output = executePythonScript({heatmapScriptName, "--r_heatmap", imagePath}, targetDir_name);
    if (output.isEmpty()) return;

    // 显示 heatmap 图像
    QPixmap heatmapPixmap(targetDir_name + output);
    QSize   labelSize = ui->heatmap->size();
    heatmapPixmap     = heatmapPixmap.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->heatmap->setPixmap(heatmapPixmap);

    // 执行 predict 脚本
    output = executePythonScript({predictScriptName, "--r", imagePath}, targetDir_name);
    if (output.isEmpty()) return;

    // 显示预测结果图像
    QPixmap predictPixmap(targetDir_name + output);
    predictPixmap = predictPixmap.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->predict_label->setPixmap(predictPixmap);
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
            MainWindow *w = new MainWindow(nullptr, &font);
            w->show();
            break;
        }
        default:
            QWidget::keyPressEvent(event);
            break;
    }
}
