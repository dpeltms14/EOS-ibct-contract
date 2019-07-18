#include <eosio/eosio.hpp>

using namespace eosio;

CONTRACT addressbook: public contract{ 
    public:
        using contract::contract;

        ACTION upsert( name user, std::string first_name, std::string last_name, uint32_t age ) {
            require_auth( user );

            address_index forUpsert( get_self(), get_self().value );

            auto itr = forUpsert.find( user.value );

            if( itr == forUpsert.end() ) {
                forUpsert.emplace( user, [&]( auto& row ) {
                    row.user = user;
                    row.first_name = first_name;
                    row.last_name = last_name;
                    row.age = age;
                });
                print( "insert success" );

            }else {
                forUpsert.modify( itr, user, [&]( auto& row ) {
                    row.user = user;
                    row.first_name = first_name;
                    row.last_name = last_name;
                    row.age = age;
                });
                print( "modify success" );
            }
        }
    

        ACTION insert( name user, std::string first_name, std::string last_name, uint32_t age ) {
            require_auth( user );

            address_index forInsert( get_self(), get_self().value ) ;  // table에 접속할 수 있게 하는 코딩
                                                                    // 유저를 추가하기 이전에 이 유저가 존재하는지 검사해야한다.
            auto itr = forInsert.find( user.value );    // find 명령은 포인터를 옮겨가며 가장 마지막까지 이동하게된다.
                                                        // 중간에 해당하는 값을 찾으면 itr이 end가 되지 않는다.

            check(itr == forInsert.end(), "already exists" );    // 조건을 만족해야 다음 명령 수행.
                                                                // 만족 못하면 "already exists" 출력
            forInsert.emplace( user, [&]( auto& row ) {            // [&](auth& row) 는 콜백함수
                                                                // emplace로 정보를 저장
                row.user = user;
                row.first_name = first_name;
                row.last_name = last_name;
                row.age = age;
            });                                                    
             print( "insert success" );
        }
        ACTION erase( name user ) {       // 삭제하기
            require_auth( user );         // 인자와 같은 계정인지 확인

            address_index forErase( get_self(), get_self().value );  // get_self<=>"baekseokinit" 도 가능
            auto itr = forErase.require_find( user.value, "no account" );
            forErase.erase( itr );

            print( "erase success" );
        }
    private:
        TABLE person {
            name user;
            std::string first_name;
            std::string last_name;
            uint32_t age;

            uint64_t primary_key() const { return user.value; }
        };

        typedef multi_index< "people"_n, person > address_index;
};