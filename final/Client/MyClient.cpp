#include "stdafx.h"
#include "Client.h"
#include <map>
#include <set>

#include <sstream>
#include <boost/algorithm/string.hpp>
#include "parseWithStream.hpp"

#define warassert(X) \
    do { if (!(X)) { \
        std::cerr << "Warning: " << #X << std::endl; \
    }} while(false)

#define DEBUG_PRINT(X) std::cout << #X << " = " << (X) << std::endl

int doPreFlopLepkepzes(int hand1, int hand2);

const char *CommandList[]={ "check", "call", "bet" };

enum Combination {AllTheSame, Full, Poker, Drill, DoublePair, Pair, Null};
enum Command { CHECK, CALL, BET };

typedef std::vector<Command> command_vector;
typedef std::vector<std::vector<command_vector> > matrix;

static const std::vector<command_vector> PRE_LOOKUP = {
    {BET, BET, CALL, CALL, CALL},
    {BET, BET, BET, CALL, CALL},
    {BET, CALL, CALL, CALL, CALL},
    {CALL, CALL, CHECK, CHECK, CHECK},
    {CALL, CALL, CALL, CHECK, CHECK },
    {CHECK, CHECK, CHECK, CHECK, CHECK}
};

static const std::map<Combination, matrix> POST_LOOKUP = {
    {AllTheSame, 
        {{{BET, BET, BET, BET, CALL}}}
    },
    {Full,
        {
            {{BET, BET, CALL, CALL, CALL}},
            {{BET, BET, BET, CALL, CALL}, {BET, BET, CALL, CALL, CALL}},
            {{BET, BET, BET, CALL, CALL}, {BET, BET, BET, BET, CALL}}
        } 
    },
    {Poker,
        {{{BET, BET, BET, CALL, CALL}, {BET, BET, BET, BET, CALL}}} 
    },
    {Drill,
        {
            {{CALL, CHECK, CHECK, CHECK, CHECK}, {CHECK, CHECK, CHECK, CHECK, CHECK}},
            {{BET, CALL, CHECK, CHECK, CHECK}, {BET, CALL, CALL, CHECK, CHECK}},
            {{BET, BET, BET, BET, CALL}, {BET, BET, CALL, CALL, CALL}, {BET, CALL, CALL, CALL, CALL}}
        }
    },
    {DoublePair,
        {
            {{BET, CALL, CALL, CHECK, CHECK}, {CALL, CALL, CHECK, CHECK, CHECK}},
            {{BET, CALL, CALL, CHECK, CHECK}, {BET, CALL, CALL, CHECK, CHECK}, {CALL, CALL, CALL, CHECK, CHECK}},
            {{CALL, CALL, CHECK, CHECK, CHECK}, {CALL, CHECK, CHECK, CHECK, CHECK}}
        }
    },
    {Pair,
        {
            {{CHECK, CHECK, CHECK, CHECK, CHECK}},
            {{CALL, CHECK, CHECK, CHECK, CHECK}, {CHECK, CHECK, CHECK, CHECK, CHECK}},
            {{CALL, CHECK, CHECK, CHECK, CHECK}, {CHECK, CHECK, CHECK, CHECK, CHECK}}
        }
    },
    {Null,
        {{{CHECK, CHECK, CHECK, CHECK, CHECK}}}
    }
};


class MYCLIENT : public CLIENT
{
public:
    virtual std::string
        HandleServerResponse(std::vector<std::string> &ServerResponse);
    virtual std::string
        GetPassword() { return std::string("4APCtg"); } // Nutellas Gereblye
    virtual bool
        NeedDebugLog() { return true; }

    void startHand(int newHandTick) {
        betCount = 0;
        currentHandTick = newHandTick;
        preFlop = true;
    }

    void flopStarted() {
        betCount = 0;
        preFlop = false;
    }

    void doBet() {
        betCount = std::min(4, betCount + 1);
    }

