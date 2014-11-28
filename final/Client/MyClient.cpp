#include "stdafx.h"
#include "Client.h"

// sample

const char *CommandList[]={ "check", "call", "bet" };

class MYCLIENT : public CLIENT
{
public:
	MYCLIENT();
protected:
	virtual std::string HandleServerResponse(std::vector<std::string> &ServerResponse);
	virtual std::string GetPassword() { return std::string("4APCtg"); } // Nutellas Gereblye
	virtual bool NeedDebugLog() { return true; }
};

MYCLIENT::MYCLIENT()
{
}

std::string MYCLIENT::HandleServerResponse(std::vector<std::string> &ServerResponse)
{
	return "";
}


CLIENT *CreateClient()
{
	return new MYCLIENT();
}
