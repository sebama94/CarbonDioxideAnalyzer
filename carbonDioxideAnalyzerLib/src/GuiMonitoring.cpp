#include "GuiMonitoring.hpp"
#include <iostream>
#include <cstddef>
#include <memory>
#include <vector>
#include <algorithm>
#include "Units.hpp"

GuiMonitoring::GuiMonitoring() {
    std::cout << "Debug: GuiMonitoring constructor called" << std::endl;
    _app = Gtk::Application::create("org.gtkmm.example");
    createPlot();
    createWindow();
}

GuiMonitoring::~GuiMonitoring() {
    std::cout << "Debug: GuiMonitoring destructor called" << std::endl;
}

void GuiMonitoring::runGui() {
    std::cout << "Debug: GuiMonitoring::run() called" << std::endl;
    _app->run();
}

void GuiMonitoring::updateData(const AllComponentData& data) {
    _allComponentData = data;
    _plotData.push_back(data.cpuData.usage);
    if (_plotData.size() > 100) {  // Keep only the last 100 data points
        _plotData.erase(_plotData.begin());
    }
    if (_plotArea) {
        std::cout << "Debug: _plotArea is valid" << std::endl;
        std::cout << "  Plot data size: " << _plotData.size() << std::endl;
        _plotArea->queue_draw(); // Schedule a redraw of the plot area
    }
}

void GuiMonitoring::createWindow() {
    std::cout << "Debug: GuiMonitoring::createWindow() called" << std::endl;
    _window = std::make_unique<Gtk::Window>();
    _window->set_title("CO2 Analyzer");
    _window->set_default_size(1200, 800);
    _window->set_child(*_plotArea);
    _window->show();
}

void GuiMonitoring::createPlot() {
    std::cout << "Debug: GuiMonitoring::createPlot() called" << std::endl;
    _plotArea = std::make_unique<Gtk::DrawingArea>();
    _plotArea->set_draw_func(sigc::mem_fun(*this, &GuiMonitoring::onDrawPlot));
    _plotArea->set_content_width(200);
    _plotArea->set_content_height(200);
    _plotArea->add_css_class("grid");
    // Pack the plot area into the main box layout
    _plotArea->show();  // Show plot area   
}

void GuiMonitoring::onDrawPlot(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
    std::cout << "Debug: GuiMonitoring::onDrawPlot() called with width=" << width << ", height=" << height << std::endl;
    
    // Draw background
    cr->set_source_rgb(1.0, 1.0, 1.0);
    cr->paint();

    // Draw plot if there's data
    if (!_plotData.empty()) {
        std::cout << "Debug: Drawing plot with " << _plotData.size() << " data points" << std::endl;
        cr->set_source_rgb(0.0, 0.0, 1.0);
        cr->set_line_width(2.0);

        double x_scale = static_cast<double>(width) / (_plotData.size() - 1);
        double y_scale = height / 100.0;  // Assuming usage is a percentage (0-100)

        std::size_t dataSize = _plotData.size();
        if (dataSize > 0) {
            cr->move_to(0, height - _plotData[0] * y_scale);
            for (std::size_t i = 1; i < dataSize; ++i) {
                double x = i * x_scale;
                double y = height - _plotData[i] * y_scale;
                cr->line_to(x, y);
            }
            cr->stroke();
        }
    } else {
        std::cout << "Debug: No plot data available" << std::endl;
    }
}
