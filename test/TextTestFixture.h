#ifndef TEXT_TEST_FIXTURE_H
#define TEXT_TEST_FIXTURE_H

#include "TVController.h"
#include "remoteKey.h"
#include "Tuner.h"
#include <ostream>
#include <string>
#include <vector>

struct TextScenario {
    std::string name;
    std::string initialChannel;
    std::vector<remoteKey> keys;
    std::vector<std::string> seekReturns;
};

class FakeTunerForText : public Tuner {
public:
    explicit FakeTunerForText(std::string initialChannel);

    std::string seekCH() override;
    void setCH(const std::string& ch) override;
    std::string getCurrentCH() override;

    void setSeekReturns(std::vector<std::string> returns);

private:
    std::string currentChannel_;
    std::vector<std::string> seekReturns_;
    std::size_t seekIndex_ = 0;
};

class TextTestFixture {
public:
    static const std::vector<TextScenario>& allScenarios();
    static const TextScenario* findScenario(const std::string& name);
    static std::string runScenario(const TextScenario& scenario);
    static void renderScenario(std::ostream& out, const TextScenario& scenario);
};

#endif  // TEXT_TEST_FIXTURE_H
