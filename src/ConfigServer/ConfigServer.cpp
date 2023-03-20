#include "ConfigServer.hpp"

ConfigServer::ConfigServer()
	: _goodFile(true)
{
}

ConfigServer::ConfigServer(const ConfigServer &config)
	: _goodFile(true)
{
	*this = config;
}

ConfigServer::ConfigServer(const std::string file)
{
	std::string stringFile = "";
	// std::cout << "File: " << file << std::endl;
	if (!readFile(file, stringFile))
	{
		_goodFile = false;
		throw std::runtime_error("Couldn't read!");
	}
	else
	{
		// for (int i = 0; i < (int)stringFile.length(); i++)
		// 	std::cout << stringFile[i];
		_goodFile = true;
		std::vector<std::string> serverBlocks = getServerBlocks(stringFile);
		printBLocks(serverBlocks);
	}
}

void ConfigServer::printBLocks(std::vector<std::string> &serverBlocks)
{
	for (std::vector<std::string>::const_iterator it = serverBlocks.begin(); it != serverBlocks.end(); ++it)
	{
		//     // Process each server block here
		std::cout << YELLOW << "Server block: " << RESET << *it << std::endl
				  << std::endl;
		std::cout << YELLOW << "Host: " << RESET << getHost(*it) << std::endl;
		std::cout << YELLOW << "Port: " << RESET << getPort(*it) << std::endl;
		std::cout << YELLOW << "Server name: " << RESET << getKeywordValue(*it, "server_name") << std::endl;
		std::cout << YELLOW << "Client Size: " << RESET << getKeywordValue(*it, "client_size") << std::endl
				  << std::endl;
		std::vector<std::string> locationBlocks = getLocationBlocks(*it);
		for (std::vector<std::string>::const_iterator itr = locationBlocks.begin(); itr != locationBlocks.end(); ++itr)
		{
			//     // Process each server block here
			std::cout << YELLOW << "Location block: " << RESET << *itr << std::endl;
		}
	}
}

ConfigServer::~ConfigServer()
{
}

ConfigServer &ConfigServer::operator=(const ConfigServer &config)
{
	if (this != &config)
	{
		// _servers = config._servers;
		_goodFile = config._goodFile;
	}
	return (*this);
}

/**
 * @brief Check if the file exists
 *
 * @param file The path to the file

	* @return return the givent file Path if it works otherwise throw an error.
 */
std::string getFile(int argc, const char **argv)
{
	std::string configFile = "configFiles/default.config";
	if (argc == 2)
	{
		configFile = argv[1];
		std::ifstream f(configFile.c_str());
		if (!f.good())
			throw std::runtime_error("File not found!");
	}
	else
	{
		std::ifstream f(configFile.c_str());
		if (!f.good())
			throw std::runtime_error("Default config file not found!");
	}
	return (configFile);
}

/**
 * @brief Check if the line is neeeded and not just a comment or just spaces
 *
 * @param file The path to the file
 * @return true if not just a comment or just spaces
 * @return false otherwise
 */
bool ConfigServer::lineNeeded(const std::string line)
{
	size_t i = line.find_first_not_of(" \t");
	bool val = (i != std::string::npos && line[i] != '#');
	return val;
}

/**
 * @brief Remove the spaces and comments from the line
 *
 * @param line The line to be modified
 * @return std::string The modified line
 */
std::string ConfigServer::cleanedLine(std::string line)
{
	int i;
	int j;
	const char *str;
	size_t commentPos;

	i = 0;
	j = static_cast<int>(line.length()) - 1;
	str = line.c_str();
	commentPos = line.find("#");
	while (i < j && std::isspace(static_cast<int>(str[i])))
		i++;
	while (j > i && std::isspace(static_cast<int>(str[j])))
		j--;
	if (commentPos != std::string::npos && commentPos > static_cast<size_t>(i))
		j = static_cast<int>(commentPos - 1);
	return (line.substr(i, j - i + 1) + "\n");
}

/**
 * @brief Read the file and store the data in a string
 *
 * @param inFile The path to the file
 * @param stringLine The string to store the data
 * @return true if the file could be open
 * @return false otherwise
 */
bool ConfigServer::readFile(const std::string inFile, std::string &stringLine)
{
	std::ifstream file(inFile.c_str());
	std::string line = "";
	while (std::getline(file, line))
	{
		if (lineNeeded(line))
			stringLine += cleanedLine(line);
	}
	file.close();
	return (true);
}

/**
 * @brief Split configStr where all file is stored into blocks of servers
 *
 * @param configStr the string where readfile() stored the file it read.
 * @return std::vector<std::string>  A vector of strings one by server
 */
std::vector<std::string> ConfigServer::getServerBlocks(const std::string &configStr)
{
	int braceCount;

	std::vector<std::string> serverBlocks;
	std::string::size_type pos = 0;
	while ((pos = configStr.find("server", pos)) != std::string::npos)
	{
		std::string::size_type bracePos = configStr.find("{", pos);
		if (bracePos != std::string::npos)
		{
			std::string::size_type endPos = bracePos + 1;
			braceCount = 1;
			while (braceCount > 0 && endPos < configStr.length())
			{
				if (configStr[endPos] == '{')
					braceCount++;
				else if (configStr[endPos] == '}')
					braceCount--;
				endPos++;
			}
			if (braceCount == 0)
			{
				std::string serverBlock = configStr.substr(pos, endPos - pos);
				serverBlocks.push_back(serverBlock);
				pos = endPos;
				continue;
			}
		}
		break; // Handle error: opening brace not found
	}
	return (serverBlocks);
}

