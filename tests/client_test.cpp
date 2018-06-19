#include <gtest/gtest.h>

#include <client.hpp>

TEST (Client, Ctor)
{
	{
		Client client;	
		EXPECT_EQ(client.balance(), 0);
	}

	{
		Client client(100);	
		EXPECT_EQ(client.balance(), 100);
	}
}

TEST (Client, Update)
{
	Client client(100);
	EXPECT_EQ(true, client.withdraw(1));
	EXPECT_EQ(true, client.withdraw(99));
	EXPECT_EQ(false, client.withdraw(99));
	client.deposit(100);
	EXPECT_EQ(true, client.withdraw(99));
}
