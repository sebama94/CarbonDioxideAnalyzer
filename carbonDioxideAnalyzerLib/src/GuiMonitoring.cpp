#include "GuiMonitoring.hpp"
#include <iostream>
#include <cstddef> // Add this line to include the definition of size_t

GuiMonitoring::GuiMonitoring() {
    std::cout << "Debug: GuiMonitoring constructor called" << std::endl;
    app = Gtk::Application::create("org.gtkmm.example");
    createWindow();
    createPlot();
}

GuiMonitoring::~GuiMonitoring() {
    std::cout << "Debug: GuiMonitoring destructor called" << std::endl;
}

void GuiMonitoring::runGui() {
    std::cout << "Debug: GuiMonitoring::run() called" << std::endl;
    // Here we pass the command line arguments
    app->run();
}

void GuiMonitoring::updateData(const std::vector<double>& data) {
    std::cout << "Debug: GuiMonitoring::updateData() called with " << data.size() << " elements" << std::endl;
    plotData = data;
    if (plotArea) {
        plotArea->queue_draw(); // Schedule a redraw of the plot area
    }
}

void GuiMonitoring::createWindow() {
    std::cout << "Debug: GuiMonitoring::createWindow() called" << std::endl;
    window = std::make_unique<Gtk::Window>();
    window->set_title("CO2 Analyzer");
    window->set_default_size(800, 600);
    // window->set_child(*plotArea); // Set the plot area as child of the window
    window->show(); // Show the window
}

void GuiMonitoring::createPlot() {
    std::cout << "Debug: GuiMonitoring::createPlot() called" << std::endl;
    plotArea = std::make_unique<Gtk::DrawingArea>();
    plotArea->set_draw_func(sigc::mem_fun(*this, &GuiMonitoring::onDrawPlot));
    // Note: The plotArea should be set in createWindow after it is created
}

void GuiMonitoring::onDrawPlot(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
    std::cout << "Debug: GuiMonitoring::onDrawPlot() called with width=" << width << ", height=" << height << std::endl;
    
    // Draw background
    cr->set_source_rgb(1.0, 1.0, 1.0);
    cr->paint();

    // Draw plot if there's data
    if (!plotData.empty()) {
        std::cout << "Debug: Drawing plot with " << plotData.size() << " data points" << std::endl;
        cr->set_source_rgb(0.0, 0.0, 1.0);
        cr->set_line_width(2.0);

        double x_scale = static_cast<double>(width) / (plotData.size() - 1);
        double y_scale = height / (*std::max_element(plotData.begin(), plotData.end()));

        std::size_t dataSize = plotData.size();
        if (dataSize > 0) {
            cr->move_to(0, height - plotData[0] * y_scale);
            for (std::size_t i = 1; i < dataSize; ++i) {
                double x = i * x_scale;
                double y = height - plotData[i] * y_scale;
                cr->line_to(x, y);
            }
            cr->stroke();
        }
    } else {
        std::cout << "Debug: No plot data available" << std::endl;
    }
}