    int betCount = 0;
    int currentHandTick = -1;
    bool preFlop = true;
};

struct Player {
    int id;
    int cash;
    std::string name;
};
typedef std::vector<Player> Players;

Command getPreflopCommand(int hand1, int hand2, int betCount) {
    int lekep = doPreFlopLepkepzes(hand1, hand2);
    if (betCount >= PRE_LOOKUP[lekep].size()) {
        warassert(false && "getPreflopCommand outindex");
        return CALL;
    }
    return PRE_LOOKUP[lekep][betCount];
}

Command getPostflopCommand(std::tuple<Combination, int, int> t, int betCount) {
    auto map = POST_LOOKUP;
    return map[std::get<0>(t)][std::get<1>(t)][std::get<2>(t)][betCount];
}

std::string commandToString(Command c) {
    if (c == CHECK) { return "check"; }
    else if (c == BET) { return "bet"; }
    else if (c == CALL) { return "call"; }
    else return "check";
}

std::tuple<Combination, int, int> doPostFlopLepkepzes(
    int hand1, int hand2,
    int flop1, int flop2, int flop3)
{
    std::array<int, 2> hand = {{hand1, hand2}};
    std::array<int, 3> flop = {{flop1, flop2, flop3}};
    std::sort(hand.begin(), hand.end());
    std::sort(flop.begin(), flop.end());

    bool drillFlop = flop[0] == flop[1] && flop[0] == flop[2];
    bool pairHand = hand[0] == hand[1];

    bool pairFlop = flop[0] == flop[1] || flop[1] == flop[2];

    if (drillFlop && pairHand) {
        if (flop[0] == hand[0]) {
            // XXX XX
            return std::make_tuple(AllTheSame, 0, 0);
        } else {
            // XXX YY
            return std::make_tuple(Full, 0, 0);
        }
    }

    if (drillFlop) {
        if (flop[0] == hand[0] || flop[0] == hand[1] ) {
            // XXX XY
            return std::make_tuple(Poker, 0, 0);
        } else {
            // XXX YZ
            if (hand[0] == 9 || hand[1] == 9) {
                return std::make_tuple(Drill, 0, 0);
            } else {
                return std::make_tuple(Drill, 0, 1);
            }
        }
    }
    if(pairFlop)
    {
        if(pairHand)
        {
            if(flop[1] == hand[0])
            { 
                // XXY XX
                return std::make_tuple(Poker, 1, 0);
            }
            
            if(flop[0] == hand[0] || flop[2] == hand[0] )
            {
                // XXY YY
                return std::make_tuple(Full, 2, flop[1] < hand[0]);
            }
            
            // XXZ YY
            return std::make_tuple(DoublePair, 0, hand[0] > flop[2]);
        }
        // no pair hand
        if(flop[0] == hand[0] && flop[2] == hand[1])
        {
            // XXY XY
            return std::make_tuple(Full, 1, flop[0] == flop[1]);
        }

        if(flop[1] == hand[0] || flop[1] == hand[1])
        {
            // XXY XZ
            return std::make_tuple(Drill, 1, flop[0] != flop[1]);
        }

        if(flop[0] == flop[1] && (flop[2] == hand[0] || flop[2] == hand[1]) )
        {
            // XXY YZ
            return std::make_tuple(DoublePair, 2, 0);
        }

        if(flop[2] == flop[1] && (flop[0] == hand[0] || flop[0] == hand[1]) )
        {
            // YXX YZ
            return std::make_tuple(DoublePair, 2, 1);
        }
        // XXY ZW
        return std::make_tuple(Pair, 0, 0);
    }

    if(pairHand)
    {   
        // XYZ XX
        if(flop[0] == hand[0])
        {            
            return std::make_tuple(Drill, 2, 2);
        }
        if(flop[1] == hand[0])
        {
            return std::make_tuple(Drill, 2, 1);
        }
        if(flop[2] == hand[0])
        {
            return std::make_tuple(Drill, 2, 0);
        }
        // XYZ WW
        if(flop[2] < hand[0])
        {
            return std::make_tuple(Pair, 2, 0);
        }
        return std::make_tuple(Pair, 2, 1);
        
    }
    std::set<int> elems(flop.begin(), flop.end());
    elems.insert(hand.begin(), hand.end());

    if(elems.size() == 3 )
    {
        // ZXY XY
        if(flop[1] == hand[0])  
        {
            return std::make_tuple(DoublePair, 1, 0);  
        }
        if(flop[1] == hand[1])
        {
            return std::make_tuple(DoublePair, 1, 2);  

        }
        return std::make_tuple(DoublePair, 1, 1);
    }
    else if( elems.size() == 4)
    {
        // XYZ XW
        if(flop[2] == hand[0] || flop[2] == hand[1])
        {
            return std::make_tuple(Pair, 1, 0);
        }
        return std::make_tuple(Pair, 1, 1);
    }
    // XYZ VW
    return std::make_tuple(Null, 0, 0);
}

