#include "Response.hpp"

Response::Response(){}

Response::Response(Request requestClient, std::vector<Server> configParsed): statusCode(-1), stringJoinedResponse(std::string()), \
clientRequest(requestClient), serverConfigData(configParsed)
{}

Response::~Response()
{}

bool	Response::compareStrings(std::string first, std::string second)
{
	return (first.compare(second));
}

Server	Response::findVirtualServer()
{
	int	defaultServerIndex = 0;
	std::vector<std::string> hostPort = ft_splitSpace(((clientRequest.getHttpHeaders()).find("Host"))->second, ':');
	if (hostPort.size() == 1)
		hostPort.push_back("80");
	for(int i = 0; i < serverConfigData.size(); i++)
	{
		if(!(compareStrings((serverConfigData[i]).get_listen(), hostPort[1])))
		{
			if(((serverConfigData[i]).get_listen()) != ((serverConfigData[i + 1]).get_listen()))
				defaultServerIndex = i;
			if (!(compareStrings((serverConfigData[i]).get_host(), hostPort[0])))
					return serverConfigData[i];
			for (std::map<std::string, std::string>::iterator it = (serverConfigData[i].get_server_names()).begin(); it != (serverConfigData[i].get_server_names()).end(); it++)
				if(!(compareStrings(it->first, hostPort[0])))
					return serverConfigData[i];
		}
	}
	return serverConfigData[defaultServerIndex];
}

Location	Response::findLocation()
{
	size_t		pos = 0;
	Location	location;
	for(std::map<std::string, Location>::iterator it = virtualServer.get_map_loc().begin(); it != virtualServer.get_map_loc().end(); it++)
	{
		if (!(it->first.compare("/")))
			location = it->second;
		if(!(clientRequest.getPath().compare(it->first)))
			return it->second;
	}
	return location;
}

bool	Response::allowedMethods(){
	for (int i = 0; i < location.get_methods().size(); i++){
		if (!(location.get_methods().at(i).compare(clientRequest.getMethod())))
			return true;
	}
	return false;
}

bool	Response::findFile(std::string &filename){
	std::ifstream f(filename.c_str());
	return f.good();
}

int     Response::findFileRequested()
{
	this->virtualServer = this->findVirtualServer();
	this->filePath = virtualServer.get_root() + '/' + clientRequest.getPath();
	this->location = this->findLocation();
	std::cout << location.get_path() << std::endl;
	if(allowedMethods())
	{
		// if(findFile(filePath))
		// {
			if (location.get_return().empty())
				return (OK);
			return (MOVEDPERMANENTLY);
		// }
		// else
			// return (NOTFOUND);
	}
	else
		return (FORBIDDEN);
}

std::string	&Response::buildResponse(){
	this->statusCode = findFileRequested();
	if (this->statusCode == OK)
	{
		stringJoinedResponse = indexFound();
	}
	else if (this->statusCode == NOTFOUND)
	{
	    stringJoinedResponse = indexNotFound();
	}
	else if (this->statusCode == FORBIDDEN)
	{
	    stringJoinedResponse = indexForbidden();
	}
	else if (this->statusCode == MOVEDPERMANENTLY)
		stringJoinedResponse = indexMovedPermanently();
	return stringJoinedResponse;
}

std::string Response::indexFound(){
	std::ifstream	indexFile;
	std::string		str;
	std::string		htmlString;
	indexFile.open("requestResponse/index.html");
	stringJoinedResponse += (clientRequest.getRequestLine())[2]; 
	stringJoinedResponse += " 200 OK \n";
	while(std::getline(indexFile, str))
		htmlString += str;
	stringJoinedResponse += "Content-Length: ";
	stringJoinedResponse += std::to_string(htmlString.length());
	stringJoinedResponse += "\n";
	stringJoinedResponse += "Connection: close\n";
	stringJoinedResponse += "Content-Type: text/html\n\n";
	stringJoinedResponse += htmlString;
	return stringJoinedResponse;
}

std::string Response::indexNotFound(){
	std::ifstream	indexFile;
	std::string		str;
	std::string		htmlString;
	indexFile.open("requestResponse/notFound.html");
	stringJoinedResponse += (clientRequest.getRequestLine())[2]; 
	stringJoinedResponse += " 404 NOTFOUND \n";
	while(std::getline(indexFile, str))
		htmlString += str;
	stringJoinedResponse += "Content-Length: ";
	stringJoinedResponse += std::to_string(htmlString.length());
	stringJoinedResponse += "\n";
	stringJoinedResponse += "Connection: close\n";
	stringJoinedResponse += "Content-Type: text/html\n\n";
	stringJoinedResponse += htmlString;
	return stringJoinedResponse;
}

std::string Response::indexForbidden(){
	std::ifstream	indexFile;
	std::string		str;
	std::string		htmlString;
	indexFile.open("requestResponse/forbidden.html");
	stringJoinedResponse += (clientRequest.getRequestLine())[2]; 
	stringJoinedResponse += " 403 Forbidden \n";
	while(std::getline(indexFile, str))
		htmlString += str;
	stringJoinedResponse += "Content-Length: ";
	stringJoinedResponse += std::to_string(htmlString.length());
	stringJoinedResponse += "\n";
	stringJoinedResponse += "Connection: close\n";
	stringJoinedResponse += "Content-Type: text/html\n\n";
	stringJoinedResponse += htmlString;
	return stringJoinedResponse;
}

std::string Response::indexMovedPermanently(){
	std::ifstream	indexFile;
	std::string		str;
	std::string		htmlString;
	indexFile.open("requestResponse/movedPermanently.html");
	stringJoinedResponse += (clientRequest.getRequestLine())[2]; 
	stringJoinedResponse += " 301 Moved Permanently \n";
	while(std::getline(indexFile, str))
		htmlString += str;
	stringJoinedResponse += "Content-Length: ";
	stringJoinedResponse += std::to_string(htmlString.length());
	stringJoinedResponse += "\n";
	stringJoinedResponse += "Connection: keep-alive\n";
	stringJoinedResponse += "Content-Type: text/html\n";
	stringJoinedResponse += "Location: redirect.html\r\n\r\n";
	stringJoinedResponse += htmlString;
	return stringJoinedResponse;
}



