#ifndef _SERVER_H
#define _SERVER_H

#include <WinSock2.h>
#include <string>
#include <fstream>

#include "MiniBlockChain.h"

#define AppVersion "1.0beta"
#define DEFAULT_PORT "80"
#define BUFFER_SIZE 1024*5 // 5KB
#define ACC_EXT_NUM 48
#define OS_TYPE_NUM 9


using std::string;

// ERROR HANDLING
extern std::ofstream logger;
extern void logError(string msg, int GetLastError = 0);
extern string ToString(int data);


class MyMiniBlockChainServer
{
public:
	MyMiniBlockChainServer(void);
	~MyMiniBlockChainServer();
	CMiniBlockChain* m_pMiniBlockChain;
	char* m_strNodeUuid;
	MyMiniBlockChainServer* m_pRegisteredServer;
	MyMiniBlockChainServer* GetOtherServer(string info);

	void CharToTchar(const char * _char, TCHAR * tchar);
	void TcharToChar(const TCHAR * tchar, char * _char);
	// Main Functions
	void Init(int port, char* timeStamp); // Initialize Server
	void Start(); // Start server
	void Stop(); // Close Server
	void Communicate(); // Main Loop.
	void ErrorQuit(); // Quit And Display An Error Message
	

	// IPs Input/Output
	void setServerIP(string ip);
	void setClientIP(string ip);
	string getServerIP() { return _serverIP; }
	string getClientIP() { return _clientIP; }
	string getPort() { return _port; }

	// Main CMD Display
	struct CommandLine{
		CommandLine();
		void Display();
		void Add(string msg);
		void Clear();
		// Content
		string _cmdMessage;
	}*CMD;
public:
	// Initialize

	// Communication Functions
	string getMessage(); // Receive Browser Requests
	int setRoute(string route);
	void sendMessage(string msg); // Send Text Files
	int  sendData(char*data); // Send Other Type Files In Binary
	void closeConnection(); // Close Connections

	// Initialize
	int _handleReturn;
	WORD _socketVersion;
	WSADATA _wsaData;
	SOCKET _listeningSocket;
	SOCKET _connectionSocket;
	addrinfo _hints, *_result;
	int _maxConnections;
	// Communication Content
	int _bytesSend;


	//  Server Info
	string _serverIP;
	string _clientIP;
	string _port;

	// Browser Request Data
	string _browserData;

};

#endif

