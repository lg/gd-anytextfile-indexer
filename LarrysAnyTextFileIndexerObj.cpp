// LarrysAnyTextFileIndexerObj.cpp : Implementation of CLarrysAnyTextFileIndexerObj

#include "stdafx.h"
#include "LarrysAnyTextFileIndexerObj.h"
#include ".\larrysanytextfileindexerobj.h"

#include <string>
#include <vector>
#include <fstream>

using namespace std;

const CLSID PluginClassID = {0xDC35EA48,0x208A,0x4776,{0xAC,0xF3,0x35,0xB0,0xD9,0xCF,0xCA,0x42}};
LarGDSPlugin GDSPlugin2(PluginClassID);

STDMETHODIMP CLarrysAnyTextFileIndexerObj::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_ILarrysAnyTextFileIndexerObj
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CLarrysAnyTextFileIndexerObj::HandleFile(BSTR RawFullPath, IDispatch* EventFactory)
{
	string FullPath = CW2A(RawFullPath);

	// Open File
	ifstream CheckFile(FullPath.c_str());
	if (!CheckFile) { return S_FALSE; }

	// Read entire file into Text
	std::istreambuf_iterator<char> beg(CheckFile), end;
	std::string Text(beg, end);
	CheckFile.close(); 

	// Get modification date on file
	HANDLE FileHandle;
	FileHandle = CreateFile(FullPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

	FILETIME *ModifiedTime = (FILETIME *)malloc(sizeof FILETIME);
	BOOL FileTimeResult = GetFileTime(FileHandle, 0, 0, ModifiedTime);
	CloseHandle(FileHandle);
	if (!FileTimeResult) {
        return S_FALSE; 
	}

	// Format modification date properly
	SYSTEMTIME SystemTime;
	FileTimeToSystemTime(ModifiedTime, &SystemTime);
	
	// Index file
	if (!GDSPlugin2.SendTextFileEvent(Text, FullPath, SystemTime)) {
		return S_FALSE;
	}

	return S_OK;
}
