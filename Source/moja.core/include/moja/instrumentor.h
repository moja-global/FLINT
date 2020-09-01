// Steps to run the profiler(performance checker):
// 1) Usage: include this header file somewhere in your code.(#include "moja/flint/ioperationmanager.h")
//
// 2) Begin session:
// MOJA_PROFILE_BEGIN_SESSION("cli_file_name.cli", "new_file_name.json");
//
// 3)Usage:
// i)Inside a Function:
//     MOJA_PROFILE_FUNCTION(); //write this statement at starting of the function and will return the function run_time
// ii)Scope:
//      {
//            MOJA_PROFILE_SCOPE("Scope_name");
//            //Rest of the code
//      }     // Place code like this in scopes you'd like to include in and it will return the Scope_Runtime
//
//
// 4)End Session: MOJA_PROFILE_END_SESSION();
//
#pragma once

#include "logging.h"

#include <chrono>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

namespace moja {

using FloatingPointMicroseconds = std::chrono::duration<double, std::micro>;

struct ProfileResult {
   std::string name;

   FloatingPointMicroseconds start;
   std::chrono::microseconds elapsed_time;
   std::thread::id thread_id;
};

struct InstrumentationSession {
   std::string name;
};

class CORE_API Instrumentor {
  private:
   std::mutex mutex_;
   InstrumentationSession* current_session_;
   std::ofstream output_stream_;

   Instrumentor() : current_session_(nullptr) {}

  public:

   void BeginSession(const std::string& name, const std::string& filepath = "results.json") {
      std::lock_guard lock(mutex_);
      if (current_session_) {
         // If there is already a current session, then close it before beginning new one.
         // Subsequent profiling output meant for the original session will end up in the
         // newly opened session instead.  That's better than having badly formatted
         // profiling output.
         MOJA_LOG_ERROR << "Instrumentor::BeginSession('" << name << "') when session '" << current_session_->name
                        << "' already open.";
         InternalEndSession();
      }
      output_stream_.open(filepath);

      if (output_stream_.is_open()) {
         current_session_ = new InstrumentationSession({name});
         WriteHeader();
      } else {
         MOJA_LOG_ERROR << "Instrumentor could not open results file '" << filepath << "'.";
      }
   }

   void EndSession() {
      std::lock_guard lock(mutex_);
      InternalEndSession();
   }

   void WriteProfile(const ProfileResult& result) {
      std::stringstream json;

      json << std::setprecision(3) << std::fixed;
      json << ",{";
      json << "\"cat\":\"function\",";
      json << "\"dur\":" << (result.elapsed_time.count()) << ',';
      json << "\"name\":\"" << result.name << "\",";
      json << "\"ph\":\"X\",";
      json << "\"pid\":0,";
      json << "\"tid\":" << result.thread_id << ",";
      json << "\"ts\":" << result.start.count();
      json << "}";

      std::lock_guard lock(mutex_);
      if (current_session_) {
         output_stream_ << json.str();
         output_stream_.flush();
      }
   }

   static Instrumentor& Get();

  private:
   void WriteHeader() {
      output_stream_ << "{\"otherData\": {},\"traceEvents\":[{}";
      output_stream_.flush();
   }

   void WriteFooter() {
      output_stream_ << "]}";
      output_stream_.flush();
   }

   // Note: you must already own lock on m_Mutex before
   // calling InternalEndSession()
   void InternalEndSession() {
      if (current_session_) {
         WriteFooter();
         output_stream_.close();
         delete current_session_;
         current_session_ = nullptr;
      }
   }
};

class InstrumentationTimer {
  public:
   InstrumentationTimer(const char* name) : name_(name), stopped_(false) {
      start_time_point_ = std::chrono::steady_clock::now();
   }

   ~InstrumentationTimer() {
      if (!stopped_) Stop();
   }

   void Stop() {
      const auto end_time_point = std::chrono::steady_clock::now();
      const auto high_res_start = FloatingPointMicroseconds{start_time_point_.time_since_epoch()};
      const auto elapsed_time =
          std::chrono::time_point_cast<std::chrono::microseconds>(end_time_point).time_since_epoch() -
          std::chrono::time_point_cast<std::chrono::microseconds>(start_time_point_).time_since_epoch();

      Instrumentor::Get().WriteProfile({name_, high_res_start, elapsed_time, std::this_thread::get_id()});

      stopped_ = true;
   }

  private:
   const char* name_;
   std::chrono::time_point<std::chrono::steady_clock> start_time_point_;
   bool stopped_;
};

namespace instrumentor_utils {

template <size_t N>
struct ChangeResult {
   char data[N];
};

template <size_t N, size_t K>
constexpr auto CleanupOutputString(const char (&expr)[N], const char (&remove)[K]) {
   ChangeResult<N> result = {};

   size_t src_index = 0;
   size_t dst_index = 0;
   while (src_index < N) {
      size_t match_index = 0;
      while (match_index < K - 1 && src_index + match_index < N - 1 &&
             expr[src_index + match_index] == remove[match_index])
         match_index++;
      if (match_index == K - 1) src_index += match_index;
      result.data[dst_index++] = expr[src_index] == '"' ? '\'' : expr[src_index];
      src_index++;
   }
   return result;
}
}  // namespace instrumentor_utils
}  // namespace moja
#define MOJA_PROFILE 1
#ifdef MOJA_PROFILE
   // Resolve which function signature macro will be used. Note that this only
// is resolved when the (pre)compiler starts, so the syntax highlighting
// could mark the wrong one in your editor!
#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || \
    defined(__ghs__)
#define MOJA_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
#define MOJA_FUNC_SIG __PRETTY_FUNCTION__
#elif (defined(__FUNCSIG__) || (_MSC_VER))
#define MOJA_FUNC_SIG __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#define MOJA_FUNC_SIG __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#define MOJA_FUNC_SIG __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#define MOJA_FUNC_SIG __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
#define MOJA_FUNC_SIG __func__
#else
#define MOJA_FUNC_SIG "MOJA_FUNC_SIG unknown!"
#endif

#define MOJA_PROFILE_BEGIN_SESSION(name, filepath) ::moja::Instrumentor::Get().BeginSession(name, filepath)
#define MOJA_PROFILE_END_SESSION() ::moja::Instrumentor::Get().EndSession()
#define MOJA_PROFILE_SCOPE(name)                                                                 \
   constexpr auto fixedName = ::moja::instrumentor_utils::CleanupOutputString(name, "__cdecl "); \
   ::moja::InstrumentationTimer timer##__LINE__(fixedName.data)
#define MOJA_PROFILE_FUNCTION() MOJA_PROFILE_SCOPE(MOJA_FUNC_SIG)
#else
#define MOJA_PROFILE_BEGIN_SESSION(name, filepath)
#define MOJA_PROFILE_END_SESSION()
#define MOJA_PROFILE_SCOPE(name)
#define MOJA_PROFILE_FUNCTION()
#endif