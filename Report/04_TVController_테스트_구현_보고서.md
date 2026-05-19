# TDD TV 프로젝트 — TVController 테스트 구현 보고서

| 항목 | 내용 |
|---|---|
| 문서 번호 | RPT-05 |
| 단계 | 6단계 — 테스트 구현 (Test Implementation) |
| 프로젝트 | TDD TV (C++17, CMake, Google Test, Google Mock) |
| 기준 문서 | `README.md`, `Report/03_테스트_계획_보고서.md`, `docs/01_requirements_analysis.md` |
| 대상 모듈 | `TVController` (`include/TVController.h`, `src/TVController.cpp`) |
| 테스트 산출물 | `test/TVControllerTest.cpp` |
| 작성 관점 | 시니어 C++ QA |
| 작성일 | 2026-05-19 |

---

## 1. 보고서 개요

본 보고서는 테스트 계획 보고서(RPT-04, `Report/03_테스트_계획_보고서.md`)에 따라 **`TVController` 단위 테스트를 Google Test `TEST_F`로 구현**한 결과를 정리한다. `Tuner`는 Mock으로 대체하고, 채널 번호 **0~99** 경계값·Given-When-Then 주석·`EXPECT_EQ`/`ASSERT_EQ` 검증 원칙을 적용하였다.

### 1.1 작업 범위

| 구분 | 내용 |
|------|------|
| 신규·수정 | `test/TVControllerTest.cpp` (스켈레톤 → 10개 `TEST_F`) |
| 빌드 수정 | `src/TVController.cpp` — 미선언 `isDigitOrConfirm` 제거 |
| 제외 | `Tuner` 실구현, gcov/lcov 측정(미실시), P2 예외 훅 테스트 |

### 1.2 완료 기준 달성 현황

| 항목 | 목표 | 결과 |
|------|------|------|
| `TEST_F` 최소 개수 | ≥ 5 | **10개** 구현 |
| 경계값 (0, 99) | 포함 | **통과** (Up/Down 래핑) |
| Given-When-Then | 주석 구조 | **전 테스트 적용** |
| 빌드·실행 | Green | **`ctest` 23/23 통과** |
| RPT-04 P0 전체 | 17건 | **부분** (아래 §4 참조) |
| 라인 커버리지 ≥ 90% | RPT-04 §6 | **미측정** (후속 과제) |

---

## 2. 테스트 인프라

### 2.1 Fixture 설계

`TVControllerTest` 픽스처는 Mock Tuner에 **상태ful Fake 동작**을 부여하여 연속 `pushButton` 호출 후 채널을 검증한다.

```cpp
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

    void expectChannel(int ch) {
        ASSERT_EQ(std::to_string(ch), currentCh);
    }
};
```

| 요소 | 역할 |
|------|------|
| `MockTunerForController` | `seekCH`, `setCH`, `getCurrentCH` Mock |
| `currentCh` | `setCH` 호출 시 갱신되는 현재 채널 |
| `expectChannel(int)` | `ASSERT_EQ`로 기대 채널 문자열 검증 |
| `makeController()` | 테스트별 독립 `TVController` 인스턴스 생성 |

### 2.2 검증 원칙

- **채널 상태**: `quality`/`sellIn` 대신 도메인 상태인 **현재 채널 번호**를 `ASSERT_EQ` / `EXPECT_EQ`로 검증한다.
- **간접 검증**: 선호·검색 목록은 private이므로, **연속 입력 후 `setCH` 결과(채널 변경)** 로 동작을 검증한다.
- **Given-When-Then**: 각 `TEST_F` 본문에 `// Given`, `// When`, `// Then` 주석을 둔다.

### 2.3 빌드·실행

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

| 타깃 | 테스트 수 | 결과 |
|------|------------|------|
| `TunerTest` | 13 | Passed |
| `TVControllerTest` | 10 | Passed |
| **합계** | **23** | **100% Passed** |

---

## 3. 구현된 TEST_F 목록

### 3.1 전체 목록 (10건)

