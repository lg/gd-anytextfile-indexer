// LarGDSPlugin 1.01 by Larry Gadea
// For the latest version of LarGDSPlugin, please visit: http://www.trivex.net
// decide between SYSTEMTIME and double...

#include "LarGDSPlugin.h"

using namespace std;

LarGDSPlugin::LarGDSPlugin(CLSID ClassID) {
	ObjClassID = ClassID;
}
 
bool LarGDSPlugin::RegisterPlugin(string Title, string Description, string IconPath) {
	return RegisterPluginWithExtensions(Title, Description, IconPath, vector <string>());
}

bool LarGDSPlugin::RegisterPluginWithExtensions(string Title, string Description, string IconPath, vector<string> &Extensions) {
	vector<string> FailedExtensions;

	// Initialize COM (if required)
    HRESULT ComInitResult = CoInitialize(NULL);
	if (FAILED(ComInitResult) && ComInitResult != RPC_E_CHANGED_MODE) {
		Extensions = vector<string> ();
		return false;
	}

	// Create connection with GDS
	CComPtr<IGoogleDesktopSearchComponentRegistration> spRegistration;
	CComPtr<IGoogleDesktopSearchComponentRegister> spRegister;
	HRESULT hr;
	hr = spRegister.CoCreateInstance(CLSID_GoogleDesktopSearchRegister);
  
	if (SUCCEEDED(hr)) {
		ATLASSERT(spRegister != NULL);
    
		// Component description is 6 strings
		CComSafeArray<VARIANT> arr_descr(6);

		arr_descr.SetAt(0, CComVariant(L"Title"));
		arr_descr.SetAt(1, CComVariant(Title.c_str()));
		arr_descr.SetAt(2, CComVariant(L"Description"));
		arr_descr.SetAt(3, CComVariant(Description.c_str()));
		arr_descr.SetAt(4, CComVariant(L"Icon"));
		arr_descr.SetAt(5, CComVariant(IconPath.c_str()));

		// our CLSID in string format
		CComBSTR clsid(ObjClassID);
    
		// Wrap description array in variant
		CComVariant descr(arr_descr.m_psa);
		
		// and register
		hr = spRegister->RegisterComponent(clsid, descr, &spRegistration);
		if (FAILED(hr)) {
			Extensions = vector<string> ();
			return false;
		}

		ATLASSERT(FAILED(hr) || spRegistration);

		bool Failed = false;
		for (int CurExtension = 0; ((CurExtension <= (int)(Extensions.size() - 1))); CurExtension++) {
			hr = spRegistration->RegisterExtension(CComBSTR(Extensions[CurExtension].c_str()));

			if (FAILED(hr)) {
				FailedExtensions.push_back(Extensions[CurExtension].c_str());
				Failed = true;
			}
		}

		// revoke our registration in case any of our extensions fail to register
		if (Failed) {
			// ignore the error - we've already failed
			HRESULT ignore;
            ignore = spRegister->UnregisterComponent(CComBSTR(clsid));

			// Uninitialize COM (if required)
			if (SUCCEEDED(ComInitResult)) {
				CoUninitialize();
			}

			Extensions = FailedExtensions;
			return false;
		}
	}

	// Uninitialize COM (if required)
	if (SUCCEEDED(ComInitResult)) {
		CoUninitialize();
	}

	Extensions = vector<string> ();
	return true;
}

bool LarGDSPlugin::UnregisterPlugin() {
	// Initialize COM (if required)
	HRESULT ComInitResult = CoInitialize(NULL);
	if (FAILED(ComInitResult) && ComInitResult != RPC_E_CHANGED_MODE) {
		return false;
	}

	// Connect to GDS
    CComPtr<IGoogleDesktopSearchComponentRegister> spRegister;
	HRESULT hr;
    hr = spRegister.CoCreateInstance(CLSID_GoogleDesktopSearchRegister);
  
	if (SUCCEEDED(hr)) { 
		// our CLSID in string format
		CComBSTR bstrClsid(ObjClassID);
  
		hr = spRegister->UnregisterComponent(bstrClsid);
	}

    // Uninitialize COM (if required)
	if (SUCCEEDED(ComInitResult)) {
		CoUninitialize();
	}

	return true;
}

bool LarGDSPlugin::SendIMEvent(string Content, string UserName, string BuddyName, string Title, unsigned long ConversationID) {
	SYSTEMTIME TimeNow;
	GetSystemTime(&TimeNow);
	
	return SendIMEvent(Content, UserName, BuddyName, Title, ConversationID, TimeNow, "");
}

