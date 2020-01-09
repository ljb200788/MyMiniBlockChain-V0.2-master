#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "MyMiniBlockChainServer.h"
#include <direct.h>  
#include <stdio.h>  
#include "Server.h"
#include "ServerDoc.h"
#include "ServerView.h"

#pragma comment(lib,"Rpcrt4.lib")  

using namespace std;

// Global Functions
std::ofstream logger("error.txt");
void logError(string msg, int GetLastError)
{
	logger << msg << std::endl;
	if (GetLastError != 0)
		logger << "Error Code: " << WSAGetLastError() << std::endl;
	logger.flush();
}
string  ToString(int data)
{
	std::stringstream buffer;
	buffer << data;
	return buffer.str();
}

// Constructor
MyMiniBlockChainServer::MyMiniBlockChainServer(void)
{
	// Socket Settings
	ZeroMemory(&_hints, sizeof(_hints));
	_hints.ai_family = AF_INET;
	_hints.ai_flags = AI_PASSIVE;
	_hints.ai_socktype = SOCK_STREAM;
	_hints.ai_protocol = IPPROTO_TCP;
	// Initialize Data
	_maxConnections = 50;
	_listeningSocket = INVALID_SOCKET;
	_connectionSocket = INVALID_SOCKET;
	_socketVersion = MAKEWORD(2, 2);
	_serverIP = "localhost";
	_clientIP = "";
	_port = DEFAULT_PORT;
	// Communication Content
	_bytesSend = 0;
	//Request Content
	_browserData = "";
	// CommandLine
	CMD = new CommandLine();
	m_pMiniBlockChain = new CMiniBlockChain;
	UUID globalUUID;
	RPC_WSTR tempRPCString;
	CString  NodeUuid;
	UuidCreate(&globalUUID);
	UuidToString(&globalUUID, &tempRPCString);
	NodeUuid.Format(_T("%s"), tempRPCString);
	RpcStringFree(&tempRPCString);
	NodeUuid.Replace(_T("-"), _T(""));

	int length = NodeUuid.GetLength();
	m_strNodeUuid = (char*)malloc(sizeof(char)*(length + 1));
	TcharToChar(NodeUuid.GetBuffer(0), m_strNodeUuid);

	m_pRegisteredServer = NULL;
}

MyMiniBlockChainServer::~MyMiniBlockChainServer()
{
	if (m_pMiniBlockChain != NULL)
	{
		free(m_pMiniBlockChain);
		m_pMiniBlockChain = NULL;
	}
	if (m_strNodeUuid != NULL)
	{
		free(m_strNodeUuid);
		m_strNodeUuid = NULL;
	}
}

#include <Iphlpapi.h>  
#pragma comment(lib,"Iphlpapi.lib") //需要添加Iphlpapi.lib库  

string GetLocalAddress()
{
	std::string strAddress;
	int nCardNo = 1;
	//PIP_ADAPTER_INFO结构体指针存储本机网卡信息  
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	//得到结构体大小,用于GetAdaptersInfo参数  
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	//调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量;其中stSize参数既是一个输入量也是一个输出量  
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	//记录网卡数量  
	int netCardNum = 0;
	//记录每张网卡上的IP地址数量  
	int IPnumPerNetCard = 0;
	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		//如果函数返回的是ERROR_BUFFER_OVERFLOW  
		//则说明GetAdaptersInfo参数传递的内存空间不够,同时其传出stSize,表示需要的空间大小  
		//这也是说明为什么stSize既是一个输入量也是一个输出量  
		//释放原来的内存空间  
		delete pIpAdapterInfo;
		//重新申请内存空间用来存储所有网卡信息  
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
		//再次调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量  
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	}
	if (ERROR_SUCCESS == nRel)
	{
		//输出网卡信息  
		//可能有多网卡,因此通过循环去判断  
		while (pIpAdapterInfo)
		{
			//可能网卡有多IP,因此通过循环去判断  
			IP_ADDR_STRING *pIpAddrString = &(pIpAdapterInfo->IpAddressList);
			switch (pIpAdapterInfo->Type)
			{
			case MIB_IF_TYPE_OTHER:
			case MIB_IF_TYPE_ETHERNET:
			case MIB_IF_TYPE_TOKENRING:
			case MIB_IF_TYPE_FDDI:
			case MIB_IF_TYPE_PPP:
			case MIB_IF_TYPE_LOOPBACK:
			case MIB_IF_TYPE_SLIP:
			{
				strAddress = pIpAddrString->IpAddress.String;
				// 需要注意的是有时可能获取的IP地址是0.0.0.0，这时需要过滤掉  
				if (std::string("0.0.0.0") == strAddress)
					break;
				//std::cout << _T("第") << nCardNo << _T("张网卡的IP地址是") << strAddress << std::endl;
				return strAddress;
				nCardNo++;
				break;
			}
			default:
				// 未知类型网卡就跳出  
				break;
			}
			pIpAdapterInfo = pIpAdapterInfo->Next;
		}
	}
	//释放内存空间  
	if (pIpAdapterInfo)
	{
		delete pIpAdapterInfo;
	}
	return strAddress;
}



