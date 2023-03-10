#ifndef DB_H
#define DB_H

#include <QSqlTableModel>

struct File{
    int operationId;
    QString name;
    QByteArray ba;
    QString type;
};

class Db
{
public:
    Db();
    QSqlQueryModel *m_getDeviceOperations(int deviceId);
    QSqlQueryModel *m_getDevices();
    QSqlQueryModel *m_getOperationFiles(int operationId, QVariant fileType);
    bool m_insertFile(File *file);

private:
    bool m_dbConfigExists(QString path);
    void m_connect();
    void m_showError(QString errorMessage);
};

#endif // DB_H
