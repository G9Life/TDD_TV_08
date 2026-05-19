# Requirements Analysis

## 목적

TDD TV C++17 프로젝트의 `README.md` 요구사항을 구현 및 Google Test 관점에서 재정리한다. 대상 모듈은 리모컨 입력을 받아 채널 상태를 관리하고, 외부 `Tuner` 인터페이스를 통해 실제 채널 변경을 위임하는 Controller이다.

## 1. 요구사항 별 규칙표

### 1.1 TDD_TV 기본 요구사항 4개 항목

| 번호 | 원문 요구사항 요약 | C++ 구현 관점 규칙 | 테스트 관점 |
|---:|---|---|---|
| 1 | 셋탑박스에서 리모컨 입력을 받아 채널을 관리하고 Tuner를 설정하는 모듈 작성 | Controller는 현재 채널, 숫자 입력 버퍼, 선호 채널 목록, 검색된 채널 목록을 관리한다. 실제 채널 변경은 `Tuner::setCH(int)` 또는 `Tuner::seekCH()`에 위임한다. | Controller 단위 테스트에서 `Tuner`는 Fake 또는 Mock으로 대체한다. |
| 2 | 리모컨 입력은 숫자, 채널 업/다운, 확인, 채널검색, 선호채널추가, 다음선호채널 버튼으로 구성 | 입력은 enum class 등 타입 안전한 키 값으로 모델링한다. 숫자 입력과 기능 버튼 입력은 처리 흐름을 분리한다. | 각 버튼별 public API 또는 `handleInput()` 동작을 독립적으로 검증한다. |
| 3 | 리모컨 센서 입력 형식은 미정이나 키 값이 입력될 것으로 예상 | 센서 원천 데이터와 Controller 입력 타입 사이에 변환 계층을 둘 수 있다. Controller는 정제된 키 값만 받도록 설계한다. | 센서 데이터 파싱은 Controller 핵심 로직과 분리해 테스트한다. |
| 4 | Tuner 업체가 Channel Tuner를 제공하며 Tuner는 정확히 동작한다고 가정 | `Tuner`는 인터페이스로 의존하고 Controller는 구체 구현을 알지 않는다. `seekCH()`, `setCH(ch)`, `getCurrentCH()` 계약만 사용한다. | Tuner 자체 동작은 테스트 대상에서 제외하고 Mock/Fake의 호출 여부와 Controller 상태 변화를 검증한다. |

### 1.2 TDD practice 요구사항 6개 항목

