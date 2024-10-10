#pragma once

#include <cmath>

class Time {
public:
    explicit Time(double seconds) : seconds_(seconds) {}

    static Time fromSeconds(double seconds) {
        return Time(seconds);
    }

    double getSeconds() const { return seconds_; }
    double getHours() const { return seconds_ * SECONDS_TO_HOURS; }

private:
    static constexpr double SECONDS_TO_HOURS = 1.0 / 3600.0;
    double seconds_;
};

class EmissionsPerKWh {
public:
    explicit EmissionsPerKWh(double kgsPerKWh) : kgsPerKWh_(kgsPerKWh) {}

    static EmissionsPerKWh fromLbsPerMWh(double lbsPerMWh) {
        return EmissionsPerKWh(lbsPerMWh * LBS_MWH_TO_KG_KWH);
    }

    static EmissionsPerKWh fromGPerKWh(double gPerKWh) {
        return EmissionsPerKWh(gPerKWh * G_KWH_TO_KG_KWH);
    }

    static EmissionsPerKWh fromKgsPerKWh(double kgsPerKWh) {
        return EmissionsPerKWh(kgsPerKWh);
    }

    double getKgsPerKWh() const { return kgsPerKWh_; }

private:
    static constexpr double LBS_MWH_TO_KG_KWH = 0.00045359237;
    static constexpr double G_KWH_TO_KG_KWH = 0.001;
    double kgsPerKWh_;
};

class Energy {
public:
    explicit Energy(double kWh) : kWh_(kWh) {}

    static Energy fromPowerAndTime(const class Power& power, const Time& time);
    
    static Energy fromUjoules(double energy) {
        return Energy(energy * UJOULES_TO_JOULES * JOULES_TO_KWH);
    }

    static Energy fromMillijoules(double energy) {
        return Energy(energy * MILLIJOULES_TO_JOULES * JOULES_TO_KWH);
    }

    static Energy fromEnergy(double kWh) {
        return Energy(kWh);
    }

    double getKWh() const { return kWh_; }

    Energy operator-(const Energy& other) const {
        return Energy(kWh_ - other.kWh_);
    }

    Energy operator+(const Energy& other) const {
        return Energy(kWh_ + other.kWh_);
    }

    Energy operator*(double factor) const {
        return Energy(kWh_ * factor);
    }

    Energy operator/(double divisor) const {
        return Energy(kWh_ / divisor);
    }

    operator double() const { return kWh_; }

private:
    static constexpr double UJOULES_TO_JOULES = 1e-6;
    static constexpr double MILLIJOULES_TO_JOULES = 1e-3;
    static constexpr double JOULES_TO_KWH = 2.77778e-7;
    double kWh_;
};

class Power {
public:
    explicit Power(double kW) : kW_(kW) {}

    static Power fromMilliWatts(double milliWatts) {
        return Power(milliWatts * MILLI_WATTS_TO_WATTS * WATTS_TO_KILO_WATTS);
    }

    static Power fromWatts(double watts) {
        return Power(watts * WATTS_TO_KILO_WATTS);
    }

    static Power fromEnergiesAndDelay(const Energy& e1, const Energy& e2, const Time& delay) {
        double deltaEnergy = std::abs(e2.getKWh() - e1.getKWh());
        double kW = (delay.getHours() != 0.0) ? deltaEnergy / delay.getHours() : 0.0;
        return Power(kW);
    }

    static Power fromEnergyDeltaAndDelay(const Energy& e, const Time& delay) {
        return fromEnergiesAndDelay(e, Energy(0), delay);
    }

    double getKW() const { return kW_; }
    double getW() const { return kW_ * 1000.0; }

    Power operator+(const Power& other) const {
        return Power(kW_ + other.kW_);
    }

    Power operator*(double factor) const {
        return Power(kW_ * factor);
    }

private:
    static constexpr double MILLI_WATTS_TO_WATTS = 0.001;
    static constexpr double WATTS_TO_KILO_WATTS = 0.001;
    double kW_;
};

inline Energy Energy::fromPowerAndTime(const Power& power, const Time& time) {
    return Energy(power.getKW() * time.getHours());
}
