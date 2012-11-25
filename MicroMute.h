#pragma once

#include "resource.h"

enum 
{ 
    // tray icon message
    MY_TRAY_ICON = WM_USER+1,
    // Micro status changed message
    MY_MICRO
};

/**
 * Class that will be notified of Micro status changes
 */
class CMicrophoneVolumeCallback : public IAudioEndpointVolumeCallback
{
    /// Reference counter (COM requirement)
    LONG m_ref;

    /// Reference to the window that owns our tray icon
    HWND m_hWndTarget;

public:
    CMicrophoneVolumeCallback(HWND hWndTarget) 
            : m_ref(1), m_hWndTarget(hWndTarget)
    {

    }

    ~CMicrophoneVolumeCallback()
    {

    }

    // IUnknown methods -- AddRef, Release and QueryInterface

    ULONG STDMETHODCALLTYPE AddRef();

    ULONG STDMETHODCALLTYPE Release();
        
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID** ppvInterface);

    // Callback method for endpoint-volume-change notifications.
    HRESULT STDMETHODCALLTYPE  OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify);
};


class CMicrophoneMuter
{
    IMMDevice* m_device;
    IAudioEndpointVolume* m_endpointVolume;
    CMicrophoneVolumeCallback m_callback;

public:

    CMicrophoneMuter(HWND hWndTarget) 
            : m_device(nullptr), m_endpointVolume(nullptr), 
              m_callback(hWndTarget)
    {

    }

    ~CMicrophoneMuter()
    {
        if (m_endpointVolume)
        {
            m_endpointVolume->UnregisterControlChangeNotify(
                    reinterpret_cast<IAudioEndpointVolumeCallback*>(&m_callback));
            m_endpointVolume->Release();
            m_endpointVolume = nullptr;
        }

        if (m_device)
        {
            m_device->Release();
            m_device = nullptr;
        }
    }

    /**
     * COM-related stuff initialization.
     * Returns TRUE on success, FALSE otherwise.
     */
    BOOL Init();

    
    BOOL IsMute()
    {
        BOOL bMute;
        m_endpointVolume->GetMute(&bMute);
        return bMute;
    }

    void SetMute(BOOL bMute)
    {
        m_endpointVolume->SetMute(bMute, nullptr);
    }
};