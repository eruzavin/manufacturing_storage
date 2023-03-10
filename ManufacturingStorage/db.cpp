#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <db.h>
using namespace std;

Db::Db()
{
    m_connect();
}

void Db::m_connect()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");

    QString dbConfigFile = QDir(QApplication::applicationDirPath()).filePath("db.ini");
    if (!m_dbConfigExists(dbConfigFile)) {
        m_showError("db.ini не найден в текущей директории");
        exit(1);
    }

    QSettings dbConfig(dbConfigFile, QSettings::IniFormat);

    db.setHostName(dbConfig.value("host", "").toString());
    db.setDatabaseName(dbConfig.value("db", "").toString());
    db.setUserName(dbConfig.value("user", "").toString());
    db.setPassword(dbConfig.value("password", "").toString());

    db.setConnectOptions("MYSQL_OPT_RECONNECT=1");

    if (!db.open()) {
        m_showError(db.lastError().text());
        exit(1);
    }
}

QSqlQueryModel *Db::m_getDevices()
{
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery("SELECT name,id FROM device_types");

    return model;
}

QSqlQueryModel *Db::m_getDeviceOperations(int deviceId)
{
    QSqlQuery query;
    query.prepare("SELECT name,id FROM operations WHERE device_type_id = ?");
    query.addBindValue(deviceId);
    query.exec();

    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(query);

    return model;
}

QSqlQueryModel *Db::m_getOperationFiles(int operationId, QVariant fileType)
{
    QSqlQuery query;
    query.prepare("SELECT f.name AS file_name, f.id "
                  "FROM operations o "
                  "JOIN files f ON f.type = ? AND o.id = f.operation_id "
                  "WHERE operation_id = ? "
                  "ORDER BY file_name;");
    query.addBindValue(fileType);
    query.addBindValue(operationId);
    query.exec();

    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(query);

    return model;
}

bool Db::m_insertFile(File *file)
{
    QSqlQuery query;
    query.prepare("INSERT into files (operation_id, name, type, body) VALUES (?, ?, ?, ?)");
    query.addBindValue(file->operationId);
    query.addBindValue(file->name);
    query.addBindValue(file->type);
    query.addBindValue(file->ba);

    return query.exec();
}

void Db::m_showError(QString errorMessage)
{
    QMessageBox msgBox;
    msgBox.setText(errorMessage);
    msgBox.exec();
}

bool Db::m_dbConfigExists(QString path) {
    QFileInfo checkFile(path);

    return checkFile.exists() && checkFile.isFile();
}