bool LarGDSPlugin::SendIMEvent(string Content, string UserName, string BuddyName, string Title, unsigned long ConversationID, SYSTEMTIME MessageTime, string Format) {
	// Convert MessageTime
	double VarMessageTime;
	SystemTimeToVariantTime(&MessageTime, &VarMessageTime);
	
	// If no format was specified, assume it's text/html
	if (Format == "") {
		Format = "text/html";
	}

	// Init COM
	HRESULT hr_coinit = CoInitialize(NULL);
	if (FAILED(hr_coinit) && hr_coinit != RPC_E_CHANGED_MODE) {
		return false;
	}

	// Assemble event
	{
		CComPtr<IGoogleDesktopSearchEventFactory> spFactory;
		HRESULT hr = spFactory.CoCreateInstance(CLSID_GoogleDesktopSearch, NULL, CLSCTX_INPROC);
		if (FAILED(hr)) {
			return false;
		}

		CComPtr<IDispatch> spEventDisp;
		hr = spFactory->CreateEvent(CComBSTR(ObjClassID), CComBSTR(L"Google.Desktop.IM"), &spEventDisp);
		if (FAILED(hr)) {
			return false;
		}

		CComQIPtr<IGoogleDesktopSearchEvent> spEvent(spEventDisp);
		ATLASSERT(spEventDisp && spEvent);
		if (spEvent == NULL) {
			return false;
		}

		if (SUCCEEDED(hr)) {
			hr = spEvent->AddProperty(CComBSTR(L"message_time"), CComVariant(VarMessageTime, VT_DATE));
		}

		if (SUCCEEDED(hr)) {
			hr = spEvent->AddProperty(CComBSTR(L"format"), CComVariant(Format.c_str()));
		}

		if (SUCCEEDED(hr)) {
			hr = spEvent->AddProperty(CComBSTR(L"content"), CComVariant(Content.c_str()));
		}

		if (SUCCEEDED(hr)) {
			hr = spEvent->AddProperty(CComBSTR(L"buddy_name"), CComVariant(BuddyName.c_str()));
		}

		if (SUCCEEDED(hr)) {
			hr = spEvent->AddProperty(CComBSTR(L"title"), CComVariant(Title.c_str()));
		}

		if (SUCCEEDED(hr)) {
		    hr = spEvent->AddProperty(CComBSTR(L"conversation_id"), CComVariant(ConversationID));
		}

		if (SUCCEEDED(hr)) {
			hr = spEvent->AddProperty(CComBSTR(L"user_name"), CComVariant(UserName.c_str())); 
		}

		if (FAILED(hr)) {
			return false;
		}

		hr = spEvent->Send(EventFlagIndexable);
		if (FAILED(hr)) {
			return false;
		}
	}

	if (SUCCEEDED(hr_coinit)) {
		CoUninitialize();
	}

	return true;
}



bool LarGDSPlugin::SendTextFileEvent(string Content, string Path, SYSTEMTIME LastModified) {
	
	// Convert date to proper format
	double ModifiedTime;
	SystemTimeToVariantTime(&LastModified, &ModifiedTime);

	// Init COM
	HRESULT hr_coinit = CoInitialize(NULL);
	if (FAILED(hr_coinit) && hr_coinit != RPC_E_CHANGED_MODE) {
		return false;
	}

	// Assemble event
	{
		CComPtr<IGoogleDesktopSearchEventFactory> spFactory;
		HRESULT hr = spFactory.CoCreateInstance(CLSID_GoogleDesktopSearch, NULL, CLSCTX_INPROC);
		if (FAILED(hr)) {
			return false;
		}

		CComPtr<IDispatch> spEventDisp;
		hr = spFactory->CreateEvent(CComBSTR(ObjClassID), CComBSTR(L"Google.Desktop.File"), &spEventDisp);
		if (FAILED(hr)) {
			return false;
		}

		CComQIPtr<IGoogleDesktopSearchEvent> spEvent(spEventDisp);
		ATLASSERT(spEventDisp && spEvent);
		if (spEvent == NULL) {
			return false;
		}

		if (SUCCEEDED(hr)) {
			hr = spEvent->AddProperty(CComBSTR(L"format"), CComVariant("text/plain"));
		}

		if (SUCCEEDED(hr)) {
			hr = spEvent->AddProperty(CComBSTR(L"content"), CComVariant(Content.c_str()));
		}

		if (SUCCEEDED(hr)) {
			hr = spEvent->AddProperty(CComBSTR(L"uri"), CComVariant(Path.c_str()));
		}

		if (SUCCEEDED(hr)) {
			hr = spEvent->AddProperty(CComBSTR(L"last_modified_time"), CComVariant(ModifiedTime, VT_DATE));
		}
		
		if (FAILED(hr)) {
			return false;
		}

		hr = spEvent->Send(EventFlagIndexable);
		if (FAILED(hr)) {
			return false;
		}
	}

	// Uninit COM
	if (SUCCEEDED(hr_coinit)) {
		CoUninitialize();
	}

	return true;
}
unsigned long LarGDSPlugin::GetConversationID(string Identifier) {
	// Default to a timeout of 15 minutes.
	return GetConversationID(Identifier, 900); 
}
unsigned long LarGDSPlugin::GetConversationID(string Identifier, int Timeout) {
	bool CreateNewConversationID = false;

	// Check if there are any conversations with OtherPartyName in memory
	if (Conversations.find(Identifier) == Conversations.end()) {
		// If no, flag for new ConversationID creation

		CreateNewConversationID = true;
	}

	if (CreateNewConversationID == false) {
		// Check to see if nothing has been said within the last 15 minutes

		if (difftime(time(0), Conversations[Identifier].LastMessageTime) > Timeout) {
			// If it has been longer than 15 minutes, flag for new ConversationID creation

			CreateNewConversationID = true;
		}
	}

	if (CreateNewConversationID == true) {
		// If flagged to create a new ConversationID, randomize it.

        srand(static_cast<unsigned>(time(0)));
		ConversationStruct Conversation;
		Conversation.ConversationID = (unsigned long) rand();
		Conversations[Identifier] = Conversation;
	}

	Conversations[Identifier].LastMessageTime = time(0);
	
	return Conversations[Identifier].ConversationID;
}

