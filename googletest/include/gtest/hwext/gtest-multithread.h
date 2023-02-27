/*
* Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
* Description:多线程测试框架(MULTI_THREAD_TEST)库接口声明
* Author:
* Create:2022/2/8
*/

#ifndef __MULTI_THREAD_TEST_H__
#define __MULTI_THREAD_TEST_H__

#include <vector>
#include <string>
#include <map>
#include <thread>
#include "gtest/gtest.h"

using ::testing::TestPartResult;

namespace testing{
    namespace mt {
        using PF = void(*)();
        using uint = unsigned int;
        const uint MMTEST_RANDOM_THREAD_ID = 0x3f3f3f3f; //线程ID常量
        static uint g_thread_count = 10 ; //全局变量,默认线程数是10
        inline void setThreadNum(uint num){
            g_thread_count = num;
        }
        class TestTask {
        public:
            TestTask() = default;
            TestTask(std::string ts, std::string tc, PF func) : m_testsuite(ts), m_testcase(tc), m_func(func){}
            std::string m_testsuite;
            std::string m_testcase;
            PF m_func;
        };
        class ThreadTaskEntry {
        public:
            ThreadTaskEntry(std::vector<TestTask> v_task) : funcList(v_task) {}
            std::thread *thread;
            std::vector<TestTask> funcList; 
        };
        class MultiThreadTest {
        private:
            std::vector<std::vector<TestTask>> threadTasks;
            std::vector<ThreadTaskEntry> threadTestEntryList;
            std::map<std::string, std::vector<TestTask>> randomTasks;
            void doTest();
        public:
            MultiThreadTest(){}
            unsigned m_threadNum;
            MultiThreadTest(unsigned int t_cnt) : m_threadNum(t_cnt) {}
            uint runTask(unsigned thread_num, PF func, std::string testsuite, std::string testcase);
            void appendTaskToList(unsigned thread_id, PF func, std::string testsuite, std::string testcase);
        };

        class MultiThreadFailCaseListener : public EmptyTestEventListener {
        private:
            MultiThreadTest *testInstance;
        public:
            MultiThreadFailCaseListener(MultiThreadTest *ins) { this->testInstance = ins; }
        };

        #define SET_THREAD_NUM(n) setThreadNum(n) 
        #define MTEST_FUNC_NAME(x, y) TC_##x_##y
        #define GTEST_RUN_TASK(hook) \
        do { \
            MultiThreadTest _test(g_thread_count);        \
            _test.runTask(_test.m_threadNum, hook, this->test_info_->test_case_name(), this->test_info_->name()); \
        } while (0)
        #define MTEST_TYPE(x, y, z, n) void MTEST_FUNC_NAME(x, y)(); \
        z(x, y) { \
        SET_THREAD_NUM(n); \
        GTEST_RUN_TASK(MTEST_FUNC_NAME(x, y)); \
        }                                                   \
        void MTEST_FUNC_NAME(x, y)()
        #define HWMTEST(x, y, w, n) MTEST_TYPE(x, y, TEST, n)
        #define HWMTEST_F(x, y, w, n) MTEST_TYPE(x, y, TEST_F, n)
    }
}
#endif /* __MULTI_THREAD_TEST_H__ */