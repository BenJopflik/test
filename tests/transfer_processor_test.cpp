#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <transfer_processor.hpp>
#include "capnp_mock.hpp"

using ::testing::Return;
using ::testing::_;

class BankInterface
{
public:
    enum class ErrorCode : uint16_t
    {
        OK = 0,
        INVALID_SRC,
        INVALID_DST,
        INSUFFICIENT_FUNDS,
        EXCEPTION
    };

 
public:
    virtual ~BankInterface() = default;

    virtual ErrorCode transfer(const uint8_t * src, const uint8_t * dst, const uint64_t amount) = 0;
};


class TransferParamsMock : public TransferParams<DataMock>
{
public:
//    TransferParamsMock(std::unique_ptr<DataMock> data) : m_data(std::move(data)) {}
    
    MOCK_METHOD0(hasSrc, bool());
    MOCK_METHOD0(hasDst, bool());

    MOCK_METHOD0(getSrc, DataMock & ());
    MOCK_METHOD0(getDst, DataMock & ());

    MOCK_METHOD0(getAmount, uint64_t());

//private:
//    std::unique_ptr<DataMock> m_data;

};

class ResultsErrorMock : public ResultsError<::Transfer::ErrorCode>
{
public: 
   MOCK_METHOD1(setError, void(::Transfer::ErrorCode));

};

class ResultsMock : public Results<ResultsErrorMock>
{
//    using Base = Results<BankInterface::ErrorCode>;

public:
//    ResultsMock(std::unique_ptr<ResultsErrorMock> results_error_mock) : m_results_error_mock(std::move(results_error_mock)) {}

    MOCK_METHOD0(getResults, ResultsErrorMock & ());

//private:
//    std::unique_ptr<ResultsErrorMock> m_results_error_mock;
};



class ContextMock : public Context<ResultsMock, TransferParamsMock>
{
public:
//    ContextMock(std::unique_ptr<ResultsMock> results, std::unique_ptr<TransferParamsMock> params) : m_results(std::move(results)), m_params(std::move(params)) {}

    MOCK_METHOD0(getResults, ResultsMock & ());
    MOCK_METHOD0(getParams, TransferParamsMock & ());

//private:
//    std::unique_ptr<ResultsMock> m_results;
//    std::unique_ptr<TransferParamsMock> m_params;
//
};



class BankMock : public BankInterface
{
public:
    MOCK_METHOD3(transfer, BankInterface::ErrorCode(const uint8_t *, const uint8_t *, const uint64_t));
};

TEST (TransactionProcessor, Transfer)
{
    auto bank = std::make_shared<BankMock>();
    
    EXPECT_CALL(*bank, transfer(_, _, 100)).WillOnce(Return(BankInterface::ErrorCode::OK))
                                           .WillOnce(Return(BankInterface::ErrorCode::INVALID_SRC))
                                           .WillOnce(Return(BankInterface::ErrorCode::INVALID_DST))
                                           .WillOnce(Return(BankInterface::ErrorCode::INSUFFICIENT_FUNDS))
                                           .WillOnce(Return(BankInterface::ErrorCode::EXCEPTION));

    // configure datamock
    DataMock data;
    EXPECT_CALL(data, size()).WillOnce(Return(false))
                             .WillOnce(Return(true))
                             .WillOnce(Return(true))
                             .WillOnce(Return(false))
                             .WillRepeatedly(Return(true));
    
    EXPECT_CALL(data, data()).WillRepeatedly(Return(nullptr));

    // configure params
    TransferParamsMock params;//(std::move(data));
    
    EXPECT_CALL(params, hasSrc()).WillOnce(Return(false))
                                 .WillRepeatedly(Return(true));

    EXPECT_CALL(params, hasDst()).WillOnce(Return(true))
                                 .WillOnce(Return(false))
                                 .WillRepeatedly(Return(true));

    EXPECT_CALL(params, getSrc()).WillRepeatedly(Return(data));
    EXPECT_CALL(params, getDst()).WillRepeatedly(Return(data));

    EXPECT_CALL(params, getAmount()).WillRepeatedly(Return(100));

    // configure results_error
    
    ResultsErrorMock results_error;
    EXPECT_CALL(results_error, setError(::Transfer::ErrorCode::OK));
    EXPECT_CALL(results_error, setError(::Transfer::ErrorCode::INVALID_SRC));
    EXPECT_CALL(results_error, setError(::Transfer::ErrorCode::INVALID_DST));
    EXPECT_CALL(results_error, setError(::Transfer::ErrorCode::INSUFFICIENT_FUNDS));
    
    // configure results
    
    ResultsMock results;//(std::move(results_error));
    EXPECT_CALL(results, getResults()).WillRepeatedly(Return(results_error));

    // configure context
    ContextMock context;//(std::move(results), std::move(params));
    EXPECT_CALL(context, getParams()).WillRepeatedly(Return(params));
    EXPECT_CALL(context, getResults()).WillRepeatedly(Return(results));



    TransactionProcessor<BankMock> processor(bank);


    processor.transfer(context);
    processor.transfer(context);
    processor.transfer(context);
    processor.transfer(context);

}



