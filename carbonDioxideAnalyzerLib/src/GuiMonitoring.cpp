#include "GuiMonitoring.hpp"
#include "loggerCpp/configurationManager.hpp"
#include <cmath>
#include <cstdio>
#include <algorithm>
#include "loggerCpp/logMacros.hpp"  // must be last

// ─── file-scope constants ─────────────────────────────────────────────────────
static constexpr double CAR_CO2_G_PER_KM = 120.0;  // gCO₂/km — EU average car

// ─── local formatting helpers ─────────────────────────────────────────────────
static std::string fmt1(double v) {
    char b[32]; std::snprintf(b, sizeof(b), "%.1f", v); return b;
}
static std::string fmt2(double v) {
    char b[32]; std::snprintf(b, sizeof(b), "%.2f", v); return b;
}

static const char* tempColor(double celsius) noexcept {
    if (celsius > 85.0) return "#ff5555";
    if (celsius > 70.0) return "#ffcc00";
    return "#d8d8e0";
}

static void co2Rgb(double gph, double& r, double& g, double& b) noexcept {
    if      (gph >= 20.0) { r=1.00; g=0.34; b=0.34; }
    else if (gph >=  5.0) { r=1.00; g=0.80; b=0.00; }
    else                   { r=0.27; g=0.87; b=0.44; }
}

static std::string fmtDuration(std::chrono::seconds s) {
    long long t = s.count();
    char b[16];
    std::snprintf(b, sizeof(b), "%02lld:%02lld:%02lld", t/3600, (t%3600)/60, t%60);
    return b;
}

// ─── Cairo draw helpers ───────────────────────────────────────────────────────

// Dark bg + grid lines + axes + title; caller draws data on top.
static void drawChrome(const Cairo::RefPtr<Cairo::Context>& cr,
                       double xOff, double yOff, double plotW, double plotH,
                       double yMax, int nLines, const char* yFmt, const char* title)
{
    cr->set_source_rgb(0.07, 0.07, 0.10);
    cr->paint();

    for (int i = 1; i <= nLines; ++i) {
        double frac = static_cast<double>(i) / nLines;
        double y    = yOff + plotH - frac * plotH;
        cr->set_source_rgba(0.20, 0.20, 0.28, 0.8);
        cr->set_line_width(0.5);
        cr->move_to(xOff, y); cr->line_to(xOff + plotW, y); cr->stroke();

        char buf[24]; std::snprintf(buf, sizeof(buf), yFmt, frac * yMax);
        cr->set_source_rgba(0.48, 0.48, 0.58, 1.0);
        cr->set_font_size(10.0);
        cr->move_to(2.0, y + 4.0);
        cr->show_text(buf);
    }

    // Axes
    cr->set_source_rgb(0.42, 0.42, 0.52);
    cr->set_line_width(1.0);
    cr->move_to(xOff, yOff);
    cr->line_to(xOff, yOff + plotH);
    cr->line_to(xOff + plotW, yOff + plotH);
    cr->stroke();

    // Plot title
    cr->set_source_rgb(0.60, 0.60, 0.72);
    cr->set_font_size(11.0);
    cr->move_to(xOff + 6.0, yOff + 14.0);
    cr->show_text(title);
}

// Draws filled area + line for a right-aligned time series.
static void drawSeries(const Cairo::RefPtr<Cairo::Context>& cr,
                       const std::vector<double>& data,
                       double xOff, double yOff, double plotW, double plotH,
                       double yMax, std::size_t cap,
                       double r, double g, double b, bool fill)
{
    if (data.size() < 2) return;
    const double xs = plotW / static_cast<double>(cap - 1);
    const double ys = (yMax > 0.0) ? plotH / yMax : plotH;
    const double x0 = xOff + static_cast<double>(cap - data.size()) * xs;

    if (fill) {
        cr->begin_new_path();
        cr->move_to(x0, yOff + plotH);
        for (std::size_t i = 0; i < data.size(); ++i)
            cr->line_to(x0 + static_cast<double>(i)*xs,
                        yOff + plotH - std::min(data[i], yMax)*ys);
        cr->line_to(x0 + static_cast<double>(data.size()-1)*xs, yOff + plotH);
        cr->close_path();
        cr->set_source_rgba(r, g, b, 0.18);
        cr->fill();
    }

    cr->begin_new_path();
    cr->move_to(x0, yOff + plotH - std::min(data[0], yMax)*ys);
    for (std::size_t i = 1; i < data.size(); ++i)
        cr->line_to(x0 + static_cast<double>(i)*xs,
                    yOff + plotH - std::min(data[i], yMax)*ys);
    cr->set_source_rgb(r, g, b);
    cr->set_line_width(2.0);
    cr->stroke();
}

