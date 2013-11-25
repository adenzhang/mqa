#ifndef MQA_MAQ_SHARED_H_
#define MQA_MAQ_SHARED_H_

#ifndef _DECL_EXPORT
#	if defined(WIN32) || defined(WIN64)
#		define _DECL_EXPORT  __declspec(dllexport)
#	else
#		define _DECL_EXPORT
#   endif
#endif

#ifndef _DECL_IMPORT
#	if defined(WIN32) || defined(WIN64)
#		define _DECL_IMPORT  __declspec(dllimport)
#	else
#		define _DECL_IMPORT
#   endif
#endif

#ifndef C_API
#   ifdef __cplusplus
#       define C_API extern "C"
#   else
#       define C_API
#   endif
#endif

#if defined(MQA_EXPORTS)
#  define MQA_SHARED _DECL_EXPORT
#else
#  define MQA_SHARED _DECL_IMPORT
#endif

#define MQA_API C_API MQA_SHARED

#endif //MQA_MAQ_SHARED_H_