// Main Functions
void MyMiniBlockChainServer::Init(int port, char* timeStamp)
{
	m_pMiniBlockChain->initBlockChain(timeStamp);
	setServerIP("127.0.0.1");
	// Start Winsocket
	_handleReturn = WSAStartup(_socketVersion, &_wsaData);
	if (0 != _handleReturn)
	{
		logError("Initialize Failed \nError Code : " + ToString(_handleReturn));
		ErrorQuit();
	}
	_port = ToString(port);
}

UINT  communicateThreadProc(LPVOID  lParam)
{
	char tempSTR[INET_ADDRSTRLEN];
	MyMiniBlockChainServer *pServer = (MyMiniBlockChainServer *)lParam;
	while (true)
	{
		// Listen
		int _handleReturn = listen(pServer->_listeningSocket, pServer->_maxConnections);
		if (SOCKET_ERROR == _handleReturn)
		{
			logError("Listening On Port " + pServer->_port + " Failed", 1);
			pServer->ErrorQuit();
		}

		sockaddr_in clientInfo;
		int clientInfoSize = sizeof(clientInfo);

		pServer->_connectionSocket = accept(pServer->_listeningSocket, (sockaddr*)&clientInfo, &clientInfoSize);
		if (INVALID_SOCKET == pServer->_connectionSocket)
		{
			logError("Accepting Connection Failed", 1);
		}
		else
		{
			// Set Client IP
			inet_ntop(AF_INET, &clientInfo.sin_addr, tempSTR, INET_ADDRSTRLEN);
			pServer->setClientIP(tempSTR);
			pServer->CMD->Add(pServer->_clientIP + ":" + pServer->_port + "\n");
			pServer->CMD->Display();
			// Main Procedure
			pServer->Communicate();
		}
	}
	return 1;
}

void MyMiniBlockChainServer::Start()
{
	// Resolve Local Address And Port
	_handleReturn = getaddrinfo(NULL, _port.c_str(), &_hints, &_result);
	if (0 != _handleReturn)
	{
		logError("Resolving Address And Port Failed \nError Code: " + ToString(_handleReturn));
		ErrorQuit();
	}
	// Create Socket
	_listeningSocket = socket(_hints.ai_family, _hints.ai_socktype, _hints.ai_protocol);
	if (INVALID_SOCKET == _listeningSocket)
	{
		logError("Could't Create Socket", 1);
		ErrorQuit();
	}
	// Bind 
	_handleReturn = bind(_listeningSocket, _result->ai_addr, (int)_result->ai_addrlen);
	if (SOCKET_ERROR == _handleReturn)
	{
		logError("Bind Socket Failed", 1);
		ErrorQuit();
	}
	AfxBeginThread(communicateThreadProc, (LPVOID)this);//启动新的线程  

}
void MyMiniBlockChainServer::Stop()
{
	closesocket(_listeningSocket);
	closesocket(_connectionSocket);
	WSACleanup();
}

