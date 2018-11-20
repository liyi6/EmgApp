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

    for (int i=0; i<CHANNEL_SIZE; i++) {
        // set plot
        QString plotControlName = "channel" + QString::number(i);
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

        // add AxisTag
        AxisTag* tag = new AxisTag(graph->valueAxis());
        tag->setPen(graph->pen());
        m_tagList.append(tag);
    }

    connect(NetConnectHelper::instance(), SIGNAL(dataComming(int,int)), this, SLOT(onDataComming(int,int)));
    connect(&m_dataTimer, SIGNAL(timeout()), this, SLOT(onDrawData()));
    m_dataTimer.start(40);
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


void MainWindow::onDataComming(int channel, int data)
{
    if (channel < 0 || channel > 15) {
        qWarning() << "Error channel number:" << channel;
        return;
    }

    // set plot
    QString plotControlName = "channel" + QString::number(channel);
    QCustomPlot* plot = this->findChild<QCustomPlot*>(plotControlName);
    if (!plot) {
        qWarning() << "Find plot error: can't find--- " << plotControlName;
    }
    QPointer<QCPGraph> graph = m_graphList.at(channel);
    AxisTag* tag = m_tagList.at(channel);

    graph->addData(graph->dataCount(), data);

    plot->xAxis->rescale();
    graph->rescaleValueAxis(false, true);

    plot->xAxis->setRange(plot->xAxis->range().upper, 2400, Qt::AlignRight);

    double graphValue = graph->dataMainValue(graph->dataCount()-1);

    tag->updatePosition(graphValue);

    tag->setText(QString::number(graphValue, 'f', 2));

    plot->replot();
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
        AxisTag* tag = m_tagList.at(channel);

        graph->addData(graph->dataCount(), data);

        plot->xAxis->rescale();
        graph->rescaleValueAxis(false, true);

        plot->xAxis->setRange(plot->xAxis->range().upper, 2400, Qt::AlignRight);

        double graphValue = graph->dataMainValue(graph->dataCount()-1);

        tag->updatePosition(graphValue);

        tag->setText(QString::number(graphValue, 'f', 2));

        plot->replot();
    }

    m_dataContainer.clear();
}
