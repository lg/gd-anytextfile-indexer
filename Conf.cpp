#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

#include "conf.h"

using namespace std;

// LoadSettings - Parses through the given filename for commands and stores them inside of the
//                Settings vector. Returns true if the file was found and loaded, or false if
//                there was an error accessing it.
bool Conf::LoadSettings(const string &Filename) {
	ifstream ConfFile(Filename.c_str());
	if (!ConfFile) { return false; }

	vector<string> Arguments;
	string CurCommandName = "";
	string CurArg = "";
	size_t QuoteStartIndex = 0;
	size_t QuoteEndIndex = 0;

	Settings.clear();
    
	char RawDataBuffer[256];
	string DataBuffer;
	while (!ConfFile.eof()) {
		ConfFile.getline(RawDataBuffer, 256);
		DataBuffer = RawDataBuffer;

		if ((DataBuffer.substr(0, 1) != "#") && (DataBuffer.substr(0, 1) != "")) {
			CurCommandName = DataBuffer.substr(0, DataBuffer.find(" "));

			Arguments.clear();
			QuoteStartIndex = DataBuffer.find("\"", 0);
			QuoteEndIndex = DataBuffer.find("\"", QuoteStartIndex + 1);
			while ((QuoteStartIndex != DataBuffer.npos) && (QuoteEndIndex != DataBuffer.npos)) {
				CurArg = DataBuffer.substr(QuoteStartIndex + 1, QuoteEndIndex - QuoteStartIndex - 1);

				Arguments.push_back(CurArg);
                
				QuoteStartIndex = DataBuffer.find("\"", QuoteEndIndex + 1);
				QuoteEndIndex = DataBuffer.find("\"", QuoteStartIndex + 1);
			}

			Settings[CurCommandName].push_back(Arguments);
		}
	}

	ConfFile.close();

	return true;
}

// GetCommandArgs - Returns a string vector of all the aruments of the specified command and number.
//                  The CommandName is the command name that was used in the .conf file. The 
//					CommandNumber is the command's number when the same command is used multiple
//                  times. For example, if the AddRealm command is used twice in the .conf file,
//                  CommandNumber being 0 would return the value of the first time the AddRealm
//                  command was called, and CommandNumber being 1 would return the value of the 
//                  second time the AddRealm command was called. CommandNumber's index starts at 0.
vector<string> Conf::GetCommandArgs(const string &CommandName, const size_t CommandNumber) {
	vector <vector <string> > Setting = Settings[CommandName];
	
	if (Setting.size() >= CommandNumber + 1) {
		return Setting[CommandNumber];
	}

	return vector <string>();
}

// GetCommandString - Returns a string of one argument of any command in the .conf file. See the
//                    description of GetCommandArgs for a description of the CommandNumber arg. 
//                    ArgumentNumber is the argument number of the command. It starts indexing at 0.
//                    If the specified command/command number/argument wasn't found, the function
//                    return the DefaultValue. CommandNumber, ArgumentNumber and DefaultValue are all
//                    optional.
string Conf::GetString(const string &CommandName, const size_t CommandNumber, const size_t ArgumentNumber, const string &DefaultValue) {
	vector <vector <string> > Setting = Settings[CommandName];
	
	if ((Setting.size() >= CommandNumber + 1) && (Setting[CommandNumber].size() >= ArgumentNumber + 1)) {
		return Setting[CommandNumber][ArgumentNumber];
	}

	return DefaultValue;
}
string Conf::GetString(const string &CommandName, const string &DefaultValue) {
	return GetString(CommandName, 0, 0, DefaultValue);
}

// GetCommandInt - Returns an integer of one argument of any command in the .conf file. See
//                 description of GetCommandString for an explanation of parameters.
int Conf::GetInt(const string &CommandName, const size_t CommandNumber, const size_t ArgumentNumber, const int DefaultValue) {
	string CommandString = "";

	CommandString = GetString(CommandName, CommandNumber, ArgumentNumber, "");
	if (CommandString == "") {
		return DefaultValue;
	}

	return atoi(CommandString.c_str());
}
int Conf::GetInt(const string &CommandName, const int DefaultValue) {
	return GetInt(CommandName, 0, 0, DefaultValue);
}

// GetCommandBool - Returns a bool of one argument of any command in the .conf file. See
//                  description of GetCOmmandString for an explanation of parameters.
bool Conf::GetBool(const string &CommandName, const size_t CommandNumber, const size_t ArgumentNumber, const bool DefaultValue) {
	string CommandString = "";
	string SendDefaultValue = "";

	if (DefaultValue == true) { SendDefaultValue = "true"; } else { SendDefaultValue = "false"; }

	CommandString = GetString(CommandName, CommandNumber, ArgumentNumber, SendDefaultValue);
	
	if ((CommandString == "true") || (CommandString == "1") || (CommandString == "yes")) {
		return true;
	} else if ((CommandString == "false") || (CommandString == "0") || (CommandString == "no")) {
		return false;
	}
	
	return DefaultValue;
}
bool Conf::GetBool(const string &CommandName, const bool DefaultValue) {
	return GetBool(CommandName, 0, 0, DefaultValue);
}


// GetCommandCount - Returns the amount of times a certain command was used in the .conf file.
//                   For example, if you call AddRealm twice from the .conf, this function will
//                   return the number 2. Index starts at 1.
size_t Conf::GetCommandCount(const string &CommandName) {
	return Settings[CommandName].size();
}