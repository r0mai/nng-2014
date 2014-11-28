#include "stdafx.h"
#include "Client.h"
#include <map>

// sample

const char *CommandList[]={ "check", "call", "bet" };

enum Combination {AllTheSame, Full, Poker, Drill, DoublePair, Pair, Null};

typedef std::vector<std::string> string_vector;
typedef std::vector<std::vector<string_vector> > matrix;

static const std::map<Combination, matrix> LOOKUP = {
    {AllTheSame, 
        {{{"bet", "bet", "bet", "bet" }}}
    },
    {Full,
        {
            {{"bet", "bet", "call", "call"}},
            {{"bet", "bet", "bet", "call"}, {"bet", "bet", "call", "call"}},
            {{"bet", "bet", "bet", "call"}, {"bet", "bet", "bet", "bet"}}
        } 
    },
    {Poker,
        {{{"bet", "bet", "bet", "call"}, {"bet", "bet", "bet", "bet"}}} 
    },
    {Drill,
        {
             
        }
    },
    {DoublePair,
        {

        }
    },
    {Pair,
        {

        }
    },
    {Null,
        {{{"check", "check", "check", "check"}}} 
    }
};

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
