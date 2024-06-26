#include "OverlayHook.h"
#include <Base/Sig.h>
#include <Base/AsmTools.h>
#include <cstdio>

#ifdef _M_AMD64
#define SIG_CD3DHAL "48 8D 05 ? ? ? ? 48 89 03 33 C0"
#define OFFSET_CD3DHAL 3
#else
#define SIG_CD3DHAL "C7 06 ? ? ? ? 89 86 ? ? ? ? 89 86"
#define OFFSET_CD3DHAL 2
#endif

void COverlayHook::Hook()
{
	std::string error_create;
	std::string error_scan;

	printf("Creating IDirect3DDevice9...\n");
	void** vtable = GetDeviceVtable_CreateDevice(&error_create);
	
	if (!vtable)
	{
		printf("Scanning for IDirect3DDevice9 vtable...\n");
		vtable = GetDeviceVtable_SigScan(&error_scan);
	}

	if (!vtable)
	{
		FATAL(
			"Failed to hook D3D9.\n"
			"Try launching the game in windowed mode (with -windowed).\n\n"
			"GetDeviceVtable_CreateDevice: %s\n"
			"GetDeviceVtable_SigScan: %s",
			error_create.c_str(), error_scan.c_str()
		);
	}
	
	m_jmp_reset.Hook(vtable[16], &Hooked_Reset);
	m_jmp_present.Hook(vtable[17], &Hooked_Present);
}

void COverlayHook::Unhook()
{
	m_jmp_reset.Unhook();
	m_jmp_present.Unhook();
}

HRESULT COverlayHook::Reset(D3DPRESENT_PARAMETERS* Params)
{
	static auto original = m_jmp_reset.Original<decltype(Hooked_Reset)*>();
	return original(m_dev, Params);
}

HRESULT COverlayHook::Present(const RECT* Src, const RECT* Dest, HWND Window, const RGNDATA* DirtyRegion)
{
	static auto original = m_jmp_present.Original<decltype(Hooked_Present)*>();
	return original(m_dev, Src, Dest, Window, DirtyRegion);
}

HRESULT WINAPI COverlayHook::Hooked_Reset(IDirect3DDevice9* thisptr, D3DPRESENT_PARAMETERS* Params)
{
	g_hk_overlay.m_dev = thisptr;
	OnReset.DispatchEvent(thisptr, Params);
	return g_hk_overlay.Reset(Params);
}

HRESULT WINAPI COverlayHook::Hooked_Present(IDirect3DDevice9* thisptr, const RECT* Src, const RECT* Dest, HWND Window, const RGNDATA* DirtyRegion)
{
	g_hk_overlay.m_dev = thisptr;

	DWORD oldstate;
	thisptr->GetRenderState(D3DRS_SRGBWRITEENABLE, &oldstate);
	thisptr->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

	OnPresent.DispatchEvent(thisptr, Src, Dest, Window, DirtyRegion);

	thisptr->SetRenderState(D3DRS_SRGBWRITEENABLE, oldstate);
	return g_hk_overlay.Present(Src, Dest, Window, DirtyRegion);
}

void** COverlayHook::GetDeviceVtable_SigScan(std::string* out_error)
{
	const auto code = reinterpret_cast<uint8_t*>(Sig::FindPattern("d3d9.dll", SIG_CD3DHAL));
	if (!code)
	{
		*out_error = "The signature scan could not find `CD3DHal::CD3DHal`";
		return nullptr;
	}
	return reinterpret_cast<void**>(AsmTools::Relative(code, OFFSET_CD3DHAL));
}

void** COverlayHook::GetDeviceVtable_CreateDevice(std::string* out_error)
{
	const HMODULE d3d_module = Base::GetModule("d3d9.dll");
	if (!d3d_module)
	{
		*out_error = "d3d9.dll is not loaded";
		return nullptr;
	}

	const auto p_Direct3DCreate9 = reinterpret_cast<decltype(Direct3DCreate9)*>(Base::GetProc(d3d_module, "Direct3DCreate9"));

	IDirect3D9* d3d = p_Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d)
	{
		*out_error = "Direct3DCreate9 failed";
		return nullptr;
	}
	
	D3DDISPLAYMODE display_mode;
	if (FAILED(d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &display_mode)))
	{
		*out_error = "GetAdapterDisplayMode failed";
		return nullptr;
	}
	
	D3DPRESENT_PARAMETERS present_params = {0};
	present_params.Windowed = TRUE;
	present_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	present_params.BackBufferFormat = display_mode.Format;

	WNDCLASSEX wnd_class = {0};
	wnd_class.cbSize = sizeof(wnd_class);
	wnd_class.style = CS_HREDRAW | CS_VREDRAW;
	wnd_class.lpfnWndProc = DefWindowProc;
	wnd_class.lpszClassName = TEXT("DummyWindowClass");
	wnd_class.hInstance = GetModuleHandle(NULL);
	
	if (!RegisterClassEx(&wnd_class))
	{
		*out_error = "Failed to register temp window class";
		return nullptr;
	}

	const HWND temp_window = CreateWindow(
		wnd_class.lpszClassName, TEXT("Dummy window"), WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, NULL, NULL, wnd_class.hInstance, NULL
	);
	if (!temp_window)
	{
		*out_error = "Failed to create temp window";
		UnregisterClass(wnd_class.lpszClassName, NULL);
		return nullptr;
	}

	IDirect3DDevice9* d3d_device;
	HRESULT err = d3d->CreateDevice(
		D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, temp_window,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&present_params, &d3d_device
	);

	// Device creation can fail when `present_params.Windowed` does not match the game's windowed state.
	// Let's try again with the opposite value.
	if (FAILED(err))
	{
		present_params.Windowed = !present_params.Windowed;
		err = d3d->CreateDevice(
			D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, temp_window,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&present_params, &d3d_device
		);
	}

	DestroyWindow(temp_window);
	UnregisterClass(wnd_class.lpszClassName, NULL);

	void** vtable = nullptr;

	if (SUCCEEDED(err))
		vtable = *reinterpret_cast<void***>(d3d_device);
	else
	{
		*out_error = "CreateDevice error code: " + std::to_string(err);
		return nullptr;
	}

	// Intentionally leak the D3D device...
	// During game startup, releasing it can cause the D3D code to be unloaded or moved.
	// MinHook will throw MH_ERROR_NOT_EXECUTABLE:
	//   "The specified pointer is invalid. It points the address of non-allocated
	//   and/or non-executable region."
	
	//d3d_device->Release();
	//d3d->Release();
	return vtable;
}