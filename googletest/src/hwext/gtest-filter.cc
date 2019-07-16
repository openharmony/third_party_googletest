// Copyright (C) 2018. Huawei Technologies Co., Ltd. All rights reserved.

#include "gtest/hwext/gtest-filter.h"
#include "gtest/hwext/gtest-tag.h"
#include "gtest/hwext/gtest-utils.h"

namespace testing {
  namespace ext {

    const char* const TestFilter::kStrictFilter = "strict_tags";

    TestFilter* TestFilter::instance() {
        static TestFilter* instance_ = NULL;
        if (instance_ == NULL) {
            instance_ = new TestFilter();
            instance_->reset();
        }
        return instance_;
    }

    void TestFilter::printHelp() const {
        printf("\nTest Filtering:\n");
        const std::vector<const TestFlag*>& sets = allHextTagSets();
        // help message line indents
        const char* indents[] = { "  ", "      " };

        for (unsigned int i = 0; i < sets.size(); i++) {
            sets.at(i)->printHelp(indents);
        }

        printf("%sSelect tests by test level, may be a list seperated by ',' or ';'.\n", indents[1]);
    }

    map<const char*, string*>& TestFilter::getAllFilterFlagsKv() {
        // no need to consider concurrence so for, because we do 
        // this before running tests
        if (flag_kvs_inited) {
            return filterFlagsKv;
        }

        flag_kvs_inited = true;
        const std::vector<const TestFlag*>& sets = allHextTagSets();
        for (unsigned int i = 0; i < sets.size(); i++) {
            filterFlagsKv.insert(pair<const char*, string*>(sets.at(i)->naming(), new string("")));
        }

        // strict mode
        filterFlagsKv.insert(pair<const char*, string*>(kStrictFilter, new string("false")));
        return filterFlagsKv;
    }

    bool TestFilter::postParsingArguments() {
        if (ready || !flag_kvs_inited) {
            // only run setup logic once
            return true;
        }

        const char* kCandidateSeps = ",;|/";
        ready = true;
        bool error = false;
        map<const char*, string*>::iterator iter;

        for (iter = filterFlagsKv.begin(); iter != filterFlagsKv.end(); iter++) {
            const char * kstr = iter->first;
            const char * vstr = iter->second->c_str();
            int flag = TestFlag::None;

            if(compareStringsByIgnoreCase(kStrictFilter, kstr)) {
                strictMode = compareStringsByIgnoreCase("true", vstr) || compareStringsByIgnoreCase("t", vstr);
            }
            else if (flagForName(kstr, vstr, flag)) {
                string strname = string(kstr);
                string strval  = string(vstr);
                vector<string> vectemp = SplitString(strval, kCandidateSeps);

                if (strname == "testsize") {
                    for (size_t i = 0; i < vectemp.size(); i++) {
                        string curr = vectemp[i];
                        if (curr == "Level0") {
                            vecTestLevel.push_back(1);
                        }
                        else if (curr == "Level1") {
                            vecTestLevel.push_back(2);
                        }
                        else if (curr == "Level2") {
                            vecTestLevel.push_back(4);
                        }
                        else if (curr == "Level3") {
                            vecTestLevel.push_back(8);
                        }
                        else if (curr == "Level4") {
                            vecTestLevel.push_back(16);
                        }
                    }
                }
            }
            else {
                // illegal arguments
                error = true;
            }
            // free the newed strings
            delete iter->second;
        }

        filterFlagsKv.clear();
        //printf("TestFilter required_Flags='0x%08x'\n", requiredFlags);
        return !error;
    }

    bool  TestFilter::accept(int flags) const {
        // both of flags and req_no should be accepted
        if (!ready) {
            return true;
        }

        int level = (flags >> 24);
        bool flags_accepted = false;
        if (!strictMode) {
            flags_accepted = IsElementInVector(vecTestLevel, level);
        }
        else {
            flags_accepted = ((flags&requiredFlags) == requiredFlags);
        }

        if (!flags_accepted) {
            return false;
        }

        return true;
    }

    void TestFilter::reset() {
        filterFlagsKv.clear();
        requiredFlags = TestFlag::None;
        flag_kvs_inited = false;
        strictMode = false;
        ready = false;
    }

  } // namespace ext
} // namespace testing