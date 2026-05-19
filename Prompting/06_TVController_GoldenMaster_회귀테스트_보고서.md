# Golden Master testing strategy design
_Exported on 5/19/2026 at 14:20:52 GMT+9 from Cursor (3.4.17)_

---

**User**

@TexttestFixture.cpp 
@remoteKey.h @Tuner.h @TVController.h @TVController.cpp

[P] 회귀 테스트(Approval/Golden Master) 설계 전문가입니다.
[C] C++17, Google Test, CMake
[T] TexttestFixture 출력 기반 Golden Master를 설계/구현해줘.
    1) expected 출력 파일 생성/보관 전략
    2) Google Test에서 파일 비교 테스트 구현
    3) CMake/ctest에 통합하는 방법
    4) CI에서 자동 실행 구성
[F] 테스트 코드 + CMake 수정안 + 실행 방법

---

**Cursor**

Golden Master 회귀 테스트를 설계·구현하기 위해 프로젝트 구조와 참조 파일을 확인합니다.


TextTestFixture와 Golden Master 회귀 테스트 인프라를 구현합니다. 먼저 시나리오 실행 후 golden 파일을 생성합니다.


TexttestFixture 기반 Golden Master(Approval) 회귀 테스트를 구현했습니다. `TexttestFixture.cpp`는 신규로 `test/TextTestFixture.cpp`에 추가했습니다.

---

## 1) Expected 출력 파일 생성·보관 전략

| 항목 | 내용 |
|------|------|
| **위치** | `test/golden/<시나리오명>.golden.txt` (시나리오 1파일) |
| **형식** | 고정 텍스트, LF, `remoteKey::to_string()` + `FakeTuner` 현재 채널 |
| **버전 관리** | Git에 golden 커밋 (소스와 함께 리뷰) |
| **갱신** | 의도적 동작 변경 시에만 `TV_UPDATE_GOLDEN=1`로 재생성 |
| **실패 산출물** | `test/golden/*.actual.txt` (`.gitignore` 제외) |

예시 golden:

```1:4:test/golden/DigitThenConfirm_ChangesToSingleDigitChannel.golden.txt
scenario DigitThenConfirm_ChangesToSingleDigitChannel
initial 6
1 -> 6
OK -> 1
```

---

## 2) Google Test 파일 비교 구현

- **`TextTestFixture`**: `FakeTunerForText` + 시나리오 정의 → 텍스트 트레이스 생성
- **`TVControllerGoldenTest`**: `TEST_P`로 시나리오별 golden 비교
- **`test/support/GoldenTestUtil.h`**: 읽기/쓰기, CRLF 정규화, `TV_UPDATE_GOLDEN` 처리

```cpp
// TV_UPDATE_GOLDEN=1 이면 golden 갱신, 아니면 바이트 단위 비교
assertMatchesGolden(scenarioName, actual);
```

19개 시나리오: `docs/01_requirements_analysis.md` §5 핵심 시나리오와 `TVControllerTest`와 정합.

---

## 3) CMake / ctest 통합

`CMakeLists.txt`에 `TVControllerGoldenTest` 타겟 추가:

- `GOLDEN_SOURCE_DIR` → `test/golden` 절대경로 compile definition
- `gtest_discover_tests(... PROPERTIES LABELS "golden;regression")`

```bash
# 전체 테스트 (42건: Tuner 13 + TVController 10 + Golden 19)
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure

# Golden만
ctest --test-dir build -L golden --output-on-failure
```

---

## 4) CI 자동 실행

`.github/workflows/ci.yml`:

1. `cmake` configure & build  
2. `ctest --output-on-failure`  
3. `test/golden/` 미커밋 변경 여부 검사  

---

## 실행 방법

### 일반 회귀 (비교 모드)

