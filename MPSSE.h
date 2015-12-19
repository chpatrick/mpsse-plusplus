#ifndef HUBSAN2_MPSSE_H
#define HUBSAN2_MPSSE_H

#include "mpsse.h"

#include <memory>

namespace mpsse {

struct MPSSE {
  enum class Interface {
    Any = IFACE_ANY,
    A = IFACE_A,
    B = IFACE_B,
    C = IFACE_C,
    D = IFACE_D,
  };

  enum class ClockRate {
    OneHundredKHz = ONE_HUNDRED_KHZ,
    FourHundredKHz = FOUR_HUNDRED_KHZ,
    OneMHz = ONE_MHZ,
    TwoMHz = TWO_MHZ,
    FiveMHz = FIVE_MHZ,
    SixMHz = SIX_MHZ,
    TenMHz = TEN_MHZ,
    TwelveMHz = TWELVE_MHZ,
    FifteenMHz = FIFTEEN_MHZ,
    ThirtyMHz = THIRTY_MHZ,
    SixtyMHz = SIXTY_MHZ,
  };

  enum class Endianness {
#pragma push_macro("MSB")
#undef MSB
    MSB    =
#pragma pop_macro("MSB")
    MSB,
#undef MSB

#pragma push_macro("LSB")
#undef LSB
    LSB =
#pragma pop_macro("LSB")
    LSB,
#undef LSB
  };

  MPSSE(const MPSSE &) = delete;
  MPSSE & operator=(const MPSSE &) = delete;
  ~MPSSE();

  // Reading
  std::unique_ptr<char, void (*)(void*)> Read(int size);
  char ReadBits(int size);

  // Writing
  void Write(const char* data, int size);
  void WriteBits(char bits, int size);

  // Miscellaneous
  void FlushAfterRead(bool flush);
  void SetLoopback(bool enable);
  int GetClock();

  std::string GetDescription();
  int GetPid();
  int GetVid();

  static std::pair<int, int> Version();

protected:
  MPSSE(MPSSE &&) = default;
  MPSSE(mpsse_context* ctx);
  std::unique_ptr<mpsse_context> ctx;
};

struct MPSSE_SPI : public MPSSE {
  MPSSE_SPI(MPSSE_SPI&&) = default;
  MPSSE_SPI& operator=(MPSSE_SPI&&) = default;

  enum class Mode {
    SPI0 = ::SPI0,
    SPI1 = ::SPI1,
    SPI2 = ::SPI2,
    SPI3 = ::SPI3
  };

  static MPSSE_SPI Simple(Mode spiMode, ClockRate freq, Endianness endianness);

  static MPSSE_SPI Open(Mode spiMode,
                        int vid,
                        int pid,
                        ClockRate freq,
                        Endianness endianness,
                        Interface interface,
                        const std::string& description,
                        const std::string& serial);

  enum class IdleMode {
    Low = 0,
    High = 1,
  };

  std::unique_ptr<char, void (*)(void*)> Transfer(char *data, int size);
  void SetCSIdle(IdleMode idleMode);
  void FastRead(char* data, int size);
  void FastWrite(const char* data, int size);

private:
  using MPSSE::MPSSE;
};

#define FACTORIES_DECL(__T__, __mode__) \
static __T__ Simple(ClockRate freq, Endianness endianness); \
\
static __T__ Open(int vid, \
                  int pid, \
                  ClockRate freq, \
                  Endianness endianness, \
                  Interface interface, \
                  const std::string& description, \
                  const std::string& serial); \
\
static __T__ OpenIndex(int vid, \
                       int pid, \
                       ClockRate freq, \
                       Endianness endianness, \
                       Interface interface, \
                       const std::string& description, \
                       const std::string& serial, \
                       int index);

struct MPSSE_I2C : public MPSSE {
  MPSSE_I2C(MPSSE_I2C&&) = default;
  MPSSE_I2C& operator=(MPSSE_I2C&&) = default;

  FACTORIES_DECL(MPSSE_I2C, I2C)

  int GetAck();
  void SendAcks();
  void SendNacks();
  void Tristate();
private:
  using MPSSE::MPSSE;
};

struct MPSSE_GPIO : public MPSSE {
  MPSSE_GPIO(MPSSE_GPIO&&) = default;
  MPSSE_GPIO& operator=(MPSSE_GPIO&&) = default;

  FACTORIES_DECL(MPSSE_GPIO, GPIO);

  enum class Pin
  {
    GPIOL0 = ::GPIOL0,
    GPIOL1 = ::GPIOL1,
    GPIOL2 = ::GPIOL2,
    GPIOL3 = ::GPIOL3,
    GPIOH0 = ::GPIOH0,
    GPIOH1 = ::GPIOH1,
    GPIOH2 = ::GPIOH2,
    GPIOH3 = ::GPIOH3,
    GPIOH4 = ::GPIOH4,
    GPIOH5 = ::GPIOH5,
    GPIOH6 = ::GPIOH6,
    GPIOH7 = ::GPIOH7,
  };

  void PinHigh(Pin pin);
  void PinLow(Pin pin);
private:
  using MPSSE::MPSSE;
};

struct MPSSE_BitBang : public MPSSE {
  MPSSE_BitBang(MPSSE_BitBang&&) = default;
  MPSSE_BitBang& operator=(MPSSE_BitBang&&) = default;

  FACTORIES_DECL(MPSSE_BitBang, BITBANG);

  int ReadPins();
  int PinState(int pin, int state);
  void WritePins(uint8_t data);
  void SetDirection(uint8_t direction);

private:
  using MPSSE::MPSSE;
};

} // namespace mpsse

#undef FACTORIES_DECL

#endif //HUBSAN2_MPSSE_H
