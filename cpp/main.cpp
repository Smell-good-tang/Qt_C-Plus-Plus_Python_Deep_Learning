#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QProcess>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QTimer>

#include "../include/forusers.h"
#include "../include/login.h"

QSqlDatabase create_and_encrypt_database(QSqlDatabase db, const QString &database_path_specific, const QString &db_access_password)
{
    db = QSqlDatabase::addDatabase("SQLITECIPHER", "EncryptedConnection");
    db.setDatabaseName(database_path_specific);  // 设置数据库名称
    db.setPassword(db_access_password);          // 密码
    db.setConnectOptions("QSQLITE_CREATE_KEY");
    db.open();
    db.close();
    QSqlDatabase::removeDatabase(db.connectionName());

    db = QSqlDatabase::addDatabase("SQLITECIPHER");
    db.setDatabaseName(database_path_specific);  // 设置数据库名称
    db.setPassword(db_access_password);          // 密码
    return db;
}

QStringList get_all_MacAddress()
{
    QStringList all_MacAddress;
    foreach (const auto &interface, QNetworkInterface::allInterfaces()) {
        all_MacAddress.append(interface.hardwareAddress());
    }
    return all_MacAddress;
}

void db_not_open_or_table_cannot_create(const QFont &font, const QString &text)
{
    int result = forusers::messagebox_question_common_2_btn(nullptr, "注意", text, font);
    if (result == QMessageBox::Yes) {
        QCoreApplication::quit();  // 关闭所有页面
        QProcess::startDetached(qApp->applicationFilePath(), QStringList());
    } else {
        QMessageBox box(QMessageBox::Critical, "好的", "程序关闭", QMessageBox::NoButton);
        QTimer::singleShot(5000, &box, SLOT(accept()));
        box.setFont(font);
        box.exec();
        QCoreApplication::quit();
    }
}

void copyDatabaseToLocal(const QString &database_path_specific)
{
    // 资源文件路径 (:/ 表示资源中的文件)
    QString resourcePath = ":/hospital_client_dl.db";

    // 目标路径：复制到本地路径（例如应用程序数据目录）
    QString localPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(localPath);  // 确保路径存在

    // 检查本地文件是否已存在
    if (QFile::exists(database_path_specific)) {
        qDebug() << "数据库文件已存在，不需要复制。";
        return;
    }

    // 打开资源文件
    QFile resourceFile(resourcePath);
    if (!resourceFile.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开资源文件。";
        return;
    }

    // 创建目标文件
    QFile localFile(database_path_specific);
    if (!localFile.open(QIODevice::WriteOnly)) {
        qDebug() << "无法创建本地文件。";
        return;
    }

    // 复制资源文件内容到本地文件
    localFile.write(resourceFile.readAll());

    // 关闭文件
    resourceFile.close();
    localFile.close();

    qDebug() << "数据库文件复制到本地成功：" << database_path_specific;
}

int main(int argc, char *argv[])
{
    qputenv("QT_QPA_PLATFORM", "windows:darkmode=0");  // 控件取消自适应Win系统深色模式

    QApplication a(argc, argv);
    a.setStyle("windowsvista");  // 标题栏取消自适应Win系统深色模式

    QFont   fontt("Microsoft YaHei", 14);
    QString database_path          = "./database";
    QString database_path_specific = database_path + "/hospital_client_dl.db";
    QString db_access_password     = "Example";
    QDir    dir;
    if (!dir.exists(database_path)) {
        dir.mkdir(database_path);
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("SQLITECIPHER");
    db.setDatabaseName(database_path_specific);  // 设置数据库名称
    db.setPassword(db_access_password);          // 密码
    if (!db.open()) {
        db.close();
        if (!QFile::remove(database_path_specific)) {
            QCoreApplication::quit();  // 关闭所有页面
            QProcess::startDetached(qApp->applicationFilePath(), QStringList());
            return 0;
        }
        db.open();
        db.close();
    } else {
        db.close();
    }

    db = create_and_encrypt_database(db, database_path_specific, db_access_password);
    if (db.open()) {
        bool      verify_database_owner = false;
        QString   table_mac             = "Mac";
        QSqlQuery query;
        QString   searchTableQuery = QString("SELECT * FROM %1").arg(table_mac);
        if (!query.exec(searchTableQuery)) {
            query.clear();
            QString createTableQuery = QString("CREATE TABLE %1 (%2 TEXT PRIMARY KEY)").arg(table_mac).arg("mac");
            if (!query.exec(createTableQuery)) {
                QCoreApplication::quit();  // 关闭所有页面
                QProcess::startDetached(qApp->applicationFilePath(), QStringList());
                return 0;
            } else {
                QStringList all_MacAddress = get_all_MacAddress();
                query.clear();
                foreach (const auto &MacAddress, all_MacAddress) {
                    if (MacAddress.isEmpty()) {
                        continue;
                    }
                    query.prepare(QString("INSERT INTO %1 (mac) VALUES (?)").arg(table_mac));
                    query.addBindValue(MacAddress);
                    query.exec();
                    query.clear();
                }
                verify_database_owner = true;
            }
        } else {
            query.clear();
            QStringList all_MacAddress = get_all_MacAddress();
            searchTableQuery           = QString("SELECT mac FROM %1").arg(table_mac);
            query.exec(searchTableQuery);
            while (query.next()) {
                QString mac           = query.value(0).toString();
                verify_database_owner = all_MacAddress.contains(mac);
                if (verify_database_owner) {
                    break;
                }
            }
            query.clear();
        }

        db.close();
        if (!verify_database_owner) {
            if (!QFile::remove(database_path_specific)) {
                QCoreApplication::quit();  // 关闭所有页面
                QProcess::startDetached(qApp->applicationFilePath(), QStringList());
                return 0;
            }
            db.open();
            db.close();
        }

        db = create_and_encrypt_database(db, database_path_specific, db_access_password);
        if (db.open()) {
            QSqlQuery query;
            QString   tableName        = "regist";
            QString   createTableQuery = QString(
                                           "CREATE TABLE IF NOT EXISTS %1 ("
                                             "account TEXT (20) PRIMARY KEY, "
                                             "password   TEXT (32) DEFAULT NULL, "
                                             "secure     TEXT (20) DEFAULT NULL, "
                                             "IDnumber TEXT (20) DEFAULT NULL)")
                                           .arg(tableName);
            if (!query.exec(createTableQuery)) {
                qDebug() << "Failed to create table:" << query.lastError();
                db_not_open_or_table_cannot_create(fontt, "无法创建用户信息表");
            } else {
                login *log = new login(nullptr, &fontt);
                log->show();
            }
        } else {
            db_not_open_or_table_cannot_create(fontt, "无法连接数据库");
        }
    } else {
        db_not_open_or_table_cannot_create(fontt, "无法连接数据库");
    }
    return a.exec();
}
