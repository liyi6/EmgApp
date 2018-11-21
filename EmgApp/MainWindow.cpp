#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    updateUi();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateUi()
{
    setWindowIcon(QIcon("res/logo.ico"));
    setWindowTitle("EMG Demo");

    for (int i=0; i<POINT_SHOW; i++) {
//        m_xAxisValue.append((double)POINT_SHOW-1-i);
        m_xAxisValue.push_front((double)i);
    }

    for (int channel=0; channel<CHANNEL_SIZE; channel++) {
        // set data container
        QVector<double>* dataList = new QVector<double>();
        for (int i=0; i<POINT_SHOW; i++) {
            dataList->push_front(0);
        }
        m_dataContainer.insert(channel, dataList);

        // set plot
        QString plotControlName = "channel" + QString::number(channel);
        QCustomPlot* plot = this->findChild<QCustomPlot*>(plotControlName);
        if (!plot) {
            qWarning() << "Find plot error: can't find~~~ " << plotControlName;
            continue;
        }
        plot->yAxis->setTickLabels(false);
        connect(plot->yAxis2, SIGNAL(rangeChanged(QCPRange)), plot->yAxis, SLOT(setRange(QCPRange))); // left axis only mirrors inner right axis
        plot->axisRect()->addAxis(QCPAxis::atRight);
        plot->axisRect()->axis(QCPAxis::atRight, 0)->setPadding(30); // add some padding to have space for tags

        // create graphs:
        QPointer<QCPGraph> graph = plot->addGraph(plot->xAxis, plot->axisRect()->axis(QCPAxis::atRight, 0));
        graph->setPen(QPen(QColor(250, 120, 0)));
        m_graphList.append(graph);
    }

    connect(&m_dataTimer, SIGNAL(timeout()), this, SLOT(onDrawData()));
    m_dataTimer.start(10);
}

void MainWindow::on_btnFsc_clicked()
{

}

void MainWindow::on_btnStart_clicked()
{
    NetConnectHelper::instance()->sendStartCmd();
}

void MainWindow::on_btnStop_clicked()
{
    NetConnectHelper::instance()->sendStopCmd();
}

void MainWindow::onDrawData()
{
    NetConnectHelper::instance()->getDataContainer(m_dataContainer);

    for (int channel=0; channel<CHANNEL_SIZE; channel++) {
        // set plot
        QString plotControlName = "channel" + QString::number(channel);
        QCustomPlot* plot = this->findChild<QCustomPlot*>(plotControlName);
        if (!plot) {
            qWarning() << "Find plot error: can't find--- " << plotControlName;
        }
        QPointer<QCPGraph> graph = m_graphList.at(channel);

        QVector<double>* dataList = m_dataContainer.value(channel);
        if (dataList) {
            if (dataList->size() > 0) {
//                for (int i=0; i<dataList->size(); i++) {
//                    graph->addData(graph->dataCount(), dataList->at(dataList->size()-i-1));
//                }
                graph->setData(m_xAxisValue, *dataList);

                plot->xAxis->rescale();
                graph->rescaleValueAxis(false, true);
                plot->xAxis->setRange(plot->xAxis->range().upper, POINT_SHOW, Qt::AlignRight);
                plot->replot();
            }
        } else {
            qWarning() << "Channel" << channel << "'s dataList is null.";
        }
    }
}
