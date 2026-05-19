#include "TextTestFixture.h"

#include <sstream>
#include <stdexcept>

FakeTunerForText::FakeTunerForText(std::string initialChannel)
    : currentChannel_(std::move(initialChannel)) {}

void FakeTunerForText::setSeekReturns(std::vector<std::string> returns) {
    seekReturns_ = std::move(returns);
    seekIndex_ = 0;
}

std::string FakeTunerForText::seekCH() {
    if (seekIndex_ >= seekReturns_.size()) {
        return "";
    }
    return seekReturns_[seekIndex_++];
}

void FakeTunerForText::setCH(const std::string& ch) {
    currentChannel_ = ch;
}

std::string FakeTunerForText::getCurrentCH() {
    return currentChannel_;
}

void TextTestFixture::renderScenario(std::ostream& out, const TextScenario& scenario) {
    out << "scenario " << scenario.name << '\n';
    out << "initial " << scenario.initialChannel << '\n';

    if (!scenario.seekReturns.empty()) {
        out << "seek_returns";
        for (const std::string& ch : scenario.seekReturns) {
            out << ' ' << ch;
        }
        out << '\n';
    }

    FakeTunerForText tuner(scenario.initialChannel);
    tuner.setSeekReturns(scenario.seekReturns);
    TVController controller(&tuner);

    for (remoteKey key : scenario.keys) {
        controller.pushButton(key);
        out << to_string(key) << " -> " << tuner.getCurrentCH() << '\n';
    }
}

std::string TextTestFixture::runScenario(const TextScenario& scenario) {
    std::ostringstream out;
    renderScenario(out, scenario);
    return out.str();
}

const std::vector<TextScenario>& TextTestFixture::allScenarios() {
    static const std::vector<TextScenario> scenarios = {
        {"DigitThenConfirm_ChangesToSingleDigitChannel", "6",
         {remoteKey::KEY_1, remoteKey::KEY_OK}, {}},
        {"TwoDigits_ChangesImmediately", "0",
         {remoteKey::KEY_1, remoteKey::KEY_2}, {}},
        {"FourDigits_ChangesAsTwoPairs", "0",
         {remoteKey::KEY_1, remoteKey::KEY_2, remoteKey::KEY_3, remoteKey::KEY_4}, {}},
        {"ThirdDigit_StartsNextPendingInput", "0",
         {remoteKey::KEY_4, remoteKey::KEY_5, remoteKey::KEY_6}, {}},
        {"PendingSingleDigitConfirm_ChangesToPendingDigit", "0",
         {remoteKey::KEY_4, remoteKey::KEY_5, remoteKey::KEY_6, remoteKey::KEY_OK}, {}},
        {"PendingSingleDigitNonConfirmFunction_InvalidatesPendingDigit", "0",
         {remoteKey::KEY_4, remoteKey::KEY_5, remoteKey::KEY_6, remoteKey::KEY_CH_UP}, {}},
        {"LeadingZeroThenDigit_ChangesToDigitChannel", "50",
         {remoteKey::KEY_0, remoteKey::KEY_7}, {}},
        {"LeadingZeroThenConfirm_ChangesToChannelZero", "50",
         {remoteKey::KEY_0, remoteKey::KEY_OK}, {}},
        {"ChannelUpWithoutSearchResult_IncrementsChannel", "6",
         {remoteKey::KEY_CH_UP}, {}},
        {"ChannelDownWithoutSearchResult_DecrementsChannel", "6",
         {remoteKey::KEY_CH_DOWN}, {}},
        {"ChannelUpWithoutSearchResult_WrapsFromMaxToMin", "99",
         {remoteKey::KEY_CH_UP}, {}},
        {"ChannelDownWithoutSearchResult_WrapsFromMinToMax", "0",
         {remoteKey::KEY_CH_DOWN}, {}},
        {"ChannelSearchThenUp_MovesWithinStoredChannels", "6",
         {remoteKey::KEY_CH_SEARCH, remoteKey::KEY_CH_UP},
         {"4", "6", "14", "6"}},
        {"ChannelUpWithSearchResult_ChangesToNextStoredChannel", "6",
         {remoteKey::KEY_CH_SEARCH, remoteKey::KEY_CH_UP},
         {"4", "6", "14", "6"}},
        {"ChannelDownWithSearchResult_ChangesToPreviousStoredChannel", "6",
         {remoteKey::KEY_CH_SEARCH, remoteKey::KEY_CH_DOWN},
         {"4", "6", "14", "6"}},
        {"ChannelUpWithSearchResult_WrapsToSmallestStoredChannel", "15",
         {remoteKey::KEY_CH_SEARCH, remoteKey::KEY_CH_UP},
         {"4", "6", "14", "6"}},
        {"ChannelDownWithSearchResult_ChangesToNearestLowerStoredChannel", "15",
         {remoteKey::KEY_CH_SEARCH, remoteKey::KEY_CH_DOWN},
         {"4", "6", "14", "6"}},
        {"NextFavorite_ChangesToNearestGreaterFavorite", "1",
         {remoteKey::KEY_FAVORITE_ADD, remoteKey::KEY_4, remoteKey::KEY_OK,
          remoteKey::KEY_FAVORITE_ADD, remoteKey::KEY_1, remoteKey::KEY_2,
          remoteKey::KEY_FAVORITE_ADD, remoteKey::KEY_5, remoteKey::KEY_6,
          remoteKey::KEY_FAVORITE_ADD, remoteKey::KEY_6, remoteKey::KEY_OK,
          remoteKey::KEY_NEXT_FAVORITE},
         {}},
        {"NextFavorite_WrapsToSmallestFavorite", "1",
         {remoteKey::KEY_FAVORITE_ADD, remoteKey::KEY_4, remoteKey::KEY_OK,
          remoteKey::KEY_FAVORITE_ADD, remoteKey::KEY_1, remoteKey::KEY_2,
          remoteKey::KEY_FAVORITE_ADD, remoteKey::KEY_5, remoteKey::KEY_6,
          remoteKey::KEY_FAVORITE_ADD, remoteKey::KEY_NEXT_FAVORITE},
         {}},
    };
    return scenarios;
}

const TextScenario* TextTestFixture::findScenario(const std::string& name) {
    for (const TextScenario& scenario : allScenarios()) {
        if (scenario.name == name) {
            return &scenario;
        }
    }
    return nullptr;
}
