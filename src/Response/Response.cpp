#include "Response.hpp"

Response::Response(const Request &request, int status) : _request(request), _status(status){
	_config = ConfigServer::getInstance();

	if (status != 200) {
		//send error
	}
	else {
		//send file
	}
}

std::string Response::getLocation() {
	std::string	location;

	// check if path has a slash
	if (_request._startLine.path.find_first_of('/', 1) == std::string::npos)
		location = _request._startLine.path;
	else
		location = _request._startLine.path.substr(0, _request._startLine.path.find_first_of('/', 1));

	return location;
}

void 		Response::setRoot(const std::string &location) {

	try
	{
		_config->_serversData[_request._serverId]._locations.at(location);
		_root = _config->_serversData[_request._serverId]._locations[location].root;
	}
	catch (const std::out_of_range& e)
	{
		(void)e;
	}

	_root = _config->_serversData[_request._serverId]._root[0];
}

std::string	Response::resolvePath() {

	std::string location = getLocation();

	return resolvePath(_request._startLine.path);
}

std::string Response::resolvePath(const std::string &path) {
	std::string resolvedPath = "Mock return" ;
	std::string file;
	std::string location;

	location = "/" + _root.substr(0, _root.find_first_of('/'));
	std::cout << BOLD_BLUE << "location: " << location << RESET << std::endl;
	_root = path;

	if (hasExtension(path))
	{
		removeFile(_root, file);
		if (existInLocation(_root))
		{
			setLocalRoot(_root);
			if (localFileExist(file))
			{
				resolvedPath = _root + file;
			}
			else
			{
				//resolve error pages 404
			}
		}
		else
		{
			_root = _config->_serversData[_request._serverId]._root[0];
			if (localFileExist(file))
			{
				resolvedPath = _root + file;
				//send response file
			}
			else
			{
				//resolve error pages 404
			}
		}
	}
	else //no extension
	{
		if (existInLocation(_root))
		{
			setLocalRoot(_root);
			resolvedPath = _root + _config->_serversData[_request._serverId]._locations[_root].index[0];
		}
		else
		{
			if(localRootExist(_root))
			{
				resolvedPath = _root + _config->_serversData[_request._serverId]._locations[_root].index[0];
			}
			else
			{
				//resolve error pages 500
			}
		}
	}

	return resolvedPath;
}

bool Response::hasExtension(const std::string &path) {
	if (path.find('.') != std::string::npos && path.find('.') != path.size() - 1)
		return true;
	return false;
}

void Response::removeFile(std::string &path, std::string &file) {
	size_t pos = path.find_last_of('/');
	file = path.substr(pos + 1);
	path = path.substr(0, pos + 1);
}

bool Response::existInLocation(const std::string &path) {

	try { _config->_serversData.at(_request._serverId)._locations.at(path); }
	catch (std::out_of_range &e) { (void)e; return false; }
	return true;
}

bool Response::localRootExist(const std::string path) {
	(void)path;
	return localFileExist("index.html");
	return localFileExist(_config->_serversData.at(_request._serverId)._index[0]);
}

bool Response::localFileExist(const std::string &file) {

	std::string path = _root +  "/" + file;
	std::ifstream fd(path.c_str());

	if (fd)
	{
		fd.close();
		return true;
	}
	return false;
}

void Response::setLocalRoot(const std::string path) {
	_root = _config->_serversData.at(_request._serverId)._locations.at(path).root;
}

void Response::resolveErrorPages(const int status) {
	_status = status;


}


