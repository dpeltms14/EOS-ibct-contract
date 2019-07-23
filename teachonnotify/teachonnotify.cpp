#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

CONTRACT teachonnotify: public contract {
    public:
    using contract::contract;

    ACTION dummy() {}

    [[eosio::on_notify("eosio.token::transfer")]]
    void ontransfer(name from, name to, asset quantity, std::string memo) {
        if(from == get_self()) {
            Receiver receiver(get_self(), get_self().value);
            auto itr = receiver.find(to.value);
            if(itr == receiver.end()) {
                receiver.emplace(from, [&](auto& row) {
                    row.user = to;
                    row.balance = quantity;
                });
            } else {
                receiver.modify(itr, from, [&](auto& row) {
                    row.balance += quantity;
                });
            }
        } else {
            Sender Sender(get_self(), get_self().value);
            auto itr = Sender.find(from.value);
            if(itr == Sender.end()) {
                Sender.emplace(to, [&](auto& row) {
                    row.user = from;
                    row.balance = quantity;
                });
            } else {
                Sender.modify(itr, to, [&](auto& row) {
                    row.balance += quantity;
                });
            }
        }
    }

    private:
    TABLE trxlist {
        name user;
        asset balance;

        uint64_t primary_key() const { return user.value; }
    };

    typedef multi_index<"eossender"_n, trxlist> Sender;
    typedef multi_index<"eosreceiver"_n, trxlist> Receiver;
};
