#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

CONTRACT autokey2: public contract {
    public:
    using contract::contract;

    ACTION dummy() {}

    ACTION eraseall() {
        require_auth( get_self() );
        To toEraseall( get_self(), get_self().value );
        From fromEraseall( get_self(), get_self().value );

        auto itr = toEraseall.begin();
        auto itr2 = fromEraseall.begin();

        while( itr != toEraseall.end() ) {
            itr = toEraseall.erase( itr );
        }
        while( itr2 != fromEraseall.end() ) {
            itr2 = fromEraseall.erase( itr2 );
        }

    }

    [[eosio::on_notify("eosio.token::transfer")]]
    void ontransfer(name from, name to, asset quantity, std::string memo) {
        if( from == get_self() ) {
            To forTo( get_self(), get_self().value );

            forTo.emplace( from, [&]( auto& row ) {
                row.mykey = forTo.available_primary_key();  //mykey가 1씩 증가
                row.user = to;
                row.balance = quantity;
            });
        } else {
            From forFrom( get_self(), get_self().value );

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

    typedef multi_index< "autoto"_n, insert > To;
    typedef multi_index< "autofrom"_n, insert > From;
};