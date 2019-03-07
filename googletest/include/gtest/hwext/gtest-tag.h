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

    // platform scope
    class PlatformSet : public TestFlag {
    public:
        PlatformSet();
        static const int Hisi = 0x01 << 0;
        static const int Qcom = 0x02 << 0;
    };

    // chip scope
    class ChipSet : public TestFlag {
    public:
        ChipSet();
        static const int Hi3660   = 0x01 << 8;
        static const int Kirin960 = 0x02 << 8;
        static const int Kirin970 = 0x04 << 8;
        static const int Q8639    = 0x08 << 8;
    };

    // device scope
    class DeviceSet : public TestFlag {
    public:
        DeviceSet();
        static const int Dual_Sim = 0x01 << 16;
        static const int Sim      = 0x02 << 16;
        static const int Sdcard   = 0x04 << 16;
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
        static const int Level5 = 32 << 24;
    };

    extern const PlatformSet Platform;
    extern const ChipSet Chip;
    extern const DeviceSet Device;
    extern const TestSizeSet TestSize;

    // get each instance of all the TestFlag implementions
    const std::vector<const TestFlag*>& allHextTagSets();
    // verify the test flagset, returns false if the flagset is illegal
    bool checkFlagsLegality(int flags);
    // convert name string to test flag value
    bool flagForName(const char* set_name, const char* ele_name, int& result);

  }// namespace ext
}  // namespace testing

#endif  // GTEST_INCLUDE_GTEST_GTEST_TAG_H_
