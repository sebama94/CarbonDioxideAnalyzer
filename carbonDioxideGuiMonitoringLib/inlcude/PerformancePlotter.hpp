#pragma once

#include <QWidget>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QTimer>
#include "ComponentManager.hpp"

class PerformancePlotter : public QWidget
{
    Q_OBJECT

public:
    explicit PerformancePlotter(QWidget *parent = nullptr);
    ~PerformancePlotter() override;

private slots:
    void updatePlot();

private:
    QChart *m_chart;
    QChartView *m_chartView;
    QLineSeries *m_series;
    QValueAxis *m_axisX;
    QValueAxis *m_axisY;
    QTimer *m_timer;
    std::unique_ptr<HwMachine> m_cpu;
    int m_dataCount;

    static constexpr int MAX_DATA_POINTS = 100;
    static constexpr int UPDATE_INTERVAL_MS = 1000;
};