void MyMiniBlockChainServer::Communicate()
{
	int routeStart, routeEnd, len;
	// Receive Browser Message
	getMessage();
	// route Is Between GET or POST and HTTP

	if (_browserData.find("GET ") != -1)
	{
		len = 4;
		routeStart = _browserData.find("GET ") + len;
	}
	else
	{
		len = 5;
		routeStart = _browserData.find("POST ") + len;
	}
	routeEnd = _browserData.find(" HTTP") - 1;
	// Checking Slashes so to find the last slash to find the route
	/*int slashPosition = 0;
	for (int i = routeStart; i <= routeEnd; i++)
		if (_browserData[i] == '/')
			slashPosition = i;*/
	string route = _browserData.substr(routeStart + 1, (routeEnd - len));

	setRoute(route);
	// Close Connection And Reset Some Vars
	closeConnection();
}


int MyMiniBlockChainServer::setRoute(string route)
{
	if (route == "chain")
	{
		CMD->Add("chain:\n");
		CMD->Display();
		std::string strResponse = "";
		Block* bTemp = m_pMiniBlockChain->getBlockChain();
		while (bTemp != NULL)
		{
			char*  bs = NULL;
			m_pMiniBlockChain->getBlockStr(bTemp, &bs);
			bTemp = bTemp->next;
			strResponse += bs;
			strResponse += "\n";
			CMD->Add(strResponse);
			CMD->Display();
		}
		CMD->Add("length:");
		std::string strTemp;
		CMD->Add(ToString(m_pMiniBlockChain->getBlockLength()));
		CMD->Add("\n");
		CMD->Display();
		sendMessage(strResponse);
	}
	else if (route == "mine")
	{
		m_pMiniBlockChain->newTransaction(1, m_strNodeUuid, "0");
		m_pMiniBlockChain->newBlock();
		CMD->Add("mine:\n");
		CMD->Display();
		char*  bs = NULL;
		m_pMiniBlockChain->getLastBlockStr(&bs);
		CMD->Add(bs);
		CMD->Display();
		std::string strResponse = "mine:\n";
		strResponse += bs;
		strResponse += "\n";
		sendMessage(strResponse);
	}
	else if (route == "transactions/new")
	{
		UUID globalUUID;
		RPC_WSTR tempRPCString;
		CString  NodeUuid;

		UuidCreate(&globalUUID);
		UuidToString(&globalUUID, &tempRPCString);
		NodeUuid.Format(_T("%s"), tempRPCString);
		RpcStringFree(&tempRPCString);
		NodeUuid.Replace(_T("-"), _T(""));

		int length = NodeUuid.GetLength();
		char* temp = (char*)malloc(sizeof(char)*(length + 1));
		TcharToChar(NodeUuid.GetBuffer(0), temp);
		m_pMiniBlockChain->newTransaction(1, m_strNodeUuid, temp);
		char tempBuffer[50];
		sprintf(tempBuffer, "Transaction info will be added to block{ %d }", m_pMiniBlockChain->getBlockLength() + 1);
		CMD->Add(tempBuffer);
		CMD->Display();

		std::string strResponse = "transactions/new:\n";
		strResponse += tempBuffer;
		strResponse += "\n";
		sendMessage(strResponse);
	}
	else if (route == "nodes/resolve")
	{
		std::string strResponse;
		if (m_pRegisteredServer == NULL)
			strResponse = "nodes/resolve:\n No Register node found\n";
	
		else if ((m_pMiniBlockChain->getBlockLength() < m_pRegisteredServer->m_pMiniBlockChain->getBlockLength())
			&& (m_pRegisteredServer->m_pMiniBlockChain->valid_chain() == 1))
		{
			strResponse = "nodes/resolve:\n  Our chain was replaced\n";
			m_pMiniBlockChain->copyChain(m_pRegisteredServer->m_pMiniBlockChain->getBlockChain(), m_pRegisteredServer->m_pMiniBlockChain->getBlockLength());
		}
		else
			strResponse = "nodes/resolve:\n  Our chain is authoritative\n";
		CMD->Add(strResponse);
		CMD->Display();
		sendMessage(strResponse);
	}
	else if (route == "nodes/register")
	{
		int nodeStart, nodeEnd;
		nodeStart = _browserData.find("nodes:[") + 7;
		nodeEnd = _browserData.find("]");
		string info = _browserData.substr(nodeStart, (nodeEnd - nodeStart));
		m_pRegisteredServer = GetOtherServer(info);
		if (m_pRegisteredServer != NULL)
		{
			char tempBuffer[50];
			sprintf(tempBuffer, "Nodes Registered address is 0x%x", m_pRegisteredServer);
			CMD->Add(tempBuffer);
			CMD->Display();
			std::string strResponse = "nodes/register:\n";
			strResponse += tempBuffer;
			strResponse += "\n";
			sendMessage(strResponse);
		}
	}
	else if (route == "valid")
	{
		int ret = m_pMiniBlockChain->valid_chain();
		std::string strResponse;
		if (ret == 0)
			strResponse = "valid:\n The Chain is empty\n";
		else if (ret == -1)
			strResponse = "valid:\n The Chain is corrupted\n";
		else if (ret == 1)
			strResponse = "valid:\n The Chain is correct\n";
		CMD->Add(strResponse);
		CMD->Display();
		sendMessage(strResponse);
	}
	else 
	{
		std::string strResponse = route;
		strResponse += "Service is not supported by Mini Block Chain！\n";
		sendMessage(strResponse);
	}
	return 0;
}

