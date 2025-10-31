# STM32-BNO055

Bu repo, BNO055 IMU sensörünü STM32 (HAL) ile hızlıca denemek için hazırlanmış küçük bir test uygulamasıdır.

İçindekiler

- `Inc/bno055.h` : BNO055 için basit API ve sabitler
- `Src/bno055.c` : I2C üzerinden BNO055'i başlatma ve Euler açılarını okuyan örnek implementasyon
- `main.c` : kütüphanenin nasıl kullanılacağına dair basit test kodu

Kısa açıklama
Bu proje, BNO055'i HAL I2C ile kullanmak için küçük, anlaşılır bir kütüphane sunar. Amaç; kütüphaneyi kendi STM32 projenize kolayca entegre edip sensörden Euler (z, y, x) açılarını okumaktır.

Önkoşullar

- STM32CubeIDE / STM32CubeMX veya benzeri (HAL kullanan bir yapı)
- HAL I2C sürücüleri proje ile eklenmiş olmalı
- BNO055 sensörü 3.3V ile beslenmelidir (5V kullanmayın)

Donanım (bağlantı)

- VCC -> 3.3V
- GND -> GND
- SDA -> MCU SDA (örn. PB7)
- SCL -> MCU SCL (örn. PB6)
- Not: I2C için pull-up dirençleri gerekebilir (modülde yoksa 4.7k–10k arası önerilir).

BNO055 adresi

- Kütüphanede `BNO055_I2C_ADDR` olarak `0x50` tanımlanmıştır. Bu değer 8-bit (7-bit adres << 1) formatındadır; bazı HAL fonksiyonları 7-bit adres bekler. Tipik 7-bit adres `0x28`'dir (0x28 << 1 == 0x50). Eğer sensörünüzün ADR pini farklıysa adresi güncelleyin.

Entegrasyon adımları (kısa, kullanıcı dostu)

1. Dosyaları kopyalama

   - `Inc/bno055.h` dosyasını projenizin `Inc/` dizinine kopyalayın.
   - `Src/bno055.c` dosyasını projenizin `Src/` dizinine kopyalayın.

2. CubeMX (Pinout & Configuration) — I2C aktif etme

   - CubeMX'te proje açın ve "Pinout & Configuration" görünümüne gelin.
   - Connectivity -> I2C (ör. `I2C1`) seçin ve enable edin. Bu adım SDA/SCL pinlerini otomatik atar veya elle atama yapmanıza izin verir.
   - I2C ayarlarında (Parameters) Timing, Speed ve Pull-up ayarlarını kontrol edin. Eğer modülünüzde pull-up yoksa kartınızda veya sensör breakout'unda 4.7k–10k pull-up kullanın.
   - System Core -> SYS -> Debug kısmında "Serial Wire" seçeneğini bırakın (SWD) — bu, SWV/ITM debug için gerekli olan Serial Wire arayüzünü sağlar.

3. Kod içinde kullanımı
   - `bno055.h` başlığını include edin.
   - HAL I2C handle'ınızı (örn. `hi2c1`) `BNO055_Init()` ve `BNO055_Read_Euler()` fonksiyonlarına gönderin.

Nereleri değiştirebilirsiniz (ve nereleri değiştirmemelisiniz)

- `BNO055_I2C_ADDR` : `Inc/bno055.h` içinde 8-bit formatta `0x50` olarak tanımlanmıştır (7-bit adres 0x28 için 0x28<<1 == 0x50). Eğer modülünüzün adres pini farklıysa veya HAL fonksiyonlarınız 7-bit adres bekliyorsa burayı güncelleyin.
- I2C handle ismi: kütüphane API'si handle'ı parametre olarak aldığı için (örn. `&hi2c1`), CubeMX tarafından oluşturulan handle ismini değiştirmeyin; sadece doğru handle'ı fonksiyona verin.
- `printf` çağrıları: `Src/bno055.c` içinde bazı `printf()` satırları vardır. Projenizde `printf` retarget edilmemişse bu çağrılar sessiz kalacak veya semihosting gerektirebilir. Aşağıda UART ile retarget örneği ekledim.
- Blocking gecikmeler: `bno055.c` içinde `HAL_Delay()` kullanılmıştır. RTOS ya da non-blocking gereksiniminiz varsa bu kısmı uyarlayın.

