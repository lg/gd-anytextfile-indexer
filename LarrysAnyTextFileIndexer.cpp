// LarrysAnyTextFileIndexer.cpp : Implementation of DLL Exports.

#include "stdafx.h"
#include "resource.h"
#include "LarrysAnyTextFileIndexer.h"
#include "LarrysAnyTextFileIndexerObj.h"

#include "Conf.h"
#include <string>
#include <vector>

const CLSID PluginClassID = {0xDC35EA48,0x208A,0x4776,{0xAC,0xF3,0x35,0xB0,0xD9,0xCF,0xCA,0x42}};
							
LarGDSPlugin GDSPlugin(PluginClassID);
HINSTANCE Instance;

class CLarrysAnyTextFileIndexerModule : public CAtlDllModuleT< CLarrysAnyTextFileIndexerModule >
{
public :
	DECLARE_LIBID(LIBID_LarrysAnyTextFileIndexerLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_LARRYSANYTEXTFILEINDEXER, "{31C56A86-A385-4ADF-A911-F857E022A002}")
};

CLarrysAnyTextFileIndexerModule _AtlModule;


// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	Instance = hInstance;
	hInstance;
    return _AtlModule.DllMain(dwReason, lpReserved); 
}


// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
    return _AtlModule.DllCanUnloadNow();
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
	// registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer();
	if (FAILED(hr)) { 
		return hr;
	}

	// Get the current folder and open the .conf file
	Conf ConfFile;
	vector<string> FileTypes;
    
	char ModuleName[255];
    ::GetModuleFileName(Instance, ModuleName, 254);

    std::string Filename(ModuleName);
    size_t LastSlash;

    LastSlash = Filename.find_last_of(".");
    Filename.erase(LastSlash);
    Filename = Filename + ".conf";

    FileTypes.clear();
    if (!ConfFile.LoadSettings(Filename.c_str())) {
		MessageBox(0, string("Error: Failed to open '" + Filename + "'. Make sure the file exists and that it is not in use.").c_str(), "Larry's Any Text File Indexer", MB_ICONEXCLAMATION);
        return -1;
    } 
	
	// Loop through all extensions and make a vector out of all of them
	unsigned int TotalAddNewExtension;
    string CurExtensionName;
	vector<string> Extensions;
    
    TotalAddNewExtension = (unsigned int)ConfFile.GetCommandCount("AddNewExtension");

    for (unsigned int CurANE = 0; CurANE <= TotalAddNewExtension - 1; CurANE++) {
        CurExtensionName = ConfFile.GetString("AddNewExtension", CurANE, 0, "");
        
		Extensions.push_back(CurExtensionName);
    }

	// Register plugin and extensions
	bool RegisterResult;
	RegisterResult = GDSPlugin.RegisterPluginWithExtensions("Larry's Any Text File Indexer 1.00 by Larry Gadea", "Index any file extension specified as plaintext", "", Extensions);

	if (!RegisterResult) {
		if (Extensions.size() > 0) {
			string FailedExtensionsList;
			for (size_t CurExtension = 0; CurExtension <= Extensions.size() - 1; CurExtension++) {
				if (CurExtension > 0) {
					FailedExtensionsList += ", ";
				}

				FailedExtensionsList += Extensions[CurExtension].c_str();
			}

			MessageBox(0, string(string("Failed to register with GDS because GDS rejected the following extensions:\n\n") + FailedExtensionsList).c_str(), "Larry's Any Text File Indexer", MB_ICONEXCLAMATION);	
		} else {
			MessageBox(0, "Failed to register with GDS.", "Larry's Any Text File Indexer", MB_ICONEXCLAMATION);	
		}

		GDSPlugin.UnregisterPlugin();
		_AtlModule.DllUnregisterServer();

		return false;
	}

	return true;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();

	if (SUCCEEDED(hr)) {
		GDSPlugin.UnregisterPlugin();
	}

	return hr;
}
