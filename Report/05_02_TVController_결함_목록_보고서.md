# TDD TV 프로젝트 — TVController 결함 목록 보고서

| 항목 | 내용 |
|---|---|
| 문서 번호 | RPT-07 |
| 단계 | 7단계 — 결함 문서화 (Defect Documentation) |
| 프로젝트 | TDD TV (C++17, CMake, Google Test, Google Mock) |
| 기준 문서 | `docs/01_requirements_analysis.md`, `Report/05_01_TVController_결함_분석_보고서.md` |
| 대상 모듈 | `TVController` (`include/TVController.h`, `src/TVController.cpp`) |
| 테스트 산출물 | `test/TVControllerTest.cpp`, `test/TunerTest.cpp` |
| 작성 관점 | QA 리드 (결함 추적·문서화) |
| 작성일 | 2026-05-19 |

---

## 1. 보고서 개요

본 보고서는 TDD TV 프로젝트에서 **발견된 테스트 실패·결함**을 QA 추적용 형식으로 정리한다. 각 항목은 **ID · Severity · FunctionName · Steps · Expected · Actual · Root Cause · Fix Summary · Status** 필드를 포함한다.

### 1.1 문서 목적

| 목적 | 설명 |
|------|------|
| 결함 추적 | Red 단계에서 관측된 오동작을 ID 기준으로 식별·관리 |
| 수정 검증 | Fix Summary 적용 후 `ctest` Green 여부와 매핑 |
| 회귀 분석 | 후속 테스트 추가 시 동일 형식으로 항목 확장 |

### 1.2 검증 현황 (2026-05-19)

```text
Test project C:/DEV/TDD_TV_08/build
23/23 tests passed — 100% Green
```

| 타깃 | 테스트 수 | 결과 |
|------|------------|------|
| `TunerTest` | 13 | Passed |
| `TVControllerTest` | 10 | Passed |
| **합계** | **23** | **100% Passed** |

> **Actual** 값은 미완성·오구현 스텁에서 관측된 **전형 실패값**이다. 현재 구현 기준 기능 결함(DEF-01~08)은 수정 완료(Fixed) 상태이다.

---

## 2. 심각도 정의

| 등급 | 기준 |
|------|------|
| **Critical** | 채널 변경 핵심 경로 불가, 다수 P0 테스트 실패 |
| **Major** | 특정 시나리오(경계·검색·선호·보류 숫자) 오동작 |
| **Minor** | 빌드·정적 분석 이슈, 단일 경로 제한 |
| **Info** | 경고·문서·커버리지 미측정 등 품질 개선 권고 |

---

## 3. 결함 목록

### 3.1 DEF-01 — 2자리 숫자 즉시 확정

| 항목 | 내용 |
|------|------|
| **ID** | DEF-01 |
| **Severity** | Critical |
| **FunctionName** | `handleDigit` |
| **Steps** | 1. `TVController` 생성, Mock Tuner 현재 채널 `"0"`<br>2. `pushButton(KEY_1)` 입력<br>3. `pushButton(KEY_2)` 입력<br>4. `expectChannel(12)` 검증 — `TVControllerTest.TwoDigits_ChangesImmediately` |
| **Expected** | 두 번째 숫자 입력 직후 채널 `"12"`로 즉시 확정 |
| **Actual** | 채널이 `"0"`으로 유지 (2자리 즉시 확정 미동작) |
| **Root Cause** | `processingCH`에 숫자만 누적하고 `size() >= 2`일 때 `confirmPendingDigits()`를 호출하지 않음 |
| **Fix Summary** | `handleDigit` 끝에 `if (processingCH.size() >= 2) { confirmPendingDigits(); }` 추가 |
| **Status** | Fixed |

---

### 3.2 DEF-02 — 1자리 + 확인 확정

| 항목 | 내용 |
|------|------|
| **ID** | DEF-02 |
| **Severity** | Critical |
| **FunctionName** | `pushButton` |
| **Steps** | 1. Mock Tuner 현재 채널 `"6"`<br>2. `pushButton(KEY_1)` 입력<br>3. `pushButton(KEY_OK)` 입력<br>4. `expectChannel(1)` 검증 — `TVControllerTest.DigitThenConfirm_ChangesToSingleDigitChannel` |
| **Expected** | 확인(KEY_OK) 후 채널 `"1"`로 확정 |
| **Actual** | 채널이 `"6"`으로 유지 (1자리 + 확인 확정 불가) |
| **Root Cause** | `pushButton`에서 `KEY_OK` 분기 없이 `switch`만 처리하여 `setTunerCh()` / `confirmPendingDigits()`가 호출되지 않음 |
| **Fix Summary** | `pushButton` 상단에 `KEY_OK` 시 `setTunerCh()` 호출 후 `return` 추가 |
| **Status** | Fixed |

