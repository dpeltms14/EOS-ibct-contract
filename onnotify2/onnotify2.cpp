#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

CONTRACT onnotify2: public contract {
    public:
    using contract::contract;

    ACTION dummy() {}

    [[eosio::on_notify2( "eosio.token::transfer" )]]
    void ontransfer( name from, name to, asset quantity, std::string memo ) {
        if( from == get_self() ) {
            sender forSend( get_self(), get_self().value );

            auto itr = forSend.find( to.value );    // 해당하는 값을 찾지 못하면 itr는 end를 반환한다.

            if( itr == forSend.end() ) {
                forSend.emplace( from, [&]( auto& row ) {    // ( 새로운 객체를 위해 ram을 지불할 계정, 콜백함수 )
                    row.user = to;
                    row.balance = quantity;
                });
            } else {
                forSend.modify( itr, from, [&]( auto& row ) {
                    row.balance += quantity;
                });
            }
        } else {
            receiver forReceive( get_self(), get_self().value );

            auto itr = forReceive.find( from.value );

            if( itr == forReceive.end() ) {
                forReceive.emplace( to, [&]( auto& row ) {
                    row.user = from;    // 받는건데 from이 필요할까?
                    row.balance = quantity;
                });
            } else {
                forReceive.modify( itr, to, [&]( auto& row ) {
                    row.balance += quantity;
                });
            }
        }
    }

    private:
    TABLE tofromlist{
        name user;
        asset balance;  // 총 보낸, 총 받은 값 저장

        uint64_t primary_key() const { return user.value; }
    };

    typedef multi_index< "tolist"_n, tofromlist > receiver;
    typedef multi_index< "fromlist"_n, tofromlist > sender;
};