#include <eosio/eosio.hpp>

using namespace eosio;

CONTRACT addressbook2: public contract{ 
    public:
        using contract::contract;

        ACTION eraseall() {
            require_auth( get_self() );

            counts forEraseAll( get_self(), get_self().value );

            auto itr = forEraseAll.begin();

            while( itr != forEraseAll.end() ) {
                itr = forEraseAll.erase( itr );
            }
            
        }


        ACTION findcount( uint64_t count ) {
            counts forFindcount( get_self(), get_self().value );

            auto forSecondary = forFindcount.get_index< "bycount"_n >();
            auto itr = forSecondary.find( count );

            if( itr!= forSecondary.end() ) {
                print( itr->user, " ", itr->count );
            }else {
                print("nononononononononono");
            }

            print( itr->count );
        }


        ACTION countuser( name user ) {
            require_auth( user );

            
            counts forCountuser( get_self(), get_self().value );
            auto itr = forCountuser.find( user.value );     // itr은 포인터

            if( itr != forCountuser.end() ) {
                forCountuser.modify( itr, user, [&]( auto& row ) {
                    row.count++;
                });

                print( user, " ", itr->count );
            }else {
                forCountuser.emplace( user, [&]( auto& row ) {
                    row.user = user;
                    row.count = 1;
                });

                print( user, " ", itr->count );
            }            
        }

        ACTION upsert( name user, uint64_t count ) {
            require_auth( user );
            counts forUpsert( get_self(), get_self().value );
            auto itr = forUpsert.find( user.value );

            if( itr == forUpsert.end() ) {
                forUpsert.emplace( user, [&]( auto& row ) {
                    row.user = user;
                    row.count = count;
                });

                print( "insert success" );

            }else {
                forUpsert.modify( itr, user, [&]( auto& row ) {
                    row.user = user;
                    row.count = count;
                });

                print( "modify success" );
            }
        }
    

        ACTION insert( name user, uint64_t count ) {
            require_auth( user );
            counts forInsert( get_self(), get_self().value ) ;                                            
            auto itr = forInsert.find( user.value );    
            check(itr == forInsert.end(), "already exists" );   

            forInsert.emplace( user, [&]( auto& row ) {                                                     
                row.user = user;
                row.count = count;
            });           

             print( "insert success" );
        }
        ACTION erase( name user ) {      
            require_auth( user );        
            counts forErase( get_self(), get_self().value );
            auto itr = forErase.require_find( user.value, "no account" );
            forErase.erase( itr );

            print( "erase success" );
        }
    private:
        TABLE person {
            name user;
            uint64_t count;

            uint64_t primary_key() const { return user.value; }
            uint64_t by_count() const { return count; } 
        };

        typedef multi_index< "counttable"_n, person,    // 블록체인 상에서 사용할 table 이름 counttable
        indexed_by< "bycount"_n, const_mem_fun< person, uint64_t, &person::by_count>> > counts;
        // secondary_index 이름 정의 ~~_n
        // 어떤 함수가 secondary_index를 반환하는지
        // typedef 를 counts로 줄여서 사용하겠다는 의미. 원래는 다 써야한다.
};