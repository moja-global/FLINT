#ifndef MOJA_CORE_TYPES_H_
#define MOJA_CORE_TYPES_H_

namespace moja {

#if defined(_MSC_VER)
	//
	// Windows/Visual C++
	//
	typedef signed char            Int8;
	typedef unsigned char          UInt8;
	typedef signed short           Int16;
	typedef unsigned short         UInt16;
	typedef signed int             Int32;
	typedef unsigned int           UInt32;
	typedef signed __int64         Int64;
	typedef unsigned __int64       UInt64;

	#if defined(_WIN64)
		#define MOJA_PTR_IS_64_BIT 1
		typedef signed __int64     IntPtr;
		typedef unsigned __int64   UIntPtr;
	#else
		typedef signed long        IntPtr;
		typedef unsigned long      UIntPtr;
	#endif
	#define MOJA_HAVE_INT64 1
#elif defined(__GNUC__) || defined(__clang__)
	//
	// Unix/GCC/Clang
	//
	typedef signed char            Int8;
	typedef unsigned char          UInt8;
	typedef signed short           Int16;
	typedef unsigned short         UInt16;
	typedef signed int             Int32;
	typedef unsigned int           UInt32;
	#if defined(_WIN64)
		#define MOJA_PTR_IS_64_BIT 1
		typedef signed long long   IntPtr;
		typedef unsigned long long UIntPtr;
		typedef signed long long   Int64;
		typedef unsigned long long UInt64;
	#else
		typedef signed long        IntPtr;
		typedef unsigned long      UIntPtr;
		#if defined(__LP64__)
			#define MOJA_PTR_IS_64_BIT 1
			#define MOJA_LONG_IS_64_BIT 1
			typedef signed long        Int64;
			typedef unsigned long      UInt64;
		#else
			typedef signed long long   Int64;
			typedef unsigned long long UInt64;
		#endif
	#endif
	#define POCO_HAVE_INT64 1
#endif

} // namespace moja


#endif // MOJA_CORE_TYPES_H_
