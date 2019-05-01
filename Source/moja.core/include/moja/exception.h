#ifndef MOJA_CORE_EXCEPTION_H_
#define MOJA_CORE_EXCEPTION_H_

#include "_core_exports.h"
#include <stdexcept>

namespace moja {

/**
 * <summary>
 * This is the base class for all exceptions defined in the moja Core class library.
 * </summary>
 */
class CORE_API Exception : public std::exception {
public:
    /**
     * <summary>    Creates an exception. </summary>
     *
     * <param name="msg">   The message. </param>
     * <param name="code">  (Optional) the code. </param>
     */
    Exception(const std::string& msg, int code = 0);

    /**
     * <summary>    Creates an exception. </summary>
     *
     * <param name="msg">   The message. </param>
     * <param name="arg">   The extended message. </param>
     * <param name="code">  (Optional) the code. </param>
     */
    Exception(const std::string& msg, const std::string& arg, int code = 0);

    /**
     * <summary>    Creates an exception and stores a clone of the nested exception. </summary>
     *
     * <param name="msg">       The message. </param>
     * <param name="nested">    The nested. </param>
     * <param name="code">      (Optional) the code. </param>
     */
    Exception(const std::string& msg, const Exception& nested, int code = 0);

    /**
     * <summary>    Copy constructor. </summary>
     */
    Exception(const Exception& exc);

    /** <summary>    Destroys the exception and deletes the nested exception. </summary> */
    ~Exception() throw();

    /**
     * <summary>    Assignment operator. </summary>
     */
    Exception& operator = (const Exception& exc);

    /**
     * <summary>    Returns a static string describing the exception. </summary>
     */
    virtual const char* name() const throw();

    /**
     * <summary>    Returns the name of the exception class. </summary>
     */
    virtual const char* className() const throw();

    /**
     * <summary>
     * Returns a static string describing the exception.
     * 
     * Same as name(), but for compatibility with std::exception.
     * </summary>
     */
    virtual const char* what() const throw();

    /**
     * <summary>
     * Returns a pointer to the nested exception, or null if no nested exception exists.
     * </summary>
     */
    const Exception* nested() const;

    /**
     * <summary>    Returns the message text. </summary>
     */
    const std::string& message() const;

    /**
     * <summary>    Returns the exception code if defined. </summary>
     */
    int code() const;

    /**
     * <summary>    Returns a string consisting of the message name and the message text. </summary>
     */
    std::string displayText() const;

    /**
     * <summary>
     * Creates an exact copy of the exception.
     * 
     * The copy can later be thrown again by invoking rethrow() on it.
     * </summary>
     */
    virtual Exception* clone() const;

    /**
     * <summary>
     * (Re)Throws the exception.
     * 
     * This is useful for temporarily storing a copy of an exception (see clone()), then throwing it
     * again.
     * </summary>
     */
    virtual void rethrow() const;

protected:
    /**
     * <summary>    Standard constructor. </summary>
     *
     * <param name="code">  (Optional) the code. </param>
     */
    Exception(int code = 0);

    /**
     * <summary>    Sets the message for the exception. </summary>
     *
     * <param name="msg">   The message. </param>
     */
    void message(const std::string& msg);

