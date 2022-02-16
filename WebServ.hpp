/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouddou <ybouddou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/08 16:33:37 by ybouddou          #+#    #+#             */
/*   Updated: 2022/02/14 16:24:38 by ybouddou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <string>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/event.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <map>

#include "parsing/Server.hpp"
#include "server/Sockets.hpp"
#include "requestResponse/Request.hpp"
#include "requestResponse/Response.hpp"

class Server;

typedef struct	Cl
{
	bool	done;
	size_t	sent;
	size_t	rem;
	size_t	size;
	char	*buffer;
}				Client;


typedef struct	ws
{
	std::vector<Sockets>				sockets;
	std::vector<Server>					servers;
    Server								*serv;
    Location							locat;
	std::map<std::string, int>			ports;
	size_t								index;
	Client								*client;
	struct kevent	event;
	int				kq;
	int				nev;
	int				port;
	int				acceptfd;
	int				sockfd;
}				t_WebServ;


void	multipleServers(t_WebServ *webserv);
void	multipleClient(t_WebServ *webserv);
bool	isServer(std::vector<Sockets> sockets, int sockfd);
int		accept_connection(int sockfd);
void	handle_connection(t_WebServ *webserv, struct kevent event);

#endif