// ─── Constructor / Destructor ─────────────────────────────────────────────────

GuiMonitoring::GuiMonitoring()
    : _sessionStart(std::chrono::steady_clock::now())
{
    _app = Gtk::Application::create("org.carbonanalyzer.app");
    createWindow();
    _dispatcher.connect(sigc::mem_fun(*this, &GuiMonitoring::onNewData));
    LOG_DEBUG("GuiMonitoring initialised");
}

GuiMonitoring::~GuiMonitoring() = default;

// ─── Public ───────────────────────────────────────────────────────────────────

void GuiMonitoring::runGui() {
    if (!_window) { LOG_ERROR("Cannot run GUI: window not initialised"); return; }
    _app->signal_activate().connect([this]() {
        _app->add_window(*_window);
        _window->show();
    });
    LOG_INFO("GUI event loop starting");
    _app->run(0, nullptr);
}

void GuiMonitoring::updateData(const AllComponentData& data) {
    { std::lock_guard lock(_dataMutex); _pendingData = data; _hasNewData = true; }
    _dispatcher.emit();
}

// ─── Private: window construction ─────────────────────────────────────────────

void GuiMonitoring::createWindow() {
    // Request dark theme
    if (auto s = Gtk::Settings::get_default())
        s->property_gtk_application_prefer_dark_theme().set_value(true);

    _window = std::make_unique<Gtk::Window>();
    _window->set_title("CO\u2082 Analyzer");
    _window->set_default_size(1340, 900);

    _mainBox = std::make_unique<Gtk::Box>(Gtk::Orientation::VERTICAL, 0);
    _window->set_child(*_mainBox);

    createHeader();
    createStatsRow();
    createPlots();
}

void GuiMonitoring::createHeader() {
    _headerBox = std::make_unique<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 0);
    _headerBox->set_margin_top(10);
    _headerBox->set_margin_bottom(10);
    _headerBox->set_margin_start(18);
    _headerBox->set_margin_end(18);

    auto title = std::make_unique<Gtk::Label>();
    title->set_markup(
        "<span font_desc='16' weight='bold' foreground='#44dd88'>"
        "CO\u2082 Analyzer</span>"
        "   <span foreground='#50505e' font_desc='11'>"
        "real-time carbon footprint of your hardware</span>");
    title->set_halign(Gtk::Align::START);
    title->set_hexpand(true);

    _sessionLabel = std::make_unique<Gtk::Label>();
    _sessionLabel->set_markup(
        "<span foreground='#50505e'>Session  </span>"
        "<span font_desc='13' weight='bold'>00:00:00</span>");
    _sessionLabel->set_halign(Gtk::Align::END);

    _headerBox->append(*title);
    _headerBox->append(*_sessionLabel);
    _mainBox->append(*_headerBox);
    _ownedWidgets.push_back(std::move(title));

    auto sep = std::make_unique<Gtk::Separator>(Gtk::Orientation::HORIZONTAL);
    _mainBox->append(*sep);
    _ownedWidgets.push_back(std::move(sep));
}

void GuiMonitoring::createStatsRow() {
    _statsBox = std::make_unique<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 10);
    _statsBox->set_margin(10);

    auto addCard = [&](std::unique_ptr<Gtk::Frame>& frame,
                       std::unique_ptr<Gtk::Label>& card,
                       const char* frameLabel)
    {
        frame = std::make_unique<Gtk::Frame>(frameLabel);
        frame->set_hexpand(true);
        card = std::make_unique<Gtk::Label>("\xe2\x80\x94");  // em dash placeholder
        card->set_use_markup(true);
        card->set_halign(Gtk::Align::START);
        card->set_valign(Gtk::Align::START);
        card->set_margin_start(14);
        card->set_margin_end(14);
        card->set_margin_top(10);
        card->set_margin_bottom(10);
        card->set_justify(Gtk::Justification::LEFT);
        frame->set_child(*card);
        _statsBox->append(*frame);
    };

    addCard(_cpuFrame, _cpuCard, "  CPU  ");
    addCard(_ramFrame, _ramCard, "  RAM  ");
    addCard(_co2Frame, _co2Card, "  CO\u2082 Emission  ");

    _mainBox->append(*_statsBox);

    auto sep = std::make_unique<Gtk::Separator>(Gtk::Orientation::HORIZONTAL);
    _mainBox->append(*sep);
    _ownedWidgets.push_back(std::move(sep));
}

