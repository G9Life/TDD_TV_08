# Defect List (작업용 요약)

> 정식 보고서: [`Report/06_TVController_결함_목록_보고서.md`](../Report/06_TVController_결함_목록_보고서.md) (RPT-07)

본 파일은 QA 추적용 요약이다. 상세 Steps·Root Cause·Fix Summary·추적 매트릭스는 Report 폴더 보고서를 참조한다.

| ID | Severity | FunctionName | Status |
|----|----------|--------------|--------|
| DEF-01 | Critical | `handleDigit` | Fixed |
| DEF-02 | Critical | `pushButton` | Fixed |
| DEF-03 | Major | `pushButton` | Fixed |
| DEF-04 | Major | `handleChannelUp` | Fixed |
| DEF-05 | Major | `handleChannelDown` | Fixed |
| DEF-06 | Major | `handleChannelSearch`, `handleChannelUp` | Fixed |
| DEF-07 | Major | `handleNextFavorite` | Fixed |
| DEF-08 | Minor | (빌드) `isDigitOrConfirm` | Fixed |
| DEF-09 | Info | `TVControllerTest` Mock | Open |

**검증:** 2026-05-19 — ctest 23/23 Passed