| 번호 | 기능 | 입력/상태 | 기대 동작 | C++ 구현 관점 규칙 |
|---:|---|---|---|---|
| 1 | 숫자 버튼으로 채널 변경 | `1`, `확인` | 1번 채널로 변경 | 한 자리 숫자는 `확인` 입력 시 확정한다. |
| 1 | 숫자 버튼으로 채널 변경 | `1`, `2` | 12번 채널로 변경 | 두 자리 숫자가 완성되면 즉시 채널을 확정한다. |
| 1 | 숫자 버튼으로 채널 변경 | `1`, `2`, `3`, `4` | 12번 채널로 변경 후 34번 채널로 변경 | 숫자 입력 버퍼는 두 자리 확정 후 초기화한다. |
| 1 | 숫자 버튼으로 채널 변경 | `4`, `5`, `6` | 45번 채널로 변경 후 6은 다음 입력 대기 | 세 번째 숫자는 새 입력 버퍼의 첫 숫자로 보관한다. |
| 1 | 숫자 버튼으로 채널 변경 | `4`, `5`, `6`, `확인` | 45번 채널 변경 후 6번 채널로 변경 | 보관 중인 한 자리 숫자는 `확인`으로 확정한다. |
| 1 | 숫자 버튼으로 채널 변경 | `4`, `5`, `6`, 기능 버튼 | 6은 무효화 | 숫자 버퍼가 있는 상태에서 숫자/확인이 아닌 기능 버튼을 누르면 버퍼를 삭제한다. |
| 1 | 숫자 버튼으로 채널 변경 | `0`, `7` | 7번 채널로 변경 | 선행 0은 십진수 채널 입력의 일부로 처리하고 결과 채널은 7이다. |
| 2 | 선호 채널 추가 | 현재 채널에서 선호채널추가 | 선호 채널이 아니면 추가 | 선호 채널 목록은 중복을 허용하지 않는다. |
| 2 | 선호 채널 추가 | 이미 선호 채널에서 선호채널추가 | 선호 채널에서 삭제 | 동일 버튼은 toggle 동작으로 구현한다. |
| 3 | 다음 선호 채널 | 선호 목록 `1, 4, 12, 56`, 현재 6 | 12번 채널로 변경 | 현재 채널보다 큰 선호 채널 중 최소값으로 이동한다. |
| 3 | 다음 선호 채널 | 선호 목록 `1, 4, 12, 56`, 현재 56 | 1번 채널로 변경 | 더 큰 선호 채널이 없으면 가장 작은 선호 채널로 순환한다. |
| 4 | 채널 검색 | 채널검색 버튼 | 모든 시청 가능 채널을 검색하여 저장 | `Tuner::seekCH()`를 사용해 검색 결과를 저장한다. 중복 없이 정렬된 목록으로 관리하는 것이 테스트와 업/다운 처리에 유리하다. |
| 5 | 업/다운, 검색 결과 없음 | 현재 6, 채널 업 | 7번 채널로 변경 | 검색 결과가 없으면 일반 채널 범위에서 1 증가한다. |
| 5 | 업/다운, 검색 결과 없음 | 현재 6, 채널 다운 | 5번 채널로 변경 | 검색 결과가 없으면 일반 채널 범위에서 1 감소한다. |
| 5 | 업/다운, 검색 결과 없음 | 현재 99, 채널 업 | 0번 채널로 변경 | 최대 채널 다음은 최소 채널로 순환한다. |
| 5 | 업/다운, 검색 결과 없음 | 현재 0, 채널 다운 | 99번 채널로 변경 | 최소 채널 이전은 최대 채널로 순환한다. |
| 6 | 업/다운, 검색 결과 있음 | 저장 채널 `4, 6, 14`, 현재 6, 업 | 14번 채널로 변경 | 검색 결과가 있으면 일반 채널이 아니라 저장된 채널 목록 기준으로 이동한다. |
| 6 | 업/다운, 검색 결과 있음 | 저장 채널 `4, 6, 14`, 현재 6, 다운 | 4번 채널로 변경 | 저장된 채널 목록에서 현재보다 작은 값 중 최대값으로 이동한다. |
| 6 | 업/다운, 검색 결과 있음 | 저장 채널 `4, 6, 14`, 현재 15, 업 | 4번 채널로 변경 | 현재보다 큰 저장 채널이 없으면 저장 목록의 최소값으로 순환한다. |
| 6 | 업/다운, 검색 결과 있음 | 저장 채널 `4, 6, 14`, 현재 15, 다운 | 14번 채널로 변경 | 현재보다 작은 저장 채널 중 최대값으로 이동한다. |

## 2. 번호 입력 시 최초 0일 경우 주의점

1. 채널 번호는 문자열 기반 입력 버퍼로 관리하는 것이 안전하다. `0`, `7` 입력은 정수 `07`이 아니라 `"0" + "7"` 조합으로 해석한 뒤 십진수 7번 채널로 확정한다.
2. C++ 구현에서 `07` 같은 리터럴 표현을 사용하면 구버전 문법에서는 8진수 리터럴로 해석될 수 있으므로 테스트 데이터와 구현 모두 정수 리터럴 `07`을 사용하지 않는다.
3. 최초 입력이 `0`이고 다음 입력이 숫자이면 두 자리 입력으로 간주하되, 확정 채널은 선행 0을 제거한 십진수 값이다.
4. 최초 입력이 `0`이고 `확인`을 누르면 0번 채널로 확정하는 규칙이 자연스럽다.
5. 최초 입력이 `0`인 상태에서 숫자나 `확인`이 아닌 기능 버튼을 누르면 보류 중인 숫자 입력은 무효화한다.

## 3. 최소 및 최대 입력 가능한 채널 번호

1. 최소 채널 번호는 `0`이다.
2. 최대 채널 번호는 `99`이다.
3. Controller 내부에서는 `constexpr int MIN_CHANNEL = 0;`, `constexpr int MAX_CHANNEL = 99;` 같은 상수로 범위를 고정한다.
4. 숫자 버튼 조합으로 만들 수 있는 유효 채널은 0부터 99까지이다.
5. 두 자리 숫자가 완성되면 즉시 채널을 확정하므로 리모컨 숫자 입력만으로 100 이상의 채널을 확정하지 않는다.

## 4. 채널 번호 음수값 입력 시 예외처리

1. 리모컨 숫자 버튼만으로는 음수 채널이 만들어질 수 없다.
2. 다만 Controller의 public API가 정수 채널 번호를 직접 받는다면 `ch < 0`은 잘못된 입력으로 처리해야 한다.
3. 음수 채널이 들어오면 `std::invalid_argument` 예외를 발생시키고 `Tuner::setCH()`를 호출하지 않는다.
4. 예외 발생 시 현재 채널, 선호 채널 목록, 검색 채널 목록, 숫자 입력 버퍼는 변경하지 않는다.
5. 채널 번호 범위 불변식은 `0 <= ch <= 99`로 정의한다. 음수뿐 아니라 99 초과 입력도 동일하게 잘못된 입력으로 보는 것이 일관된다.