```powershell
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

### Golden 재생성 (의도적 변경 후)

```powershell
$env:TV_UPDATE_GOLDEN = "1"
ctest --test-dir build -R TVControllerGoldenTest --output-on-failure
Remove-Item Env:TV_UPDATE_GOLDEN
# 또는
.\scripts\update_golden.ps1
```

Linux:

```bash
./scripts/update_golden.sh
```

### 새 시나리오 추가 절차

1. `TextTestFixture.cpp`의 `allScenarios()`에 `TextScenario` 추가  
2. `TV_UPDATE_GOLDEN=1`로 golden 생성  
3. golden 파일을 커밋  

---

## 추가된 파일 요약

| 파일 | 역할 |
|------|------|
| `test/TextTestFixture.h/.cpp` | 시나리오 실행·텍스트 렌더링 |
| `test/TVControllerGoldenTest.cpp` | Approval 테스트 |
| `test/support/GoldenTestUtil.h` | 파일 I/O·업데이트 플래그 |
| `test/golden/*.golden.txt` | Expected 출력 (19개) |
| `scripts/update_golden.ps1` / `.sh` | Golden 일괄 갱신 |
| `.github/workflows/ci.yml` | CI |

로컬에서 **42/42 테스트 통과**를 확인했습니다. Golden은 Mock 검증(`TVControllerTest`)과 별도로 **end-to-end 텍스트 스냅샷** 회귀를 담당합니다.

---

**User**

gitignore 파일을 만들어서 @.github/workflows/ci.yml 이 파일 추가해줘

---

**Cursor**

프로젝트에 `.gitignore`를 추가하고 `.github/workflows/ci.yml`이 포함되도록 확인합니다.


루트 `.gitignore`를 생성하고 `.github/workflows/ci.yml`이 무시되지 않도록 설정합니다.


루트 `.gitignore`를 추가했고, `.github/workflows/ci.yml`은 무시되지 않도록 예외 규칙을 넣었습니다.

### `.gitignore` 요약
- **무시:** `build/`, `build-*/`, CMake/Ninja 산출물, 컴파일 결과물, IDE 설정, 커버리지 파일
- **무시:** `test/golden/*.actual.txt` (golden 비교 실패 시 생성되는 임시 파일)
- **추적 유지:** `.github/workflows/*.yml` (CI 워크플로)

### `.github/workflows/ci.yml`
이미 존재하며, `.gitignore`에 의해 제외되지 않습니다. 커밋할 때는 예를 들어:

```powershell
git add .gitignore .github/workflows/ci.yml
```

`build-golden/` 같은 빌드 디렉터리는 `/build-*/` 규칙으로 자동 제외됩니다.

---

**User**

Report 폴더에 보고서를 작성해줘

---

**Cursor**

Report 폴더의 기존 보고서 형식을 확인한 뒤 Golden Master 회귀 테스트 보고서를 작성합니다.


`Report/06_TVController_GoldenMaster_회귀테스트_보고서.md` (문서 번호 **RPT-06**)를 작성했습니다.

### 보고서 구성

| 섹션 | 내용 |
|------|------|
| §1 개요 | 목적, 작업 범위, 완료 기준 (42/42 테스트) |
| §2 Golden Master 설계 | Approval 개념, golden 파일 전략, 출력 형식 |
| §3 TextTestFixture | FakeTuner, Mock과의 차이 |
| §4 Google Test | `TEST_P`, `TV_UPDATE_GOLDEN` 갱신 모드 |
| §5 시나리오 목록 | Golden 19건 + 요구사항 매핑 |
| §6 CMake/ctest | 타깃, 실행 명령, 라벨 `golden` |
| §7 CI | `.github/workflows/ci.yml`, golden 무결성 검사 |
| §8 TVControllerTest 관계 | 이중 테스트 피라미드, RPT-05 보완 |
| §9~12 | 추적 매트릭스, 운영 가이드, 후속 과제, 결론 |

기존 `Report/03`~`05` 보고서와 동일한 표·메타데이터 형식을 따랐으며, Golden Master 구현·실행·CI 내용을 한 문서에 정리했습니다.

