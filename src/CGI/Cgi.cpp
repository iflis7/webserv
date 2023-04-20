#include "Cgi.hpp"
#include <unistd.h>

char **vectorToChar(std::vector<std::string> &vec) {
	char **ret = new char*[vec.size() + 1];
	for (size_t i = 0; i < vec.size(); i++) {
		ret[i] = strdup(vec[i].c_str());
	}
	ret[vec.size()] = NULL;
	return ret;
}

CGI::CGI(Request &request) :	_request(request),
								_serverData(ConfigServer::getInstance()->getServerData()[_request._serverId]) {
	_setEnv();
}

std::string CGI::_getRequestMethod() {	Log::debugFunc(__FUNCTION__);

	if (_request._startLine.type == GET)
		return "GET";
	else if (_request._startLine.type == POST)
		return "POST";
	else if (_request._startLine.type == DELETE)
		return "DELETE";
	else
		return "UNKNOWN";
}

std::string	CGI::_getScriptName() {	Log::debugFunc(__FUNCTION__);

	return _request._startLine.path;
}

void CGI::_setEnv() { Log::debugFunc(__FUNCTION__);

	_env.push_back("SERVER_SOFTWARE=webserv/1.0");
//	_env.push_back("SERVER_NAME=" + _serverData._serverNames[0]); TODO: error
	_env.push_back("GATEWAY_INTERFACE=CGI/1.1");

	_env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	_env.push_back("SERVER_PORT=" + std::to_string(_serverData._serverPorts[0]));
	_env.push_back("REQUEST_METHOD=" + _getRequestMethod());
	_env.push_back("PATH_INFO=");		// TODO: understand what is path info
	_env.push_back("SCRIPT_NAME=" + _getScriptName());
	_env.push_back("QUERY_STRING=" + _request._startLine.queryString);


	_env.push_back("CONTENT_TYPE=");	// TODO: function to get content type from request
	_env.push_back("CONTENT_LENGTH=");	// TODO: function to get content length from request

	_env.push_back("HTTP_ACCEPT=");		// TODO: function to get accept from request
	_env.push_back("HTTP_USER_AGENT=");	// TODO: function to get user agent from request
	_env.push_back("HTTP_COOKIE=");		// TODO: function to get cookie from request
}

void	CGI::executeCgi() { Log::debugFunc(__FUNCTION__);

	char *pathToCgi = strdup("data/cgi/CGI.py");
	char *args[] = {pathToCgi, NULL};

	pid_t pid = fork();


	if (pid == -1) {
		Log::log(Log::ERROR,"Error forking process.");
		_request._status = 500;
	}
	else if (pid == 0) {

		Log::log(Log::DEBUG,"Executing CGI script.");

		dup2(_request._cgiFd[PIPE_WRITE], 1);
		close(_request._cgiFd[PIPE_READ]);

		char **_envp = vectorToChar(_env);

		execve(args[0], args, _envp);
		Log::log(Log::ERROR,"Error executing CGI script.");
		close(_request._cgiFd[PIPE_WRITE]);
		_request._status = 500;
		exit(1);
	}
}

CGI::~CGI() {
}
