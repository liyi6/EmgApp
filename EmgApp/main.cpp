#include "StartDlg.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    StartDlg dlg;
    MainWindow main;
    if (QDialog::Accepted == dlg.exec()) {
        main.show();
    }
    return a.exec();
}