İzleme / debug (konsol çıktısı) seçenekleri

1. SWV / ITM (CubeIDE - "SWV ITM Data Console")

- Avantaj: düşük gecikme, kolay printf yönlendirme (ITM)
- Gereksinimler: MCU'nuzun SWO pini ve ST-LINK'in SWO desteği olmalı. Ayrıca System Core debug olarak "Serial Wire" etkin olmalı.
- CubeIDE'de: Debug konfigürasyonunda SWV/ITM (Trace) özelliğini etkinleştirin ve "SWV ITM Data Console" penceresini açın. Bu yöntem F4xx (ör. F446) gibi panellerde sorunsuz çalışır.
- Not: Tüm kartlarda/şeritlerde SWV desteklenmez; ST-LINK sürümünüzün ve kartınızın SWO pin çıkışının olduğundan emin olun.

2. UART (evrensel, her kartta çalışır)

- Eğer SWV kullanılamıyorsa (ör. bazı F411 kartlarında SWV/ITM eksikse), UART kullanın. Bu yöntem evrenseldir ve seri monitör/programlayıcı üzerinden okunabilir.
- CubeMX'te USART/USARTx aktif edin, baud rate ve pin ayarlarını yapın. `MX_USARTx_UART_Init()` fonksiyonunu generate ettikten sonra aşağıdaki `printf` retarget kodunu ekleyin.

Printf -> UART retarget (örnek, `uart` handle: `huart1`)

```c
// stdio retarget (GCC newlib / semihosting olmayan projeler için)
#include "usart.h" // huart1 extern tanımı için

int _write(int file, char *ptr, int len)
{
	 // huart1 CubeMX ile oluşturulmuş olmalı (MX_USART1_UART_Init gibi)
	 HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, HAL_MAX_DELAY);
	 return len;
}
```

Kullanım örneği (BNO055 açılarını UART ile yazdırma)

```c
BNO055_EulerData_t euler;
char buf[128];

if (BNO055_Read_Euler(&hi2c1, &euler) == HAL_OK)
{
	 int n = snprintf(buf, sizeof(buf), "Euler X: %.2f  Y: %.2f  Z: %.2f\r\n", euler.x, euler.y, euler.z);
	 HAL_UART_Transmit(&huart1, (uint8_t*)buf, n, HAL_MAX_DELAY);
}
```

3. Semihosting veya SWO olmayan debuglar

- Semihosting (printf üzerinden debugger) bazı durumlarda yavaş olabilir veya belirli debug modlarında çalışır. Daha stabil çözüm için SWV/ITM veya UART tercih edin.

# STM32-BNO055

This repository contains a small test application to try the BNO055 IMU with an STM32 using HAL.

Contents

- `Inc/bno055.h` — simple API and constants for the BNO055
- `Src/bno055.c` — example implementation that initializes the sensor and reads Euler angles over I2C
- `main.c` — a small test harness showing how to use the library

Overview
This project provides a compact, easy-to-read library intended to help you quickly integrate the BNO055 into your STM32/HAL project and read Euler angles (Z, Y, X).

Prerequisites

- STM32CubeIDE / STM32CubeMX (or any project using HAL)
- HAL I2C drivers added to your project
- BNO055 must be powered with 3.3V (do not use 5V)

Wiring (hardware)

- VCC -> 3.3V
- GND -> GND
- SDA -> MCU SDA pin (e.g. PB7)
- SCL -> MCU SCL pin (e.g. PB6)
- Note: Use pull-up resistors (4.7k–10k) if your breakout board doesn't include them.

BNO055 I2C address
The library defines `BNO055_I2C_ADDR` as `0x50`. This is the 8-bit address format (7-bit address << 1). The common 7-bit address is `0x28` (0x28 << 1 == 0x50). If your module's ADR pin is set differently or your HAL functions expect 7-bit addresses, update the define accordingly.

Integration steps (user-friendly)

