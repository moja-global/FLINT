#include "moja/exception.h"

#include <typeinfo>

namespace moja {

Exception::Exception(int code) : _pNested(0), _code(code) {}

Exception::Exception(const std::string& msg, int code) : _msg(msg), _pNested(0), _code(code) {}

Exception::Exception(const std::string& msg, const std::string& arg, int code) : _msg(msg), _pNested(0), _code(code) {
   if (!arg.empty()) {
      _msg.append(": ");
      _msg.append(arg);
   }
}

Exception::Exception(const std::string& msg, const Exception& nested, int code)
    : _msg(msg), _pNested(nested.clone()), _code(code) {}

Exception::Exception(const Exception& exc) : std::exception(exc), _msg(exc._msg), _code(exc._code) {
   _pNested = exc._pNested ? exc._pNested->clone() : 0;
}

Exception::~Exception() throw() { delete _pNested; }

Exception& Exception::operator=(const Exception& exc) {
   if (&exc != this) {
      delete _pNested;
      _msg = exc._msg;
      _pNested = exc._pNested ? exc._pNested->clone() : 0;
      _code = exc._code;
   }

   return *this;
}

const char* Exception::name() const throw() { return "Exception"; }

const char* Exception::className() const throw() { return typeid(*this).name(); }

const char* Exception::what() const throw() { return name(); }

std::string Exception::displayText() const {
   std::string txt = name();
   if (!_msg.empty()) {
      txt.append(": ");
      txt.append(_msg);
   }
   return txt;
}

void Exception::extendedMessage(const std::string& arg) {
   if (!arg.empty()) {
      if (!_msg.empty()) _msg.append(": ");
      _msg.append(arg);
   }
}

Exception* Exception::clone() const { return new Exception(*this); }

void Exception::rethrow() const { throw *this; }

MOJA_IMPLEMENT_EXCEPTION(LogicException, Exception, "Logic exception")
MOJA_IMPLEMENT_EXCEPTION(AssertionViolationException, LogicException, "Assertion violation")
MOJA_IMPLEMENT_EXCEPTION(NullPointerException, LogicException, "Null pointer")
MOJA_IMPLEMENT_EXCEPTION(NullValueException, LogicException, "Null value")
MOJA_IMPLEMENT_EXCEPTION(BugcheckException, LogicException, "Bugcheck")
MOJA_IMPLEMENT_EXCEPTION(InvalidArgumentException, LogicException, "Invalid argument")
MOJA_IMPLEMENT_EXCEPTION(NotImplementedException, LogicException, "Not implemented")
MOJA_IMPLEMENT_EXCEPTION(RangeException, LogicException, "Out of range")
MOJA_IMPLEMENT_EXCEPTION(IllegalStateException, LogicException, "Illegal state")
MOJA_IMPLEMENT_EXCEPTION(InvalidAccessException, LogicException, "Invalid access")
MOJA_IMPLEMENT_EXCEPTION(SignalException, LogicException, "Signal received")
MOJA_IMPLEMENT_EXCEPTION(UnhandledException, LogicException, "Unhandled exception")

MOJA_IMPLEMENT_EXCEPTION(RuntimeException, Exception, "Runtime exception")
MOJA_IMPLEMENT_EXCEPTION(NotFoundException, RuntimeException, "Not found")
MOJA_IMPLEMENT_EXCEPTION(ExistsException, RuntimeException, "Exists")
MOJA_IMPLEMENT_EXCEPTION(TimeoutException, RuntimeException, "Timeout")
MOJA_IMPLEMENT_EXCEPTION(SystemException, RuntimeException, "System exception")
MOJA_IMPLEMENT_EXCEPTION(RegularExpressionException, RuntimeException, "Error in regular expression")
MOJA_IMPLEMENT_EXCEPTION(LibraryLoadException, RuntimeException, "Cannot load library")
MOJA_IMPLEMENT_EXCEPTION(LibraryAlreadyLoadedException, RuntimeException, "Library already loaded")
MOJA_IMPLEMENT_EXCEPTION(LibrarySymbolLoadException, RuntimeException, "Cannot load library symbol")
MOJA_IMPLEMENT_EXCEPTION(NoThreadAvailableException, RuntimeException, "No thread available")
MOJA_IMPLEMENT_EXCEPTION(PropertyNotSupportedException, RuntimeException, "Property not supported")
MOJA_IMPLEMENT_EXCEPTION(PoolOverflowException, RuntimeException, "Pool overflow")
MOJA_IMPLEMENT_EXCEPTION(NoPermissionException, RuntimeException, "No permission")
MOJA_IMPLEMENT_EXCEPTION(OutOfMemoryException, RuntimeException, "Out of memory")
MOJA_IMPLEMENT_EXCEPTION(DataException, RuntimeException, "Data error")

MOJA_IMPLEMENT_EXCEPTION(DataFormatException, DataException, "Bad data format")
MOJA_IMPLEMENT_EXCEPTION(SyntaxException, DataException, "Syntax error")
MOJA_IMPLEMENT_EXCEPTION(CircularReferenceException, DataException, "Circular reference")
MOJA_IMPLEMENT_EXCEPTION(PathSyntaxException, SyntaxException, "Bad path syntax")
MOJA_IMPLEMENT_EXCEPTION(IOException, RuntimeException, "I/O error")
MOJA_IMPLEMENT_EXCEPTION(ProtocolException, IOException, "Protocol error")
MOJA_IMPLEMENT_EXCEPTION(FileException, IOException, "File access error")
MOJA_IMPLEMENT_EXCEPTION(FileExistsException, FileException, "File exists")
MOJA_IMPLEMENT_EXCEPTION(FileNotFoundException, FileException, "File not found")
MOJA_IMPLEMENT_EXCEPTION(PathNotFoundException, FileException, "Path not found")
MOJA_IMPLEMENT_EXCEPTION(FileReadOnlyException, FileException, "File is read-only")
MOJA_IMPLEMENT_EXCEPTION(FileAccessDeniedException, FileException, "Access to file denied")
MOJA_IMPLEMENT_EXCEPTION(CreateFileException, FileException, "Cannot create file")
MOJA_IMPLEMENT_EXCEPTION(OpenFileException, FileException, "Cannot open file")
MOJA_IMPLEMENT_EXCEPTION(WriteFileException, FileException, "Cannot write file")
MOJA_IMPLEMENT_EXCEPTION(ReadFileException, FileException, "Cannot read file")
MOJA_IMPLEMENT_EXCEPTION(UnknownURISchemeException, RuntimeException, "Unknown URI scheme")

MOJA_IMPLEMENT_EXCEPTION(ApplicationException, Exception, "Application exception")
MOJA_IMPLEMENT_EXCEPTION(BadCastException, RuntimeException, "Bad cast exception")

}  // namespace moja
