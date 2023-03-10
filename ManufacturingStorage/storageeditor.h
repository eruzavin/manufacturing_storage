#ifndef STORAGEEDITOR_H
#define STORAGEEDITOR_H

#include <QFile>
#include <QListView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <db.h>

struct Device{
    int dbId;
    QString name;
};

struct Operation{
    int dbId;
    QString name;
};

class StorageEditor : public QWidget
{
    Q_OBJECT
public:
    StorageEditor(Db *db, Device *device, QWidget *parent = nullptr);
    ~StorageEditor();

private:
    Db *db;
    Device *device;
    Operation operation;
    QFile *file;
    QLineEdit* fileNameLineEdit;
    QListView *docListView;
    QListView *firmwareListView;
    QListView *operationsListView;
    QPushButton* uploadFileButton;
    QSqlQueryModel *operationsModel;
    QString fileName;
    QTabWidget *rightSideTabs;

    QHBoxLayout* m_getUploadButtons();
    QVBoxLayout *m_getLeftSideLayout();
    QVBoxLayout *m_getRightSideLayout();
    void m_getSelectedOperation();

signals:
    void deviceList();

private slots:
    bool m_isOperationListEmpty();
    bool m_isFirmwareTabSelected();
    void m_getOperationFiles();
    void m_openDeviceList();
    void m_selectFileButton_clicked();
    void m_uploadButton_clicked();
};

#endif // STORAGEEDITOR_H
