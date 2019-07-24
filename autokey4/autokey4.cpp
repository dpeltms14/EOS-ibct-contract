#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

CONTRACT autokey4: public contract {
    public:
    using contract::contract;

    ACTION dummy() {}

    [[eosio::on_notify( "eosio.token::transfer" )]]
    void ontransfer( name from, name to, asset quantity, std::string memo ) {
        if( from == get_self() ) {
            To forTo( get_self(), to.value );

            forTo.emplace( from, [&]( auto& row ) {
                row.mykey = forTo.available_primary_key();  //mykey가 1씩 증가
                row.user = to;
                row.balance = quantity;
            });
        } else {
            From forFrom( get_self(), from.value );

            forFrom.emplace( to, [&]( auto& row ) {
                row.mykey = forFrom.available_primary_key();
                row.user = from;
                row.balance = quantity;
            });
        }
    }        

    private:
    TABLE insert {
        uint64_t mykey;     // 부호가 없는 64비트로 선언
        name user;
        asset balance;

        uint64_t primary_key() const { return mykey; }
    };

    typedef multi_index< "totototo"_n, insert > To;
    typedef multi_index< "fromfrom"_n, insert > From;
};