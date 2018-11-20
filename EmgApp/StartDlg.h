#ifndef STARTDLG_H
#define STARTDLG_H

#include <QDialog>
#include <QPainter>
#include <QMessageBox>

#include "NetConnectHelper.h"
#include "MainWindow.h"

namespace Ui {
class StartDlg;
}

class StartDlg : public QDialog
{
    Q_OBJECT

public:
    explicit StartDlg(QWidget *parent = 0);
    ~StartDlg();

protected:
    void paintEvent(QPaintEvent*);

private slots:
    void on_connect_clicked();
    void onConnectResult(bool connected);

private:
    void updateUi();

private:
    Ui::StartDlg* ui;
};

#endif // STARTDLG_H
