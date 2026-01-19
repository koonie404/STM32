# RGB LED Module Test - NUCLEO-F103RB

RGB LED 모듈을 STM32F103 NUCLEO 보드에서 PWM을 이용하여 제어하는 프로젝트입니다.

## 📌 개요

RGB LED 모듈은 Red, Green, Blue 세 가지 색상의 LED가 하나의 패키지에 통합된 모듈입니다. PWM(Pulse Width Modulation)을 이용하여 각 색상의 밝기를 조절하고, 이를 혼합하여 다양한 색상을 만들어낼 수 있습니다.

## 🛠 하드웨어 구성

### 필요 부품
| 부품 | 수량 | 비고 |
|------|------|------|
| NUCLEO-F103RB | 1 | STM32F103RB 탑재 |
| RGB LED 모듈 | 1 | KY-016 또는 호환 모듈 |
| 점퍼 와이어 | 4 | Female-Female |

### 핀 연결

```
RGB LED Module          NUCLEO-F103RB
┌─────────────┐        ┌─────────────┐
│     R  ─────┼────────┤ PA0 (TIM2_CH1)
│     G  ─────┼────────┤ PA1 (TIM2_CH2)
│     B  ─────┼────────┤ PB10 (TIM2_CH3)
│   GND  ─────┼────────┤ GND
└─────────────┘        └─────────────┘
```

> ⚠️ **주의**: 공통 애노드(Common Anode) 타입의 경우 GND 대신 3.3V에 연결하고, PWM 극성을 반전시켜야 합니다.

### 회로도

```
        ┌─────────────────────────────┐
        │        RGB LED Module       │
        │                             │
PA0 ────┤ R (Red)     ┌───┐           │
        │             │ R │           │
PA1 ────┤ G (Green)   │ G │  ───┬─────┤ GND
        │             │ B │     │     │
PB10 ───┤ B (Blue)    └───┘     │     │
        │                       │     │
        └───────────────────────┼─────┘
                               GND
```

## 💻 소프트웨어

### 주요 기능

1. **기본 색상 출력**: Red, Green, Blue, Yellow, Cyan, Magenta, White
2. **페이드 효과**: 각 색상의 점진적 밝기 변화
3. **레인보우 효과**: HSV 색상환 순환

### PWM 설정

```c
Timer: TIM2
Prescaler: 63 (64MHz / 64 = 1MHz)
Period: 999 (1MHz / 1000 = 1kHz PWM)
Channels: CH1(PA0), CH2(PA1), CH3(PB10)
```

### 주요 함수

```c
// RGB 색상 설정 (0~255 값)
void RGB_SetColor(uint8_t red, uint8_t green, uint8_t blue);

// 페이드 효과 데모
void RGB_Demo_Fade(void);

// 레인보우 효과 데모
void RGB_Demo_Rainbow(void);
```

### 색상 혼합 원리

| 색상 | R | G | B | 설명 |
|------|---|---|---|------|
| Red | 255 | 0 | 0 | 빨강 |
| Green | 0 | 255 | 0 | 초록 |
| Blue | 0 | 0 | 255 | 파랑 |
| Yellow | 255 | 255 | 0 | R + G |
| Cyan | 0 | 255 | 255 | G + B |
| Magenta | 255 | 0 | 255 | R + B |
| White | 255 | 255 | 255 | R + G + B |

## 📂 프로젝트 구조

```
01_RGB_LED/
├── main.c          # 메인 소스 코드
└── README.md       # 프로젝트 설명서
```

## 🔧 빌드 및 실행

### STM32CubeIDE 사용 시
1. 새 STM32 프로젝트 생성 (NUCLEO-F103RB 선택)
2. `main.c` 내용을 프로젝트에 복사
3. 빌드 후 보드에 플래시

```c
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include "stm32f1xx_hal.h"
/* USER CODE END Includes */
```

```c
/* USER CODE BEGIN PTD */
void RGB_SetColor(uint8_t red, uint8_t green, uint8_t blue);
void RGB_Demo_Fade(void);
void RGB_Demo_Rainbow(void);
/* USER CODE END PTD */
```

```c
/* USER CODE BEGIN PD */
#define PWM_PERIOD      999     // PWM 주기 (0~999 = 1000단계)
/* USER CODE END PD */
```

```c
/* USER CODE BEGIN PFP */
/* UART printf 리다이렉션 */
int __io_putchar(int ch) {
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
/* USER CODE END PFP */
```

