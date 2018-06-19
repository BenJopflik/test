#include <gtest/gtest.h>

#include <bank.hpp>


TEST (Bank, CreateDeleteUser)
{
	Bank bank;

	uint64_t user_1[] = {0x1, 0x2, 0x3, 0x4};		
	
	EXPECT_EQ(Bank::ErrorCode::OK, bank.create_user(reinterpret_cast<const uint8_t *>(user_1), 100));
	EXPECT_EQ(Bank::ErrorCode::ALREADY_EXISTS, bank.create_user(reinterpret_cast<const uint8_t *>(user_1), 100));
	bank.delete_user(reinterpret_cast<const uint8_t *>(user_1));
	EXPECT_EQ(Bank::ErrorCode::OK, bank.create_user(reinterpret_cast<const uint8_t *>(user_1), 1));
}


TEST (Bank, Transfer)
{
	Bank bank;

	uint64_t user_1[] = {0x1, 0x2, 0x3, 0x4};		
	uint64_t user_2[] = {0x2, 0x3, 0x4, 0x1};		

	EXPECT_EQ(Bank::ErrorCode::INVALID_SRC, bank.transfer(reinterpret_cast<const uint8_t *>(user_1), reinterpret_cast<const uint8_t *>(user_2), 100));

	bank.create_user(reinterpret_cast<const uint8_t *>(user_1), 100);
	
	EXPECT_EQ(Bank::ErrorCode::INVALID_DST, bank.transfer(reinterpret_cast<const uint8_t *>(user_1), reinterpret_cast<const uint8_t *>(user_2), 100));

	bank.create_user(reinterpret_cast<const uint8_t *>(user_2), 0);
	EXPECT_EQ(Bank::ErrorCode::OK, bank.transfer(reinterpret_cast<const uint8_t *>(user_1), reinterpret_cast<const uint8_t *>(user_2), 100));
	EXPECT_EQ(Bank::ErrorCode::INSUFFICIENT_FUNDS, bank.transfer(reinterpret_cast<const uint8_t *>(user_1), reinterpret_cast<const uint8_t *>(user_2), 100));
	EXPECT_EQ(Bank::ErrorCode::OK, bank.transfer(reinterpret_cast<const uint8_t *>(user_2), reinterpret_cast<const uint8_t *>(user_1), 10));
	EXPECT_EQ(Bank::ErrorCode::OK, bank.transfer(reinterpret_cast<const uint8_t *>(user_1), reinterpret_cast<const uint8_t *>(user_2), 5));
}