void GuiMonitoring::createPlots() {
    _powerPlotArea = std::make_unique<Gtk::DrawingArea>();
    _powerPlotArea->set_draw_func(sigc::mem_fun(*this, &GuiMonitoring::onDrawPowerPlot));
    _powerPlotArea->set_expand(true);
    _mainBox->append(*_powerPlotArea);

    auto sep = std::make_unique<Gtk::Separator>(Gtk::Orientation::HORIZONTAL);
    _mainBox->append(*sep);
    _ownedWidgets.push_back(std::move(sep));

    _co2PlotArea = std::make_unique<Gtk::DrawingArea>();
    _co2PlotArea->set_draw_func(sigc::mem_fun(*this, &GuiMonitoring::onDrawCo2Plot));
    _co2PlotArea->set_expand(true);
    _mainBox->append(*_co2PlotArea);
}

// ─── Private: onNewData (runs on GUI thread) ──────────────────────────────────

void GuiMonitoring::onNewData() {
    AllComponentData data;
    {
        std::lock_guard lock(_dataMutex);
        if (!_hasNewData) return;
        data        = _pendingData;
        _hasNewData = false;
    }
    _currentData = data;

    const double totalPowW = data.cpuData.powerConsumption + data.ramData.powerConsumption;
    const double co2Now    = totalPowW * 0.001 * CARBON_INTENSITY; // gCO₂/h
    _totalCo2g     += co2Now   / 3600.0;  // gCO₂ per 1-second tick
    _totalEnergyWh += totalPowW / 3600.0; // Wh per 1-second tick

    auto push = [](std::vector<double>& v, double val, std::size_t cap) {
        v.push_back(val);
        if (v.size() > cap) v.erase(v.begin());
    };
    push(_cpuPowerPlotData, data.cpuData.powerConsumption, MAX_PLOT_POINTS);
    push(_ramPowerPlotData, data.ramData.powerConsumption, MAX_PLOT_POINTS);
    push(_co2PlotData,      co2Now,                        MAX_PLOT_POINTS);

    // ── Session label ─────────────────────────────────────────────────────
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - _sessionStart);
    _sessionLabel->set_markup(
        "<span foreground='#50505e'>Session  </span>"
        "<span font_desc='13' weight='bold'>" + fmtDuration(elapsed) + "</span>");

    // ── CPU card ──────────────────────────────────────────────────────────
    _cpuCard->set_markup(
        "<span weight='bold' font_desc='13' foreground='#18b8fa'>CPU</span>\n"
        "<span foreground='#888898'>Usage   </span>"
            "<span weight='bold'>" + fmt1(data.cpuData.usage) + " %</span>\n"
        "<span foreground='#888898'>Temp    </span>"
            "<span weight='bold' foreground='" + tempColor(data.cpuData.temperature) + "'>"
            + fmt1(data.cpuData.temperature) + " \u00b0C</span>\n"
        "<span foreground='#888898'>Power   </span>"
            "<span weight='bold'>" + fmt1(data.cpuData.powerConsumption) + " W</span>");

    // ── RAM card ──────────────────────────────────────────────────────────
    _ramCard->set_markup(
        "<span weight='bold' font_desc='13' foreground='#ff8c00'>RAM</span>\n"
        "<span foreground='#888898'>Usage   </span>"
            "<span weight='bold'>" + fmt1(data.ramData.usage) + " %</span>\n"
        "<span foreground='#888898'>Power   </span>"
            "<span weight='bold'>" + fmt1(data.ramData.powerConsumption) + " W</span>\n"
        "<span foreground='#404050'>\xe2\x80\x94</span>");

    // ── CO₂ card ──────────────────────────────────────────────────────────
    double rc, gc, bc;
    co2Rgb(co2Now, rc, gc, bc);
    char hex[8];
    std::snprintf(hex, sizeof(hex), "#%02x%02x%02x",
        static_cast<int>(rc*255), static_cast<int>(gc*255), static_cast<int>(bc*255));

    const char* lvl = (co2Now >= CO2_HIGH_THR)  ? "\u25cf HIGH"
                    : (co2Now >= CO2_MEDIUM_THR) ? "\u25cf MEDIUM"
                    :                              "\u25cf LOW";

    // Car-driving equivalent
    double carM = _totalCo2g / CAR_CO2_G_PER_KM * 1000.0;
    std::string carStr = carM >= 1000.0 ? fmt2(carM/1000.0) + " km" : fmt1(carM) + " m";

    _co2Card->set_markup(
        "<span weight='bold' font_desc='13' foreground='" + std::string(hex) + "'>"
            "CO\u2082 Emission  " + lvl + "</span>\n"
        "<span foreground='#888898'>Rate    </span>"
            "<span weight='bold' font_desc='14' foreground='" + std::string(hex) + "'>"
            + fmt1(co2Now) + " gCO\u2082/h</span>\n"
        "<span foreground='#888898'>Total   </span>"
            "<span weight='bold'>" + fmt2(_totalCo2g) + " gCO\u2082</span>"
            "<span foreground='#50505e' font_desc='10'>   \u2248 "
            + carStr + " driving</span>\n"
        "<span foreground='#888898'>Energy  </span>"
            "<span weight='bold'>" + fmt2(_totalEnergyWh) + " Wh</span>");

    _powerPlotArea->queue_draw();
    _co2PlotArea->queue_draw();
}

