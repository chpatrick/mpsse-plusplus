#include <sstream>
#include "MPSSE.h"

namespace mpsse {

MPSSE::MPSSE(mpsse_context *ctx)
  : ctx(ctx)
{
}

MPSSE::~MPSSE() {
  Close(ctx.get());
}

inline void check(bool condition, mpsse_context* ctx, const std::string& operation) {
  if (!condition) {
    std::stringstream message;
    message << operation << " failed: " << ::ErrorString(ctx);

    throw std::runtime_error(message.str());
  }
}

std::unique_ptr<char, void (*)(void*)> MPSSE::Read(int size) {
  char* result = ::Read(ctx.get(), size);
  check(result != nullptr, ctx.get(), "Read");
  return std::unique_ptr<char, void (*)(void*)>(result, free);
}


static mpsse_context* SimpleImpl(::modes mode,
                                 MPSSE::ClockRate freq,
                                 MPSSE::Endianness endianness) {
  auto ctx = ::MPSSE(mode,
                     static_cast<int>(freq),
                     static_cast<int>(endianness));
  check(ctx->open == 1, ctx, "Simple");

  return ctx;
}

static mpsse_context* OpenImpl(::modes mode,
                               int vid,
                               int pid,
                               MPSSE::ClockRate freq,
                               MPSSE::Endianness endianness,
                               MPSSE::Interface interface,
                               const std::string& description,
                               const std::string& serial) {
  auto ctx = ::Open(vid,
                    pid,
                    mode,
                    static_cast<int>(freq),
                    static_cast<int>(endianness),
                    static_cast<int>(interface),
                    description.empty() ? nullptr : description.c_str(),
                    serial.empty() ? nullptr : serial.c_str());
  check(ctx->open == 1, ctx, "Open");
  return ctx;
}

static mpsse_context* OpenIndexImpl(::modes mode,
                                    int vid,
                                    int pid,
                                    MPSSE::ClockRate freq,
                                    MPSSE::Endianness endianness,
                                    MPSSE::Interface interface,
                                    const std::string& description,
                                    const std::string& serial,
                                    int index) {
  auto ctx = ::OpenIndex(vid,
                         pid,
                         mode,
                         static_cast<int>(freq),
                         static_cast<int>(endianness),
                         static_cast<int>(interface),
                         description.empty() ? nullptr : description.c_str(),
                         serial.empty() ? nullptr : serial.c_str(),
                         index);
  check(ctx->open == 1, ctx, "OpenIndex");

  return ctx;
}

// Mode-specific factory methods.
#define FACTORIES_IMPL(__T__, __mode__) \
__T__ __T__::Simple(ClockRate freq, Endianness endianness) { \
return __T__(SimpleImpl(__mode__, freq, endianness)); \
} \
 \
__T__ __T__::Open(int vid, \
                  int pid, \
                  ClockRate freq, \
                  Endianness endianness, \
                  Interface interface, \
                  const std::string& description, \
                  const std::string& serial) { \
  return __T__(OpenImpl(__mode__, vid, pid, freq, endianness, interface, description, serial)); \
} \
\
__T__ __T__::OpenIndex(int vid, \
                       int pid, \
                       ClockRate freq, \
                       Endianness endianness, \
                       Interface interface, \
                       const std::string& description, \
                       const std::string& serial, \
                       int index) { \
  return __T__(OpenIndexImpl(__mode__, vid, pid, freq, endianness, interface, description, serial, index)); \
} \

FACTORIES_IMPL(MPSSE_I2C, I2C)

int MPSSE_I2C::GetAck() {
  return ::GetAck(ctx.get());
}

void MPSSE_I2C::SendAcks() {
  ::SendAcks(ctx.get());
}

void MPSSE_I2C::SendNacks() {
  ::SendNacks(ctx.get());
}

void MPSSE_I2C::Tristate() {
  int result = ::Tristate(ctx.get());
  if (result != MPSSE_OK) {
    throw std::runtime_error("Tristate failed.");
  }
}

MPSSE_SPI MPSSE_SPI::Simple(Mode spiMode,
                            ClockRate freq,
                            Endianness endianness) {
  return MPSSE_SPI(SimpleImpl(static_cast<modes>(spiMode), freq, endianness));
}

MPSSE_SPI MPSSE_SPI::Open(Mode spiMode,
                          int vid,
                          int pid,
                          ClockRate freq,
                          Endianness endianness,
                          Interface interface,
                          const std::string& description,
                          const std::string& serial) {
  return MPSSE_SPI(OpenImpl(static_cast<modes>(spiMode),
                            vid,
                            pid,
                            freq,
                            endianness,
                            interface,
                            description,
                            serial));
}

std::unique_ptr<char, void (*)(void*)> MPSSE_SPI::Transfer(char *data, int size) {
  char* result = ::Transfer(ctx.get(), data, size);
  check(result != nullptr, ctx.get(), "Transfer");
  return std::unique_ptr<char, void (*)(void*)>(result, free);
}

void MPSSE_SPI::SetCSIdle(MPSSE_SPI::IdleMode idleMode) {
  ::SetCSIdle(ctx.get(), static_cast<int>(idleMode));
}

void MPSSE_SPI::FastRead(char *data, int size) {
  int result = ::FastRead(ctx.get(), data, size);
  check(result == MPSSE_OK, ctx.get(), "FastRead");
}

void MPSSE_SPI::FastWrite(const char *data, int size) {
  int result = ::FastWrite(ctx.get(),
                           const_cast<char*>(data), // :(
                           size);
  check(result == MPSSE_OK, ctx.get(), "FastWrite");
}

char MPSSE::ReadBits(int size) {
  return ::ReadBits(ctx.get(), size);
}

void MPSSE::Write(const char *data, int size) {
  int result = ::Write(ctx.get(),
                       const_cast<char*>(data), // :(
                       size);
  check(result == MPSSE_OK, ctx.get(), "FastWrite");
}

void MPSSE::WriteBits(char bits, int size) {
  int result = ::WriteBits(ctx.get(), bits, size);
  check(result == MPSSE_OK, ctx.get(), "Write");
}

FACTORIES_IMPL(MPSSE_GPIO, GPIO)

void MPSSE_GPIO::PinHigh(MPSSE_GPIO::Pin pin) {
  int result = ::PinHigh(ctx.get(), static_cast<int>(pin));
  check(result == MPSSE_OK, ctx.get(), "PinHigh");
}

void MPSSE_GPIO::PinLow(MPSSE_GPIO::Pin pin) {
  int result = ::PinLow(ctx.get(), static_cast<int>(pin));
  check(result == MPSSE_OK, ctx.get(), "PinLow");
}

int MPSSE_BitBang::ReadPins() {
  return ::ReadPins(ctx.get());
}

int MPSSE_BitBang::PinState(int pin, int state) {
  ::PinState(ctx.get(), pin, state);
}

void MPSSE_BitBang::WritePins(uint8_t data) {
  int result = ::WritePins(ctx.get(), data);
  check(result == MPSSE_OK, ctx.get(), "WritePins");
}

void MPSSE_BitBang::SetDirection(uint8_t direction) {
  int result = ::SetDirection(ctx.get(), direction);
  check(result == MPSSE_OK, ctx.get(), "SetDirection");
}

void MPSSE::FlushAfterRead(bool flush) {
  ::FlushAfterRead(ctx.get(), static_cast<int>(flush));
}

void MPSSE::SetLoopback(bool enable) {
  ::SetLoopback(ctx.get(), static_cast<int>(enable));
}

int MPSSE::GetClock() {
  int result = ::GetClock(ctx.get());
}

std::string MPSSE::GetDescription() {
  const char* desc = ::GetDescription(ctx.get());
  check(desc != nullptr, ctx.get(), "GetDescription");
}

int MPSSE::GetPid() {
  int pid = ::GetPid(ctx.get());
  check(pid != 0, ctx.get(), "GetPid");
}

int MPSSE::GetVid() {
  int vid = ::GetVid(ctx.get());
  check(vid != 0, ctx.get(), "GetVid");
}

std::pair<int, int> MPSSE::Version() {
  char version = ::Version();
  if (version == 0) {
    throw std::runtime_error("Version failed.");
  }
  return { version >> 4, version & 0xF };
}

} // namespace mpsse