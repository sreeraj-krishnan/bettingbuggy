

ACCOUNT=$1
CONTRACT=$2

if [[ $# -ne 2 ]]; then

	echo "USAGE: comptract.sh <ACCOUNT NAME> <CONTRACT NAME> from within the dir"
	exit 1
fi

eosiocpp -o ${CONTRACT}.wast ${CONTRACT}.cpp 
eosiocpp -g ${CONTRACT}.abi ${CONTRACT}.cpp 

#alias cleos='~/eos/build/programs/cleos/cleos --wallet-url=http://localhost:8899 --url http://kylin.fn.eosbixin.com '

~/eos/build/programs/cleos/cleos --wallet-url=http://localhost:8899 --url http://kylin.fn.eosbixin.com set contract ${ACCOUNT} ../${CONTRACT} ${CONTRACT}.wasm ${CONTRACT}.abi -p ${ACCOUNT}@active


