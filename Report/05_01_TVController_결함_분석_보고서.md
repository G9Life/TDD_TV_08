# TDD TV 프로젝트 — TVController 결함 분석 보고서

| 항목 | 내용 |
|---|---|
| 문서 번호 | RPT-06 |
| 단계 | 7단계 — 디버깅 및 결함 분석 (Defect Analysis) |
| 프로젝트 | TDD TV (C++17, CMake, Google Test, Google Mock) |
| 기준 문서 | `README.md`, `docs/01_requirements_analysis.md`, `Report/04_TVController_테스트_구현_보고서.md` |
| 대상 모듈 | `TVController` (`include/TVController.h`, `src/TVController.cpp`) |
| 테스트 산출물 | `test/TVControllerTest.cpp`, `test/TunerTest.cpp` |
| 작성 관점 | C++ QA 엔지니어 (디버깅·결함 분석) |
| 작성일 | 2026-05-19 |

---

## 1. 보고서 개요

본 보고서는 `TVController` 및 `Tuner` Mock 테스트(`TVControllerTest`, `TunerTest`)를 대상으로 **ctest 실행 결과를 분석**하고, `TVController.cpp` 전 함수 단위로 결함 가능 지점을 특정·분류하며, **최소 변경(C++17) 수정 방안**과 Green 확인 절차를 정리한다.

### 1.1 분석 범위

| 구분 | 내용 |
|------|------|
| 구현 대상 | `src/TVController.cpp` (전 함수) |
| 테스트 대상 | `test/TVControllerTest.cpp` (10건), `test/TunerTest.cpp` (13건) |
| 제약 | `Item` 구조체 수정 금지 (본 프로젝트에 해당 타입 없음) |
| 제외 | `Tuner` 실구현 동작 검증 (Mock/Fake 계약만 사용) |

### 1.2 ctest 실행 결과 (2026-05-19)

```text
Test project C:/DEV/TDD_TV_08/build
23/23 tests passed — 100% Green
```

| 타깃 | 테스트 수 | 결과 |
|------|------------|------|
| `TunerTest` | 13 | Passed |
| `TVControllerTest` | 10 | Passed |
| **합계** | **23** | **100% Passed** |

**결론:** 현재 워크스페이스의 `TVController.cpp`는 기존 테스트 스위트 기준 **수정이 필요한 Red 결함이 없다.** 본 보고서 §3~§5는 전형적인 미구현·오구현 시나리오와 현재 구현의 대응 관계를 문서화한다.

---

## 2. EXPECT_EQ / ASSERT_EQ 실패 요약

### 2.1 분석 방법

- 실패 로그가 제공되지 않은 경우, `TVControllerTest`·`TunerTest`의 **기대 동작**과 **전형적인 스텁/미완성 구현**을 대조하여 Expected/Actual 차이를 추론한다.
- 실제 Red 로그가 확보되면 §2.2 표의 Actual 열을 실측값으로 갱신한다.

### 2.2 TVControllerTest — 전형 Red 시나리오

