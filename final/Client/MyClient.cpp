#include "stdafx.h"
#include "Client.h"

#include <sstream>
#include <boost/algorithm/string.hpp>
#include "parseWithStream.hpp"

#define warassert(X) \
	do { if (!(X)) { \
		std::cerr << "Warning: " << #X << std::endl; \
	}} while(false)

#define DEBUG_PRINT(X) std::cout << #X << " = " << (X) << std::endl
// sample

const char *CommandList[]={ "check", "call", "bet" };

class MYCLIENT : public CLIENT
{
public:
	MYCLIENT();
protected:
	virtual std::string
		HandleServerResponse(std::vector<std::string> &ServerResponse);
	virtual std::string
		GetPassword() { return std::string("4APCtg"); } // Nutellas Gereblye
	virtual bool
		NeedDebugLog() { return true; }
};

MYCLIENT::MYCLIENT()
{
}

struct Player {
	int id;
	int cash;
	std::string name;
};
typedef std::vector<Player> Players;

std::string MYCLIENT::HandleServerResponse(std::vector<std::string> &response)
{

	using boost::starts_with;

	if (response.size() == 0) {
		return "";
	}
	std::string tmp;

	int tick, hand_tick, our_id, our_cash;
	unsigned player_count;
	int hand1, hand2;
	int c = 0;
	parseWithStream(response[c++], tmp, tick, hand_tick);
	warassert(tmp == "tick");

	parseWithStream(response[c++], tmp, our_id);
	warassert(tmp == "id");

	parseWithStream(response[c++], tmp, our_cash);
	warassert(tmp == "cash");

	parseWithStream(response[c++], tmp, hand1, hand2);
	warassert(tmp == "hand");

	parseWithStream(response[c++], tmp, player_count);
	warassert(tmp == "players");

	Players players(player_count);

	for (unsigned i = 0; i < player_count; ++i) {
		std::stringstream ss(response[c++]);
		ss >> players[i].id >> players[i].cash;
		std::string name;
		while (ss >> name) {
			players[i].name += name + " ";
		}
	}

	int pot, blind;

	parseWithStream(response[c++], tmp, pot);
	warassert(tmp == "pot");

	parseWithStream(response[c++], tmp, blind);
	warassert(tmp == "blind");

	int to_call = -1;
	if (starts_with(response[c], "to_call")) {
		parseWithStream(response[c++], tmp, to_call);
		warassert(tmp == "to_call");
	}

	unsigned card_count;
	parseWithStream(response[c++], tmp, card_count);
	warassert(tmp == "cards");

	warassert(card_count == 0 || card_count == 3);

	std::vector<int> cards(card_count);
	for (unsigned i = 0; i < card_count; ++i) {
		parseWithStream(response[c++], cards[i]);
	}


	int next = -1;
	for (unsigned i = c; i < response.size() - 1; ++i) {
		if (starts_with(response[i], "action")) {

		} else if (starts_with(response[i], "fold")) {

		} else if (starts_with(response[i], "next")) {
			parseWithStream(response[i], tmp, next);
			warassert(tmp == "next");
		} else if (starts_with(response[i], "winner")) {

		} else if (starts_with(response[i], "showdown")) {

		} else {
			warassert(false && "unknown command");
		}
	}
	if (next == our_id) {
		std::cout << "Mi jovunk!" << std::endl;
		//Mi jovunk!!!
		return "";
	} else {
		std::cout << "Nem mi jovunk!" << std::endl;
		//Nem mi jovunk :(
		return "";
	}
}


CLIENT *CreateClient()
{
	return new MYCLIENT();
}
