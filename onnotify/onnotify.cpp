#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

CONTRACT onnotify: public contract {
    public:
    using contract::contract;

    ACTION dummy() {}

    [[eosio::on_notify( "eosio.token::transfer" )]]
    void ontransfer( name from, name to, asset quantity, std::string memo ) {
        check( from == get_self(), "no out" );

        outs myTable( get_self(), get_self().value );    // table 선언, get_self().value로 스코프 하나로 고정
        
        if( myTable.begin() == myTable.end() ) {    // 아무것도 없는경우 새로운~~를 만들어줘야한다.
            myTable.emplace( from, [&]( auto& row ) {   // 위에서 from과 get_self()를 비교했기 때문에 from을 get_self()로 해도된다.
                row.balance = quantity;
            });      
        } else {
            auto itr = myTable.begin();     // 원소 하나 밖에 없기 때문에 find대신 begin으로 사용할 수 있다.
            myTable.modify( itr, from, [&]( auto& row ) {
                row.balance += quantity;
            });
        }
    }

    private:
    TABLE outstruct {
        asset balance;  // 내가 상대에게 얼마나 보냈는지에 대한 누적그맥

        uint64_t primary_key() const { return balance.symbol.code().raw(); };
    };

    typedef multi_index< "out"_n, outstruct > outs;
};