// ─── Private: draw callbacks ──────────────────────────────────────────────────

void GuiMonitoring::onDrawPowerPlot(const Cairo::RefPtr<Cairo::Context>& cr,
                                     int width, int height)
{
    constexpr double xOff = 52.0, yOff = 22.0;
    const double plotW = width  - xOff - 14.0;
    const double plotH = height - yOff - 28.0;

    // Auto-scale Y to max observed power, rounded up to nearest 10 W
    double yMax = 10.0;
    for (auto v : _cpuPowerPlotData) yMax = std::max(yMax, v);
    for (auto v : _ramPowerPlotData) yMax = std::max(yMax, v);
    yMax = std::ceil(yMax / 10.0) * 10.0;

    drawChrome(cr, xOff, yOff, plotW, plotH, yMax, 4, "%.0f W",
               "Power consumption (W)    \u2500  CPU (cyan) / RAM (orange)");

    drawSeries(cr, _cpuPowerPlotData, xOff, yOff, plotW, plotH, yMax, MAX_PLOT_POINTS,
               0.094, 0.722, 0.980, true);
    drawSeries(cr, _ramPowerPlotData, xOff, yOff, plotW, plotH, yMax, MAX_PLOT_POINTS,
               1.000, 0.549, 0.000, true);

    // Latest-value dot + annotation
    auto annotate = [&](const std::vector<double>& data, double r, double g, double b) {
        if (data.empty()) return;
        const double xs = plotW / static_cast<double>(MAX_PLOT_POINTS - 1);
        const double ys = plotH / yMax;
        const double x0 = xOff + static_cast<double>(MAX_PLOT_POINTS - data.size()) * xs;
        const double lx = x0 + static_cast<double>(data.size()-1) * xs;
        const double ly = yOff + plotH - std::min(data.back(), yMax) * ys;
        cr->arc(lx, ly, 4.0, 0.0, 2.0 * M_PI);
        cr->set_source_rgb(r, g, b);
        cr->fill();
        char buf[16]; std::snprintf(buf, sizeof(buf), "%.1f W", data.back());
        cr->set_font_size(11.0);
        cr->move_to(lx + 6.0, ly + 4.0);
        cr->show_text(buf);
    };
    annotate(_cpuPowerPlotData, 0.094, 0.722, 0.980);
    annotate(_ramPowerPlotData, 1.000, 0.549, 0.000);
}