```c
/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
 * @brief RGB LED 색상 설정 (0~255)
 */
void RGB_SetColor(uint8_t red, uint8_t green, uint8_t blue)
{
    /* 0~255를 0~PWM_PERIOD로 변환 */
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, (red * PWM_PERIOD) / 255);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, (green * PWM_PERIOD) / 255);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, (blue * PWM_PERIOD) / 255);
}

/**
 * @brief 페이드 효과 데모
 */
void RGB_Demo_Fade(void)
{
    /* Red 페이드 인/아웃 */
    for (int i = 0; i <= 255; i += 5) {
        RGB_SetColor(i, 0, 0);
        HAL_Delay(10);
    }
    for (int i = 255; i >= 0; i -= 5) {
        RGB_SetColor(i, 0, 0);
        HAL_Delay(10);
    }

    /* Green 페이드 인/아웃 */
    for (int i = 0; i <= 255; i += 5) {
        RGB_SetColor(0, i, 0);
        HAL_Delay(10);
    }
    for (int i = 255; i >= 0; i -= 5) {
        RGB_SetColor(0, i, 0);
        HAL_Delay(10);
    }

    /* Blue 페이드 인/아웃 */
    for (int i = 0; i <= 255; i += 5) {
        RGB_SetColor(0, 0, i);
        HAL_Delay(10);
    }
    for (int i = 255; i >= 0; i -= 5) {
        RGB_SetColor(0, 0, i);
        HAL_Delay(10);
    }
}

/**
 * @brief 레인보우 효과 데모 (색상환 순환)
 */
void RGB_Demo_Rainbow(void)
{
    uint8_t r, g, b;

    for (int i = 0; i < 360; i += 2) {
        /* HSV to RGB 변환 (S=1, V=1 고정) */
        int region = i / 60;
        int remainder = (i - (region * 60)) * 255 / 60;

        switch (region) {
            case 0:  r = 255; g = remainder; b = 0; break;
            case 1:  r = 255 - remainder; g = 255; b = 0; break;
            case 2:  r = 0; g = 255; b = remainder; break;
            case 3:  r = 0; g = 255 - remainder; b = 255; break;
            case 4:  r = remainder; g = 0; b = 255; break;
            default: r = 255; g = 0; b = 255 - remainder; break;
        }

        RGB_SetColor(r, g, b);
        HAL_Delay(20);
    }

    RGB_SetColor(0, 0, 0);
}
/* USER CODE END 0 */
```

```c
  /* USER CODE BEGIN 2 */
  /* PWM 시작 */
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);  // Red
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);  // Green
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);  // Blue

  printf("\r\n========================================\r\n");
  printf("  RGB LED Module Test - NUCLEO-F103RB\r\n");
  printf("========================================\r\n\n");
  /* USER CODE END 2 */
```

```c
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  /* 기본 색상 테스트 */
	         printf("[Test 1] Basic Colors\r\n");

	         printf("  Red...\r\n");
	         RGB_SetColor(255, 0, 0);
	         HAL_Delay(1000);

	         printf("  Green...\r\n");
	         RGB_SetColor(0, 255, 0);
	         HAL_Delay(1000);

	         printf("  Blue...\r\n");
	         RGB_SetColor(0, 0, 255);
	         HAL_Delay(1000);

	         printf("  Yellow (R+G)...\r\n");
	         RGB_SetColor(255, 255, 0);
	         HAL_Delay(1000);

	         printf("  Cyan (G+B)...\r\n");
	         RGB_SetColor(0, 255, 255);
	         HAL_Delay(1000);

	         printf("  Magenta (R+B)...\r\n");
	         RGB_SetColor(255, 0, 255);
	         HAL_Delay(1000);

	         printf("  White (R+G+B)...\r\n");
	         RGB_SetColor(255, 255, 255);
	         HAL_Delay(1000);

	         printf("  OFF...\r\n\n");
	         RGB_SetColor(0, 0, 0);
	         HAL_Delay(500);

	         /* 페이드 효과 */
	         printf("[Test 2] Fade Effect\r\n");
	         RGB_Demo_Fade();
	         HAL_Delay(500);

	         /* 레인보우 효과 */
	         printf("[Test 3] Rainbow Effect\r\n");
	         RGB_Demo_Rainbow();
	         HAL_Delay(500);

	         printf("\r\n--- Cycle Complete ---\r\n\n");
    /* USER CODE END WHILE */
```
## 📊 시리얼 출력 예시

```
========================================
  RGB LED Module Test - NUCLEO-F103RB
========================================

[Test 1] Basic Colors
  Red...
  Green...
  Blue...
  Yellow (R+G)...
  Cyan (G+B)...
  Magenta (R+B)...
  White (R+G+B)...
  OFF...

[Test 2] Fade Effect
[Test 3] Rainbow Effect

--- Cycle Complete ---
```

## 🔍 트러블슈팅

| 증상 | 원인 | 해결 방법 |
|------|------|----------|
| LED가 켜지지 않음 | 배선 오류 | 핀 연결 확인 |
| 색상이 반대로 동작 | 공통 애노드 타입 | PWM 극성 반전 |
| 색상이 어두움 | PWM 주기 문제 | Period 값 조정 |
| 특정 색상만 동작 | GPIO 설정 오류 | AF 설정 확인 |

## 🖼️ 이미지 (Images)
## Tera Term
<img width="827" height="550" alt="스크린샷 2026-01-19 105406" src="https://github.com/user-attachments/assets/6a0d92ce-e77e-4c52-9ad8-2d321a267553" />


## 📚 참고 자료

- [STM32F103 Reference Manual](https://www.st.com/resource/en/reference_manual/rm0008-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [KY-016 RGB LED Module Datasheet](https://arduinomodules.info/ky-016-rgb-full-color-led-module/)


