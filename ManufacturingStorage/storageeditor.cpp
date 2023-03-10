#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QMessageBox>
#include <QSqlRecord>
#include <QTabWidget>
#include <storageeditor.h>

StorageEditor::StorageEditor(Db *dbConnection, Device *chosenDevice, QWidget *parent)
    : QWidget(parent)
{  
    db = dbConnection;
    device = chosenDevice;

    setWindowTitle("Хранилище");

    QVBoxLayout *leftSide = m_getLeftSideLayout();
    QVBoxLayout *rightSide = m_getRightSideLayout();

    QGridLayout *mainFrame = new QGridLayout;
    mainFrame->addLayout(leftSide, 0, 0);
    mainFrame->addLayout(rightSide, 0, 1);

    setLayout(mainFrame);
}

QVBoxLayout* StorageEditor::m_getLeftSideLayout()
{
    QVBoxLayout *layout = new QVBoxLayout;

    QLabel *deviceNameLabel = new QLabel(device->name);

    operationsListView = new QListView();
    operationsModel = db->m_getDeviceOperations(device->dbId);
    operationsListView->setModel(operationsModel);
    operationsListView->setCurrentIndex(operationsModel->index(0, 0));
    connect(operationsListView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(m_getOperationFiles()));

    QPushButton* endEditing = new QPushButton("Завершить редактирование", this);
    connect(endEditing, SIGNAL(clicked()),this,SLOT(m_openDeviceList()));

    layout->addWidget(deviceNameLabel, 0, Qt::AlignTop);
    layout->addWidget(operationsListView);
    layout->addWidget(endEditing);

    return layout;
}

QVBoxLayout* StorageEditor::m_getRightSideLayout()
{
    QVBoxLayout *layout = new QVBoxLayout;
    m_getSelectedOperation();

    docListView = new QListView();
    firmwareListView = new QListView();

    m_getOperationFiles();

    rightSideTabs = new QTabWidget;
    rightSideTabs->addTab(docListView, "Инструкции");
    rightSideTabs->addTab(firmwareListView, "Прошивки");

    fileNameLineEdit = new QLineEdit();
    fileNameLineEdit->setPlaceholderText(" путь к файлу");
    fileNameLineEdit->setReadOnly(true);

    layout->addWidget(rightSideTabs);
    layout->addWidget(fileNameLineEdit);
    layout->addLayout(m_getUploadButtons());

    return layout;
}

QHBoxLayout* StorageEditor::m_getUploadButtons()
{
    QPushButton* selectFileButton = new QPushButton("Выбрать файл", this);
    uploadFileButton = new QPushButton("Загрузить в хранилище", this);

    uploadFileButton->setEnabled(false);
    if (m_isOperationListEmpty()) {
        selectFileButton->setEnabled(false);
    }

    connect(selectFileButton, SIGNAL(clicked()),this,SLOT(m_selectFileButton_clicked()));
    connect(uploadFileButton, SIGNAL(clicked()),this,SLOT(m_uploadButton_clicked()));

    QHBoxLayout *uploadButtonslayout = new QHBoxLayout;
    uploadButtonslayout->addWidget(selectFileButton);
    uploadButtonslayout->addWidget(uploadFileButton);

    return uploadButtonslayout;
}

bool StorageEditor::m_isOperationListEmpty()
{
    return operationsModel->rowCount() == 0;
}

void StorageEditor::m_uploadButton_clicked()
{
    File uploadFile;

    file = new QFile(fileName);
    QFileInfo fileInfo(*file);

    if (file->open(QIODevice::ReadOnly))
    {
        uploadFile.ba = file->readAll();
        uploadFile.name = fileInfo.fileName();
        uploadFile.operationId = operation.dbId;

        if (m_isFirmwareTabSelected()) {
            uploadFile.type = "firmware";
        }
        else {
            uploadFile.type = "doc";
        }

        if (db->m_insertFile(&uploadFile)) {
            QMessageBox msgBox;
            msgBox.setText("File uploaded");
            msgBox.exec();

            m_getOperationFiles();
        }
    }
}

bool StorageEditor::m_isFirmwareTabSelected()
{
    return rightSideTabs->currentIndex();
}

void StorageEditor::m_selectFileButton_clicked()
{
    fileName = QFileDialog::getOpenFileName(this, "Выбор файла", "", "Документация (*.pdf *.doc *.docx);;Прошивки (*.hex *.bin);;Все типы (*.*)");
    fileNameLineEdit->setText(fileName);

    if (!fileName.isEmpty()) {
        uploadFileButton->setEnabled(true);
    }
}

void  StorageEditor::m_getOperationFiles()
{
    m_getSelectedOperation();

    QSqlQueryModel *firmwareModel = db->m_getOperationFiles(operation.dbId, "firmware");
    firmwareListView->setModel(firmwareModel);

    QSqlQueryModel *docModel = db->m_getOperationFiles(operation.dbId, "doc");
    docListView->setModel(docModel);
}

void StorageEditor::m_getSelectedOperation()
{
    int row = operationsListView->currentIndex().row();
    QSqlRecord selectedOperation = operationsModel->record(row);

    operation.dbId = selectedOperation.value("id").toInt();
    operation.name = selectedOperation.value("name").toString();
}

void StorageEditor::m_openDeviceList()
{
    this->close();
    emit deviceList();
    delete this;
}

StorageEditor::~StorageEditor()
{
}