void GuiMonitoring::onDrawCo2Plot(const Cairo::RefPtr<Cairo::Context>& cr,
                                   int width, int height)
{
    constexpr double xOff = 60.0, yOff = 22.0;
    const double plotW = width  - xOff - 14.0;
    const double plotH = height - yOff - 32.0;

    // Auto-scale Y, minimum 25 gCO₂/h, rounded up to nearest 5
    double yMax = 25.0;
    for (auto v : _co2PlotData) yMax = std::max(yMax, v * 1.15);
    yMax = std::ceil(yMax / 5.0) * 5.0;

    drawChrome(cr, xOff, yOff, plotW, plotH, yMax, 5, "%.1f",
               "CO\u2082 emission rate (gCO\u2082/h)    intensity: 450 gCO\u2082/kWh");

    // Y-axis unit label
    cr->set_source_rgba(0.44, 0.44, 0.56, 1.0);
    cr->set_font_size(9.0);
    cr->move_to(2.0, yOff + plotH + 16.0);
    cr->show_text("gCO\u2082/h");

    // Dashed threshold lines
    auto dashed = [&](double val, const char* lbl, double r, double g, double b) {
        if (val >= yMax) return;
        double y = yOff + plotH - (val / yMax) * plotH;
        std::vector<double> d{5.0, 4.0};
        cr->set_dash(d, 0.0);
        cr->set_source_rgba(r, g, b, 0.50);
        cr->set_line_width(0.8);
        cr->move_to(xOff, y); cr->line_to(xOff + plotW, y); cr->stroke();
        cr->unset_dash();
        cr->set_font_size(9.0);
        cr->set_source_rgba(r, g, b, 0.70);
        cr->move_to(xOff + 6.0, y - 3.0);
        cr->show_text(lbl);
    };
    dashed(CO2_MEDIUM_THR, "medium", 1.0, 0.80, 0.00);
    dashed(CO2_HIGH_THR,   "high",   1.0, 0.34, 0.34);

    if (_co2PlotData.size() < 2) return;

    const double xs    = plotW / static_cast<double>(MAX_PLOT_POINTS - 1);
    const double ys    = plotH / yMax;
    const double x0    = xOff + static_cast<double>(MAX_PLOT_POINTS - _co2PlotData.size()) * xs;
    const double lastX = x0 + static_cast<double>(_co2PlotData.size() - 1) * xs;

    // Build closed path for gradient fill
    cr->begin_new_path();
    cr->move_to(x0, yOff + plotH);
    for (std::size_t i = 0; i < _co2PlotData.size(); ++i)
        cr->line_to(x0 + static_cast<double>(i)*xs,
                    yOff + plotH - std::min(_co2PlotData[i], yMax)*ys);
    cr->line_to(lastX, yOff + plotH);
    cr->close_path();

    // Gradient: green (0, bottom) → yellow (medium) → red (high+, top)
    // Gradient coords: (0,yOff)=top=high, (0,yOff+plotH)=bottom=0
    const double medStop = 1.0 - CO2_MEDIUM_THR / yMax;
    const double hiStop  = 1.0 - CO2_HIGH_THR   / yMax;

    auto fillGrad = Cairo::LinearGradient::create(0, yOff, 0, yOff + plotH);
    fillGrad->add_color_stop_rgba(0.0,     1.00, 0.34, 0.34, 0.75);
    fillGrad->add_color_stop_rgba(hiStop,  1.00, 0.34, 0.34, 0.65);
    fillGrad->add_color_stop_rgba(medStop, 1.00, 0.80, 0.00, 0.50);
    fillGrad->add_color_stop_rgba(1.0,     0.27, 0.87, 0.44, 0.28);
    cr->set_source(fillGrad);
    cr->fill_preserve();

    // Same gradient for the stroke line
    auto lineGrad = Cairo::LinearGradient::create(0, yOff, 0, yOff + plotH);
    lineGrad->add_color_stop_rgb(0.0,     1.00, 0.34, 0.34);
    lineGrad->add_color_stop_rgb(hiStop,  1.00, 0.34, 0.34);
    lineGrad->add_color_stop_rgb(medStop, 1.00, 0.80, 0.00);
    lineGrad->add_color_stop_rgb(1.0,     0.27, 0.87, 0.44);
    cr->set_source(lineGrad);
    cr->set_line_width(2.5);
    cr->stroke();

    // Latest-value dot + annotation
    const double lv  = _co2PlotData.back();
    const double lx2 = lastX;
    const double ly2 = yOff + plotH - std::min(lv, yMax) * ys;
    double dr, dg, db;
    co2Rgb(lv, dr, dg, db);
    cr->arc(lx2, ly2, 5.0, 0.0, 2.0 * M_PI);
    cr->set_source_rgb(dr, dg, db);
    cr->fill();
    char buf[48]; std::snprintf(buf, sizeof(buf), "%.2f gCO\u2082/h", lv);
    cr->set_font_size(12.0);
    cr->set_source_rgb(dr, dg, db);
    cr->move_to(lx2 + 8.0, ly2 + 4.0);
    cr->show_text(buf);
}
