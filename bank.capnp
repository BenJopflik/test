
@0x94f8245178b1f6c1;

# add namespace 
#using Cxx = import "/capnp/c++.capnp";
#$Cxx.namespace("L3");

interface Transfer
{
	enum ErrorCode
	{
		ok @0;
		invalidSrc @1;
		invalidDst @2;
		insufficientFunds @3;
        	malformed @4;
	}	

	transfer @0 (src : Data, dst : Data, amount : UInt64) -> (error : ErrorCode);
}

interface ClientManager
{
	enum ErrorCode
	{
		ok @0;
		alreadyExists @1;
      	malformed @2;
	}
	
	createUser @0 (id : Data, amount : UInt64) -> (error : ErrorCode);
	deleteUser @1 (id : Data) -> (error : ErrorCode);
}
