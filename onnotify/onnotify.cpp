#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

CONTRACT onnotify: public contract {
    public:
    using contract::contract;

    ACTION dummy() {}

    [[eosio::on_notify( "eosio.token::transfer" )]]
    void ontransfer( name from, name to, asset quantity, std::string memo ) {
        if( from == get_self() ) {
            outs outTable( get_self() , get_self().value );

            auto itr = outTable.find( to.value );

            if( itr == outTable.end() ) {
                outTable.emplace( from, [&]( auto& row ) {
                    row.user = to;
                    row.balance = quantity;
                });
            } else {
                outTable.modify( itr, from, [&]( auto& row ) {
                    row.balance += quantity;
                });
            }

            print( "give success" );
        } else {
            ins inTable( get_self() , get_self().value );

            auto itr = inTable.find( from.value );  // 이부분에서 잘 선언해줘야 여러 user의 정보를 저장할 수 있다.

            if( itr == inTable.end() ) {
                inTable.emplace( to, [&]( auto& row ) {
                    row.user = from;
                    row.balance = quantity;
                });
            } else {
                inTable.modify( itr, to, [&]( auto& row ) {
                    row.balance += quantity;
                });
            }

            print( "get success" );
        }
    }

    private:
    TABLE outstruct {
        name user;
        asset balance;  // 내가 상대에게 얼마나 보냈는지에 대한 누적금액

        uint64_t primary_key() const { return user.value; } // 심볼별로 계정을 분류하면 얼마를 주고받았는지 모르기 때문에
                                                            // 테이블 내에 계정별로 분류되는 이유가 된다.
    };

    typedef multi_index< "out2"_n, outstruct > outs;
    typedef multi_index< "in2"_n, outstruct > ins;
};