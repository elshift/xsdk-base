#include "Interfaces.h"
#include "Base.h"
#include "Sig.h"
#include "AsmTools.h"
#include "Wrappers/EngineClientWrappers.h"
#include "Wrappers/ClientDLLWrappers.h"
#include "Wrappers/ClientModeWrappers.h"
#include "Wrappers/EntityListWrappers.h"
#include "Wrappers/EngineVGUIWrappers.h"
#include "Wrappers/EngineTraceWrappers.h"
#include "Wrappers/ModelRenderWrappers.h"
#include "Wrappers/MaterialSystemWrappers.h"
#include "Wrappers/CvarWrappers.h"
#include <SDK/icliententitylist.h>
#include <SDK/IPanel.h>
#include <SDK/vgui_baseui_interface.h>
#include <SDK/ienginetool.h>
#include <SDK/memalloc.h>

#ifdef _WIN64
#define SIG_CLIENTMODE "8B 0D ? ? ? ? 48 8B 01"
#else
#define SIG_CLIENTMODE "8B 0D ? ? ? ? 8B 01"
#endif

class IKeyValuesSystem;
IMemAlloc* g_pMemAlloc;

static CreateInterfaceFn GetFactory(const char* Module)
{
	HMODULE hmod = Base::GetModule(Module);
	return (CreateInterfaceFn)Base::GetProc(hmod, "CreateInterface");
}

static void AssertInterfacePointer(const char* name, void* value) {
	if (!value)
		Base::Fatal("Unsupported game interfaces", "Could not find interface '%s'\n%s:%d", name, __FILE__, __LINE__);
}

void Interfaces::CreateInterfaces()
{
	HMODULE tier0_dll;

	// Wait for game libraries to load
	while (!GetModuleHandleA("client.dll"))
		Sleep(100);
	while (!(tier0_dll = GetModuleHandleA("tier0.dll")))
		Sleep(100);
	
	// Tier0 exports //
	g_pMemAlloc = (IMemAlloc*)GetProcAddress(tier0_dll, "g_pMemAlloc");
	
	// Engine factory //
	CreateInterfaceFn fn = GetFactory("engine.dll");

	if (void* engine13 = fn("VEngineClient013", 0))
		engine = new IEngineClientWrapper013(engine13);
	else if (void* engine14 = fn("VEngineClient014", 0))
		engine = new IEngineClientWrapper014(engine14);

	if (void* trace3 = fn("EngineTraceClient003", 0))
		trace = new IEngineTrace003Wrapper(trace3);
	else if (void* trace4 = fn("EngineTraceClient004", 0))
		trace = new IEngineTrace004Wrapper(trace4);
	
	if (void* engine_tool3 = fn(VENGINETOOL_INTERFACE_VERSION, 0))
		engine_tool = (IEngineTool*)engine_tool3;
	
	if (void* model_render16 = fn("VEngineModel016", 0))
	{
		if (engine->GetAppID() == EAppID::GMOD)
			model_render = new IVModelRenderWrapperGMod(model_render16);
		else
			model_render = new IVModelRenderWrapperSDK(model_render16);
	}

	if (void* vgui1 = fn(VENGINE_VGUI_VERSION, 0))
	{
		if (engine->GetAppID() == EAppID::CSGO)
			vgui = new CEngineVGUIWrapperCSGO(vgui1);
		else
			vgui = new CEngineVGUIWrapperSDK(vgui1);
	} else if (void* vgui2 = fn("VEngineVGui002", 0)) {
		vgui = new CEngineVGUIWrapperTF2(vgui2);
	}

	render_view = (IVRenderView*)fn("VEngineRenderView014", 0);

	// Client factory //
	fn = GetFactory("client.dll");

	if (void* clientdll17 = fn("VClient017", 0))
		hlclient = new IClientDLLWrapper017(clientdll17);
	else if (void* clientdll18 = fn("VClient018", 0))
		hlclient = new IClientDLLWrapper018(clientdll18);
	if (void* entitylist = fn(VCLIENTENTITYLIST_INTERFACE_VERSION, 0))
		entlist = new IClientEntityListWrapper003(entitylist);

	// VGUI factory //
	fn = GetFactory("vgui2.dll");

	if (void* vguipanel = fn(VGUI_PANEL_INTERFACE_VERSION, 0))
		panels = (vgui::IPanel*)vguipanel;
	
	// MaterialSystem factory
	fn = GetFactory("materialsystem.dll");

	if (void* matsystem80 = fn(MATERIAL_SYSTEM_INTERFACE_VERSION, 0))
	{
		if (engine->GetAppID() == EAppID::GMOD)
			mat_system = new IMaterialSystemWrapperGMod(matsystem80);
		else
			mat_system = new IMaterialSystemWrapperSDK(matsystem80);
	}
	else if (void* matsystem81 = fn("VMaterialSystem081", 0))
		mat_system = new IMaterialSystemWrapper081(matsystem81);

	// Studio render //
	fn = GetFactory("studiorender.dll");
	studio_render = (IStudioRender*)fn(STUDIO_RENDER_INTERFACE_VERSION, 0);

	// vstdlib //
	fn = GetFactory("vstdlib.dll");
	if (void* cvar4 = fn("VEngineCvar004", 0))
		cvar = new ICvarWrapperSDK(cvar4);
	else if (void* cvar7 = fn("VEngineCvar007", 0))
		cvar = new ICvarWrapper007(cvar7);

	AssertInterfacePointer("IEngineClient", engine);
	AssertInterfacePointer("IClientDLL", hlclient);
	AssertInterfacePointer("IClientEntityList", entlist);
	AssertInterfacePointer("IPanel", panels);
	AssertInterfacePointer("CEngineVGUI", vgui);
	AssertInterfacePointer("IEngineTrace", trace);
	AssertInterfacePointer("IStudioRender", studio_render);
	AssertInterfacePointer("IVModelRender", model_render);
	AssertInterfacePointer("CMaterialSystem", mat_system);
	AssertInterfacePointer("IVRenderView", render_view);
	AssertInterfacePointer("ICvar", cvar);

	// Misc //

	void* hudprocinput = (*(void***)hlclient->Inst())[10];
	void* clientref = Sig::FindPattern<void*>(hudprocinput, 0x10, SIG_CLIENTMODE);
	if (!clientref)
		FATAL("Failed signature to g_pClientModeShared");

	void* volatile* pp_clientmode = AsmTools::Relative<void**>(clientref, 2);
	while (!*pp_clientmode)
		Sleep(100); // Wait for clientmode to be to be initialized
	
	void* clientmode = *pp_clientmode;
	switch (engine->GetAppID())
	{
	case static_cast<int>(EAppID::CSGO):
		client = new IClientModeWrapperCSGO(clientmode);
		break;
	default:
		client = new IClientModeWrapperSDK(clientmode);
	}
}

void Interfaces::DestroyInterfaces()
{
	delete engine;
	delete hlclient;
	delete entlist;
	delete client;
	delete trace;
	delete vgui;
	delete mat_system;
	delete model_render;
	delete cvar;
	engine = nullptr;
	hlclient = nullptr;
	entlist = nullptr;
	client = nullptr;
	trace = nullptr;
	vgui = nullptr;
	mat_system = nullptr;
	model_render = nullptr;
	cvar = nullptr;
}