1. Copy files

   - Copy `Inc/bno055.h` to your project's `Inc/` folder.
   - Copy `Src/bno055.c` to your project's `Src/` folder.

2. CubeMX — enable I2C and pins

   - Open your project in CubeMX and go to "Pinout & Configuration".
   - Enable Connectivity -> I2C (e.g. `I2C1`). CubeMX will assign SDA/SCL pins automatically or let you assign them manually.
   - Check I2C Parameters: Timing, speed, and pull-up options. If your sensor board lacks pull-ups, add external 4.7k–10k pull-ups.
   - In System Core -> SYS -> Debug, keep "Serial Wire" enabled (SWD). This is required for SWV/ITM trace if you plan to use SWV.

3. In your code
   - `#include "bno055.h"`
   - Call `BNO055_Init(&hi2c1)` and `BNO055_Read_Euler(&hi2c1, &euler)` using the HAL I2C handle from CubeMX (e.g. `hi2c1`).

What you can change (and what to leave)

- `BNO055_I2C_ADDR` in `Inc/bno055.h`: change this if your module uses a different address or if your HAL interface requires 7-bit addresses.
- I2C handle name: the library accepts an `I2C_HandleTypeDef *` parameter, so you can pass `&hi2c1`, `&hi2c2`, etc. No change needed in the library itself.
- `printf` calls in `Src/bno055.c`: the file contains `printf()` calls for logging. If your project does not route `printf` to UART or SWV, either retarget `printf` or remove these calls.
- Blocking delays: `bno055.c` uses `HAL_Delay()` during setup. If you need non-blocking behavior (RTOS or low-latency applications), refactor these delays.

Monitoring / Debugging (console output)

1. SWV / ITM (CubeIDE "SWV ITM Data Console")

   - Pros: low overhead, easy printf routing via ITM
   - Requirements: MCU SWO pin available and ST-LINK that supports SWO; "Serial Wire" (SWD) must be enabled in CubeMX.
   - In CubeIDE: enable SWV/ITM (Trace) in the debug configuration and open the "SWV ITM Data Console". This works well on many F4 series boards (e.g. F446).
   - Note: Not all boards support SWV. Check your ST-LINK and board for SWO availability.

2. UART (works on any board)
   - If SWV is not available (for example, some F411 boards), use UART. This method is universal and can be viewed via a serial terminal.
   - Enable a USART in CubeMX, configure baud rate and pins, and generate code (e.g. `MX_USART1_UART_Init()`). Then add a `printf` retarget to UART as shown below.

Printf -> UART retarget (example, using `huart1`)

```c
// Add this in a source file (e.g. retarget.c). Include the generated usart header for huart1.
#include "usart.h" // provides extern UART_HandleTypeDef huart1

int _write(int file, char *ptr, int len)
{
	 // Ensure huart1 is initialized (MX_USART1_UART_Init called before using printf)
	 HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, HAL_MAX_DELAY);
	 return len;
}
```

Example: print Euler angles over UART

```c
BNO055_EulerData_t euler;
char buf[128];

if (BNO055_Read_Euler(&hi2c1, &euler) == HAL_OK)
{
	 int n = snprintf(buf, sizeof(buf), "Euler X: %.2f  Y: %.2f  Z: %.2f\r\n", euler.x, euler.y, euler.z);
	 HAL_UART_Transmit(&huart1, (uint8_t*)buf, n, HAL_MAX_DELAY);
}
```

3. Semihosting or other debug methods
   - Semihosting (debugger-based printf) can be used but is sometimes slow or depends on debugger settings. Prefer SWV/ITM or UART for reliable runtime logging.

Practical tips / Troubleshooting

- If `BNO055_Init()` reports device not found: check wiring, pull-ups, and power.
- If Chip ID verification fails: double-check the I2C address format (7-bit vs 8-bit) or wiring.
- If SWV does not work: verify SWV/ITM is enabled in the debug configuration and your ST-LINK/board supports SWO; otherwise use UART.

This repo is intended for testing and quick prototyping. For production use consider adding robust error handling, timeouts, retries, and non-blocking or RTOS-friendly behavior.

License
See the `LICENSE` file in this repository.

Enjoy!
