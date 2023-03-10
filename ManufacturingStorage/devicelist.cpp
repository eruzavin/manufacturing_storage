#include <QDebug>
#include <QMessageBox>
#include <QSqlRecord>
#include <devicelist.h>

DeviceList::DeviceList(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Устройства");
    QVBoxLayout *layout = new QVBoxLayout(this);

    devicesModel = db.m_getDevices();
    deviceListView = new QListView(this);
    deviceListView->setModel(devicesModel);

    editButton = new QPushButton("Редактировать хранилище", this);
    editButton->setEnabled(false);
    connect(deviceListView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(m_enableEditButton()));

    connect(editButton, SIGNAL(clicked()),this,SLOT(m_openStorageEditor()));

    layout->addWidget(deviceListView);
    layout->addWidget(editButton);
    setLayout(layout);
}

void DeviceList::m_enableEditButton()
{
    editButton->setEnabled(true);
}

void DeviceList::m_openStorageEditor()
{
    Device device = m_getSelectedDevice();

    storageEditor = new StorageEditor(&db, &device);
    connect(storageEditor, &StorageEditor::deviceList, this, &DeviceList::show);

    storageEditor->show();
    this->close();
}

Device DeviceList::m_getSelectedDevice()
{
    int row = deviceListView->currentIndex().row();
    QSqlRecord selectedDevice = devicesModel->record(row);

    Device device;
    device.dbId = selectedDevice.value("id").toInt();
    device.name = selectedDevice.value("name").toString();

    return device;
}

DeviceList::~DeviceList()
{
}
