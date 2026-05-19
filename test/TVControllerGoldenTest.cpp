#include <gtest/gtest.h>

#include "TextTestFixture.h"
#include "support/GoldenTestUtil.h"

#include <filesystem>
#include <string>

#ifndef GOLDEN_SOURCE_DIR
#define GOLDEN_SOURCE_DIR "test/golden"
#endif

namespace {

std::filesystem::path goldenSourceDir() {
    return std::filesystem::path(GOLDEN_SOURCE_DIR);
}

void assertMatchesGolden(const std::string& scenarioName, const std::string& actual) {
    const std::filesystem::path goldenFile =
        golden::goldenPath(goldenSourceDir(), scenarioName);

    if (golden::shouldUpdateGolden()) {
        golden::writeFile(goldenFile, actual);
        GTEST_LOG_(INFO) << "Updated golden file: " << goldenFile.string();
        return;
    }

    ASSERT_TRUE(std::filesystem::exists(goldenFile))
        << "Missing golden file: " << goldenFile.string()
        << "\nRun with TV_UPDATE_GOLDEN=1 to generate expected output.";

    const std::string expected = golden::readFile(goldenFile);
    if (expected == actual) {
        return;
    }

    const std::filesystem::path actualFile =
        goldenSourceDir() / (scenarioName + ".actual.txt");
    golden::writeFile(actualFile, actual);

    FAIL() << "Golden mismatch for " << scenarioName << "\n"
           << "  expected: " << goldenFile.string() << "\n"
           << "  actual:   " << actualFile.string() << "\n"
           << "Regenerate with: TV_UPDATE_GOLDEN=1 ctest -R TVControllerGoldenTest";
}

class TVControllerGoldenTest : public ::testing::TestWithParam<std::string> {};

TEST_P(TVControllerGoldenTest, MatchesApprovedOutput) {
    const std::string scenarioName = GetParam();
    const TextScenario* scenario = TextTestFixture::findScenario(scenarioName);
    ASSERT_NE(scenario, nullptr) << "Unknown scenario: " << scenarioName;

    const std::string actual = TextTestFixture::runScenario(*scenario);
    assertMatchesGolden(scenarioName, actual);
}

std::string scenarioName(const ::testing::TestParamInfo<std::string>& info) {
    return info.param;
}

}  // namespace

INSTANTIATE_TEST_SUITE_P(
    TVControllerRegression,
    TVControllerGoldenTest,
    ::testing::ValuesIn([]() {
        std::vector<std::string> names;
        for (const TextScenario& scenario : TextTestFixture::allScenarios()) {
            names.push_back(scenario.name);
        }
        return names;
    }()),
    scenarioName);
