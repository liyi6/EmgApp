#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>
#include "NetConnectHelper.h"
#include "axistag.h"
#include "qcustomplot.h"
#include "Defines.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void on_btnFsc_clicked();
    void on_btnStart_clicked();
    void on_btnStop_clicked();
    void onDrawData();
    //void onDrawData2();

private:
    void updateUi();

private:
    Ui::MainWindow *ui;
    QList<QPointer<QCPGraph>>  m_graphList;
    QList<AxisTag*>            m_tagList;
    QTimer                     m_dataTimer;
    QHash<int, QVector<double>*> m_dataContainer;
};

#endif // MAINWINDOW_H
