#include <eosio/eosio.hpp>

using namespace eosio;

CONTRACT money: public contract {
    public:
        using contract::contract;

        ACTION insert( name user ) {
            require_auth( user );

            stats forInsert( get_self(), get_self().value );
            auto itr = forInsert.find( user.value );

            check( itr == forInsert.end(), "already exists !" );

            forInsert.emplace( user, [&]( auto& row ) {
                row.user = user;
                row.mymoney = 0;
            });

            print( "insert success" );
        }

        ACTION income( name user, uint64_t mymoney ) {
            require_auth( user );

            stats forIncome( get_self(), get_self().value );
            auto itr = forIncome.find( user.value );

            check( itr != forIncome.end(), "no account !" );

            forIncome.modify( itr, user, [&]( auto& row ) {
                // row.user = user;
                row.mymoney += mymoney;
            });

            print( "income success" );
        }

        ACTION expenditure( name user, uint64_t mymoney ) {
            require_auth( user );

            stats forExpenditure( get_self(), get_self().value );
            auto itr = forExpenditure.find( user.value );

            check( itr != forExpenditure.end(), "no account !" );

            forExpenditure.modify( itr, user, [&]( auto& row ) {
                // row.user = user;
                row.mymoney -= mymoney;
            });

            print( "expenditure success" );
        }

        ACTION remove( name user ) {
            require_auth( get_self() );

            stats forRemove( get_self(), get_self().value );
            auto itr = forRemove.require_find( user.value, "no account !" );

            forRemove.erase( itr );

            print( "remove success" );
        }

    private:
        TABLE cog {
            name user;
            uint64_t mymoney;
            
            uint64_t primary_key() const { return user.value; }
        };

        typedef multi_index< "cog"_n, cog > stats;
};