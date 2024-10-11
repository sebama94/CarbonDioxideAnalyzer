#include "PerformancePlotter.hpp"
#include <QVBoxLayout>

PerformancePlotter::PerformancePlotter(QWidget *parent)
    : QWidget(parent), m_dataCount(0)
{
    m_chart = new QChart();
    m_chartView = new QChartView(m_chart);
    m_series = new QLineSeries();
    m_axisX = new QValueAxis();
    m_axisY = new QValueAxis();

    m_chart->addSeries(m_series);
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    m_series->attachAxis(m_axisX);
    m_series->attachAxis(m_axisY);

    m_axisX->setRange(0, MAX_DATA_POINTS);
    m_axisY->setRange(0, 100);

    m_chart->setTitle("CPU Usage");
    m_axisX->setTitleText("Time (s)");
    m_axisY->setTitleText("Usage (%)");

    m_chartView->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_chartView);
    setLayout(layout);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &PerformancePlotter::updatePlot);
    m_timer->start(UPDATE_INTERVAL_MS);

    m_cpu = HwMachineFactory::createMachine(HwMachineFactory::MachineType::CPU);
}

PerformancePlotter::~PerformancePlotter()
{
    delete m_chart;
    delete m_chartView;
    delete m_series;
    delete m_axisX;
    delete m_axisY;
    delete m_timer;
}

void PerformancePlotter::updatePlot()
{
    double cpuUsage = m_cpu->getUsage();
    m_series->append(m_dataCount, cpuUsage);

    if (m_dataCount >= MAX_DATA_POINTS) {
        m_series->remove(0);
        m_axisX->setRange(m_dataCount - MAX_DATA_POINTS + 1, m_dataCount);
    }

    m_dataCount++;
    m_chart->update();
}
