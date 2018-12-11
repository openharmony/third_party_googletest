// Copyright (C) 2018. Huawei Technologies Co., Ltd. All rights reserved.

#ifndef GTEST_INCLUDE_GTEST_GTEST_FILTER_H_
#define GTEST_INCLUDE_GTEST_GTEST_FILTER_H_

#include <string>
#include <map>

namespace testing {
  namespace ext {
    using ::std::string;
    using ::std::map;

    class TestFilter {
    public:
        map<const char*, string*>& getAllFilterFlagsKv();
        void printHelp() const;
        bool postParsingArguments();
        int requireFlags() const { return requiredFlags; }
        bool accept(int flags) const;
        void reset(); // for unittest
        static TestFilter* instance();
    private:
        TestFilter() {};
        //static const char* const kReqNoKey;
        static const char* const kStrictFilter;
        const map<const char*, string*> allFilterFlagsKv();
        int requiredFlags;
        // strcit filter requires the entirely same test flags and require no
        bool strictMode;
        bool flag_kvs_inited; // teels iff the filter kvs has been parsed
        bool ready; // teels iff the filter are ready to be used
        map<const char*, string*> filterFlagsKv;
    };
  }// namespace ext
}  // namespace testing

#endif  // GTEST_INCLUDE_GTEST_GTEST_FILTER_H_

