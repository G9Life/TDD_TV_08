# Defect documentation format
_Exported on 5/19/2026 at 14:09:00 GMT+9 from Cursor (3.4.17)_

---

**User**

[P] QA 리드입니다.
[C] 현재까지 발견된 테스트 실패/결함들을 문서화합니다.
[T] 발견된 결함을 다음 형식으로 defect_list.md에 정리해줘.
    항목: [ID] [Severity] [FunctionName] [Steps] [Expected] [Actual] [Root Cause] [Fix Summary]
[F] Markdown. docs 폴더에 defect_list.md로 저장

---

**Cursor**

발견된 결함을 문서화하기 위해 테스트 결과·결함 분석 보고서·소스를 확인합니다.


테스트 코드에서 재현 단계를 확인합니다.


결함 분석 보고서와 테스트 코드를 바탕으로 `docs/defect_list.md`를 작성합니다.


`docs/defect_list.md`에 결함 목록을 정리해 두었습니다.

## 문서 구성

- **DEF-01 ~ DEF-09**: 결함 분석 보고서(RPT-06)와 `TVControllerTest` 기준
- 각 항목: **ID · Severity · FunctionName · Steps · Expected · Actual · Root Cause · Fix Summary · Status**
- 숫자 입력 관련 **DEF-01a/01b, DEF-02a** 보조 시나리오 표
- **테스트 추적 매트릭스** 및 **후속 미구현 테스트** 참고 섹션

## 요약

| 구분 | 내용 |
|------|------|
| 기능 결함 | DEF-01~08 — TDD Red 단계에서 발견, **2026-05-19 ctest 23/23 Green으로 수정 완료** |
| 품질 이슈 | DEF-09 — gmock Uninteresting 경고 (**Info**, Open) |
| Actual 값 | 미완성 스텁 기준 전형 실패값 (보고서 §2.2) |

경로: [`docs/defect_list.md`](c:\DEV\TDD_TV_08\docs\defect_list.md)

---

**User**

report 폴더에 보고서로 작성해줘

---

**Cursor**

Report 폴더의 기존 보고서 형식을 확인한 뒤 동일한 형식으로 작성합니다.


기존 보고서 형식(RPT 헤더·섹션 구조)에 맞춰 `Report` 폴더에 결함 목록 보고서를 작성합니다.


`Report/06_TVController_결함_목록_보고서.md` (RPT-07)로 정리했습니다.

## 보고서 구성

| 섹션 | 내용 |
|------|------|
| §1 | 보고서 개요·검증 현황 (ctest 23/23 Green) |
| §2 | 심각도 정의 |
| §3 | DEF-01 ~ DEF-09 결함 상세 (요청 형식 8필드 + Status) |
| §4 | 연관 시나리오 (DEF-01a/01b, DEF-02a) |
| §5 | 테스트 추적 매트릭스 |
| §6 | 후속 미구현 테스트 계획 |
| §7 | 결함 현황 요약·결론 |
| §8 | 참조 문서 |

기존 `Report/05_01_*` 보고서와 동일하게 문서 번호·단계·작성 관점 헤더를 맞춰 두었습니다.

`docs/defect_list.md`는 Report 보고서를 가리키는 **작업용 요약**으로 축소했습니다. 상세 내용은 Report 폴더 보고서를 기준으로 보시면 됩니다.

