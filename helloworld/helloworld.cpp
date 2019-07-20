#include <eosiolib/eosio.hpp>

using namespace eosio;

CONTRACT helloworld: public contract {
    public:
        using contract::contract;

        ACTION hi( name user ) {
            require_auth( user );
            print( "hello, ", user );
        }

        ACTION getaccount( name user ) {
            require_auth( get_self() );
            print( is_account( user ) );
        }

        // ACTION gettimenow(){
        //     require_auth( get_self() );
        //     print( current_time() );
        // }

    private:
};