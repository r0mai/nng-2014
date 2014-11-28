#include "stdafx.h"
#include "Client.h"
#define SERVER_PORT 4242

CLIENT::CLIENT()
{
#ifdef WIN32
	mConnectionSocket = INVALID_SOCKET;
#else
	mConnectionSocket = -1;
#endif
}

CLIENT::~CLIENT()
{
#ifdef WIN32
	if( mConnectionSocket != INVALID_SOCKET )
	{
		closesocket( mConnectionSocket );
	}
	WSACleanup();
#else
	if (mConnectionSocket!=-1)
	{
		close(mConnectionSocket);
	}
#endif
}

bool CLIENT::Init( std::string aIPAddress )
{
#ifdef WIN32
	WSADATA WSAData;

	if( WSAStartup( 0x101,&WSAData ) != 0 )
	{
		std::cout << "Error: Cannot start windows sockets!" << std::endl;
		return false;
	}
#endif
	unsigned long addr = inet_addr( aIPAddress.c_str() );
	sockaddr_in ServerSocketAddress;	
	ServerSocketAddress.sin_addr.s_addr = addr;
	ServerSocketAddress.sin_family = AF_INET;
	ServerSocketAddress.sin_port = htons( SERVER_PORT );
	mConnectionSocket = socket( AF_INET, SOCK_STREAM, 0 );
#ifdef WIN32
	if( mConnectionSocket == INVALID_SOCKET )
#else
	if (mConnectionSocket == -1 )
#endif
	{
		std::cout << "Error: Cannot open a socket!" << std::endl;
		return false;
	}
	if ( connect( mConnectionSocket,(struct sockaddr*)&ServerSocketAddress, sizeof( ServerSocketAddress ) ) )
	{
		std::cout << "Error: Cannot connect to " << aIPAddress << "!" << std::endl;
#ifdef WIN32
		closesocket( mConnectionSocket );
#else
		close( mConnectionSocket );
#endif
		return false;
	}	
	return true;
}

void CLIENT::ConnectionClosed()
{
	std::cout<<"Connection closed"<<std::endl;
	exit(-1);
}

void CLIENT::SendMessage( std::string aMessage )
{
	if (aMessage.length()==0) return;
	if (aMessage[aMessage.length()-1]!='\n') aMessage+="\n";
	if (NeedDebugLog() && mDebugLog.is_open())
	{
		mDebugLog<<"Sent: "<<aMessage;
	}	
	int SentBytes = send( mConnectionSocket, aMessage.c_str(), int(aMessage.size()), 0 );
	if (SentBytes!=aMessage.size())
	{
		ConnectionClosed();
	}
}

void CLIENT::Run()
{
	SendMessage("login " + GetPassword());
	if (NeedDebugLog())
	{
		mDebugLog.open("debug.log", std::ofstream::out | std::ofstream::app);
	}
	
	std::string strLastLineRemaining;
	std::vector<std::string> LastServerResponse;
	for(;;)
	{
		const size_t ReceiveBufferSize = 1<<16;
		char ReceiveBuffer[ ReceiveBufferSize+1 ] = {0};

		int ReceivedBytesCount = recv( mConnectionSocket, ReceiveBuffer, ReceiveBufferSize, 0 );

		if( ReceivedBytesCount == 0 || ReceivedBytesCount == -1)
		{
			// connection is closed or failed
			ConnectionClosed();
			return;
		}
		ReceiveBuffer[ReceivedBytesCount]=0;
		char *line_start = ReceiveBuffer;
		for(;;)
		{
			char *s = strchr(line_start, '\n');
			if (!s)
			{
				strLastLineRemaining = line_start;
				break;
			} else
			{
				std::string alma=strLastLineRemaining;
				*s=0;
				alma+=line_start;
				line_start = s+1;
				strLastLineRemaining = "";
				LastServerResponse.push_back(alma);
				if (alma==".")
				{
					if (NeedDebugLog() && mDebugLog.is_open())
					{
						for(unsigned int i=0;i<LastServerResponse.size();i++)
							mDebugLog<<LastServerResponse[i]<<std::endl;
					}
					std::string strResponse = HandleServerResponse(LastServerResponse);
					if (!strResponse.empty())
					{
						SendMessage(strResponse);
					}
					LastServerResponse.clear();
				}				
			}
		}
	}
}


int main(int argc, char* argv[])
{
	std::cout.sync_with_stdio(false);
	std::string server_address;
	if (argc<2)
	{
		server_address = "172.22.22.250";
		std::cout<<"using default server address: " + server_address <<std::endl;
	} else
	{
		server_address = argv[1];
	}
	CLIENT *pClient = CreateClient();
	/* for debugging:  */
	std::ifstream debug_file("test.txt");
	if (debug_file.is_open())
	{
		std::string line;
		std::vector<std::string> full;
		while (std::getline(debug_file, line))
		{
			full.push_back(line);
		}
		std::string resp = pClient->DebugResponse(full);
		std::cout<<"response: "<<resp <<std::endl;
	}
	/**/
	
	if (!pClient->Init(server_address))
	{
		std::cout<<"Connection failed"<<std::endl;
	} else
	{
		pClient->Run();
	}
	delete pClient;
	return 0;
}