MyMiniBlockChainServer* MyMiniBlockChainServer::GetOtherServer(string info)
{
	//http:://127.0.0.1:5001
	CServerView* pView = theApp.pServerView;
	for (int i = 0; i < pView->serverNumber; i++)
	{
		string strTemp = "http://";
		strTemp += pView->server[i]->getServerIP();
		strTemp += ":";
		strTemp += pView->server[i]->getPort();
		if (strTemp == info)
			return pView->server[i];
	}
	return NULL;
}

void MyMiniBlockChainServer::ErrorQuit()
{
	Stop();
	exit(-1);
}

// IPs Input/Output
void MyMiniBlockChainServer::setServerIP(string ip)
{
	_serverIP = ip;
}

void MyMiniBlockChainServer::setClientIP(string ip)
{
	_clientIP = ip;
}

string MyMiniBlockChainServer::getMessage()
{
	char buffer[BUFFER_SIZE + 1];
	int bytes;

	bytes = recv(_connectionSocket, buffer, BUFFER_SIZE, 0);
	buffer[bytes] = '\0';
	_browserData = buffer;

	while (BUFFER_SIZE == bytes)
	{
		bytes = recv(_connectionSocket, buffer, BUFFER_SIZE, 0);
		buffer[bytes] = '\0';
		_browserData += buffer;
	}
	return _browserData;
}

void MyMiniBlockChainServer::sendMessage(string msg)
{
	// Send Only BUFFER SIZE
	int msgSize = msg.size();
	_bytesSend += send(_connectionSocket, msg.c_str(), msg.size(), 0);
}

int MyMiniBlockChainServer::sendData(char* data)
{
	_bytesSend += send(_connectionSocket, data, BUFFER_SIZE, 0);
	return _bytesSend;
}

void MyMiniBlockChainServer::closeConnection()
{

	if (shutdown(_connectionSocket, SD_BOTH) == SOCKET_ERROR)
	{

		logError("ShutDown Connection Failed", 1);
		exit(-1);
	}
	_bytesSend = 0;
	_clientIP = "";
	_browserData = "";
}

// CMD Functions
MyMiniBlockChainServer::CommandLine::CommandLine()
{
	_cmdMessage = "";
}
void MyMiniBlockChainServer::CommandLine::Add(string msg)
{
	_cmdMessage += msg;
}
void MyMiniBlockChainServer::CommandLine::Display()
{
	cout << _cmdMessage;
	CServerView* pView = theApp.pServerView;
	pView->AddInfo((char*)_cmdMessage.data());
	pView->Invalidate();
	pView->UpdateWindow();
	Clear();
}
void MyMiniBlockChainServer::CommandLine::Clear()
{
	_cmdMessage = "";
}

void MyMiniBlockChainServer::CharToTchar(const char * _char, TCHAR * tchar)
{
	int iLength = MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, tchar, iLength);
	return;
}

void MyMiniBlockChainServer::TcharToChar(const TCHAR * tchar, char * _char)
{
	int iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, FALSE);
	WideCharToMultiByte(CP_ACP, NULL, tchar, -1, _char, iLength, NULL, FALSE);
	return;
}