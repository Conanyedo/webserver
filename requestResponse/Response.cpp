#include "Response.hpp"

Response::Response(){
}

Response::Response(Request requestClient, std::vector<Server> configParsed): statusCode(-1), errorLog(false), location(), stringJoinedResponse(std::string()), \
clientRequest(requestClient), serverConfigData(configParsed){
}

Response::~Response(){
}

Server	*Response::findVirtualServer()
{
	int	defaultServerIndex = 0;
	std::vector<std::string> hostPort = ft_splitSpace(((clientRequest.getHttpHeaders()).find("Host"))->second, ':');
	if (hostPort.size() == 1)
		hostPort.push_back("80");
	for(int i = 0; i < serverConfigData.size(); i++)
	{
		if(!((serverConfigData[i]).get_listen()).compare(hostPort[1]))
		{
			if(i && ((serverConfigData[i - 1]).get_listen()) != ((serverConfigData[i]).get_listen()))
				defaultServerIndex = i;
			if(!((serverConfigData[i]).get_host()).compare(hostPort[0]))
					return (new Server(serverConfigData[i]));
			for (std::map<std::string, std::string>::iterator it = (serverConfigData[i].get_server_names()).begin(); it != (serverConfigData[i].get_server_names()).end(); it++)
				if(!(it->first.compare(hostPort[0])))
					return (new Server(serverConfigData[i]));
		}
	}
	return (new Server(serverConfigData[defaultServerIndex]));
}

void	Response::findLocation(){
	size_t		pos = 0;
	for(std::map<std::string, Location>::iterator it = virtualServer->get_map_loc().begin(); it != virtualServer->get_map_loc().end(); it++)
	{
		if (!(it->first.compare("/")))
			this->location = it->second;
		if(!(clientRequest.getPath().compare(it->first))){
			this->location = it->second;
			return;
		}
	}
}

bool	Response::allowedMethods(){
	for (int i = 0; i < location.get_methods().size(); i++){
		if (!(location.get_methods().at(i).compare(clientRequest.getMethod())))
			return true;
	}
	return false;
}

bool	Response::accessFile(std::string filename){
	return (!access(filename.c_str(), F_OK) && !access(filename.c_str(), R_OK) && !access(filename.c_str(), W_OK));
}

int		Response::returnStatus(int status_code, std::string status_message){
	statusMessage = status_message;
	statusCode = status_code;
	if (errorLog)
	{
		for(std::map<int, std::string>::iterator it = virtualServer->get_err_pages().begin(); it != virtualServer->get_err_pages().end(); it++)
		{
			if (it->first == statusCode)
			{
				filePath = virtualServer->get_root() + it->second;
				return (statusCode);
			}
		}
		filePath = virtualServer->get_root() + "/errors/" + std::to_string(statusCode) + ".html";
	}
	return (statusCode);
}

bool	Response::badRequest()
{
	// testing request
	// std::vector<std::string> methods = {"CONNECT", "DELETE", "GET", "HEAD", "OPTIONS", "PATCH", "POST", "PUT", "TRACE"};
	return false;
}

