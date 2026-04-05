#pragma once

#include <gtkmm.h>
#include <mutex>
#include <vector>
#include <memory>
#include "Units.hpp"

class GuiMonitoring {
public:
    GuiMonitoring();
    ~GuiMonitoring();

    void runGui();
    // Thread-safe: may be called from any thread.
    void updateData(const AllComponentData& data);

private:
    void createWindow();
    void createPlot();
    void onNewData();
    void onDrawPlot(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);

    Glib::RefPtr<Gtk::Application> _app;
    std::unique_ptr<Gtk::Window>      _window;
    std::unique_ptr<Gtk::Box>         _mainBox;
    std::unique_ptr<Gtk::DrawingArea> _plotArea;
    std::unique_ptr<Gtk::Label>       _trackData;

    // Cross-thread signalling
    Glib::Dispatcher  _dispatcher;
    std::mutex        _dataMutex;
    AllComponentData  _pendingData{};
    bool              _hasNewData{false};

    // GUI-thread-only state (no locking needed)
    AllComponentData    _currentData{};
    std::vector<double> _cpuPlotData;
    std::vector<double> _ramPlotData;
};
