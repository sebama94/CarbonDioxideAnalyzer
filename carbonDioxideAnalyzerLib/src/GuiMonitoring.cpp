#include "GuiMonitoring.hpp"
#include "loggerCpp/configurationManager.hpp"
#include <cmath>
#include <iomanip>
#include <sstream>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
static std::string fmt1(double v) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(1) << v;
    return ss.str();
}

static constexpr std::size_t MAX_PLOT_POINTS = 100;

// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------
GuiMonitoring::GuiMonitoring() {
    _app = Gtk::Application::create("org.carbonanalyzer.app");
    createPlot();
    createWindow();
    _dispatcher.connect(sigc::mem_fun(*this, &GuiMonitoring::onNewData));
    LOG_DEBUG("GuiMonitoring initialised");
}

GuiMonitoring::~GuiMonitoring() = default;

// ---------------------------------------------------------------------------
// Public
// ---------------------------------------------------------------------------
void GuiMonitoring::runGui() {
    if (!_window) {
        LOG_ERROR("Cannot run GUI: window was not initialised");
        return;
    }
    _app->signal_activate().connect([this]() {
        _app->add_window(*_window);
        _window->show();
    });
    LOG_INFO("GUI event loop starting");
    _app->run(0, nullptr);
}

void GuiMonitoring::updateData(const AllComponentData& data) {
    {
        std::lock_guard<std::mutex> lock(_dataMutex);
        _pendingData = data;
        _hasNewData  = true;
    }
    _dispatcher.emit();  // posts to the GLib main loop — thread-safe
}

// ---------------------------------------------------------------------------
// Private — all called on the GUI (main) thread
// ---------------------------------------------------------------------------
void GuiMonitoring::onNewData() {
    AllComponentData data;
    {
        std::lock_guard<std::mutex> lock(_dataMutex);
        if (!_hasNewData) return;
        data        = _pendingData;
        _hasNewData = false;
    }

    _currentData = data;

    auto pushCapped = [](std::vector<double>& v, double val) {
        v.push_back(val);
        if (v.size() > MAX_PLOT_POINTS) v.erase(v.begin());
    };
    pushCapped(_cpuPlotData, data.cpuData.usage);
    pushCapped(_ramPlotData, data.ramData.usage);

    _plotArea->queue_draw();

    double co2 = data.cpuData.powerConsumption * 0.001 * 450.0;  // gCO2/h
    _trackData->set_text(
        "CPU: "     + fmt1(data.cpuData.usage)            + "%  "
        "Temp: "    + fmt1(data.cpuData.temperature)      + "\u00b0C  "
        "Power: "   + fmt1(data.cpuData.powerConsumption) + "W  "
        "CO\u2082: "+ fmt1(co2)                           + " gCO\u2082/h"
        "   |   "
        "RAM: "     + fmt1(data.ramData.usage)            + "%  "
        "Power: "   + fmt1(data.ramData.powerConsumption) + "W"
    );
}

void GuiMonitoring::createWindow() {
    _window = std::make_unique<Gtk::Window>();
    _window->set_title("CO\u2082 Analyzer");
    _window->set_default_size(1200, 800);
    if (_mainBox) {
        _window->set_child(*_mainBox);
    } else {
        LOG_ERROR("Main box not initialised before createWindow()");
    }
}

void GuiMonitoring::createPlot() {
    _mainBox = std::make_unique<Gtk::Box>(Gtk::Orientation::VERTICAL);
    _mainBox->set_spacing(8);
    _mainBox->set_margin(8);

    _plotArea = std::make_unique<Gtk::DrawingArea>();
    _plotArea->set_draw_func(sigc::mem_fun(*this, &GuiMonitoring::onDrawPlot));
    _plotArea->set_expand(true);

    _trackData = std::make_unique<Gtk::Label>("Waiting for data...");
    _trackData->set_margin(10);

    _mainBox->append(*_plotArea);
    _mainBox->append(*_trackData);
}

// ---------------------------------------------------------------------------
// Drawing
// ---------------------------------------------------------------------------
void GuiMonitoring::onDrawPlot(const Cairo::RefPtr<Cairo::Context>& cr,
                                int width, int height)
{
    // Dark background
    cr->set_source_rgb(0.10, 0.10, 0.12);
    cr->paint();

    const double margin = 45.0;
    const double plotW  = width  - 2.0 * margin;
    const double plotH  = height - 2.0 * margin;
    const double yScale = plotH / 100.0;

    // Grid (25 / 50 / 75 / 100 %)
    cr->set_line_width(0.5);
    cr->set_source_rgba(0.35, 0.35, 0.35, 0.8);
    for (int i = 1; i <= 4; ++i) {
        double y = margin + plotH - (i * 25.0 / 100.0) * plotH;
        cr->move_to(margin, y);
        cr->line_to(margin + plotW, y);
        cr->stroke();
    }

    // Helper lambda: draw a data series
    auto drawLine = [&](const std::vector<double>& data,
                        double r, double g, double b) {
        if (data.size() < 2) return;
        double xScale = plotW / static_cast<double>(data.size() - 1);
        cr->set_source_rgb(r, g, b);
        cr->set_line_width(2.0);
        cr->move_to(margin, margin + plotH - data[0] * yScale);
        for (std::size_t i = 1; i < data.size(); ++i) {
            cr->line_to(margin + static_cast<double>(i) * xScale,
                        margin + plotH - data[i] * yScale);
        }
        cr->stroke();

        // Dot + value annotation on last point
        double lx = margin + static_cast<double>(data.size() - 1) * xScale;
        double ly = margin + plotH - data.back() * yScale;
        cr->arc(lx, ly, 4.0, 0.0, 2.0 * M_PI);
        cr->fill();
        cr->move_to(lx + 6.0, ly + 4.0);
        cr->set_font_size(12.0);
        cr->show_text(fmt1(data.back()) + "%");
    };

    drawLine(_cpuPlotData, 0.18, 0.72, 0.98);  // cyan  — CPU
    drawLine(_ramPlotData, 1.00, 0.55, 0.00);  // orange — RAM

    // Axes
    cr->set_source_rgb(0.75, 0.75, 0.75);
    cr->set_line_width(1.0);
    cr->move_to(margin, margin);
    cr->line_to(margin, margin + plotH);
    cr->line_to(margin + plotW, margin + plotH);
    cr->stroke();

    // Y-axis labels
    cr->set_font_size(11.0);
    for (int i = 0; i <= 4; ++i) {
        double y = margin + plotH - (i * 25.0 / 100.0) * plotH;
        cr->move_to(2.0, y + 4.0);
        cr->show_text(std::to_string(i * 25) + "%");
    }

    // Legend
    cr->set_font_size(12.0);
    cr->set_source_rgb(0.18, 0.72, 0.98);
    cr->move_to(margin + 10.0, margin + 20.0);
    cr->show_text("\u2014 CPU");
    cr->set_source_rgb(1.00, 0.55, 0.00);
    cr->move_to(margin + 80.0, margin + 20.0);
    cr->show_text("\u2014 RAM");
}