int     Response::buildResponse()
{
	struct stat buf;
	this->virtualServer = this->findVirtualServer();
	this->filePath = virtualServer->get_root() + clientRequest.getPath();
	this->findLocation();
	stat(filePath.c_str(), &buf);
	if (badRequest())
	{
		errorLog = true;
		return (returnStatus(BADREQUEST, std::string("Bad Request")));
	}
	if(this->clientRequest.getHttpHeaders().find("If-None-Match") != this->clientRequest.getHttpHeaders().end())
	{
		errorLog = true;
		return (returnStatus(NOTMODIFIED, "Not Modified"));
	}
	if (this->allowedMethods())
	{
		if (accessFile(filePath))
		{
			if (!location.get_return().empty())
			{
				redirection = location.get_return()[301];
				errorLog = true;
				return (returnStatus(MOVEDPERMANENTLY, std::string("Moved Permanently")));
			}
			if(!S_ISDIR(buf.st_mode))
				return (returnStatus(OK, std::string("OK")));
			else
			{
				if (filePath.back() != '/')
				{
					redirection = clientRequest.getPath() + std::string("/");
					errorLog = true;
					return (returnStatus(MOVEDPERMANENTLY, std::string("Moved Permanently")));
				}
				else
				{
						for(int i = 0; i < this->location.get_index().size(); i++)
						{
							if (accessFile(filePath + '/' + location.get_index().at(i)))
							{
								filePath += location.get_index().at(i);
								return (returnStatus(OK, std::string("OK")));
							}
						}
						if(!location.get_autoindex().compare("on"))
						{
							std::cout << "autoindex on need to create the appropriate webpage!!!" << std::endl;
							exit(0);
						}
					errorLog = true;
					return (returnStatus(NOTFOUND, "NOT FOUND"));
				}
			}
		}
		else
		{
			errorLog = true;
			return (returnStatus(FORBIDDEN, "FORBIDDEN"));
		}
	}
	else
	{
		errorLog = true;
		return (returnStatus(METHODNOTALLOWED, std::string("METHOD NOT ALLOWED")));
	}
	errorLog = true;
	return (returnStatus(NOTFOUND, "NOT FOUND"));
}

std::string	&Response::returnResponse(){
	buildResponse();
	return indexFound();
}

std::string	DateGMT(){
	time_t rawtime;
    struct tm * timeinfo;
	std::string date;
	static const char wday_name[][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	static const char mon_name[][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    time( &rawtime );
    timeinfo = localtime( &rawtime );
	date +=  wday_name[timeinfo->tm_wday]; 
	date += std::string(", ");
	date += std::to_string(timeinfo->tm_mday);
	date += std::string(" ");
	date += std::string(mon_name[timeinfo->tm_mon]);
	date += std::string(" ");
	date += std::to_string(1900 + timeinfo->tm_year);
	date += std::string(" ");
	date += std::to_string(timeinfo->tm_hour - 1);
	date += std::string(":");
	date += std::to_string(timeinfo->tm_min);
	date += std::string(":");
	date += std::to_string(timeinfo->tm_sec);
	date += std::string(" GMT");
	return (date);
}

std::string &Response::indexFound(){
	std::ifstream	indexFile;
	std::string		str;
	std::string		htmlString;
	indexFile.open(filePath);
	stringJoinedResponse += clientRequest.getHttpVersion() + " "; 
	stringJoinedResponse += std::to_string(statusCode) + " ";
	stringJoinedResponse += statusMessage + " \r\n";
	while(std::getline(indexFile, str))
		htmlString += str;
	stringJoinedResponse += "Content-Length: ";
	stringJoinedResponse += std::to_string(htmlString.length());
	stringJoinedResponse += "\r\n";
	if (statusCode == MOVEDPERMANENTLY)
	{
		stringJoinedResponse += "Location: ";
		stringJoinedResponse +=	redirection; 
		stringJoinedResponse +=	" \r\n"; 
	}	
	if (statusCode == METHODNOTALLOWED)
	{
		stringJoinedResponse += "Allow: ";
		stringJoinedResponse +=	redirection; 
		stringJoinedResponse +=	" \r\n"; 
	}	
	stringJoinedResponse += "Connection: close\r\n";
	stringJoinedResponse += "Content-Type: text/";
	if(this->clientRequest.getHttpHeaders().find("Sec-Fetch-Dest")->second == std::string("style"))
		stringJoinedResponse +=  "css\r\n";
	else if (this->clientRequest.getHttpHeaders().find("Sec-Fetch-Dest")->second == std::string("script"))
		stringJoinedResponse +=  "javascript\r\n";
	else
		stringJoinedResponse += "html\r\n";
	stringJoinedResponse += "Date: ";
	stringJoinedResponse += DateGMT();
	stringJoinedResponse += "\r\n\r\n";
	stringJoinedResponse += htmlString;
	return stringJoinedResponse;
}

