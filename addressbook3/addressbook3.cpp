#include <eosio/eosio.hpp>

using namespace eosio;

CONTRACT addressbook3: public contract{ 
    public:
        using contract::contract;

        ACTION upsert( name user, std::string first_name, std::string last_name, uint64_t age ) {
            require_auth( user );
            address_index forUpsert( get_first_receiver(), get_first_receiver().value );
            auto itr = forUpsert.find( user.value );

            if( itr == forUpsert.end() ) {
                forUpsert.emplace( user, [&]( auto& row ) {
                    row.user = user;
                    row.first_name = first_name;
                    row.last_name = last_name;
                    row.age = age;
                });
                send_summary( user, " successfully emplaced record to addressbook" );

            }else {
                forUpsert.modify( itr, user, [&]( auto& row ) {
                    row.user = user;
                    row.first_name = first_name;
                    row.last_name = last_name;
                    row.age = age;
                });

                send_summary( user, " successfully modified record to addressbook" );
            }
        }
    
        ACTION erase( name user ) {      
            require_auth( user );        
            address_index forErase( get_first_receiver(), get_first_receiver().value );

            auto itr = forErase.find( user.value );
            check(itr != forErase.end(), "Record does not exist");
            forErase.erase( itr );

            send_summary( user, " successfully erased record from addressbook" );
        }

        ACTION notify( name user, std::string msg ) {
            require_auth(get_self());
            require_recipient( user );
        }

    private:
        TABLE person {
            name user;
            std::string first_name;
            std::string last_name;
            uint64_t age;

            uint64_t primary_key() const { return user.value; }
            uint64_t by_age() const { return age; }  // secondary_key_age()를 by_age() 도 가능
        };

        void send_summary( name user, std::string message ) {
            action(
                permission_level{ get_self(),"active"_n },
                get_self(),
                "notify"_n,
                std::make_tuple( user, name{ user }.to_string() + message )
            ).send();
        };

        typedef multi_index< "peoplethree"_n, person, // people 은 table
        indexed_by< "byage"_n, const_mem_fun< person, uint64_t, &person::by_age>> > address_index;
};