# STM32 NUCLEO-F411RE Practice

STM32 NUCLEO-F411RE 보드를 활용해 MCU 주변장치 제어를 학습한 실습 기록 저장소입니다. Buzzer, CLCD, Servo Motor, Sensor Kit 등 기본 입출력 장치를 제어하며 임베디드 펌웨어 개발의 기초를 정리했습니다.

> 취업 포트폴리오의 대표 프로젝트는 아니지만, STM32 기반 제어와 주변장치 연동을 학습한 기록으로 보관합니다.

## 학습 목표

- STM32CubeIDE 기반 프로젝트 구조 이해
- GPIO, Timer, PWM 등 MCU 기본 제어 흐름 학습
- 센서와 액추에이터를 이용한 입출력 제어 실습
- 임베디드 C 기반 펌웨어 작성 및 디버깅 경험 축적

## 실습 환경

| 항목 | 내용 |
| --- | --- |
| Board | STM32 NUCLEO-F411RE |
| MCU | STM32F411RE |
| IDE | STM32CubeIDE |
| Language | C |
| 주요 실습 | Buzzer, CLCD, Servo Motor, Sensor Kit |

## 저장소 구조

```text
.
├─ NUCLEO_F411RE/          # 개인 실습 코드
│  ├─ 07.Buzzer
│  ├─ 08.CLCD
│  ├─ 09.ServoMotor
│  ├─ 36.Vector
│  └─ SENSOR_KIT
└─ NUCLEO_F411RE_교수님/   # 수업 참고 코드
   ├─ 07.Buzzer
   ├─ 08.CLCD
   ├─ 09.ServoMotor
   ├─ 36.Vector
   └─ SENSOR_KIT
```

## 주요 실습 내용

### Buzzer

- GPIO 또는 Timer 기반 출력 제어
- 주파수와 출력 패턴에 따른 부저 동작 확인

### CLCD

- Character LCD 제어 흐름 학습
- 문자열 출력 및 표시 상태 제어

### Servo Motor

- PWM 신호를 이용한 서보모터 각도 제어
- Timer 설정과 duty cycle 변화 확인

### Sensor Kit

- 센서 입력값 확인
- 입력 데이터 기반 출력 장치 제어 흐름 학습

## 포트폴리오와의 연결

이 저장소에서 학습한 STM32 주변장치 제어 경험은 이후 아래 프로젝트의 기반이 되었습니다.

- [Smart Grip Car v1.0](https://github.com/koonie404/Smart-Grip-Car-v1.0)
- [Smart Grip Car v1.1](https://github.com/koonie404/Smart-Grip-Car-Ver1.1)

특히 GPIO, PWM, 센서 입력, 액추에이터 제어 경험은 Smart Grip Car의 모터·그리퍼·센서 제어 구현으로 확장되었습니다.

## Note

이 저장소는 학습 과정의 실습 코드 보관용입니다. 취업용 대표 프로젝트는 아래 포트폴리오 저장소에서 확인할 수 있습니다.

- [Portfolio](https://github.com/koonie404/Portfolio)

