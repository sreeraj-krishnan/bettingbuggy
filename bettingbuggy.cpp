#include <utility>
#include <string>
#include <cstdlib>
#include <eosiolib/eosio.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/contract.hpp>
#include <eosiolib/crypto.h>
#include <eosiolib/print.hpp>

using eosio::key256;
using eosio::indexed_by;
using eosio::asset;
using eosio::permission_level;
using eosio::action;
using eosio::print;
using eosio::name;

using namespace std;
using namespace eosio;

uint32_t get_random_number(account_name player, uint32_t to)
{
        uint64_t seed = current_time() + player;
        checksum256 result;
        sha256((char *)&seed, sizeof(seed), &result);
        seed = result.hash[1];
        seed <<= 32;
        seed |= result.hash[0];
        return (uint32_t)(seed % to);
}

class bettingbuggy : public eosio::contract {
  public:
	using contract::contract;


      	bettingbuggy(account_name self):
	  contract(self),
	  accountsg(_self, _self)       
	{}

	
	// @abi action
	void withdraw(account_name to, asset quantity)
	{
		require_auth(to);
                eosio_assert( quantity.is_valid(), "invalid quantity" );
                eosio_assert( quantity.amount > 0, "amount > 0" );
		
		auto itr = accountsg.find(to);
		eosio_assert( itr != accountsg.end(), "cannot find account");
		eosio_assert( itr->eos_balance.amount >= quantity.amount , "not sufficient balance");
		
		action(
                       	permission_level{ _self, N(active) },
                       	N(eosio.token), N(transfer),
                       	std::make_tuple(_self, to, quantity , std::string("amount successfully withdrawn!"))
                ).send();

		accountsg.modify( itr, 0, [&]( auto& acnt ) {
        		acnt.eos_balance - quantity;
	        });
			
	}

	// @abi action
	void deposit(account_name user, asset quantity)
	{
		require_auth(user);
                eosio_assert( quantity.is_valid(), "invalid quantity" );
                eosio_assert( quantity.amount > 0, "amount > 0" );
		
		auto itr = accountsg.find(user);
         	if( itr == accountsg.end() )
	       	{
            		itr = accountsg.emplace(_self, [&](auto& acnt){
               			acnt.owner = user;
            		});
         	}
		 action(
                                permission_level{ user, N(active) },
                                N(eosio.token), N(transfer),
                                std::make_tuple(user, _self, quantity, std::string("betting stakes from user"))
                              ).send();
		
		 accountsg.modify( itr, 0, [&]( auto& acnt ) {
            		acnt.eos_balance += quantity;
         	});	
			
	}
		

     	 // @abi action
      	void betrandom(account_name user, asset quantity , string guess)
	{
		require_auth(user);
                eosio_assert( quantity.is_valid(), "invalid quantity" );
                eosio_assert( quantity.amount > 0, "amount > 0" );
		
		auto itr = accountsg.find(user);

		eosio_assert( itr != accountsg.end() , "user has no deposit");
		eosio_assert( itr->eos_balance.amount >= quantity.amount , "insufficient balance to bet");
		
		// staked amount needs to be deducted from balance
		accountsg.modify( itr, 0, [&]( auto& acnt ) {
            			acnt.eos_balance -= quantity;
	        });

		uint32_t random_value=  get_random_number(user,10);
		
		if( atoi(guess.c_str()) == random_value)
		{
			// deposit the winnings
			accountsg.modify( itr, 0, [&]( auto& acnt ) {
        	    		acnt.eos_balance += 2*quantity;
	        	});
			withdraw(user, 2*quantity);
		}
        }
     	 
	// @abi action
      	void buggybet(account_name user, asset quantity , string guess)
	{
		require_auth(user);
		
		uint32_t random_value=  get_random_number(user,10);
		
		if( atoi(guess.c_str()) == random_value)
		{
			action(
                        	permission_level{ _self, N(active) },
                        	N(eosio.token), N(transfer),
                        	std::make_tuple(_self,user, quantity*2 , std::string("You win double the staked EOS !!"))
                        ).send();

		}
        }


  private:
	 
	 //@abi table accountsb i64
	 struct accountsb {
         accountsb( account_name o = account_name() ):owner(o){}

         account_name owner;
         asset  eos_balance;

         bool is_empty()const { return !( eos_balance.amount ); }

         uint64_t primary_key()const { return owner; }

         EOSLIB_SERIALIZE( accountsb, (owner)(eos_balance) )
      };

       typedef eosio::multi_index< N(accountsb), accountsb> account_index;
       account_index     accountsg;
};

EOSIO_ABI( bettingbuggy, (betrandom)(deposit)(withdraw)(buggybet) )
