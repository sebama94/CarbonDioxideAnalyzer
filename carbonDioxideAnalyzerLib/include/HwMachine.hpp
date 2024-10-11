#pragma once

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

class HwMachine 
{
    public:
        virtual double getUsage() const = 0;
        virtual double getTemperature() const { return 0.0; }   
        virtual ~HwMachine() = default;
        virtual double getPowerConsumption() const = 0;
        
        HwMachine() = default;
        HwMachine(const HwMachine&) = delete;
        HwMachine& operator=(const HwMachine&) = delete;
        HwMachine(HwMachine&&) = delete;
        HwMachine& operator=(HwMachine&&) = delete;

        double computeCO2Emission() const;

    protected:
        #ifdef _WIN32
        static constexpr DWORD MS_VC_EXCEPTION = 0x406D1388;
        #pragma pack(push,8)
        typedef struct tagTHREADNAME_INFO
        {
            DWORD dwType;
            LPCSTR szName;
            DWORD dwThreadID;
            DWORD dwFlags;
        } THREADNAME_INFO;
        #pragma pack(pop)
        #endif
};