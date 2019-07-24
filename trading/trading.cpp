#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

CONTRACT trading: public contract {
    public:
    using contract::contract;

    ACTION dummy() {}   // 액션이 없는경우 dummy를 하나 생성해줘야한다.

    ACTION eraseall() {
        require_auth( get_self() );

        sReceiver forReceiveritr( get_self(), get_self().value );
        auto sReceiveritr = forReceiveritr.begin();
        
        while( sReceiveritr != forReceiveritr.end() ) {     // 처음부터 끝까지 돌면서 삭제
            receiver forreceiverErase( get_self(), sReceiveritr->sco );     // scope는 
            auto receiveritr = forreceiverErase.begin();
            
            while( receiveritr != forreceiverErase.end() ) {
                receiveritr = forreceiverErase.erase( receiveritr );
            }

            sReceiveritr = forReceiveritr.erase( sReceiveritr );
        }

        sSender forSenderitr( get_self(), get_self().value );
        auto sSenderitr = forSenderitr.begin();

        while( sSenderitr != forSenderitr.end() ) {
            sender forsenderErase( get_self(), sSenderitr->sco );
            auto senderitr = forsenderErase.begin();

            while( senderitr != forsenderErase.end() ) {
                senderitr = forsenderErase.erase( senderitr );
            }

            sSenderitr = forSenderitr.erase( sSenderitr );
        }
    }

    [[eosio::on_notify( "eosio.token::transfer" )]]
    void ontransfer( name from, name to, asset quantity, std::string memo ) {
        if( from == get_self() ) {
            sReceiver forSreceiver( get_self(), get_self().value );
            auto empReceiveritr = forSreceiver.find( to.value );

            if( empReceiveritr == forSreceiver.end() ) {
                forSreceiver.emplace( get_self(), [&]( auto& row ) {
                    row.sco = to.value;
                });
            }

            receiver forReceiver( get_self(), to.value );

            forReceiver.emplace( get_self(), [&]( auto& row ) {
                row.mykey = forReceiver.available_primary_key();  //mykey가 1씩 증가
                row.user = to;
                row.balance = quantity;
            });


        } else {
            sSender forSsender( get_self(), get_self().value );
            auto empSenderitr = forSsender.find( from.value );

            if( empSenderitr == forSsender.end() ) {
                forSsender.emplace( get_self(), [&]( auto& row ) {
                    row.sco = from.value;
                });
            }

            sender forSender( get_self(), from.value );

            forSender.emplace( get_self(), [&]( auto& row ) {
                row.mykey = forSender.available_primary_key();
                row.user = from;
                row.balance = quantity;
            });

        }
    }        

    private:
    TABLE insert {
        uint64_t mykey;     // 부호가 없는 64비트로 선언
        name user;
        asset balance;
        
        uint64_t primary_key() const { return mykey; }
    };

    TABLE savesco {
        uint64_t sco;

        uint64_t primary_key() const { return sco; }
    };

    typedef multi_index< "receiver"_n, insert > receiver;
    typedef multi_index< "sreceiver"_n, savesco > sReceiver;
    typedef multi_index< "sender"_n, insert > sender;
    typedef multi_index< "ssender"_n, savesco > sSender;
};