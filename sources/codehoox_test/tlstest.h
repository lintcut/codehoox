#pragma once

namespace CodeHooxTest {

class TlsInstance
{
public:
    TlsInstance();
    ~TlsInstance();

    bool initialized();

private:
    volatile long initFlag;
};

}