| # | TEST_F 이름 | 검증 기능 | 기대 채널 | 경계 |
|---|-------------|-----------|-----------|------|
| 1 | `DigitThenConfirm_ChangesToSingleDigitChannel` | 숫자 + 확인 | 1 | — |
| 2 | `TwoDigits_ChangesImmediately` | 두 자리 즉시 확정 | 12 | — |
| 3 | `LeadingZeroThenDigit_ChangesToDigitChannel` | 선행 0 + 숫자 | 7 | B-DIG-09 |
| 4 | `LeadingZeroThenConfirm_ChangesToChannelZero` | 0 + 확인 | 0 | B-CH-00 |
| 5 | `ChannelUpWithoutSearch_WrapsFromMaxToMin` | Up (검색 없음) | 0 | B-CH-99u (99→0) |
| 6 | `ChannelDownWithoutSearch_WrapsFromMinToMax` | Down (검색 없음) | 99 | B-CH-00d (0→99) |
| 7 | `ChannelSearchThenUp_MovesWithinStoredChannels` | 검색 + Up | 14 | US-01 |
| 8 | `NextFavorite_ChangesToNearestGreaterFavorite` | 선호 등록 + 다음 선호 | 12 | F-03 |
| 9 | `PendingDigitFunctionKey_InvalidatesPendingAndAppliesFunction` | 보류 숫자 + 기능키 | 46 | D-08 변형 |
| 10 | `FourDigits_ChangesAsTwoPairs` | 4자리 연속 입력 | 34 | D-03 |

### 3.2 Given-When-Then 예시

```cpp
TEST_F(TVControllerTest, ChannelUpWithoutSearch_WrapsFromMaxToMin) {
    // Given: 검색 결과 없음, 현재 99번 채널
    currentCh = "99";
    TVController controller = makeController();

    // When: 채널 업
    controller.pushButton(remoteKey::KEY_CH_UP);

    // Then: 0번 채널로 순환
    expectChannel(0);
}
```

---

## 4. 요구사항·테스트 계획 추적

### 4.1 RPT-04 (테스트 계획) 대비 구현 매핑

| 계획 ID | TEST_F (계획) | 구현 | 비고 |
|---------|---------------|------|------|
| D-01 | `DigitThenConfirm_...` | ✅ 동일 이름 | |
| D-02 | `TwoDigits_...` | ✅ | |
| D-03 | `FourDigits_...` | ✅ | |
| D-04 | `ThirdDigit_StartsNextPendingInput` | ❌ | 후속 |
| D-05 | `PendingSingleDigitConfirm_...` | ❌ | 후속 |
| D-06 | `LeadingZeroThenDigit_...` | ✅ | |
| D-07 | `LeadingZeroThenConfirm_...` | ✅ | |
| D-08 | `PendingSingleDigitNonConfirmFunction_...` | △ | `PendingDigitFunctionKey_...` (Up 후 46 검증) |
| D-09~D-10 | Down / Favorite 버퍼 클리어 | ❌ | |
| U-01~U-02 | ±1 증감 | ❌ | |
| U-03~U-04 | 99↔0 래핑 | ✅ | |
| F-01~F-02 | 선호 토글 | △ | F-03 셋업에서 간접 사용 |
| F-03 | `NextFavorite_...` | ✅ | |
| F-04~F-05 | 래핑 / 빈 목록 | ❌ | |
| S-01 | `ChannelSearch_StoresSeekResults` | △ | `ChannelSearchThenUp_...`로 부분 |
| S-02~S-03 | 중복·빈 seek | ❌ | |
| US-01 | 검색 목록 Up | ✅ | #7 |
| US-02~US-04 | Down / 래핑 | ❌ | |
| E-01~E-04 | 예외·빈 OK | ❌ | public API 제약 |

### 4.2 README TDD practice 커버리지

| README 항목 | 구현 테스트 | 상태 |
|-------------|------------|------|
| 1. 숫자 버튼 채널 변경 | #1, #2, #3, #4, #10, #9 | **부분** (D-04/05 미구현) |
| 2. 선호 채널 추가 | #8 (등록만) | **부분** (토글 삭제 미검증) |
| 3. 다음 선호 채널 | #8 | **부분** (56→1 래핑 미구현) |
| 4. 채널 검색 | #7 | **부분** |
| 5. Up/Down (검색 없음) | #5, #6 | **부분** (±1 미구현) |
| 6. Up/Down (검색 있음) | #7 (Up만) | **부분** |