int doPreFlopLepkepzes(int hand1, int hand2) {
    std::array<int, 2> hand = {{hand1, hand2}};
    std::sort(hand.begin(), hand.end());
    if (hand[0] == 8 && hand[1] == 9) {
        return 0;
    }

    if (hand[0] == 9 && hand[1] == 9) {
        return 1;
    }

    if (hand[0] == 8 && hand[1] == 8) {
        return 2;
    }

    if (hand[0] == 7 && hand[1] == 7) {
        return 2;
    }

    if (hand[0] == hand[1] && hand[0] <= 6 && hand[0] >= 4) {
        return 3;
    }

    if (hand[0] >= 7 && hand[1] >= 7) {
        return 4;
    }

    return 5;
}

void printRanking(Players players) {
    std::sort(players.begin(), players.end(),
            [](const Player& lhs, const Player& rhs) {
                return lhs.cash > rhs.cash;
            });

    for (unsigned i = 0; i < players.size(); ++i) {
        std::cout << i+1 << ".\t" <<
            players[i].cash << " : " <<
            players[i].name << std::endl;
    }
}

std::string MYCLIENT::HandleServerResponse(std::vector<std::string> &response)
{

    using boost::starts_with;

    std::cout << "in HandleServerResponse" << std::endl;

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

    if (hand_tick != currentHandTick) {
        startHand(currentHandTick);
    }

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
    printRanking(players);

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

    if (preFlop && card_count == 3) {
        flopStarted();
    }

    std::vector<int> cards(card_count);
    for (unsigned i = 0; i < card_count; ++i) {
        parseWithStream(response[c++], cards[i]);
    }


    int next = -1;
    std::cout << "response : ";
    for (unsigned i = c; i < response.size() - 1; ++i) {
        std::cout << i;
        if (starts_with(response[i], "action")) {
            if (boost::ends_with(response[i], "bet")) {
                doBet();
            }
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
    std::cout << std::endl;
    DEBUG_PRINT(betCount);
    DEBUG_PRINT(preFlop);
    if (next == our_id) {
        //Mi jovunk!!!
        std::cout << "Mi jovunk! cash = " << our_cash << std::endl;
        if (cards.size() == 0) {
            auto c = commandToString(
		    getPreflopCommand(hand1, hand2, betCount));
            std::cout << "Sent before flop: " << c << " " << hand1 <<
                " " << hand2 << std::endl;
            return c;
        } else {
            auto c = commandToString(
                    getPostflopCommand(
                        doPostFlopLepkepzes(
                            hand1, hand2, cards[0], cards[1], cards[2]),
                    betCount));
            std::cout << "Sent after flop: " << c << " " << hand1 <<
                " " << hand2 << std::endl;
            return c;
        }
    } else {
        std::cout << "Nem mi jovunk!\n" << std::endl;
        //Nem mi jovunk :(
        return "";
    }
}


CLIENT *CreateClient()
{
    return new MYCLIENT();
}
