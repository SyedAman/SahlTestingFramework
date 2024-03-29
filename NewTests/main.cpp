/**
 * Custom Parallelized Test Framework That Beats Google Test
 *
 * Requirements
    1. Output whether a test passes or fails
    Requirement 1.1: System identifies a passing test.
    Requirement 1.2: System identifies a failing test.
    Requirement 1.3: System logs the pass/fail status of each test.
    Requirement 1.4: System displays the pass/fail status after test execution.

    2. Print out the total number of tests
    Requirement 2.1: System counts the total number of tests in the suite.
    Requirement 2.2: System stores the total test count.
    Requirement 2.3: System displays the total test count before test execution.
    Requirement 2.4: System displays the total test count after test execution.

    3. Output the number of failed tests
    Requirement 3.1: System tracks each failed test during execution.
    Requirement 3.2: System counts the total number of failed tests.
    Requirement 3.3: System stores the count of failed tests.
    Requirement 3.4: System displays the count of failed tests after execution.

    4. Output the exception or error
    Requirement 4.1: System captures exceptions or errors from each test.
    Requirement 4.2: System logs details of exceptions or errors.
    Requirement 4.3: System associates errors with the corresponding test.
    Requirement 4.4: System displays exception/error details after a test fails.

    5. Parallelization: Run all tests using all threads
    Requirement 5.1: System identifies tests that can run in parallel.
    Requirement 5.2: System divides tests into batches for parallel execution.
    Requirement 5.3: System assigns test batches to available threads.
    Requirement 5.4: System initiates parallel test execution across threads.

    6. Optimize thread utilization with minimal idling
    Requirement 6.1: System monitors load and availability of each thread.
    Requirement 6.2: System reassigns tests to idle threads.
    Requirement 6.3: System balances the load evenly across threads.
    Requirement 6.4: System minimizes idle time for each thread.

    7. Ability to run the tests
    Requirement 7.1: System provides a command to start test execution.
    Requirement 7.2: System coordinates test execution processes.
    Requirement 7.3: System handles the execution of individual tests.
    Requirement 7.4: System ensures proper sequence of test execution.
 */

#include <cassert>
#include <string>
#include <chrono>
#include <thread>
#include <iostream>
#include <vector>
#include <queue>
#include <mutex>
#include <atomic>
#include <functional>

static const int ITERATIONS_FOR_ONE_MS_FIB_N_I9_1200K = 1136363; // Approximate number of iterations to take 1ms on i9-1200K
struct TestState {
    int total = 0;
    int passed = 0;
};

void testBoolean(bool assertion, TestState &state) {
    state.total += 1;
    state.passed += assertion;
}

std::string createTotalTestCountString(const TestState state) {
    return "Total Tests: " + std::to_string(state.total);
}

bool checkTestStateIsEqual(const TestState testState, const int expectedTestCount, const int expectedPassedTests) {
    return testState.total == expectedTestCount && testState.passed == expectedPassedTests;
}

int getTotalFailedTests(const TestState state) {
    return state.total - state.passed;
}

int fibonacci(int n) {
    if (n <= 1) {
        return n;
    }
    int a = 0, b = 1, c;
    for (int i = 2; i <= n; ++i) {
        c = a + b;
        a = b;
        b = c;
    }
    return b;
}

std::string createRuntimeString(int runtimeMilliseconds) {
    return "Runtime: " + std::to_string(runtimeMilliseconds) + "ms";
}

void outputRuntime(int runtimeMilliseconds)
{
    std::cout << createRuntimeString(runtimeMilliseconds) << std::endl;
}