| # | TEST_F | 실패 지점 | Expected | Actual (전형 버그) | 연관 함수·줄 |
|---|--------|-----------|----------|-------------------|--------------|
| 1 | `TwoDigits_ChangesImmediately` | `expectChannel(12)` | `"12"` | `"0"` | `handleDigit` 65–69 |
| 2 | `FourDigits_ChangesAsTwoPairs` | 1·2 입력 후 / 최종 | `"12"` / `"34"` | `"0"` / `"12"` | `handleDigit` 65–69 |
| 3 | `DigitThenConfirm_ChangesToSingleDigitChannel` | `expectChannel(1)` | `"1"` | `"6"` | `pushButton` 162–164 |
| 4 | `LeadingZeroThenConfirm_ChangesToChannelZero` | `expectChannel(0)` | `"0"` | `"50"` | `setTunerCh` → `confirmPendingDigits` 47–54 |
| 5 | `LeadingZeroThenDigit_ChangesToDigitChannel` | `expectChannel(7)` | `"7"` | `"50"` 또는 `"0"` | `handleDigit` + `parseChannel` 37–40 |
| 6 | `ChannelUpWithoutSearch_WrapsFromMaxToMin` | `expectChannel(0)` | `"0"` | `"100"` 또는 `"99"` | `handleChannelUp` 74–76 |
| 7 | `ChannelDownWithoutSearch_WrapsFromMinToMax` | `expectChannel(99)` | `"99"` | `"-1"` 또는 `"0"` | `handleChannelDown` 90–92 |
| 8 | `ChannelSearchThenUp_MovesWithinStoredChannels` | `expectChannel(14)` | `"14"` | `"7"` 또는 `"6"` | `handleChannelSearch` 129–154, `handleChannelUp` 80–85 |
| 9 | `NextFavorite_ChangesToNearestGreaterFavorite` | `expectChannel(12)` | `"12"` | `"6"` | `handleNextFavorite` 115–126 |
| 10 | `PendingDigitFunctionKey_InvalidatesPendingAndAppliesFunction` | `expectChannel(46)` | `"46"` | `"6"` 등 | `pushButton` 167–169, `handleChannelUp` 72–77 |

### 2.3 TunerTest

`TunerTest`는 `MockTuner`에 대한 **계약 검증**이며, `TVController` 구현과 무관하다. 현재 13건 모두 Passed.

---

## 3. TVController.cpp 함수별 결함 위치 특정

| 함수 | 줄 | 전형 결함 설명 | 현재 상태 |
|------|-----|----------------|-----------|
| `TVController` (생성자) | 16–17 | `processingCH` 미초기화 | ✅ 정상 |
| `isDigitKey` | 19–21 | 숫자 키 범위 판별 오류 | ✅ 정상 |
| `digitChar` | 23–25 | enum → 문자 변환 오류 | ✅ 정상 |
| `getCurrentChannel` | 27–29 | `getCurrentCH()` 미호출 | ✅ 정상 |
| `validateChannel` | 31–35 | 0~99 범위 검증 누락 | ✅ 정상 |
| `parseChannel` | 37–41 | `stoi` 후 검증 누락 | ✅ 정상 |
| `setChannel` | 43–46 | 검증 없이 `setCH` 호출 | ✅ 정상 |
| `confirmPendingDigits` | 47–54 | 빈 버퍼·확정·초기화 흐름 오류 | ✅ 정상 |
| `setTunerCh` | 56–58 | `confirmPendingDigits` 미위임 | ✅ 정상 |
| `clearPendingDigits` | 60–63 | 버퍼 미삭제 | ✅ 정상 |
| **`handleDigit`** | **65–69** | **2자리 즉시 확정 누락** | ✅ 구현됨 |
| **`handleChannelUp`** | **72–86** | **래핑·검색 목록 이동 오류** | ✅ 구현됨 |
| **`handleChannelDown`** | **88–102** | **래핑·`lower_bound`+`prev` 오류** | ✅ 구현됨 |
| `toggleFavorite` | 104–113 | toggle·정렬·중복 처리 오류 | ✅ 정상 |
| `handleNextFavorite` | 115–127 | `upper_bound`·순환 미구현 | ✅ 정상 |
| `handleChannelSearch` | 129–154 | `seekCH` 루프·중복·정렬 오류 | ✅ 정상 |
| **`pushButton`** | **156–190** | **KEY_OK·보류 버퍼 클리어 누락** | ✅ 구현됨 |

### 3.1 핵심 구현 근거 (Green)

**2자리 즉시 확정 (`handleDigit`):**

```cpp
void TVController::handleDigit(remoteKey key) {
    processingCH += digitChar(key);
    if (processingCH.size() >= 2) {
        confirmPendingDigits();
    }
}
```

**확인 버튼·보류 숫자 무효화 (`pushButton`):**

```cpp
    if (key == remoteKey::KEY_OK) {
        setTunerCh();
        return;
    }

    if (!processingCH.empty()) {
        clearPendingDigits();
    }
```

**채널 Up 래핑 (검색 결과 없음):**