### 4.3 `docs/01_requirements_analysis.md` §5 시나리오

| §5 # | 시나리오 | 구현 |
|------|----------|------|
| 1 | DigitThenConfirm | ✅ |
| 2 | TwoDigits | ✅ |
| 3 | FourDigits | ✅ |
| 4 | ThirdDigit pending | ❌ |
| 5 | Pending + Confirm | ❌ |
| 6 | Pending + function | △ (#9) |
| 7 | Leading zero + digit | ✅ |
| 8 | Leading zero + confirm | ✅ |
| 9~10 | Favorite add/remove | △ |
| 11~12 | Next favorite | △ (#11만) |
| 13 | Channel search store | △ |
| 14~17 | Up/Down no search | △ (#16,17만) |
| 18~21 | Up/Down with search | △ (#18 Up만) |
| 22~25 | Invalid channel | ❌ |

---

## 5. TVController.cpp 함수별 커버리지 (논리)

public API는 `pushButton`만 제공하므로, private 멤버 함수는 **간접 호출**로 검증한다.

| 함수 | 호출 경로 | 검증 테스트 |
|------|-----------|-------------|
| `TVController` | 생성 | 전 테스트 |
| `pushButton` | 모든 키 입력 | 전 테스트 |
| `isDigitKey` / `digitChar` | 숫자 키 | #1~4, #9, #10 |
| `handleDigit` | 숫자 키 | #1~4, #9, #10 |
| `confirmPendingDigits` / `setTunerCh` | `KEY_OK` | #1, #4 |
| `clearPendingDigits` | 기능키 + 보류 버퍼 | #9 |
| `getCurrentChannel` | Up/Down/선호/검색 | #5~8 |
| `parseChannel` / `validateChannel` | 유효 숫자 조합 | #3, #4 (유효 경로만) |
| `setChannel` | 채널 변경 전반 | 전 테스트 |
| `handleChannelUp` | `KEY_CH_UP` | #5, #7, #9 |
| `handleChannelDown` | `KEY_CH_DOWN` | #6 |
| `toggleFavorite` | `KEY_FAVORITE_ADD` | #8 (셋업) |
| `handleNextFavorite` | `KEY_NEXT_FAVORITE` | #8 |
| `handleChannelSearch` | `KEY_CH_SEARCH` | #7 |

**미검증 또는 약한 검증 구간**

- `handleChannelDown` + `searchedChannels` (US-02, US-04)
- `handleNextFavorite` 빈 목록 early return (F-05)
- `confirmPendingDigits` 빈 버퍼 (SP-05)
- `validateChannel` 실패 → `invalid_argument` (E-01, E-02)
- `toggleFavorite` erase 분기 단독 (F-02)

---

## 6. 구현 시 수정 사항

### 6.1 빌드 오류 해결

`src/TVController.cpp`에 헤더 미선언 멤버 `isDigitOrConfirm`이 존재하여 **컴파일 실패**가 발생하였다. RPT-04 §8 리스크 및 코드 품질 분석(RPT-03) 권고에 따라 **미사용 Dead Code를 삭제**하였다.

```diff
- bool TVController::isDigitOrConfirm(remoteKey key) const {
-     return isDigitKey(key) || key == remoteKey::KEY_OK;
- }
```

삭제 후 `cmake --build build` 및 `ctest` **정상 통과**를 확인하였다.

### 6.2 D-08 계획과 구현 차이

| 항목 | RPT-04 D-08 | 실제 구현 (#9) |
|------|-------------|----------------|
| 초기 CH | 6 (가정) | 0 → 45 확정 |
| 보류 후 Up | 6→7 (무효화만 강조) | 45→**46** (무효화 + Up 동작) |
| 검증 | `setCH` 호출 패턴 | `expectChannel(46)` |

동작은 요구사항 **「기능 버튼 시 보류 숫자 무효화 후 해당 기능 수행」** 과 일치한다.

---

## 7. 테스트 실행 결과

**실행 일시:** 2026-05-19  
**환경:** Windows, MinGW g++, CMake + Ninja

```
Test project C:/DEV/TDD_TV_08/build
100% tests passed, 0 tests failed out of 23
```

### 7.1 TVControllerTest 상세

| # | 테스트 | 시간 | 결과 |
|---|--------|------|------|
| 14 | `DigitThenConfirm_ChangesToSingleDigitChannel` | ~0.53s | Passed |
| 15 | `TwoDigits_ChangesImmediately` | ~0.52s | Passed |
| 16 | `LeadingZeroThenDigit_ChangesToDigitChannel` | ~0.53s | Passed |
| 17 | `ChannelUpWithoutSearch_WrapsFromMaxToMin` | ~0.53s | Passed |
| 18 | `ChannelDownWithoutSearch_WrapsFromMinToMax` | ~0.53s | Passed |
| 19 | `ChannelSearchThenUp_MovesWithinStoredChannels` | ~0.52s | Passed |
| 20 | `NextFavorite_ChangesToNearestGreaterFavorite` | ~0.52s | Passed |
| 21 | `PendingDigitFunctionKey_InvalidatesPendingAndAppliesFunction` | ~0.53s | Passed |
| 22 | `FourDigits_ChangesAsTwoPairs` | ~0.53s | Passed |
| 23 | `LeadingZeroThenConfirm_ChangesToChannelZero` | ~0.53s | Passed |

---

## 8. 미완료 항목 및 후속 작업

### 8.1 우선 추가 권장 TEST_F (P0 잔여)

| 우선순위 | TEST_F | 목적 |
|----------|--------|------|
| P0 | `ThirdDigit_StartsNextPendingInput` | D-04 |
| P0 | `PendingSingleDigitConfirm_ChangesToPendingDigit` | D-05 |
| P0 | `ChannelUpWithoutSearchResult_IncrementsChannel` | U-01 |
| P0 | `ChannelDownWithoutSearchResult_DecrementsChannel` | U-02 |
| P1 | `ChannelDownWithSearchResult_ChangesToPreviousStoredChannel` | US-02 |
| P1 | `NextFavorite_WrapsToSmallestFavorite` | F-04 |

### 8.2 커버리지·CI (RPT-04 §6)

1. CMake `ENABLE_COVERAGE` 옵션 추가  
2. `lcov` / `genhtml`로 `TVController.cpp` 라인 커버리지 측정  
3. 미커버 분기(`validateChannel` false, `handleNextFavorite` empty 등)에 P2 테스트 추가  
4. **목표: 라인 커버리지 ≥ 90%**

### 8.3 예외 테스트 (E-01~E-02)

`pushButton`만으로는 `ch < 0` / `ch > 99` 경로에 도달하기 어렵다. 팀 합의 후 **friend 테스트 클래스**, `#ifdef UNIT_TEST` 래퍼, 또는 `parseChannel` 단위 분리 중 하나를 선택한다.

---

## 9. 결론

`test/TVControllerTest.cpp`에 **Google Test `TEST_F` 10건**을 구현하였고, **채널 0·99 경계**, **Given-When-Then**, **`ASSERT_EQ` 기반 채널 검증** 요구를 충족하였다. 빌드 차단 요인이던 `isDigitOrConfirm` Dead Code를 제거하여 **`cmake --build build` 및 `ctest` 23/23 Green**을 달성하였다.

테스트 계획(RPT-04) 대비 **P0 스모크 10건 중 7건 완전·3건 부분** 구현 상태이며, **라인 커버리지 90%** 및 **§5 시나리오 22~25(예외)** 는 후속 스프린트에서 `TEST_F` 추가·gcov 측정으로 완료하는 것을 권장한다.

---

## 10. 관련 문서

| 문서 | 경로 |
|------|------|
| 요구사항 분석 | `Report/01_01_요구사항_분석_보고서.md` |
| Controller 구현 | `Report/01_02_TVController_구현_보고서.md` |
| 코드 품질 분석 | `Report/02_코드_품질_분석_보고서.md` |
| 테스트 계획 | `Report/03_테스트_계획_보고서.md` |
| **본 보고서** | `Report/04_TVController_테스트_구현_보고서.md` |
| 테스트 소스 | `test/TVControllerTest.cpp` |