int main() {
    int expectedFailedTests = 1;
    TestState testState{};

    // Production code
    testBoolean(true == true, testState);
    testBoolean(1 == 1, testState);
    testBoolean(1 != 2, testState);
    testBoolean(1 == 3, testState);

    // Has correct test count (pass, fail, total)
    assert(checkTestStateIsEqual(testState, 4, 3));
    assert(getTotalFailedTests(testState) == expectedFailedTests);
    assert(createTotalTestCountString(testState) == "Total Tests: 4");

    testBoolean(15 + 100 - 10 == 100 + 5, testState);
    assert(checkTestStateIsEqual(testState, 5, 4));
    assert(getTotalFailedTests(testState) == 1);
    assert(createTotalTestCountString(testState) == "Total Tests: 5");

    testBoolean(95 + 5 == 100, testState);
    assert(checkTestStateIsEqual(testState, 6, 5));
    assert(getTotalFailedTests(testState) == 1);
    assert(createTotalTestCountString(testState) == "Total Tests: 6");


    // ==================== Parallelization ====================

    // Requirement 5. Parallelization: Run all tests using all threads

    // Requirement 5.1: System identifies tests that can run in parallel.

    auto parallelTest = 1;
    std::vector<int> actualParallelTests;
    actualParallelTests.push_back(parallelTest);
    actualParallelTests.push_back(parallelTest);
    actualParallelTests.push_back(parallelTest);
    actualParallelTests.push_back(parallelTest);
    std::vector<int> expectedParallelTests = {1, 1, 1, 1};
    assert(actualParallelTests == expectedParallelTests);


    // No parallelization -- 1 thread -- 4 tasks -- tolerance 40ms
    int expectedDuration = 1000;
    int individualRuntime = 250;
    int fourTasks = 4;
    int tolerance = 40;

    auto starTime = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < fourTasks; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds (individualRuntime));
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    int finalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - starTime).count();
    outputRuntime(finalDuration);

    assert(std::abs(finalDuration - expectedDuration) <= tolerance);

    // Static parallelization -- 4 threads -- 4 tasks -- tolerance 25ms
    int expectedDuration2 = 250;
    int individualRuntime2 = 250;
    int tolerance2 = 25;
    int threads4 = 4;

    auto starTime2 = std::chrono::high_resolution_clock::now();
    std::vector<std::thread> threadVector;
    threadVector.reserve(threads4);
    for (int i = 0; i < threads4; i++) {
        threadVector.emplace_back([individualRuntime2]() {
            std::this_thread::sleep_for(std::chrono::milliseconds (individualRuntime2));
        });
    }
    for (auto &thread : threadVector) {
        thread.join();
    }
    auto endTime2 = std::chrono::high_resolution_clock::now();
    int finalDuration2 = std::chrono::duration_cast<std::chrono::milliseconds>(endTime2 - starTime2).count();
    outputRuntime(finalDuration2);

    assert(std::abs(finalDuration2 - expectedDuration2) <= tolerance2);

    // Dynamic parallelization -- 16 threads -- 64 tasks -- tolerance 50ms -- sorted queue based
    int expectedDuration3 = 100;
    int tolerance3 = 50;
    int threads16 = 16;

    std::vector<int> tasks = {100, 50, 25, 0, 10, 0, 50, 25, 50, 50, 0, 25, 0, 5, 5, 5,
                              100, 50, 25, 0, 10, 0, 50, 25, 50, 50, 0, 25, 0, 5, 5, 5,
                              100, 50, 25, 0, 10, 0, 50, 25, 50, 50, 0, 25, 0, 5, 5, 5,
                              100, 50, 25, 0, 10, 0, 50, 25, 50, 50, 0, 25, 0, 5, 5, 5};
    std::atomic<int> answer;
    answer = 0;
    const auto startTime3 = std::chrono::high_resolution_clock::now();
    std::vector<std::thread> threadVector3;
    threadVector3.reserve(threads16);
    std::mutex queueMutex;
    std::queue<int> taskQueue;
    std::sort(tasks.begin(), tasks.end(), std::greater<int>());
    // Queue based task distribution
    for (int task : tasks) {
        taskQueue.push(task);
    }
    for (int i = 0; i < threads16; i++) {
        threadVector3.emplace_back([tasks, &answer, &taskQueue, &queueMutex]() {
            while (true) {
                int task;
                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    if (taskQueue.empty()) break;
                    task = taskQueue.front();
                    taskQueue.pop();
                }
                fibonacci(ITERATIONS_FOR_ONE_MS_FIB_N_I9_1200K * task);
                answer += task;
            }
        });
    }
    for (auto &thread : threadVector3) {
        thread.join();
    }
    const auto endTime3 = std::chrono::high_resolution_clock::now();
    int totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime3 - startTime3).count();
    outputRuntime(totalDuration);

    assert(answer == 1600);
    assert(std::abs(totalDuration - expectedDuration3) <= tolerance3);


    // Dynamic parallelization -- 16 threads -- 64 tasks -- tolerance 15ms -- greedy no queue
    int expectedDuration4 = 100;
    int tolerance4 = 20;

    std::vector<int> tasks4 = {100, 50, 25, 0, 10, 0, 50, 25, 50, 50, 0, 25, 0, 5, 5, 5,
                              100, 50, 25, 0, 10, 0, 50, 25, 50, 50, 0, 25, 0, 5, 5, 5,
                              100, 50, 25, 0, 10, 0, 50, 25, 50, 50, 0, 25, 0, 5, 5, 5,
                              100, 50, 25, 0, 10, 0, 50, 25, 50, 50, 0, 25, 0, 5, 5, 5};
    int totalWorkload = 0;
    for (int task : tasks4) {
        totalWorkload += task;
    }
    int optimalThreadWorkload = totalWorkload / threads16;
    std::vector<std::vector<int>> threadTasks;
    threadTasks.reserve(threads16);
    std::sort(tasks4.begin(), tasks4.end(), std::greater<int>());
    for (int i = 0; i < threads16; i++) {
        int workload = 0;
        std::vector<int> threadTask;
        while (workload < optimalThreadWorkload) {
            threadTask.push_back(tasks4.back());
            workload += tasks4.back();
            tasks4.pop_back();
        }
        threadTasks.push_back(threadTask);
    }

    std::atomic<int> answer4;
    answer4 = 0;
    std::vector<std::thread> threadVector4;
    threadVector4.reserve(threads16);
    const auto startTime4 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < threads16; i++) {
        std::vector<int> currentThreadTasks = threadTasks[i];
        threadVector4.emplace_back([currentThreadTasks, &answer4, i]() {
            for (int task : currentThreadTasks) {
                fibonacci(ITERATIONS_FOR_ONE_MS_FIB_N_I9_1200K * task);
                answer4 += task;
            }
        });
    }
    for (auto &thread : threadVector4) {
        thread.join();
    }
    const auto endTime4 = std::chrono::high_resolution_clock::now();
    int totalDuration4 = std::chrono::duration_cast<std::chrono::milliseconds>(endTime4 - startTime4).count();
    std::cout << "Runtime: " << totalDuration4 << "ms" << std::endl;
    assert(answer4 == 1600);
    assert(std::abs(totalDuration4 - expectedDuration4) <= tolerance4);


    // ==================== End ====================

    return 0;
}