```cpp
    const int next = (current == MAX_CHANNEL) ? MIN_CHANNEL : current + 1;
```

---

## 4. 결함 심각도 분류 및 근거

| ID | 영역 | 위치 | 심각도 | 근거 |
|----|------|------|--------|------|
| DEF-01 | 숫자 입력 | `handleDigit` 67–68 | **Critical** | 2자리 채널 확정 불가 → 핵심 UX 전면 실패 |
| DEF-02 | 숫자 입력 | `pushButton` 162–164 | **Critical** | 1자리 + 확인 확정 불가 |
| DEF-03 | 숫자 입력 | `pushButton` 167–169 | **Major** | 보류 숫자가 기능키와 충돌 (D-08 요구사항 위반) |
| DEF-04 | Up/Down | `handleChannelUp` 75 | **Major** | 99→0 래핑 실패 → 경계 채널 고착 |
| DEF-05 | Up/Down | `handleChannelDown` 91 | **Major** | 0→99 래핑 실패 |
| DEF-06 | 검색+Up/Down | `handleChannelSearch`, `handleChannelUp/Down` | **Major** | 검색 결과 무시 시 일반 채널 증감으로 오동작 |
| DEF-07 | 선호 채널 | `handleNextFavorite` 121–126 | **Major** | 다음 선호 채널 이동·순환 실패 |
| DEF-08 | 빌드 | (과거) `isDigitOrConfirm` | **Minor** | 헤더 미선언으로 컴파일 실패 (RPT-05에서 제거 완료) |
| DEF-09 | 테스트 품질 | gmock Uninteresting warning | **Info** | `ON_CALL` 기본 동작 경고, 기능 영향 없음 |

### 4.1 심각도 정의 (본 프로젝트)

| 등급 | 기준 |
|------|------|
| **Critical** | 채널 변경 핵심 경로 불가, 다수 P0 테스트 실패 |
| **Major** | 특정 시나리오(경계·검색·선호·보류 숫자) 오동작 |
| **Minor** | 빌드·정적 분석 이슈, 단일 경로 제한 |
| **Info** | 경고·문서·커버리지 미측정 등 품질 개선 권고 |

---

## 5. 최소 변경 수정 방안 (C++17)

**제약:** `Item` 구조체 수정 금지 — 본 프로젝트에는 해당 구조체가 없으며, 수정 대상도 아니다.

현재 구현은 아래 방안을 **이미 반영**하였다. Red 상태에서 복구할 때 적용할 최소 패치 요약이다.

### 5.1 `handleDigit` — 2자리 즉시 확정

```diff
 void TVController::handleDigit(remoteKey key) {
     processingCH += digitChar(key);
+    if (processingCH.size() >= 2) {
+        confirmPendingDigits();
+    }
 }
```

### 5.2 `pushButton` — 확인·보류 버퍼 처리

```diff
+    if (key == remoteKey::KEY_OK) {
+        setTunerCh();
+        return;
+    }
+
+    if (!processingCH.empty()) {
+        clearPendingDigits();
+    }
+
     switch (key) {
```

### 5.3 `handleChannelUp` / `handleChannelDown` — 경계 래핑

```cpp
// 검색 결과 없음 — Up
const int next = (current == MAX_CHANNEL) ? MIN_CHANNEL : current + 1;

// 검색 결과 없음 — Down
const int next = (current == MIN_CHANNEL) ? MAX_CHANNEL : current - 1;
```

### 5.4 검색·선호 — STL 알고리즘

```cpp
// 검색 후 Up: current보다 큰 저장 채널 중 최소
const auto it = std::upper_bound(searchedChannels.begin(), searchedChannels.end(), current);
if (it == searchedChannels.end()) {
    setChannel(searchedChannels.front());
} else {
    setChannel(*it);
}

// 다음 선호: favoriteChannels에 동일 패턴 적용
```

---

## 6. 수정 diff 제안 및 Green 확인 절차

### 6.1 현재 상태

추가 소스 패치 **불필요**. 아래 절차로 Green 재확인만 수행한다.

### 6.2 빌드·테스트 (PowerShell)

