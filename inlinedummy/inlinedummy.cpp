#include <eosio/eosio.hpp>

using namespace eosio;

CONTRACT inlinedummy: public contract {
    public:
    using contract::contract;

    ACTION dummy( name user ) {
        require_auth ( user );

        print( "dummy" );

        action(
            //permission,   // 액션 수행하는 주체 설정, ( 계정이름, 권한이름 )
            permission_level( user, "active"_n ),

            //contract name,    // 컨트랙트 이름을 바꿔줘야한다.
            get_self(),

            //action name,
            "dummytwo"_n,

            //parameters
            std::make_tuple( user )
        ).send();        //이러한 기본적인 형태를 가진다.
    }

    ACTION dummytwo( name user ) {
        require_auth( user );
        
        print( "dummy two, ", user );   // dummy 안에서 dummytwo가 실행되는 것을 확인하기 위해
    }

    private:
};