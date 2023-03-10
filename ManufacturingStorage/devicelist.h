#ifndef DEVICELIST_H
#define DEVICELIST_H

#include <QListView>
#include <QPushButton>
#include <QSqlTableModel>
#include <QVBoxLayout>
#include <QWidget>
#include <db.h>
#include <storageeditor.h>

class DeviceList : public QWidget
{
    Q_OBJECT

public:
    DeviceList(QWidget *parent = nullptr);
    ~DeviceList();

private:
    Db db;
    QListView *deviceListView;
    QPushButton *editButton;
    QSqlQueryModel *devicesModel;
    StorageEditor *storageEditor;

    Device m_getSelectedDevice();

private slots:
    void m_enableEditButton();
    void m_openStorageEditor();
};
#endif // DEVICELIST_H