```powershell
cd c:\DEV\TDD_TV_08
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

### 6.3 단일 테스트 재현 (Red 디버깅 시)

```powershell
.\build\TVControllerTest.exe --gtest_filter=TVControllerTest.TwoDigits_ChangesImmediately
.\build\TVControllerTest.exe --gtest_filter=TVControllerTest.PendingDigitFunctionKey_InvalidatesPendingAndAppliesFunction
```

### 6.4 Green 확인 체크리스트

| 단계 | 명령 | 기대 결과 |
|------|------|-----------|
| 1 | `cmake --build build` | 빌드 성공 (exit 0) |
| 2 | `ctest --test-dir build --output-on-failure` | 23/23 Passed |
| 3 | 실패 시 | `--gtest_filter`로 단일 테스트 격리 후 §2.2·§3 표와 대조 |

### 6.5 실행 기록 (2026-05-19)

| 항목 | 결과 |
|------|------|
| `cmake --build build` | 성공 |
| `ctest` | **23/23 Passed** |
| 총 소요 시간 | 약 12.5초 (real) |

---

## 7. 요구사항·테스트 추적

### 7.1 README TDD practice 대비

| README 항목 | 관련 TEST_F | 현재 구현 |
|-------------|-------------|-----------|
| 1자리 + 확인 | `DigitThenConfirm_...` | ✅ |
| 2자리 즉시 | `TwoDigits_...` | ✅ |
| 4자리 2쌍 | `FourDigits_...` | ✅ |
| 0, 7 → 7번 | `LeadingZeroThenDigit_...` | ✅ |
| 0 + 확인 → 0번 | `LeadingZeroThenConfirm_...` | ✅ |
| 보류 숫자 + 기능키 | `PendingDigitFunctionKey_...` | ✅ |
| 99↔0 래핑 | `ChannelUp/Down WithoutSearch_...` | ✅ |
| 검색 후 Up | `ChannelSearchThenUp_...` | ✅ |
| 다음 선호 | `NextFavorite_...` | ✅ |

### 7.2 RPT-04 미구현 테스트 (후속)

| 계획 ID | TEST_F (계획) | 상태 |
|---------|---------------|------|
| D-04 | `ThirdDigit_StartsNextPendingInput` | ❌ 후속 |
| D-05 | `PendingSingleDigitConfirm_...` | ❌ 후속 |
| U-01~U-02 | ±1 증감 (검색 없음) | ❌ 후속 |
| S-02 | `ChannelDownWithSearchResult_...` | ❌ 후속 |
| F-04~F-05 | 선호 래핑 / 빈 목록 | ❌ 후속 |

후속 테스트 추가 시 본 보고서 §2.2·§3 표에 행을 확장하여 재분석한다.

---

## 8. 결론 및 권고

1. **현재 `TVController.cpp`는 `ctest` 23/23 Green**이며, §3에 열거한 전형 결함은 구현으로 해소되어 있다.
2. 실패 로그가 재현되면 §2.2 Actual 열을 실측값으로 갱신하고, §3 줄 번호와 1:1 매핑하여 패치 범위를 최소화한다.
3. RPT-04 잔여 P0/P1 시나리오는 `test/TVControllerTest.cpp`에 `TEST_F`를 추가한 뒤 본 보고서 형식으로 회귀 분석한다.
4. gmock Uninteresting warning은 `EXPECT_CALL` 정밀화 또는 테스트 전용 `-Wno-*` 없이 **의도된 호출만 EXPECT** 하는 방향으로 Info 수준 개선 가능하다.

---

## 9. 참조 문서

| 문서 | 경로 |
|------|------|
| 요구사항 분석 | `docs/01_requirements_analysis.md` |
| 구현 보고서 | `Report/01_02_TVController_구현_보고서.md` |
| 테스트 계획 | `Report/03_테스트_계획_보고서.md` |
| 테스트 구현 | `Report/04_TVController_테스트_구현_보고서.md` |
| 프로젝트 README | `README.md` |

---

*문서 끝 — RPT-06 TVController 결함 분석 보고서*
