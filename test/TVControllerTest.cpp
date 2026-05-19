#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Tuner.h"
#include "TVController.h"
#include "remoteKey.h"
#include <string>

class MockTunerForController : public Tuner {
public:
    MOCK_METHOD(std::string, seekCH, (), (override));
    MOCK_METHOD(void, setCH, (const std::string& ch), (override));
    MOCK_METHOD(std::string, getCurrentCH, (), (override));
};

class TVControllerTest : public ::testing::Test {
protected:
    MockTunerForController mockTuner;
    std::string currentCh = "6";

    void SetUp() override {
        ON_CALL(mockTuner, getCurrentCH())
            .WillByDefault([this]() { return currentCh; });
        ON_CALL(mockTuner, setCH(::testing::_))
            .WillByDefault([this](const std::string& ch) { currentCh = ch; });
    }

    TVController makeController() {
        return TVController(&mockTuner);
    }

    remoteKey digitKey(int d) {
        return static_cast<remoteKey>(static_cast<int>(remoteKey::KEY_0) + d);
    }

    void expectChannel(int ch) {
        ASSERT_EQ(std::to_string(ch), currentCh);
    }
};

// pushButton -> handleDigit, confirmPendingDigits, setChannel
TEST_F(TVControllerTest, DigitThenConfirm_ChangesToSingleDigitChannel) {
    // Given: 현재 채널 6, 숫자 입력 버퍼 비어 있음
    currentCh = "6";
    TVController controller = makeController();

    // When: '1' 입력 후 확인
    controller.pushButton(remoteKey::KEY_1);
    controller.pushButton(remoteKey::KEY_OK);

    // Then: 1번 채널로 변경
    expectChannel(1);
}

// pushButton -> handleDigit (두 자리 즉시 확정)
TEST_F(TVControllerTest, TwoDigits_ChangesImmediately) {
    // Given: 현재 채널 0
    currentCh = "0";
    TVController controller = makeController();

    // When: '1', '2' 연속 입력
    controller.pushButton(remoteKey::KEY_1);
    controller.pushButton(remoteKey::KEY_2);

    // Then: 12번 채널로 즉시 변경
    expectChannel(12);
}

// pushButton -> handleDigit (선행 0 처리), parseChannel
TEST_F(TVControllerTest, LeadingZeroThenDigit_ChangesToDigitChannel) {
    // Given: 현재 채널 50
    currentCh = "50";
    TVController controller = makeController();

    // When: '0', '7' 입력
    controller.pushButton(remoteKey::KEY_0);
    controller.pushButton(remoteKey::KEY_7);

    // Then: 07 -> 7번 채널
    expectChannel(7);
}

// pushButton -> handleChannelUp, 경계값 99 -> 0
TEST_F(TVControllerTest, ChannelUpWithoutSearch_WrapsFromMaxToMin) {
    // Given: 검색 결과 없음, 현재 99번 채널
    currentCh = "99";
    TVController controller = makeController();

    // When: 채널 업
    controller.pushButton(remoteKey::KEY_CH_UP);

    // Then: 0번 채널로 순환
    expectChannel(0);
}

// pushButton -> handleChannelDown, 경계값 0 -> 99
TEST_F(TVControllerTest, ChannelDownWithoutSearch_WrapsFromMinToMax) {
    // Given: 검색 결과 없음, 현재 0번 채널
    currentCh = "0";
    TVController controller = makeController();

    // When: 채널 다운
    controller.pushButton(remoteKey::KEY_CH_DOWN);

    // Then: 99번 채널로 순환
    expectChannel(99);
}

// pushButton -> handleChannelSearch, handleChannelUp (검색 결과 기준)
TEST_F(TVControllerTest, ChannelSearchThenUp_MovesWithinStoredChannels) {
    // Given: 현재 6번, seekCH가 4 -> 6 -> 14 -> 6 순으로 반환
    currentCh = "6";
    TVController controller = makeController();

    ::testing::InSequence seq;
    EXPECT_CALL(mockTuner, seekCH())
        .WillOnce(::testing::Return("4"))
        .WillOnce(::testing::Return("6"))
        .WillOnce(::testing::Return("14"))
        .WillOnce(::testing::Return("6"));

    // When: 채널 검색 후 채널 업
    controller.pushButton(remoteKey::KEY_CH_SEARCH);
    controller.pushButton(remoteKey::KEY_CH_UP);

    // Then: 저장 목록 {4,6,14}에서 6 다음은 14
    expectChannel(14);
}

// pushButton -> toggleFavorite, handleNextFavorite
TEST_F(TVControllerTest, NextFavorite_ChangesToNearestGreaterFavorite) {
    // Given: 선호 채널 1, 4, 12, 56 등록 후 현재 6번 시청
    currentCh = "1";
    TVController controller = makeController();

    controller.pushButton(remoteKey::KEY_FAVORITE_ADD);
    currentCh = "4";
    controller.pushButton(remoteKey::KEY_FAVORITE_ADD);
    controller.pushButton(remoteKey::KEY_1);
    controller.pushButton(remoteKey::KEY_2);
    controller.pushButton(remoteKey::KEY_FAVORITE_ADD);
    controller.pushButton(remoteKey::KEY_5);
    controller.pushButton(remoteKey::KEY_6);
    controller.pushButton(remoteKey::KEY_FAVORITE_ADD);
    currentCh = "6";

    // When: 다음 선호 채널
    controller.pushButton(remoteKey::KEY_NEXT_FAVORITE);

    // Then: 6보다 큰 선호 채널 중 최소값 12
    expectChannel(12);
}

// pushButton -> clearPendingDigits, handleChannelUp
TEST_F(TVControllerTest, PendingDigitFunctionKey_InvalidatesPendingAndAppliesFunction) {
    // Given: 45 확정 후 6 보류 상태
    currentCh = "0";
    TVController controller = makeController();

    controller.pushButton(remoteKey::KEY_4);
    controller.pushButton(remoteKey::KEY_5);
    ASSERT_EQ("45", currentCh);

    controller.pushButton(remoteKey::KEY_6);
    ASSERT_EQ("45", currentCh);

    // When: 보류 중 기능 버튼(채널 업) 입력
    controller.pushButton(remoteKey::KEY_CH_UP);

    // Then: 6은 무효화되고 45에서 채널 업 -> 46
    expectChannel(46);
}

// pushButton -> handleDigit (4자리), confirmPendingDigits
TEST_F(TVControllerTest, FourDigits_ChangesAsTwoPairs) {
    // Given: 현재 0번
    currentCh = "0";
    TVController controller = makeController();

    // When: '1','2','3','4' 연속 입력
    controller.pushButton(remoteKey::KEY_1);
    controller.pushButton(remoteKey::KEY_2);
    ASSERT_EQ("12", currentCh);

    controller.pushButton(remoteKey::KEY_3);
    controller.pushButton(remoteKey::KEY_4);

    // Then: 12 후 34로 변경
    expectChannel(34);
}

// pushButton -> confirmPendingDigits (선행 0 + 확인)
TEST_F(TVControllerTest, LeadingZeroThenConfirm_ChangesToChannelZero) {
    // Given: 현재 50번
    currentCh = "50";
    TVController controller = makeController();

    // When: '0' 입력 후 확인
    controller.pushButton(remoteKey::KEY_0);
    controller.pushButton(remoteKey::KEY_OK);

    // Then: 0번 채널
    expectChannel(0);
}
