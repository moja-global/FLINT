
#ifndef MOJA_CORE_STOPWATCH_H_
#define MOJA_CORE_STOPWATCH_H_

#include <chrono>

namespace moja {
template <typename Clock = std::chrono::high_resolution_clock>
class Stopwatch {
  public:
   using duration = typename Clock::duration;

   Stopwatch();
   ~Stopwatch() = default;
   Stopwatch(const Stopwatch&) = delete;
   Stopwatch& operator=(const Stopwatch&) = delete;

   void start();
   void stop();
   void reset();
   void restart();
   duration elapsed() const;
   double elapsedSeconds() const;

  private:
   std::chrono::time_point<Clock> _start;
   bool _running;
   typename Clock::duration _elapsed;
};

template <typename Clock>
Stopwatch<Clock>::Stopwatch() : _running(false), _elapsed(0) {}

template <typename Clock>
void Stopwatch<Clock>::start() {
   if (!_running) {
      _start = Clock::now();
      _running = true;
   }
}

template <typename Clock>
void Stopwatch<Clock>::stop() {
   if (_running) {
      _elapsed += Clock::now() - _start;
      _running = false;
   }
}

template <typename Clock>
void Stopwatch<Clock>::reset() {
   _elapsed = {};
   _running = false;
}

template <typename Clock>
void Stopwatch<Clock>::restart() {
   _elapsed = {};
   _start = Clock::now();
   _running = true;
}

template <typename Clock>
typename Stopwatch<Clock>::duration Stopwatch<Clock>::elapsed() const {
   if (_running) return _elapsed + (Clock::now() - _start);
   return _elapsed;
}

template <typename Clock>
double Stopwatch<Clock>::elapsedSeconds() const {
   return std::chrono::duration_cast<std::chrono::duration<double, std::chrono::seconds::period>>(_elapsed).count();
}
}  // namespace moja

#endif  // MOJA_CORE_STOPWATCH_H_