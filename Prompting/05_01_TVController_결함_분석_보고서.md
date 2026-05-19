# TVController test failure analysis
_Exported on 5/19/2026 at 14:04:32 GMT+9 from Cursor (3.4.17)_

---

**User**

@TunerTest.cpp @TVControllerTest.cpp
@TVController.cpp

[P] 디버깅과 결함 분석에 능한 C++ QA 엔지니어입니다.
[T] (여기에 ctest 실패 로그를 붙여넣을 것)
    1) EXPECT_EQ 실패의 기대/실제 차이 요약
    2) TVController.cpp 전체 함수에 버그 위치 특정 (파일명:줄번호)
    3) 결함 심각도 (Critical/Major/Minor/Info) 분류 및 근거
    4) 최소 변경 수정 방안 (C++17 스타일)
       - 단, Item 구조체 수정 금지
[F] 수정 diff 제안 + cmake --build build && ctest Green 확인 절차


---

**Cursor**

ctest 실패 로그가 아직 없으므로, 관련 소스와 테스트를 읽고 `ctest`를 실행해 실패 내용을 확인합니다.


ctest는 모두 통과했습니다. 이전 실패 로그와 git 이력을 확인해 결함 분석을 진행합니다.


# TVController 결함 분석 보고서 (QA)

> **참고:** 프롬프트의 `(여기에 ctest 실패 로그를 붙여넣을 것)` 자리에 실패 로그가 없어, 워크스페이스에서 `ctest --output-on-failure`를 직접 실행했습니다.

## 실행 결과 (현재 워크스페이스)

```
23/23 tests passed — 100% Green
```

현재 `src/TVController.cpp`는 커밋(`91fd091`)과 동일하며, **수정이 필요한 실패 케이스는 없습니다.**  
아래는 **테스트·요구사항 대비 전형적인 결함 패턴**과, 현재 구현이 이를 어떻게 막는지에 대한 분석입니다. 실제 Red 로그가 있으면 §1 표에 그대로 대입하면 됩니다.

---

## 1) EXPECT_EQ / ASSERT_EQ 실패 요약 (전형 Red 시나리오)

| 테스트 | 실패 지점 | Expected | Actual (전형 버그) | 원인 요약 |
|--------|-----------|----------|-------------------|-----------|
| `TwoDigits_ChangesImmediately` | `expectChannel(12)` | `"12"` | `"0"` | 2자리 즉시 확정 미구현 |
| `FourDigits_ChangesAsTwoPairs` | 1·2 입력 후 / 최종 | `"12"` / `"34"` | `"0"` / `"12"` | 동일 |
| `DigitThenConfirm_ChangesToSingleDigitChannel` | `expectChannel(1)` | `"1"` | `"6"` | `KEY_OK` → `confirmPendingDigits` 미연결 |
| `LeadingZeroThenConfirm_ChangesToChannelZero` | `expectChannel(0)` | `"0"` | `"50"` | 확인 시 버퍼 미확정 |
| `ChannelUpWithoutSearch_WrapsFromMaxToMin` | `expectChannel(0)` | `"0"` | `"100"` 또는 `"99"` | 99→0 래핑 누락 |
| `ChannelDownWithoutSearch_WrapsFromMinToMax` | `expectChannel(99)` | `"99"` | `"-1"` 또는 `"0"` | 0→99 래핑 누락 |
| `ChannelSearchThenUp_MovesWithinStoredChannels` | `expectChannel(14)` | `"14"` | `"7"` 또는 `"6"` | 검색 목록 미저장 / 일반 Up 사용 |
| `NextFavorite_ChangesToNearestGreaterFavorite` | `expectChannel(12)` | `"12"` | `"6"` | `upper_bound`·순환 미구현 |
| `PendingDigitFunctionKey_...` | `expectChannel(46)` | `"46"` | `"6"` 등 | 기능키 전 `processingCH` 미삭제 |

**현재 코드:** 위 시나리오는 모두 통과합니다.

---

## 2) TVController.cpp 버그 위치 (함수별)

| 함수 | 줄 | 전형 결함 | 현재 |
|------|-----|-----------|------|
| `handleDigit` | 65–69 | `size() >= 2` 시 `confirmPendingDigits()` 누락 | ✅ 구현됨 |
| `confirmPendingDigits` | 47–54 | 빈 버퍼·`parseChannel`·`setChannel` 흐름 | ✅ |
| `pushButton` | 162–164 | `KEY_OK` → `setTunerCh()` 미연결 | ✅ |
| `pushButton` | 167–169 | 기능키 전 `clearPendingDigits()` 누락 | ✅ |
| `handleChannelUp` | 74–76 | `MAX_CHANNEL` 래핑 | ✅ |
| `handleChannelDown` | 90–92 | `MIN_CHANNEL` 래핑 | ✅ |
| `handleChannelUp` | 80–85 | 검색 목록 `upper_bound` | ✅ |
| `handleChannelDown` | 96–101 | 검색 목록 `lower_bound` + `prev` | ✅ |
| `handleNextFavorite` | 121–126 | `upper_bound` + 맨 앞 순환 | ✅ |
| `handleChannelSearch` | 129–154 | `seekCH` 루프·중복 제거·정렬 | ✅ |
| `toggleFavorite` | 104–112 | toggle + 정렬 | ✅ |

**현재 워크스페이스:** 위 위치에 **수정 대상 결함 없음**.

---

## 3) 결함 심각도 분류