    /**
     * <summary>    Sets the extended message for the exception. </summary>
     *
     * <param name="arg">   The extended message. </param>
     */
    void extendedMessage(const std::string& arg);

private:
    std::string _msg;
    Exception*  _pNested;
    int			_code;
};

inline const Exception* Exception::nested() const {
    return _pNested;
}

inline const std::string& Exception::message() const {
    return _msg;
}

inline void Exception::message(const std::string& msg) {
    _msg = msg;
}

inline int Exception::code() const {
    return _code;
}

// Macros for quickly declaring and implementing exception classes.
// Unfortunately, we cannot use a template here because character
// pointers (which we need for specifying the exception name)
// are not allowed as template arguments.
#define MOJA_DECLARE_EXCEPTION_CODE(API, CLS, BASE, CODE)											\
    class API CLS: public BASE {																	\
    public:																							\
        CLS(int code = CODE);																		\
        CLS(const std::string& msg, int code = CODE);												\
        CLS(const std::string& msg, const std::string& arg, int code = CODE);						\
        CLS(const std::string& msg, const moja::Exception& exc, int code = CODE);					\
        CLS(const CLS& exc);																		\
        ~CLS() throw();																				\
        CLS& operator = (const CLS& exc);															\
        const char* name() const throw();															\
        const char* className() const throw();														\
        moja::Exception* clone() const;																\
        void rethrow() const;																		\
    };

#define MOJA_DECLARE_EXCEPTION(API, CLS, BASE)														\
    MOJA_DECLARE_EXCEPTION_CODE(API, CLS, BASE, 0)

#define MOJA_IMPLEMENT_EXCEPTION(CLS, BASE, NAME)													\
    CLS::CLS(int code): BASE(code) {																\
    }																								\
    CLS::CLS(const std::string& msg, int code): BASE(msg, code) {									\
    }																								\
    CLS::CLS(const std::string& msg, const std::string& arg, int code): BASE(msg, arg, code) {		\
    }																								\
    CLS::CLS(const std::string& msg, const moja::Exception& exc, int code): BASE(msg, exc, code) {	\
    }																								\
    CLS::CLS(const CLS& exc): BASE(exc) {															\
    }																								\
    CLS::~CLS() throw() {																			\
    }																								\
    CLS& CLS::operator = (const CLS& exc) {															\
        BASE::operator = (exc);																		\
        return *this;																				\
    }																								\
    const char* CLS::name() const throw() {															\
        return NAME;																				\
    }																								\
    const char* CLS::className() const throw() {													\
        return typeid(*this).name();																\
    }																								\
    moja::Exception* CLS::clone() const {															\
        return new CLS(*this);																		\
    }																								\
    void CLS::rethrow() const {																		\
        throw *this;																				\
    }

// Standard exception classes.
MOJA_DECLARE_EXCEPTION(CORE_API, LogicException, Exception)
MOJA_DECLARE_EXCEPTION(CORE_API, AssertionViolationException, LogicException)
MOJA_DECLARE_EXCEPTION(CORE_API, NullPointerException, LogicException)
MOJA_DECLARE_EXCEPTION(CORE_API, NullValueException, LogicException)
MOJA_DECLARE_EXCEPTION(CORE_API, BugcheckException, LogicException)
MOJA_DECLARE_EXCEPTION(CORE_API, InvalidArgumentException, LogicException)
MOJA_DECLARE_EXCEPTION(CORE_API, NotImplementedException, LogicException)
MOJA_DECLARE_EXCEPTION(CORE_API, RangeException, LogicException)
MOJA_DECLARE_EXCEPTION(CORE_API, IllegalStateException, LogicException)
MOJA_DECLARE_EXCEPTION(CORE_API, InvalidAccessException, LogicException)
MOJA_DECLARE_EXCEPTION(CORE_API, SignalException, LogicException)
MOJA_DECLARE_EXCEPTION(CORE_API, UnhandledException, LogicException)

MOJA_DECLARE_EXCEPTION(CORE_API, RuntimeException, Exception)
MOJA_DECLARE_EXCEPTION(CORE_API, NotFoundException, RuntimeException)
MOJA_DECLARE_EXCEPTION(CORE_API, ExistsException, RuntimeException)
MOJA_DECLARE_EXCEPTION(CORE_API, TimeoutException, RuntimeException)
MOJA_DECLARE_EXCEPTION(CORE_API, SystemException, RuntimeException)
MOJA_DECLARE_EXCEPTION(CORE_API, RegularExpressionException, RuntimeException)
MOJA_DECLARE_EXCEPTION(CORE_API, LibraryLoadException, RuntimeException)
MOJA_DECLARE_EXCEPTION(CORE_API, LibraryAlreadyLoadedException, RuntimeException)
MOJA_DECLARE_EXCEPTION(CORE_API, LibrarySymbolLoadException, RuntimeException)
MOJA_DECLARE_EXCEPTION(CORE_API, NoThreadAvailableException, RuntimeException)
MOJA_DECLARE_EXCEPTION(CORE_API, PropertyNotSupportedException, RuntimeException)
MOJA_DECLARE_EXCEPTION(CORE_API, PoolOverflowException, RuntimeException)
MOJA_DECLARE_EXCEPTION(CORE_API, NoPermissionException, RuntimeException)
MOJA_DECLARE_EXCEPTION(CORE_API, OutOfMemoryException, RuntimeException)
MOJA_DECLARE_EXCEPTION(CORE_API, DataException, RuntimeException)

MOJA_DECLARE_EXCEPTION(CORE_API, DataFormatException, DataException)
MOJA_DECLARE_EXCEPTION(CORE_API, SyntaxException, DataException)
MOJA_DECLARE_EXCEPTION(CORE_API, CircularReferenceException, DataException)
MOJA_DECLARE_EXCEPTION(CORE_API, PathSyntaxException, SyntaxException)
MOJA_DECLARE_EXCEPTION(CORE_API, IOException, RuntimeException)
MOJA_DECLARE_EXCEPTION(CORE_API, ProtocolException, IOException)
MOJA_DECLARE_EXCEPTION(CORE_API, FileException, IOException)
MOJA_DECLARE_EXCEPTION(CORE_API, FileExistsException, FileException)
MOJA_DECLARE_EXCEPTION(CORE_API, FileNotFoundException, FileException)
MOJA_DECLARE_EXCEPTION(CORE_API, PathNotFoundException, FileException)
MOJA_DECLARE_EXCEPTION(CORE_API, FileReadOnlyException, FileException)
MOJA_DECLARE_EXCEPTION(CORE_API, FileAccessDeniedException, FileException)
MOJA_DECLARE_EXCEPTION(CORE_API, CreateFileException, FileException)
MOJA_DECLARE_EXCEPTION(CORE_API, OpenFileException, FileException)
MOJA_DECLARE_EXCEPTION(CORE_API, WriteFileException, FileException)
MOJA_DECLARE_EXCEPTION(CORE_API, ReadFileException, FileException)
MOJA_DECLARE_EXCEPTION(CORE_API, UnknownURISchemeException, RuntimeException)

MOJA_DECLARE_EXCEPTION(CORE_API, ApplicationException, Exception)
MOJA_DECLARE_EXCEPTION(CORE_API, BadCastException, RuntimeException)

} // namespace moja

#endif // MOJA_CORE_EXCEPTION_H_