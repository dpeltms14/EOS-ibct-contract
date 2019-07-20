#include <eosio/eosio.hpp>

using namespace eosio;

CONTRACT sayhi: public contract {
    public:
        using contract::contract;

        ACTION hi( name user ) {
            require_auth( user );       // 인자로 받은 계정과 다르면 권한에러 출력
            say_hi forHi( get_self(), get_self().value );            
            auto itr = forHi.require_find( user.value, "please, insert account" );

            print("Hi, ", user);
        }

        ACTION insert( name user ) {
            require_auth( get_self() );
            //check( has_auth( get_self() ), "You are not Contracter" );

            say_hi forInsert( get_self(), get_self().value );
            auto itr = forInsert.find( user.value );

            check( itr == forInsert.end(), "already exists" );

            forInsert.emplace( get_self(), [&]( auto& row ) {
                row.user = user;
            });

            print( "insert success" );
        }

        ACTION remove(name user) {
            check( has_auth( get_self() ), "You are not Contracter" );

            say_hi forRemove( get_self(), get_self().value );
            auto itr = forRemove.require_find( user.value, "no account" );
            forRemove.erase(itr);

            print( "remove success" );
        }

    private:
        TABLE allowance {
            name user;
            
            uint64_t primary_key() const { return user.value; }
        };

        typedef multi_index< "allowance"_n, allowance > say_hi;
};