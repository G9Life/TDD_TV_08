# Defect management document creation
_Exported on 5/19/2026 at 15:50:16 GMT+9 from Cursor (3.4.17)_

---

**User**

@05_02_defect_list.md @01_requirements_analysis.md @03_test_plan.md

[P] QA 리드 엔지니어입니다.
[T] 결함 관리 문서를 defect_report.md로 작성해줘.
    1) 결함 분류 체계:
       Severity (Critical/Major/Minor/Info) x ItemType (5종) 매트릭스 표
    2) 결함 보고서 템플릿 (재현/기대/실제/원인/수정/검증)
    3) 품질 메트릭 수집 계획
       - 테스트 통과율, 커버리지, 단계별 결함 발견율
       - C++: gcov/lcov / Java: JaCoCo / Python: pytest-cov
    4) (선택) GitHub Issues 연동 워크플로우
[F] Markdown. defect_report.md로 저장

---

**Cursor**

결함 관리 문서 작성을 위해 참조 문서와 기존 결함 목록을 확인합니다.



프로젝트 결함·테스트 계획을 반영해 `docs/defect_report.md`를 작성합니다.


`docs/defect_report.md` (DOC-QA-08)를 작성해 두었습니다. 요청하신 네 가지 항목을 모두 포함했습니다.

### 포함 내용 요약

1. **결함 분류 체계**
   - Severity 4단계 (Critical / Major / Minor / Info) 정의
   - ItemType 5종: **IT-F** Functional, **IT-L** Logic, **IT-I** Interface, **IT-B** Build, **IT-T** TestQuality
   - **4×5 매트릭스** (대응 우선순위 P0~P3)
   - `05_02_defect_list.md`의 DEF-01~09 매핑표

2. **결함 보고서 템플릿**
   - 재현 / 기대 / 실제 / 원인 / 수정 / 검증 필드
   - DEF-01 예시, 상태 전이 다이어그램

3. **품질 메트릭 수집 계획**
   - 테스트 통과율, 커버리지, 단계별 결함 발견율 정의·목표치
   - **C++:** gcov/lcov (`03_test_plan.md` §6 반영)
   - **Java:** JaCoCo, **Python:** pytest-cov (다언어 확장 참고)
   - 2026-05-19 기준 스냅샷 (23/23 Passed)

4. **(선택) GitHub Issues 연동**
   - 라벨 체계, Issue↔DEF-ID 매핑, PR `Fixes #N`, CI 연동 흐름

기준 문서(`01_requirements_analysis.md`, `03_test_plan.md`, `05_02_defect_list.md`)와 RPT-07 보고서를 상호 참조하도록 링크를 걸어 두었습니다.

---

**User**

report 폴더에 보고서로 작성해줘

---

**Cursor**

Report 폴더의 기존 보고서 명명 규칙을 확인한 뒤 `defect_report.md`를 Report에 맞게 작성합니다.


Report 폴더 명명 규칙에 맞춰 `05_03_TVController_결함_관리_보고서.md`를 작성하고, `docs/defect_report.md`는 Report로 안내하도록 정리합니다.


`Report/05_03_TVController_결함_관리_보고서.md` (RPT-08)로 정식 보고서를 작성했습니다.

### 변경 사항

| 파일 | 내용 |
|------|------|
| **`Report/05_03_TVController_결함_관리_보고서.md`** | 결함 분류 매트릭스, 보고 템플릿, 품질 메트릭, GitHub Issues 워크플로우 (기존 `docs/defect_report.md` 전체 내용) |
| **`docs/defect_report.md`** | Report 경로 안내용으로 축소 |
| **`docs/05_02_defect_list.md`** | RPT-08 링크 추가, RPT-07 경로 수정 |
| **`Report/05_02_TVController_결함_목록_보고서.md`** | RPT-08·작업용 목록 경로 참조 갱신 |

Report 폴더의 다른 보고서와 동일하게 **문서 번호(RPT-08)·단계·기준 문서·연관 보고서(RPT-06/07)** 헤더 형식을 맞춰 두었습니다.

