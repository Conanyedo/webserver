#ifndef REQUEST_HPP
# define REQUEST_HPP
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream>

#define METHOD		0
#define PATH		1
#define HTTPVERSION	2

typedef struct	s_Body
{
	std::string		body;
	std::string		ContentDispo;
	std::string		ContentType;
	std::string		name;
	std::string		fileName;
}				Body;

class Request{
	protected:
		bool								upload;
		std::vector<std::string>			requestLine;
		std::map<std::string, std::string>	URLVariable;
		std::map<std::string, std::string>	httpHeaders;

		// Body variables
		std::vector<Body>					bodies;
		std::string							contentType;
		int									contentLength;
		std::string							boundary;
	public:
		Request();
		Request(std::string);
		~Request();

		// Getters
		std::vector<std::string>			&getRequestLine();
		std::string							&getMethod();
		std::string							&getPath();
		std::vector<Body>					&getBodies();
		std::string							&getHttpVersion();
		std::string							&getContentType();
		int									&getContentLength();
		std::map<std::string, std::string>	&getHttpHeaders();
		bool								getUpload();

		// Setters
		void								setContentType();
		void								setContentLength();
		void								setFormData(std::string& req, std::string type);
		
		std::vector<std::string>    		ft_splitCrlf(std::string &str,const std::string &c);
		void								parseBody(std::string req);
		void								SplitHeader(std::vector<std::string> vect, char c);
		void								SplitFirstLine(std::string& requestString);
		void								parseParam(std::string	&variableURL, size_t &pos);
		bool								uploadFile();
};

#endif