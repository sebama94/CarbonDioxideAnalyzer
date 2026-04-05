#pragma once

#include <gtkmm.h>
#include <mutex>
#include <vector>
#include <memory>
#include <chrono>
#include "Units.hpp"

class GuiMonitoring {
public:
    GuiMonitoring();
    ~GuiMonitoring();

    void runGui();
    void updateData(const AllComponentData& data);  // thread-safe

private:
    // ── Construction ──────────────────────────────────────────────────────────
    void createWindow();
    void createHeader();
    void createStatsRow();
    void createPlots();

    // ── Event handlers (GUI thread only) ──────────────────────────────────────
    void onNewData();
    void onDrawPowerPlot(const Cairo::RefPtr<Cairo::Context>& cr, int w, int h);
    void onDrawCo2Plot  (const Cairo::RefPtr<Cairo::Context>& cr, int w, int h);

    // ── App / window ──────────────────────────────────────────────────────────
    Glib::RefPtr<Gtk::Application> _app;
    std::unique_ptr<Gtk::Window>   _window;
    std::unique_ptr<Gtk::Box>      _mainBox;

    // ── Header ────────────────────────────────────────────────────────────────
    std::unique_ptr<Gtk::Box>    _headerBox;
    std::unique_ptr<Gtk::Label>  _sessionLabel;

    // ── Stat cards ────────────────────────────────────────────────────────────
    std::unique_ptr<Gtk::Box>    _statsBox;
    std::unique_ptr<Gtk::Frame>  _cpuFrame;
    std::unique_ptr<Gtk::Frame>  _ramFrame;
    std::unique_ptr<Gtk::Frame>  _co2Frame;
    std::unique_ptr<Gtk::Label>  _cpuCard;
    std::unique_ptr<Gtk::Label>  _ramCard;
    std::unique_ptr<Gtk::Label>  _co2Card;

    // ── Plots ─────────────────────────────────────────────────────────────────
    std::unique_ptr<Gtk::DrawingArea> _powerPlotArea;
    std::unique_ptr<Gtk::DrawingArea> _co2PlotArea;

    // ── Widgets owned for lifetime only (no updates needed) ───────────────────
    std::vector<std::unique_ptr<Gtk::Widget>> _ownedWidgets;

    // ── Cross-thread signalling ────────────────────────────────────────────────
    Glib::Dispatcher _dispatcher;
    std::mutex       _dataMutex;
    AllComponentData _pendingData{};
    bool             _hasNewData{false};

    // ── GUI-thread state ───────────────────────────────────────────────────────
    AllComponentData _currentData{};
    std::chrono::steady_clock::time_point _sessionStart;

    static constexpr std::size_t MAX_PLOT_POINTS  = 120;   // 2 min at 1 Hz
    static constexpr double      CARBON_INTENSITY = 450.0; // gCO₂/kWh (IEA avg)
    static constexpr double      CO2_MEDIUM_THR   = 5.0;   // gCO₂/h
    static constexpr double      CO2_HIGH_THR     = 20.0;  // gCO₂/h

    std::vector<double> _cpuPowerPlotData;  // W
    std::vector<double> _ramPowerPlotData;  // W
    std::vector<double> _co2PlotData;       // gCO₂/h

    double _totalCo2g{0.0};     // gCO₂ accumulated since session start
    double _totalEnergyWh{0.0}; // Wh accumulated since session start
};
