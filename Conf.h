#include <string>
#include <vector>
#include <map>

class Conf {

private:
	std::map<std::string, std::vector <std::vector <std::string> > > Settings;
		
public:
	bool LoadSettings(const std::string &Filename);
	std::vector<std::string> GetCommandArgs(const std::string &CommandName, const size_t CommandNumber = 0);
	std::string Conf::GetString(const std::string &CommandName, const size_t CommandNumber, const size_t ArgumentNumber, const std::string &DefaultValue = "");
	std::string Conf::GetString(const std::string &CommandName, const std::string &DefaultValue = "");
	int Conf::GetInt(const std::string &CommandName, const size_t CommandNumber, const size_t ArgumentNumber, const int DefaultValue);
	int Conf::GetInt(const std::string &CommandName, const int DefaultValue = 0);
	bool Conf::GetBool(const std::string &CommandName, const size_t CommandNumber, const size_t ArgumentNumber, const bool DefaultValue);
	bool Conf::GetBool(const std::string &CommandName, const bool DefaultValue = false);
	size_t GetCommandCount(const std::string &CommandName);
};