## 5. Google Test 기준 테스트 시나리오 목록

1. `DigitThenConfirm_ChangesToSingleDigitChannel`: `1`, `Confirm` 입력 시 1번 채널로 변경된다.
2. `TwoDigits_ChangesImmediately`: `1`, `2` 입력 시 12번 채널로 변경된다.
3. `FourDigits_ChangesAsTwoPairs`: `1`, `2`, `3`, `4` 입력 시 12번 변경 후 34번으로 변경된다.
4. `ThirdDigit_StartsNextPendingInput`: `4`, `5`, `6` 입력 시 45번으로 변경되고 6은 보류된다.
5. `PendingSingleDigitConfirm_ChangesToPendingDigit`: `4`, `5`, `6`, `Confirm` 입력 시 최종 6번 채널로 변경된다.
6. `PendingSingleDigitNonConfirmFunction_InvalidatesPendingDigit`: `4`, `5`, `6`, `ChannelUp` 입력 시 6 입력은 무효화되고 기능 버튼 동작만 수행된다.
7. `LeadingZeroThenDigit_ChangesToDigitChannel`: `0`, `7` 입력 시 7번 채널로 변경된다.
8. `LeadingZeroThenConfirm_ChangesToChannelZero`: `0`, `Confirm` 입력 시 0번 채널로 변경된다.
9. `FavoriteButton_AddsCurrentChannelWhenNotFavorite`: 현재 채널이 선호 채널이 아니면 선호 목록에 추가된다.
10. `FavoriteButton_RemovesCurrentChannelWhenAlreadyFavorite`: 현재 채널이 이미 선호 채널이면 선호 목록에서 삭제된다.
11. `NextFavorite_ChangesToNearestGreaterFavorite`: 선호 목록 `1, 4, 12, 56`, 현재 6에서 다음 선호 채널은 12이다.
12. `NextFavorite_WrapsToSmallestFavorite`: 선호 목록 `1, 4, 12, 56`, 현재 56에서 다음 선호 채널은 1이다.
13. `ChannelSearch_StoresSeekResults`: 채널검색 버튼 입력 시 `Tuner::seekCH()` 결과를 검색 채널 목록에 저장한다.
14. `ChannelUpWithoutSearchResult_IncrementsChannel`: 검색 결과가 없고 현재 6에서 채널 업을 누르면 7번으로 변경된다.
15. `ChannelDownWithoutSearchResult_DecrementsChannel`: 검색 결과가 없고 현재 6에서 채널 다운을 누르면 5번으로 변경된다.
16. `ChannelUpWithoutSearchResult_WrapsFromMaxToMin`: 검색 결과가 없고 현재 99에서 채널 업을 누르면 0번으로 변경된다.
17. `ChannelDownWithoutSearchResult_WrapsFromMinToMax`: 검색 결과가 없고 현재 0에서 채널 다운을 누르면 99번으로 변경된다.
18. `ChannelUpWithSearchResult_ChangesToNextStoredChannel`: 저장 채널 `4, 6, 14`, 현재 6에서 채널 업을 누르면 14번으로 변경된다.
19. `ChannelDownWithSearchResult_ChangesToPreviousStoredChannel`: 저장 채널 `4, 6, 14`, 현재 6에서 채널 다운을 누르면 4번으로 변경된다.
20. `ChannelUpWithSearchResult_WrapsToSmallestStoredChannel`: 저장 채널 `4, 6, 14`, 현재 15에서 채널 업을 누르면 4번으로 변경된다.
21. `ChannelDownWithSearchResult_ChangesToNearestLowerStoredChannel`: 저장 채널 `4, 6, 14`, 현재 15에서 채널 다운을 누르면 14번으로 변경된다.
22. `SetChannelWithNegativeValue_ThrowsInvalidArgument`: 직접 채널 설정 API에 음수값 입력 시 `std::invalid_argument`가 발생한다.
23. `SetChannelWithNegativeValue_DoesNotCallTuner`: 음수값 입력 시 `Tuner::setCH()`가 호출되지 않는다.
24. `SetChannelOutOfRange_ThrowsInvalidArgument`: 100 이상 채널 번호 입력 시 `std::invalid_argument`가 발생한다.
25. `InvalidChannelInput_PreservesPreviousState`: 잘못된 채널 입력 예외 발생 후 현재 채널과 내부 목록 상태가 유지된다.
