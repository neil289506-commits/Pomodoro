#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include "pomo/Announcer.h"
namespace pomo {
struct WinVolume : IVolume {
    bool setPercent(int p) override {
        CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        IMMDeviceEnumerator* en = nullptr; IMMDevice* dev = nullptr; IAudioEndpointVolume* ep = nullptr; bool ok = false;
        if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&en))) &&
            SUCCEEDED(en->GetDefaultAudioEndpoint(eRender, eMultimedia, &dev)) &&
            SUCCEEDED(dev->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**)&ep)))
            ok = SUCCEEDED(ep->SetMasterVolumeLevelScalar(p / 100.0f, nullptr));
        if (ep) ep->Release(); if (dev) dev->Release(); if (en) en->Release();
        return ok;
    }
};
std::unique_ptr<IVolume> createPlatformVolume() { return std::make_unique<WinVolume>(); }
}
