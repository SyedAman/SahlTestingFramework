# TestingFramework

A unit testing framework for C++ that includes:
* Test running
* Test specification
* Expectations
* Mocking

The heavy usage of macros takes inspiration from Google Test. Although the implementation is different, the interface for creating tests and mocking has similaries to gtest and gmock. 

## Example Usage
You can take a look at the automation tests under the `/Tests` directory for the most up-to-date self-documentation. But here are some test usages if you don't want to do that:

```cpp
class SomeInterface
{
public:
    virtual int SomeMethod() = 0;
    virtual int SomeOtherMethod() = 0;
    virtual bool SomeBoolMethod() = 0;
    virtual std::string SomeStringMethod() = 0;
};

class MockImplementation : public SomeInterface
{
    MOCK_METHOD(int, SomeMethod)
    MOCK_METHOD(int, SomeOtherMethod)
    MOCK_METHOD(bool, SomeBoolMethod)
    MOCK_METHOD(std::string, SomeStringMethod)
};

S_TEST(MyTestSuite, MyTest)
{
    MockImplementation mockImplementation;
    ON_CALL(mockImplementation, SomeMethod).WillByDefault([]() { return 5; });
    S_EXPECT_EQ(mockImplementation.SomeMethod(), 5);

    ON_CALL(mockImplementation, SomeOtherMethod).WillByDefault([]() { return 6; });
    S_EXPECT_EQ(mockImplementation.SomeOtherMethod(), 6);

    ON_CALL(mockImplementation, SomeBoolMethod).WillByDefault([]() { return true; });
    S_EXPECT_TRUE(mockImplementation.SomeBoolMethod());
    ON_CALL(mockImplementation, SomeBoolMethod).WillByDefault([]() { return false; });
    S_EXPECT_FALSE(mockImplementation.SomeBoolMethod());

    ON_CALL(mockImplementation, SomeStringMethod).WillByDefault([]() { return "Hello"; });
    S_EXPECT_STREQ(mockImplementation.SomeStringMethod().c_str(), "Hello");
}
```

# Installation

You will need to following dependencies:
* [CMake](https://cmake.org/download/)
* [GCC/G++ (Linux)](https://gcc.gnu.org/install/)
* [MSVC (Windows)](https://visualstudio.microsoft.com/downloads/)
* [xlib w/ XRandR and XTest (Linux)](https://www.x.org/wiki/)


## Linux
```bash
sudo apt-get install libxrandr-dev
sudo apt-get install libxtst-dev

cd SahlTestingFramework
cmake -B build
cmake --build build

# Run tests to make sure everything is working
cd build
ctest --extra-verbose
```