---

### 3.3 DEF-03 — 보류 숫자 + 기능키 충돌

| 항목 | 내용 |
|------|------|
| **ID** | DEF-03 |
| **Severity** | Major |
| **FunctionName** | `pushButton` |
| **Steps** | 1. Mock Tuner 현재 채널 `"0"`<br>2. `pushButton(KEY_4)`, `pushButton(KEY_5)` → 채널 `"45"` 확정<br>3. `pushButton(KEY_6)` → 보류 상태 유지 (`"45"`)<br>4. `pushButton(KEY_CH_UP)` 입력<br>5. `expectChannel(46)` 검증 — `TVControllerTest.PendingDigitFunctionKey_InvalidatesPendingAndAppliesFunction` |
| **Expected** | 보류 숫자 `6` 무효화 후 45에서 채널 업 → `"46"` |
| **Actual** | 보류 숫자가 반영되어 `"6"` 등 잘못된 채널로 변경 |
| **Root Cause** | 기능키 처리 전 `processingCH` 버퍼를 비우지 않아 보류 숫자와 기능키 동작이 충돌 (요구사항 D-08 위반) |
| **Fix Summary** | `KEY_OK` 이후, `switch` 진입 전 `if (!processingCH.empty()) { clearPendingDigits(); }` 추가 |
| **Status** | Fixed |

---

### 3.4 DEF-04 — 채널 업 경계 래핑 (99→0)

| 항목 | 내용 |
|------|------|
| **ID** | DEF-04 |
| **Severity** | Major |
| **FunctionName** | `handleChannelUp` |
| **Steps** | 1. 검색 결과 없음, Mock Tuner 현재 채널 `"99"`<br>2. `pushButton(KEY_CH_UP)` 입력<br>3. `expectChannel(0)` 검증 — `TVControllerTest.ChannelUpWithoutSearch_WrapsFromMaxToMin` |
| **Expected** | 최대 채널(99) 다음 채널 업 시 `"0"`으로 순환 |
| **Actual** | `"100"` 또는 `"99"` 유지 (경계 래핑 실패) |
| **Root Cause** | 검색 목록 없을 때 `current + 1`만 수행하고 `MAX_CHANNEL(99)` → `MIN_CHANNEL(0)` 래핑 미구현 |
| **Fix Summary** | `const int next = (current == MAX_CHANNEL) ? MIN_CHANNEL : current + 1;` 적용 |
| **Status** | Fixed |

---

### 3.5 DEF-05 — 채널 다운 경계 래핑 (0→99)

| 항목 | 내용 |
|------|------|
| **ID** | DEF-05 |
| **Severity** | Major |
| **FunctionName** | `handleChannelDown` |
| **Steps** | 1. 검색 결과 없음, Mock Tuner 현재 채널 `"0"`<br>2. `pushButton(KEY_CH_DOWN)` 입력<br>3. `expectChannel(99)` 검증 — `TVControllerTest.ChannelDownWithoutSearch_WrapsFromMinToMax` |
| **Expected** | 최소 채널(0)에서 채널 다운 시 `"99"`로 순환 |
| **Actual** | `"-1"` 또는 `"0"` 유지 (경계 래핑 실패) |
| **Root Cause** | 검색 목록 없을 때 `current - 1`만 수행하고 `MIN_CHANNEL(0)` → `MAX_CHANNEL(99)` 래핑 미구현 |
| **Fix Summary** | `const int next = (current == MIN_CHANNEL) ? MAX_CHANNEL : current - 1;` 적용 |
| **Status** | Fixed |

---

### 3.6 DEF-06 — 검색 결과 내 채널 업

