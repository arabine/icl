#ifndef NETHELPER_H
#define NETHELPER_H

#include "JsonValue.h"
#include "Common.h"
#include "Player.h"

namespace helper
{

struct Reply
{
    std::uint32_t dest;
    JsonObject data;

    Reply(std::uint32_t d, const JsonObject &obj)
        : dest(d)
        , data(obj)
    {

    }
};

inline bool FromJson(Tarot::Distribution &dist, const JsonObject &obj)
{
    bool ret = true;

    dist.mFile = obj.GetValue("file").GetString();
    dist.mSeed = obj.GetValue("seed").GetInteger();
    dist.TypeFromString(obj.GetValue("type").GetString());

    return ret;
}

inline void ToJson(const Tarot::Distribution &dist, JsonObject &obj)
{
    obj.AddValue("file", dist.mFile);
    obj.AddValue("seed", dist.mSeed);
    obj.AddValue("type", dist.TypeToString());
}

// Network context storage interface
// Used to store "optional" information that is not necessary for some implementations (eg: bots)
class IContext
{
public:
    struct Message
    {
        std::uint32_t src;
        std::uint32_t dst;
        std::string msg;
    };

    struct Member
    {
        std::string nickname;
        std::uint32_t table;
    };

    virtual void Initialize() = 0;
    virtual void AddTable(const std::string &name, std::uint32_t uuid) = 0;
    virtual void AddMessage(const Message &msg) = 0;
    virtual void UpdateMember(std::uint32_t uuid, const Member &member, const std::string &event) = 0;
};

class EmptyContext : public IContext
{
public:
    virtual void Initialize() { }
    virtual void AddTable(const std::string &name, std::uint32_t uuid) { (void) name; (void) uuid; }
    virtual void AddMessage(const Message &msg) { (void) msg;}
    virtual void UpdateMember(std::uint32_t uuid, const Member &member, const std::string &event) { (void) uuid; (void) member; (void) event; }
};


class Context : public IContext
{
public:
    virtual void Initialize()
    {
        mTables.clear();
        mMessages.clear();
    }

    virtual void AddTable(const std::string &name, std::uint32_t uuid)
    {
        mTables[uuid] = name;
    }

    virtual void AddMessage(const Message &msg)
    {
        mMessages.push_back(msg);
    }

    virtual void UpdateMember(std::uint32_t uuid, const Member &member, const std::string &event)
    {
        if (event == "")
        mMembers[uuid] = member;
    }

private:
    std::map<std::uint32_t, std::string> mTables;
    std::vector<IContext::Message> mMessages;
    std::map<std::uint32_t, IContext::Member> mMembers;
};


struct BasicClient : public Player
{
public:
    enum Event
    {
        JSON_ERROR,
        BAD_EVENT,
        REQ_LOGIN,
        ACCESS_GRANTED,
        MESSAGE,
        PLAYER_LIST,
    };

    BasicClient &operator = (const Deck &d)
    {
        Player::Deck::operator =(d);
        return *this;
    }

    bool TestDiscard(const Deck &discard);
    Contract CalculateBid();
    void UpdateStatistics();
    Card Play();
    bool IsValid(const Card &c);
    Deck AutoDiscard();

    // Network serializers
    void BuildChangeNickname(std::vector<Reply> &out);

    Deck::Statistics   mStats;   // statistics on player's cards
    Tarot::Game mGame;
    Tarot::Bid  mBid;
    Tarot::Distribution mShuffle;
    Deck mCurrentTrick;
    Deck mDog;
    std::uint8_t mNbPlayers;
    std::uint32_t mTableId;
    std::string mNickName;
    std::string mUserName;
    Place mPlace;           ///< Assigned Place
    std::uint32_t mUuid;    ///< Assigned UUID


    Event Decode(uint32_t src_uuid, uint32_t dest_uuid, const std::string &arg, IContext &ctx, std::vector<helper::Reply> &out);
};


} // End of namespace helper


#endif // NETHELPER_H
