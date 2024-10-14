#pragma once

#include <gtkmm.h>
#include <memory>
#include <vector>

class GuiMonitoring {
public:
    GuiMonitoring();
    ~GuiMonitoring();

    void runGui();
    void updateData(const std::vector<double>& data);

private:
    void createWindow();
    void createPlot();
    void onDrawPlot(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);

    Glib::RefPtr<Gtk::Application> app;
    std::unique_ptr<Gtk::Window> window;
    std::unique_ptr<Gtk::DrawingArea> plotArea;
    std::vector<double> plotData;
};