| 항목 | 내용 |
|------|------|
| **ID** | DEF-06 |
| **Severity** | Major |
| **FunctionName** | `handleChannelSearch`, `handleChannelUp` |
| **Steps** | 1. Mock Tuner 현재 채널 `"6"`, `seekCH()`가 `"4"` → `"6"` → `"14"` → `"6"` 순 반환 설정<br>2. `pushButton(KEY_CH_SEARCH)` 입력<br>3. `pushButton(KEY_CH_UP)` 입력<br>4. `expectChannel(14)` 검증 — `TVControllerTest.ChannelSearchThenUp_MovesWithinStoredChannels` |
| **Expected** | 검색 저장 목록 `{4, 6, 14}` 기준, 현재 6 다음 업 → `"14"` |
| **Actual** | 일반 채널 증감으로 `"7"` 또는 `"6"` 유지 |
| **Root Cause** | `searchedChannels` 목록 미사용 또는 `std::upper_bound` 기반 다음 채널 탐색·순환 미구현 |
| **Fix Summary** | `handleChannelSearch`에서 `seekCH()` 루프로 목록 수집·정렬 후, `handleChannelUp`/`Down`에서 `upper_bound`/`lower_bound`로 저장 채널 간 이동 및 끝에서 순환 |
| **Status** | Fixed |

---

### 3.7 DEF-07 — 다음 선호 채널

| 항목 | 내용 |
|------|------|
| **ID** | DEF-07 |
| **Severity** | Major |
| **FunctionName** | `handleNextFavorite` |
| **Steps** | 1. 선호 채널 1, 4, 12, 56 등록 후 현재 채널 `"6"`<br>2. `pushButton(KEY_NEXT_FAVORITE)` 입력<br>3. `expectChannel(12)` 검증 — `TVControllerTest.NextFavorite_ChangesToNearestGreaterFavorite` |
| **Expected** | 현재 채널(6)보다 큰 선호 채널 중 최소값 → `"12"` |
| **Actual** | `"6"` 유지 또는 잘못된 채널로 이동 |
| **Root Cause** | `favoriteChannels`에 대한 `upper_bound` 탐색 및 목록 끝에서 최소 선호 채널로 순환 미구현 |
| **Fix Summary** | `handleNextFavorite`에 `std::upper_bound`로 다음 선호 채널 탐색, `end()`이면 `favoriteChannels.front()`로 순환 |
| **Status** | Fixed |

---

### 3.8 DEF-08 — 빌드 오류 (미선언 함수)

| 항목 | 내용 |
|------|------|
| **ID** | DEF-08 |
| **Severity** | Minor |
| **FunctionName** | (빌드) `isDigitOrConfirm` |
| **Steps** | 1. 소스에서 미선언 `isDigitOrConfirm` 참조 포함 상태로 빌드 시도 |
| **Expected** | 컴파일 성공 |
| **Actual** | 헤더 미선언으로 컴파일 실패 |
| **Root Cause** | 헤더에 선언되지 않은 헬퍼 함수 참조 (RPT-05 이전 잔존 코드) |
| **Fix Summary** | 미사용 `isDigitOrConfirm` 참조 제거 (프로젝트에서는 제거 완료) |
| **Status** | Fixed |

---

### 3.9 DEF-09 — gmock Uninteresting 경고

| 항목 | 내용 |
|------|------|
| **ID** | DEF-09 |
| **Severity** | Info |
| **FunctionName** | (테스트) `TVControllerTest` — `MockTunerForController` |
| **Steps** | 1. `ctest` 또는 `TVControllerTest` 실행<br>2. 콘솔에 `GMOCK WARNING: Uninteresting mock function call` 출력 확인 |
| **Expected** | 의도된 Mock 호출만 기록, 불필요한 경고 없음 |
| **Actual** | `ON_CALL` 기본 동작으로 `setCH`/`getCurrentCH` 호출 시 Uninteresting call 경고 다수 출력 (기능 영향 없음) |
| **Root Cause** | `expectChannel`은 `currentCh`만 검증하고 `EXPECT_CALL`로 Mock 호출을 명시하지 않음 |
| **Fix Summary** | 시나리오별 `EXPECT_CALL(mockTuner, setCH(...))` 정밀화 또는 `NiceMock` 사용 검토 (선택적 품질 개선) |
| **Status** | Open (Info) |

---

## 4. 연관 시나리오 (DEF-01·DEF-02 확장)

동일 루트 원인군에서 별도 `TEST_F`로 검증된 시나리오이다.

