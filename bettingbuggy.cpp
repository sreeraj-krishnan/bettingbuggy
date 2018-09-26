#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include "random.h"


using namespace std;
using namespace eosio;

class bettingbuggy : public eosio::contract {
  public:
	using contract::contract;


      	bettingbuggy(account_name self):contract(self) {}

     	 // @abi
      	void betrandom(account_name user, uint64_t guess){
        	 print( "bettingbuggy, ", name{user} );
		 require_auth(user);
		 if( guess == random_gen::get_instance(user).range(10))
		 {
			 	
		 }
        }
  private:
};

EOSIO_ABI( bettingbuggy, (betrandom) );
