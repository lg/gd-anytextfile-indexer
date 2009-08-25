// LarrysAnyTextFileIndexerObj.h : Declaration of the CLarrysAnyTextFileIndexerObj

#pragma once
#include "resource.h"       // main symbols

#include "LarrysAnyTextFileIndexer.h"


// CLarrysAnyTextFileIndexerObj

class ATL_NO_VTABLE CLarrysAnyTextFileIndexerObj : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CLarrysAnyTextFileIndexerObj, &CLSID_LarrysAnyTextFileIndexerObj>,
	public ISupportErrorInfo,
	public IDispatchImpl<ILarrysAnyTextFileIndexerObj, &IID_ILarrysAnyTextFileIndexerObj, &LIBID_LarrysAnyTextFileIndexerLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CLarrysAnyTextFileIndexerObj()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_LARRYSANYTEXTFILEINDEXEROBJ)


BEGIN_COM_MAP(CLarrysAnyTextFileIndexerObj)
	COM_INTERFACE_ENTRY(ILarrysAnyTextFileIndexerObj)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

public:

	STDMETHOD(HandleFile)(BSTR RawFullPath, IDispatch* EventFactory);
};

OBJECT_ENTRY_AUTO(__uuidof(LarrysAnyTextFileIndexerObj), CLarrysAnyTextFileIndexerObj)
