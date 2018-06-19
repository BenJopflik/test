#pragma once

#include <gmock/gmock.h>

template <typename Error>
class ResultsError
{
public:
    virtual ~ResultsError() = default;

    virtual void setError(Error) = 0;

};

template <typename ResultsError>
class Results
{
public:
    virtual ~Results() = default;
    
    virtual ResultsError & getResults() = 0;

};

class Data
{
public:
    virtual ~Data() = default;
    
    virtual size_t size() = 0;
    virtual const uint8_t * data() = 0;

};

class DataMock : public Data
{
public:
    MOCK_METHOD0(size, size_t());
    MOCK_METHOD0(data, const uint8_t *());
};

template <typename Data>
class TransferParams
{
public:
    virtual ~TransferParams() = default;

    virtual bool hasSrc() = 0;
    virtual bool hasDst() = 0;

    virtual uint64_t getAmount() = 0;
    virtual Data & getSrc() = 0;
    virtual Data & getDst() = 0;

};

template <typename Data>
class ClientManagerParams
{
public:
    virtual ~ClientManagerParams() = default;

    virtual bool hasId() = 0;
    virtual Data getId() = 0;
    virtual uint64_t getAmount() = 0;

};

template <typename Results, typename Params>
class Context
{
public:
    virtual ~Context() = 0;

    virtual Results & getResults() = 0;
    virtual Params  & getParams() = 0;

};



