// Copyright (C) 2018. Huawei Technologies Co., Ltd. All rights reserved.

#ifndef GTEST_INCLUDE_GTEST_GTEST_TAG_H_
#define GTEST_INCLUDE_GTEST_GTEST_TAG_H_

#include <string>
#include <map>
#include <vector>

namespace testing {
  namespace ext {

    using ::std::string;
    using ::std::map;
    using ::std::pair;
    using ::std::vector;

    // base class of tag flag::bitwise integers
    class TestFlag {
    public:
        static const int None = 0;
    private:
        const char* const name;
        const char* const desc;
        const int mask;
        map<int, const char*> elementMap;
        int eleCount;
    protected:
        TestFlag(const char*  n, const char* d, int m);
        void element(const char* desc, int hex);
    public:
        bool verify(const int hex, char* err) const;
        const char* naming() const { return name; }
        const char* description() const { return desc; }
        bool eleForName(const char* name, int& result) const;
        void printHelp(const char** indents) const;
    };

    // test size scope
    class  TestSizeSet : public TestFlag {
    public:
        TestSizeSet();
        static const int Level0 = 1  << 24;
        static const int Level1 = 2  << 24;
        static const int Level2 = 4  << 24;
        static const int Level3 = 8  << 24;
        static const int Level4 = 16 << 24;
    };

    extern const TestSizeSet TestSize;

    // get each instance of all the TestFlag implementions
    const vector<const TestFlag*>& allHextTagSets();
    // verify the test flagset, returns false if the flagset is illegal
    bool checkFlagsLegality(int flags);
    // convert name string to test flag value
    bool flagForName(const char* set_name, const char* ele_name, int& result);

  } // namespace ext
} // namespace testing

#endif  // GTEST_INCLUDE_GTEST_GTEST_TAG_H_
