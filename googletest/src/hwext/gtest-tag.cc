// Copyright (C) 2018. Huawei Technologies Co., Ltd. All rights reserved.

#include "gtest/hwext/gtest-tag.h"
#include "gtest/hwext/gtest-utils.h"

namespace testing {
  namespace ext {

    // instantiate the static global members [never do this in .h]
    const PlatformSet Platform;
    const ChipSet Chip;
    const DeviceSet Device;
    const TestSizeSet TestSize;

    TestFlag::TestFlag(const char*  n, const char* d, int m) : name(n), desc(d), mask(m) {
        eleCount = 0;
    }

    void TestFlag::element(const char* desc, int hex) {
        elementMap.insert(pair<int, const char* const>(hex, desc));
    }

    void TestFlag::printHelp(const char** indents) const {
        map<int, const char*>::const_iterator c_iter;
        int i = 0;
        printf("%s--gtest_%s=(", indents[0], naming());
        for (c_iter = elementMap.begin(); c_iter != elementMap.end(); c_iter++) {
            printf("%s%s", (i > 0) ? "|" : "", c_iter->second);
            i++;
        }
        printf(")\n");
        printf("%s%s\n", indents[1], description());
    }

    bool TestFlag::verify(int hex, char* err) const {
        // wipe bits not belong to this set
        int masked = hex&mask;

        map<int, const char*>::const_iterator c_iter;
        for (c_iter = elementMap.begin(); c_iter != elementMap.end(); c_iter++) {
            // wipe each bit defined in this set
            masked &= ~c_iter->first;
        }

        // masked!=0 means find undefined bit(s)
        const bool nice = masked == 0;
        if (!nice&&err != NULL) {
            sprintf(err, "Illegal %s value '0x%08x'\n", naming(), hex);
        }

        return nice;
    }

    bool TestFlag::eleForName(const char* name, int& result) const {
        if (name == NULL) {
            // NOTE:: strlen(NULL) will cause seg fault
            return false;
        }

        const int result_raw = result;
        // must set to 0 here, because we will do bit-OR arithmetic later
        result = 0;
        // the parameter 'name' may contatin more than one element
        const char* kCandidateSeps = ",;|/";
        char name_cpoy[256];
        memset(name_cpoy, 0, 256);
        memcpy(name_cpoy, name, strlen(name));

        char * pch;
        pch = strtok(name_cpoy, kCandidateSeps);
        while (pch != NULL)
        {
            map<int, const char*>::const_iterator c_iter;
            bool matched = false;
            // try to matchNaming all the elements
            for (c_iter = elementMap.begin(); c_iter != elementMap.end(); c_iter++) {
                const char* ele_name = c_iter->second;
                const string ele_full_name_string = string(naming()) + "." + ele_name;
                if (compareStringsByIgnoreCase(pch, ele_name) || compareStringsByIgnoreCase(pch, ele_full_name_string.c_str())) {
                    // set this bit
                    result |= c_iter->first;
                    matched = true;
                    break;
                }
            }
            if (!matched) {
                fprintf(stderr, "Unrecognized %s value '%s'\n", naming(), pch);
                // abort the matching, recover result to raw value
                result = result_raw;
                return false;
            }
            pch = strtok(NULL, kCandidateSeps);
        }
        return true;
    }

    PlatformSet::PlatformSet() :TestFlag("platform", "Select tests by platform requirment.", 0xff << 0) {
        element("Hisi", Hisi);
        element("Qcom", Qcom);
    }

    ChipSet::ChipSet() : TestFlag("chip", "Select tests by chip requirment.", 0xff << 8) {
        element("Hi3660", Hi3660);
        element("Kirin960", Kirin960);
        element("Kirin970", Kirin970);
        element("Q8639", Q8639);
    }

    DeviceSet::DeviceSet() : TestFlag("device", "Select tests by device configuration requirment.", 0xff << 16) {
        element("Dual_Sim", Dual_Sim);
        element("Sim", Sim);
        element("Sdcard", Sdcard);
    }

    TestSizeSet::TestSizeSet() : TestFlag("testsize", "Select tests by test size hint.", 0xff << 24) {
        element("Level0", Level0);
        element("Level1", Level1);
        element("Level2", Level2);
        element("Level3", Level3);
        element("Level4", Level4);
        element("Level5", Level5);
    }

    static std::vector<const TestFlag*> sets;

    static void ensureSetsInit() {
        if (sets.size() > 0) {
            return;
        }
        sets.push_back(&Platform);
        sets.push_back(&Chip);
        sets.push_back(&Device);
        sets.push_back(&TestSize);
    }

    const std::vector<const TestFlag*>& allHextTagSets() {
        ensureSetsInit();
        return sets;
    }

    bool checkFlagsLegality(int flags) {
        ensureSetsInit();
        char buf[256];
        for (unsigned int i = 0; i < sets.size(); i++)
        {
            const TestFlag* set = sets.at(i);
            memset(buf, 0, 256);
            if (!set->verify(flags, buf)) {
                return false;
            }
        }
        return true;
    }

    bool flagForName(const char* set_name, const char* ele_name, int& result) {
        ensureSetsInit();
        for (unsigned int i = 0; i < sets.size(); i++)
        {
            const TestFlag* set = sets.at(i);
            if (!compareStringsByIgnoreCase(set_name, set->naming())) {
                continue;
            }
            return set->eleForName(ele_name, result);
        }
        fprintf(stderr, "Unrecognized flagset '%s'\n", set_name);
        return false;
    }

  }
}
