// MicroMute.cpp : définit le point d'entrée pour l'application.
//

#include "stdafx.h"
#include "MicroMute.h"

#define MAX_LOADSTRING 100
#define IDM_QUIT 1

// Variables globales :
HINSTANCE hInst;								// instance actuelle
TCHAR szTitle[MAX_LOADSTRING];					// Le texte de la barre de titre
TCHAR szWindowClass[MAX_LOADSTRING];			// le nom de la classe de fenêtre principale

NOTIFYICONDATA nid;
ATOM hotKeyAtom = 0;
IMMDevice* mike = nullptr;

// Pré-déclarations des fonctions incluses dans ce module de code :
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    CoInitialize(nullptr);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: placez ici le code.
	MSG msg;
	HACCEL hAccelTable;

	// Initialise les chaînes globales
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MICROMUTE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Effectue l'initialisation de l'application :
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MICROMUTE));

	// Boucle de messages principale :
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FONCTION : MyRegisterClass()
//
//  BUT : inscrit la classe de fenêtre.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MICROMUTE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MICROMUTE);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_MICROMUTE));

	return RegisterClassEx(&wcex);
}

//
//   FONCTION : InitInstance(HINSTANCE, int)
//
//   BUT : enregistre le handle de l'instance et crée une fenêtre principale
//
//   COMMENTAIRES :
//
//        Dans cette fonction, nous enregistrons le handle de l'instance dans une variable globale, puis
//        créons et affichons la fenêtre principale du programme.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Stocke le handle d'instance dans la variable globale

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, SW_HIDE);
   UpdateWindow(hWnd);

   return TRUE;
}


//
//  FONCTION : WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  BUT :  traite les messages pour la fenêtre principale.
//
//  WM_COMMAND	- traite le menu de l'application
//  WM_PAINT	- dessine la fenêtre principale
//  WM_DESTROY	- génère un message d'arrêt et retourne
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static CMicrophoneMuter muter(hWnd);

	switch (message)
	{
    case WM_CREATE:
        // At window creation
        {   
            muter.Init();

            // create tray icon    
            ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
            nid.cbSize = sizeof(NOTIFYICONDATA);
            nid.hWnd = hWnd;
            nid.uID = 0;

            LPCTSTR iconStr = muter.IsMute() ? MAKEINTRESOURCE(IDI_OFF) : MAKEINTRESOURCE(IDI_ON);
            nid.hIcon = LoadIcon(GetModuleHandle(nullptr), iconStr);
            nid.uCallbackMessage = MY_TRAY_ICON;
            nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
            wcscpy_s(nid.szTip, _T("MicroMute"));
            Shell_NotifyIcon(NIM_ADD, &nid);

            // Register global keyboard shortcut
            hotKeyAtom = GlobalAddAtom(L"StarterHotKey");
            BOOL bHotKey = RegisterHotKey(hWnd, hotKeyAtom, 0, VK_SCROLL);
            muter.Init();
            break;
        }
    case WM_COMMAND:
        if (LOWORD(wParam) == IDM_EXIT)
        {
            DestroyWindow(hWnd);
        }
        break;
    
    case MY_TRAY_ICON:
        if (lParam == WM_RBUTTONUP)
        {
            HMENU hmenu;
            HMENU hpopup;
            POINT pos;
            GetCursorPos(&pos);
            hmenu = LoadMenu(GetModuleHandle(NULL), _T("LEMENU"));
            hpopup = GetSubMenu(hmenu, 0);
            SetForegroundWindow(hWnd);
            TrackPopupMenuEx(hpopup, 0, pos.x, pos.y, hWnd, nullptr);              
            DestroyMenu(hmenu);
        }
        break;

    case MY_MICRO:
        {
            BOOL bMuted = static_cast<BOOL>(wParam);
            LPCTSTR iconStr = bMuted ? MAKEINTRESOURCE(IDI_OFF) : MAKEINTRESOURCE(IDI_ON);
            nid.hIcon = LoadIcon(GetModuleHandle(nullptr), iconStr);
            Shell_NotifyIcon(NIM_MODIFY, &nid);
        }
        break;

    case WM_HOTKEY:
        muter.SetMute(!muter.IsMute());
        break;

	case WM_DESTROY:
		Shell_NotifyIcon(NIM_DELETE, &nid);
        UnregisterHotKey(hWnd, hotKeyAtom);
        GlobalDeleteAtom(hotKeyAtom);
        if (mike)
        {
            mike->Release();
            mike = nullptr;
        }

        CoUninitialize();
        PostQuitMessage(0);
        break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

ULONG STDMETHODCALLTYPE CMicrophoneVolumeCallback::AddRef()
{
    return InterlockedIncrement(&m_ref);
}

ULONG STDMETHODCALLTYPE CMicrophoneVolumeCallback::Release()
{
    ULONG ulRef = InterlockedDecrement(&m_ref);
    if (0 == ulRef)
    {
        delete this;
    }
    return ulRef;
}

HRESULT STDMETHODCALLTYPE CMicrophoneVolumeCallback::QueryInterface(REFIID riid, VOID** ppvInterface)
{
    if (IID_IUnknown == riid)
    {
        AddRef();
        *ppvInterface = reinterpret_cast<IUnknown*>(this);
    }
    else if (__uuidof(IAudioEndpointVolumeCallback) == riid)
    {
        AddRef();
        *ppvInterface = reinterpret_cast<IAudioEndpointVolumeCallback*>(this);
    }
    else
    {
        *ppvInterface = nullptr;
        return E_NOINTERFACE;
    }
    return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE  CMicrophoneVolumeCallback::OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify)
{
    if (pNotify == nullptr)
    {
        return E_INVALIDARG;
    }

    // Alert listening window the micro status changed
    PostMessage(m_hWndTarget, MY_MICRO, pNotify->bMuted, 0);

    return S_OK;
}

BOOL CMicrophoneMuter::Init()
{
    // Get a device enumerator
    IMMDeviceEnumerator* deviceEnumerator = nullptr;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, 
            CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), 
            reinterpret_cast<LPVOID*>(&deviceEnumerator));

    if (FAILED(hr))
    {
        return FALSE;
    }

    // Get the microphone
    IMMDevice* defaultDevice = nullptr;
    hr = deviceEnumerator->GetDefaultAudioEndpoint(eCapture, eCommunications, 
        &defaultDevice);

    deviceEnumerator->Release();
    deviceEnumerator = nullptr;

    if (FAILED(hr))
    {            
        return FALSE;
    }

    m_device = defaultDevice;

    // Get the interface to volume control
    m_endpointVolume = nullptr;
    hr = m_device->Activate(__uuidof(IAudioEndpointVolume), 
            CLSCTX_INPROC_SERVER, nullptr, 
            reinterpret_cast<LPVOID*>(&m_endpointVolume));

    if (FAILED(hr))
    {
        m_device->Release();
        m_device = nullptr;
        return FALSE;
    }

    hr = m_endpointVolume->RegisterControlChangeNotify(&m_callback);
    if (FAILED(hr))
    {
        m_endpointVolume->Release();
        m_endpointVolume = nullptr;
        m_device->Release();
        m_device = nullptr;
        return FALSE;
    }

    return TRUE;
}