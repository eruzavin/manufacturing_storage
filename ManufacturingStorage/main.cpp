#include <QApplication>
#include <devicelist.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DeviceList d;
    d.show();

    return a.exec();
}
