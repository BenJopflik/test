
@0x94f8245178b1f6c1;

interface Bank
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
	
	create @0 (id : Data, amount : UInt64) -> (error : ErrorCode);
}
