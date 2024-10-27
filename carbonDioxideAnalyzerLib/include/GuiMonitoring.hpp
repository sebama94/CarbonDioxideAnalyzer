#pragma once

#include <gtkmm.h>
#include "Units.hpp"

class GuiMonitoring {
public:
    GuiMonitoring();
    ~GuiMonitoring();

    void runGui();
    void updateData(const AllComponentData& data);

private:
    void createWindow();
    void createPlot();
    void onDrawPlot(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);

    Glib::RefPtr<Gtk::Application> _app;
    std::unique_ptr<Gtk::Window> _window;
    std::unique_ptr<Gtk::DrawingArea> _plotArea;
    std::vector<double> _plotData;
    AllComponentData _allComponentData;
    std::unique_ptr<Gtk::Box> _mainBox;
};