| ID | Severity | FunctionName | Steps | Expected | Actual (전형) | Root Cause | Fix Summary | Status |
|----|----------|--------------|-------|----------|---------------|------------|-------------|--------|
| DEF-01a | Critical | `handleDigit` | 현재 `"0"` → `KEY_1`,`KEY_2`,`KEY_3`,`KEY_4` — `FourDigits_ChangesAsTwoPairs` | 중간 `"12"`, 최종 `"34"` | `"0"` / `"12"` | 2자리 확정·버퍼 초기화 누락 | DEF-01과 동일 | Fixed |
| DEF-02a | Critical | `confirmPendingDigits`, `setTunerCh` | 현재 `"50"` → `KEY_0`, `KEY_OK` — `LeadingZeroThenConfirm_ChangesToChannelZero` | `"0"` | `"50"` | 확인 시 단일 `"0"` 버퍼 미확정 | DEF-02와 동일 | Fixed |
| DEF-01b | Critical | `handleDigit`, `parseChannel` | 현재 `"50"` → `KEY_0`, `KEY_7` — `LeadingZeroThenDigit_ChangesToDigitChannel` | `"7"` | `"50"` 또는 `"0"` | 선행 0 두 자리 확정·`parseChannel` 검증 누락 | DEF-01 + `parseChannel` 0~99 검증 | Fixed |

---

## 5. 테스트 추적 매트릭스

| Defect ID | TEST_F | ctest 결과 (2026-05-19) |
|-----------|--------|-------------------------|
| DEF-01 | `TwoDigits_ChangesImmediately` | Passed |
| DEF-01a | `FourDigits_ChangesAsTwoPairs` | Passed |
| DEF-01b | `LeadingZeroThenDigit_ChangesToDigitChannel` | Passed |
| DEF-02 | `DigitThenConfirm_ChangesToSingleDigitChannel` | Passed |
| DEF-02a | `LeadingZeroThenConfirm_ChangesToChannelZero` | Passed |
| DEF-03 | `PendingDigitFunctionKey_InvalidatesPendingAndAppliesFunction` | Passed |
| DEF-04 | `ChannelUpWithoutSearch_WrapsFromMaxToMin` | Passed |
| DEF-05 | `ChannelDownWithoutSearch_WrapsFromMinToMax` | Passed |
| DEF-06 | `ChannelSearchThenUp_MovesWithinStoredChannels` | Passed |
| DEF-07 | `NextFavorite_ChangesToNearestGreaterFavorite` | Passed |
| DEF-08 | (빌드) | N/A — 해소됨 |
| DEF-09 | (전체 `TVControllerTest`) | Passed (경고만 잔존) |

---

## 6. 후속 테스트 계획 (결함 미등록)

RPT-04·RPT-06에 따르면 아래 시나리오는 아직 `TEST_F`가 없어 본 목록에 Red 결함으로 등록하지 않았다. 테스트 추가 후 실패 시 §3에 항목을 확장한다.

| 계획 ID | 시나리오 |
|---------|----------|
| D-04 | 세 번째 숫자 입력 시 다음 보류 버퍼 시작 |
| D-05 | 보류 1자리 + 확인 확정 |
| U-01~U-02 | 검색 없음 ±1 증감 |
| S-02 | 검색 결과 있음 채널 다운 |
| F-04~F-05 | 선호 채널 순환 / 빈 목록 |

---

## 7. 결함 현황 요약

| Severity | 건수 | Fixed | Open |
|----------|------|-------|------|
| Critical | 4 (+2 시나리오) | 6 | 0 |
| Major | 4 | 4 | 0 |
| Minor | 1 | 1 | 0 |
| Info | 1 | 0 | 1 |
| **합계** | **9 (+3 시나리오)** | **11** | **1** |

### 7.1 결론

1. 기능 결함 **DEF-01~08**은 최소 변경 패치 적용 후 **ctest 23/23 Green**으로 수정 완료되었다.
2. **DEF-09**는 테스트 품질 Info 수준 개선 권고이며, 기능 회귀와 무관하다.
3. RPT-04 잔여 P0/P1 시나리오는 `TEST_F` 추가 후 본 보고서 §3·§5를 갱신한다.

---

## 8. 참조 문서

| 문서 | 경로 |
|------|------|
| 요구사항 분석 | `docs/01_requirements_analysis.md` |
| 결함 분석 | `Report/05_01_TVController_결함_분석_보고서.md` |
| 테스트 구현 | `Report/04_TVController_테스트_구현_보고서.md` |
| 결함 목록 (작업용) | `docs/defect_list.md` |
| 프로젝트 README | `README.md` |

---

*문서 끝 — RPT-07 TVController 결함 목록 보고서*