| ID | 영역 | 심각도 | 근거 |
|----|------|--------|------|
| D-01 | 2자리 즉시 확정 (`handleDigit` 67–68) | **Critical** | 핵심 채널 입력 불가 |
| D-02 | 확인 버튼 (`pushButton` 162–164) | **Critical** | 1자리 채널 확정 불가 |
| D-03 | 보류 숫자 무효화 (`pushButton` 167–169) | **Major** | 45+6+Up 등 혼합 입력 오동작 |
| D-04 | CH Up/Down 래핑 (75, 91) | **Major** | 0/99 경계 위반 |
| D-05 | 검색 후 Up/Down (80–85, 96–101) | **Major** | 검색 기능 무의미 |
| D-06 | 다음 선호 (121–126) | **Major** | 선호 채널 UX 핵심 |
| D-07 | `isDigitOrConfirm` 헤더 미선언 | **Minor** | 빌드 실패 (이미 제거됨) |
| D-08 | gmock Uninteresting warning | **Info** | 동작에는 영향 없음 |

---

## 4) 최소 변경 수정 방안 (C++17, Item 구조체 변경 없음)

현재 구현이 이미 요구사항을 만족합니다. **Red였을 때의 최소 diff**는 아래와 같습니다.

### 4-A. `handleDigit` — 2자리 즉시 확정

```cpp
void TVController::handleDigit(remoteKey key) {
    processingCH += digitChar(key);
    if (processingCH.size() >= 2) {
        confirmPendingDigits();
    }
}
```

### 4-B. `pushButton` — 확인·보류 버퍼 처리

```cpp
    if (key == remoteKey::KEY_OK) {
        setTunerCh();
        return;
    }

    if (!processingCH.empty()) {
        clearPendingDigits();
    }
```

### 4-C. CH Up/Down — 래핑

```cpp
// handleChannelUp (검색 없음)
const int next = (current == MAX_CHANNEL) ? MIN_CHANNEL : current + 1;

// handleChannelDown (검색 없음)
const int next = (current == MIN_CHANNEL) ? MAX_CHANNEL : current - 1;
```

### 4-D. 검색·선호 — `<algorithm>` 기반

```cpp
const auto it = std::upper_bound(searchedChannels.begin(), searchedChannels.end(), current);
// end이면 searchedChannels.front(), 아니면 *it
```

`Item` 구조체는 프로젝트에 없으며, 수정 대상도 아닙니다.

---

## 5) 수정 diff 제안 + Green 확인 절차

### 상태

**추가 patch 불필요** — 아래 절차만으로 Green 확인 가능합니다.

### 빌드·테스트 (PowerShell)

```powershell
cd c:\DEV\TDD_TV_08
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

### 단일 실패 재현 시 (예: 2자리 확정)

```powershell
.\build\TVControllerTest.exe --gtest_filter=TVControllerTest.TwoDigits_ChangesImmediately
```

### 확인 결과 (2026-05-19 실행)

| 항목 | 결과 |
|------|------|
| `TunerTest` | 13/13 Passed |
| `TVControllerTest` | 10/10 Passed |
| **합계** | **23/23 Passed** |

---

## 핵심 구현 스냅샷 (현재 Green 근거)

```65:69:c:\DEV\TDD_TV_08\src\TVController.cpp
void TVController::handleDigit(remoteKey key) {
    processingCH += digitChar(key);
    if (processingCH.size() >= 2) {
        confirmPendingDigits();
    }
}
```

```156:169:c:\DEV\TDD_TV_08\src\TVController.cpp
void TVController::pushButton(remoteKey key) {
    if (isDigitKey(key)) {
        handleDigit(key);
        return;
    }

    if (key == remoteKey::KEY_OK) {
        setTunerCh();
        return;
    }

    if (!processingCH.empty()) {
        clearPendingDigits();
    }
```

---

## 다음 단계

실제 **Red 로그**가 있으면 `(여기에 ctest 실패 로그를 붙여넣을 것)` 자리에 붙여 주시면, §1 표를 **실측 Expected/Actual**로 갱신하고 해당 줄만 집중한 patch diff를 작성하겠습니다.

RPT-04 기준 미구현 테스트(`ThirdDigit_StartsNextPendingInput`, `ChannelDownWithSearchResult_...` 등) 추가 시에도 같은 형식으로 분석 가능합니다.

---

**User**

Report 폴더에 보고서로 작성해줘

---

**Cursor**

Report 폴더의 기존 보고서 형식을 확인한 뒤 QA 결함 분석 보고서를 작성합니다.


`Report/05_TVController_결함_분석_보고서.md`에 **RPT-06** 결함 분석 보고서를 작성했습니다.

### 문서 구성

| 섹션 | 내용 |
|------|------|
| §1 | 개요, ctest 23/23 Green 결과 |
| §2 | EXPECT_EQ/ASSERT_EQ 전형 Red 시나리오 표 |
| §3 | `TVController.cpp` 함수별 결함 위치(파일:줄) |
| §4 | Critical/Major/Minor/Info 심각도 분류 |
| §5 | C++17 최소 변경 수정 diff (Item 구조체 변경 없음) |
| §6 | `cmake --build` + `ctest` Green 확인 절차 |
| §7~§8 | 요구사항 추적, 결론·권고 |

기존 `Report/04_...` 보고서와 동일한 표·메타데이터 형식을 맞춰 두었습니다. 실제 ctest 실패 로그가 생기면 §2.2의 Actual 열만 실측값으로 갱신하면 됩니다.

