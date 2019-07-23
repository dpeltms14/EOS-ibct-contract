#include <eosio/eosio.hpp>

using namespace eosio;

CONTRACT autokey: public contract {
    public:
        using contract::contract;

        ACTION insertkey() {
            require_auth( get_self() ); //주석으로 바꿔주면 모든 계정이 접근할 수 있다.

            Autokey myTable( get_self(), get_self().value );
            myTable.emplace( get_self(), [&]( auto& row ) {
                row.mykey = myTable.available_primary_key();    //mykey가 1씩 증가된 값이 저장된다.
            });
        }

    private:
        TABLE example_struct {
            uint64_t mykey;

            uint64_t primary_key() const { return mykey; }
        };

        typedef multi_index< "autokeys"_n, example_struct > Autokey;
};