/**
 * @brief Split the server block into a string of location
 *
 * @param configStr the input, should be the sting server block
 * @return std::vector<std::string> the vector of locations.
 */
std::vector<std::string> ConfigServer::getLocationBlocks(const std::string &configStr)
{
	std::vector<std::string> locationBlocks;
	std::string::size_type pos = 0;
	while ((pos = configStr.find("location ", pos)) != std::string::npos)
	{
		std::string::size_type endPos = configStr.find("}", pos);
		if (endPos == std::string::npos)
			break; // Handle error: closing brace not found
		std::string locationBlock = configStr.substr(pos, endPos - pos + 1);
		locationBlocks.push_back(locationBlock);
		pos = endPos + 1;
	}
	return (locationBlocks);
}

std::string ConfigServer::getHost(const std::string &configStr)
{
	std::string host;
	std::string::size_type pos = configStr.find("listen");
	// std::cout << "pos: " << pos << std::endl;
	if (pos != std::string::npos)
	{
		pos += 6; // skip "listen "
		std::string::size_type endPos = configStr.find(":", pos);
		if (endPos != std::string::npos)
		{
			while (pos < endPos && isspace(configStr[pos]))
				pos++;
			host = configStr.substr(pos, endPos - pos);
		}
		else
			host = configStr.substr(pos);
	}
	return (host);
}

/**
 * @brief Get the port from the config string
 *
 * @param configStr The block to get the port from
 * @return std::string the port value
 */
std::string ConfigServer::getPort(const std::string &configStr)
{
	std::string port;
	std::string::size_type pos = configStr.find("listen");
	if (pos != std::string::npos)
	{
		pos += 6; // skip "listen "
		std::string::size_type endPos = configStr.find(";", pos);
		if (endPos != std::string::npos)
		{
			std::string::size_type colonPos = configStr.find(":", pos);
			if (colonPos != std::string::npos && colonPos < endPos)
			{
				port = configStr.substr(colonPos + 1, endPos - colonPos - 1);
			}
		}
	}
	return (port);
}

/**
 * @brief Get the value depeding on the derective from the config string
 *
 * @param configStr The block to get the server name from
 * @return std::string the server name value
 */
std::string ConfigServer::getKeywordValue(const std::string &configStr,
										  const std::string &derective)
{
	std::string server_name;
	std::string::size_type pos = configStr.find(derective);
	if (pos != std::string::npos)
	{
		pos += derective.length(); // skip "server_name "
		std::string::size_type endPos = configStr.find(";", pos);
		if (endPos != std::string::npos)
		{
			while (pos < endPos && isspace(configStr[pos]))
				pos++;
			server_name = configStr.substr(pos, endPos - pos);
		}
		else
			server_name = configStr.substr(pos);
	}
	return (server_name);
}

TEST_CASE("ConfigServer::lineNeeded")
{
	ConfigServer cs;
	SUBCASE("Returns false for an empty line")
	{
		std::string line = "";
		CHECK_FALSE(cs.lineNeeded(line));
	}
	SUBCASE("Returns false for a line with only whitespace")
	{
		std::string line = "   \t\t ";
		CHECK_FALSE(cs.lineNeeded(line));
	}
	SUBCASE("Returns false for a commented line")
	{
		std::string line = "# this is a comment";
		CHECK_FALSE(cs.lineNeeded(line));
	}
	SUBCASE("Returns true for a line with non-whitespace characters")
	{
		std::string line = "listen 127.0.0.1:8080";
		CHECK(cs.lineNeeded(line));
	}
	SUBCASE("Returns true for a line with leading whitespace")
	{
		std::string line = "\t    listen 127.0.0.1:8080";
		CHECK(cs.lineNeeded(line));
	}
}

TEST_CASE("ConfigServer::getFile")
{
	// Should pass
	SUBCASE("valid file provided as command line argument")
	{
		const char *argv[] = {"test", "configFiles/default.config"};
		std::string configFile = getFile(2, argv);
		CHECK(configFile == "configFiles/default.config");
	}

	// Should pass
	SUBCASE("no command line argument, default config file exists")
	{
		const char *argv[] = {"test"};
		std::string configFile = getFile(1, argv);
		CHECK(configFile == "configFiles/default.config");
	}

	// Should fail
	SUBCASE("no command line argument, default config file does not exist")
	{
		const char *argv[] = {"test"};
		std::string configFile = getFile(1, argv);
		CHECK_THROWS(getFile(1, argv));
	}

	// Should pass
	SUBCASE("invalid file provided as command line argument")
	{
		const char *argv[] = {"test", "configFiles/nonexistent.config"};
		CHECK_THROWS(getFile(2, argv));
	}
}

// TEST_CASE("test ConfFIle param ")
// {
// 	ConfigServer config;
// 	ConfigServer config1;

// 	/* Check If file could be open // Should pass */
// 	CHECK(config.fileExists("configFiles/default.config") == 1);

// 	/* Check If file could be open // Should fail */
// 	CHECK(config1.fileExists("configFiles/default.configur") == 1);

// 	ConfigServer ConfigT("configFiles/default.config");
// 	ConfigServer ConfigT1("configFiles/default.configur");
// }
