#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stddef.h>
#include <setjmp.h>




#define METH_VARARGS  0x0001
#ifdef STACKLESS
#define Py_TPFLAGS_HAVE_STACKLESS_EXTENSION (3UL << 15)
#else
#define Py_TPFLAGS_HAVE_STACKLESS_EXTENSION 0
#endif
#define Py_TPFLAGS_DEFAULT  ( \
                 Py_TPFLAGS_HAVE_STACKLESS_EXTENSION | \
                0)
#define Py_TPFLAGS_BASETYPE (1UL << 10)


#ifndef HAVE_LONG_LONG
#define HAVE_LONG_LONG 1
#endif
#ifndef PY_LONG_LONG
#define PY_LONG_LONG long long
/* If LLONG_MAX is defined in limits.h, use that. */
#define PY_LLONG_MIN LLONG_MIN
#define PY_LLONG_MAX LLONG_MAX
#define PY_ULLONG_MAX ULLONG_MAX
#endif

#define PY_UINT32_T uint32_t
#define PY_UINT64_T uint64_t

/* Signed variants of the above */
#define PY_INT32_T int32_t
#define PY_INT64_T int64_t
typedef unsigned long     size_t;
typedef long int         intptr_t;

#ifndef Py_EXPORTS_H
#define Py_EXPORTS_H

#if defined(_WIN32) || defined(__CYGWIN__)
    #define Py_IMPORTED_SYMBOL __declspec(dllimport)
    #define Py_EXPORTED_SYMBOL __declspec(dllexport)
    #define Py_LOCAL_SYMBOL
#else
/*
 * If we only ever used gcc >= 5, we could use __has_attribute(visibility)
 * as a cross-platform way to determine if visibility is supported. However,
 * we may still need to support gcc >= 4, as some Ubuntu LTS and Centos versions
 * have 4 < gcc < 5.
 */
    #ifndef __has_attribute
      #define __has_attribute(x) 0  // Compatibility with non-clang compilers.
    #endif
    #if (defined(__GNUC__) && (__GNUC__ >= 4)) ||\
        (defined(__clang__) && __has_attribute(visibility))
        #define Py_IMPORTED_SYMBOL __attribute__ ((visibility ("default")))
        #define Py_EXPORTED_SYMBOL __attribute__ ((visibility ("default")))
        #define Py_LOCAL_SYMBOL  __attribute__ ((visibility ("hidden")))
    #else
        #define Py_IMPORTED_SYMBOL
        #define Py_EXPORTED_SYMBOL
        #define Py_LOCAL_SYMBOL
    #endif
#endif

#endif /* Py_EXPORTS_H */

#ifdef HAVE_PY_SSIZE_T

#elif HAVE_SSIZE_T
typedef ssize_t         Py_ssize_t;
#elif SIZEOF_VOID_P == SIZEOF_SIZE_T
typedef intptr_t  Py_ssize_t;
#else
#   error "Python needs a typedef for Py_ssize_t in pyport.h."
#endif
typedef Py_ssize_t Py_hash_t;

#ifdef Py_TRACE_REFS
/* Define pointers to support a doubly-linked list of all live heap objects. */
#define _PyObject_HEAD_EXTRA            \
    struct _object *_ob_next;           \
    struct _object *_ob_prev;

#define _PyObject_EXTRA_INIT 0, 0,

#else
#  define _PyObject_HEAD_EXTRA
#  define _PyObject_EXTRA_INIT
#endif

/* PyObject_HEAD defines the initial segment of every PyObject. */
#define PyObject_HEAD                   PyObject ob_base;

#define PyObject_HEAD_INIT(type)        \
    { _PyObject_EXTRA_INIT              \
    1, type },

#define PyVarObject_HEAD_INIT(type, size)       \
    { PyObject_HEAD_INIT(type) size },

/* PyObject_VAR_HEAD defines the initial segment of all variable-size
 * container objects.  These end with a declaration of an array with 1
 * element, but enough space is malloc'ed so that the array actually
 * has room for ob_size elements.  Note that ob_size is an element count,
 * not necessarily a byte count.
 */
#define PyObject_VAR_HEAD      PyVarObject ob_base;
#define Py_INVALID_SIZE (Py_ssize_t)-1


/* PyTypeObject structure is defined in cpython/object.h.
   In Py_LIMITED_API, PyTypeObject is an opaque structure. */
typedef struct _typeobject PyTypeObject;
typedef struct _object {
    _PyObject_HEAD_EXTRA
    Py_ssize_t ob_refcnt;
    PyTypeObject *ob_type;
} PyObject;
typedef struct {
    PyObject ob_base;
    Py_ssize_t ob_size; /* Number of items in variable part */
} PyVarObject;
typedef PyObject *(*getter)(PyObject *, void *);
typedef int (*setter)(PyObject *, PyObject *, void *);

typedef struct PyGetSetDef {
    const char *name;
    getter get;
    setter set;
    const char *doc;
    void *closure;
} PyGetSetDef;
typedef struct PyMemberDef {
    const char *name;
    int type;
    Py_ssize_t offset;
    int flags;
    const char *doc;
} PyMemberDef;
typedef PyObject *(*PyCFunction)(PyObject *, PyObject *);
struct PyMethodDef {
    const char  *ml_name;   /* The name of the built-in function/method */
    PyCFunction ml_meth;    /* The C function that implements it */
    int         ml_flags;   /* Combination of METH_xxx flags, which mostly
                               describe the args expected by the C func */
    const char  *ml_doc;    /* The __doc__ attribute, or NULL */
};
typedef PyObject * (*unaryfunc)(PyObject *);
typedef PyObject * (*binaryfunc)(PyObject *, PyObject *);
typedef PyObject * (*ternaryfunc)(PyObject *, PyObject *, PyObject *);
typedef int (*inquiry)(PyObject *);
typedef struct {
    /* Number implementations must check *both*
       arguments for proper type and implement the necessary conversions
       in the slot functions themselves. */

    binaryfunc nb_add;
    binaryfunc nb_subtract;
    binaryfunc nb_multiply;
    binaryfunc nb_remainder;
    binaryfunc nb_divmod;
    ternaryfunc nb_power;
    unaryfunc nb_negative;
    unaryfunc nb_positive;
    unaryfunc nb_absolute;
    inquiry nb_bool;
    unaryfunc nb_invert;
    binaryfunc nb_lshift;
    binaryfunc nb_rshift;
    binaryfunc nb_and;
    binaryfunc nb_xor;
    binaryfunc nb_or;
    unaryfunc nb_int;
    void *nb_reserved;  /* the slot formerly known as nb_long */
    unaryfunc nb_float;

    binaryfunc nb_inplace_add;
    binaryfunc nb_inplace_subtract;
    binaryfunc nb_inplace_multiply;
    binaryfunc nb_inplace_remainder;
    ternaryfunc nb_inplace_power;
    binaryfunc nb_inplace_lshift;
    binaryfunc nb_inplace_rshift;
    binaryfunc nb_inplace_and;
    binaryfunc nb_inplace_xor;
    binaryfunc nb_inplace_or;

    binaryfunc nb_floor_divide;
    binaryfunc nb_true_divide;
    binaryfunc nb_inplace_floor_divide;
    binaryfunc nb_inplace_true_divide;

    unaryfunc nb_index;

    binaryfunc nb_matrix_multiply;
    binaryfunc nb_inplace_matrix_multiply;
} PyNumberMethods;
typedef Py_ssize_t (*lenfunc)(PyObject *);
typedef PyObject *(*ssizeargfunc)(PyObject *, Py_ssize_t);
typedef PyObject *(*ssizessizeargfunc)(PyObject *, Py_ssize_t, Py_ssize_t);
typedef int(*ssizeobjargproc)(PyObject *, Py_ssize_t, PyObject *);
typedef int(*ssizessizeobjargproc)(PyObject *, Py_ssize_t, Py_ssize_t, PyObject *);
typedef int(*objobjargproc)(PyObject *, PyObject *, PyObject *);
typedef int (*objobjproc)(PyObject *, PyObject *);
typedef struct {
    lenfunc sq_length;
    binaryfunc sq_concat;
    ssizeargfunc sq_repeat;
    ssizeargfunc sq_item;
    void *was_sq_slice;
    ssizeobjargproc sq_ass_item;
    void *was_sq_ass_slice;
    objobjproc sq_contains;

    binaryfunc sq_inplace_concat;
    ssizeargfunc sq_inplace_repeat;
} PySequenceMethods;

typedef struct {
    lenfunc mp_length;
    binaryfunc mp_subscript;
    objobjargproc mp_ass_subscript;
} PyMappingMethods;

typedef enum {
    PYGEN_RETURN = 0,
    PYGEN_ERROR = -1,
    PYGEN_NEXT = 1,
} PySendResult;

typedef PySendResult (*sendfunc)(PyObject *iter, PyObject *value, PyObject **result);

typedef struct {
    unaryfunc am_await;
    unaryfunc am_aiter;
    unaryfunc am_anext;
    sendfunc am_send;
} PyAsyncMethods;

typedef struct bufferinfo {
    void *buf;
    PyObject *obj;        /* owned reference */
    Py_ssize_t len;
    Py_ssize_t itemsize;  /* This is Py_ssize_t so it can be
                             pointed to by strides in simple case.*/
    int readonly;
    int ndim;
    char *format;
    Py_ssize_t *shape;
    Py_ssize_t *strides;
    Py_ssize_t *suboffsets;
    void *internal;
} Py_buffer;

typedef int (*getbufferproc)(PyObject *, Py_buffer *, int);
typedef void (*releasebufferproc)(PyObject *, Py_buffer *);
typedef struct {
     getbufferproc bf_getbuffer;
     releasebufferproc bf_releasebuffer;
} PyBufferProcs;

typedef struct PyMethodDef PyMethodDef;
typedef int (*visitproc)(PyObject *, void *);
typedef int (*traverseproc)(PyObject *, visitproc, void *);
typedef void (*freefunc)(void *);
typedef void (*destructor)(PyObject *);
typedef PyObject *(*getattrfunc)(PyObject *, char *);
typedef PyObject *(*getattrofunc)(PyObject *, PyObject *);
typedef int (*setattrfunc)(PyObject *, char *, PyObject *);
typedef int (*setattrofunc)(PyObject *, PyObject *, PyObject *);
typedef PyObject *(*reprfunc)(PyObject *);
typedef Py_hash_t (*hashfunc)(PyObject *);
typedef PyObject *(*richcmpfunc) (PyObject *, PyObject *, int);
typedef PyObject *(*getiterfunc) (PyObject *);
typedef PyObject *(*iternextfunc) (PyObject *);
typedef PyObject *(*descrgetfunc) (PyObject *, PyObject *, PyObject *);
typedef int (*descrsetfunc) (PyObject *, PyObject *, PyObject *);
typedef int (*initproc)(PyObject *, PyObject *, PyObject *);
typedef PyObject *(*newfunc)(PyTypeObject *, PyObject *, PyObject *);
typedef PyObject *(*allocfunc)(PyTypeObject *, Py_ssize_t);
typedef PyObject *(*vectorcallfunc)(PyObject *callable, PyObject *const *args,
                                    size_t nargsf, PyObject *kwnames);


struct _typeobject {
    PyObject_VAR_HEAD
    const char *tp_name; /* For printing, in format "<module>.<name>" */
    Py_ssize_t tp_basicsize, tp_itemsize; /* For allocation */

    /* Methods to implement standard operations */

    destructor tp_dealloc;
    Py_ssize_t tp_vectorcall_offset;
    getattrfunc tp_getattr;
    setattrfunc tp_setattr;
    PyAsyncMethods *tp_as_async; /* formerly known as tp_compare (Python 2)
                                    or tp_reserved (Python 3) */
    reprfunc tp_repr;

    /* Method suites for standard classes */

    PyNumberMethods *tp_as_number;
    PySequenceMethods *tp_as_sequence;
    PyMappingMethods *tp_as_mapping;

    /* More standard operations (here for binary compatibility) */

    hashfunc tp_hash;
    ternaryfunc tp_call;
    reprfunc tp_str;
    getattrofunc tp_getattro;
    setattrofunc tp_setattro;

    /* Functions to access object as input/output buffer */
    PyBufferProcs *tp_as_buffer;

    /* Flags to define presence of optional/expanded features */
    unsigned long tp_flags;

    const char *tp_doc; /* Documentation string */

    /* Assigned meaning in release 2.0 */
    /* call function for all accessible objects */
    traverseproc tp_traverse;

    /* delete references to contained objects */
    inquiry tp_clear;

    /* Assigned meaning in release 2.1 */
    /* rich comparisons */
    richcmpfunc tp_richcompare;

    /* weak reference enabler */
    Py_ssize_t tp_weaklistoffset;

    /* Iterators */
    getiterfunc tp_iter;
    iternextfunc tp_iternext;

    /* Attribute descriptor and subclassing stuff */
    struct PyMethodDef *tp_methods;
    struct PyMemberDef *tp_members;
    struct PyGetSetDef *tp_getset;
    // Strong reference on a heap type, borrowed reference on a static type
    struct _typeobject *tp_base;
    PyObject *tp_dict;
    descrgetfunc tp_descr_get;
    descrsetfunc tp_descr_set;
    Py_ssize_t tp_dictoffset;
    initproc tp_init;
    allocfunc tp_alloc;
    newfunc tp_new;
    freefunc tp_free; /* Low-level free-memory routine */
    inquiry tp_is_gc; /* For PyObject_IS_GC */
    PyObject *tp_bases;
    PyObject *tp_mro; /* method resolution order */
    PyObject *tp_cache;
    PyObject *tp_subclasses;
    PyObject *tp_weaklist;
    destructor tp_del;

    /* Type attribute cache version tag. Added in version 2.6 */
    unsigned int tp_version_tag;

    destructor tp_finalize;
    vectorcallfunc tp_vectorcall;
};








#if defined(Py_ENABLE_SHARED) || defined(__CYGWIN__)
#       if defined(HAVE_DECLSPEC_DLL)
#               if defined(Py_BUILD_CORE) && !defined(Py_BUILD_CORE_MODULE)
#                       define PyAPI_FUNC(RTYPE) Py_EXPORTED_SYMBOL RTYPE
#                       define PyAPI_DATA(RTYPE) extern Py_EXPORTED_SYMBOL RTYPE
        /* module init functions inside the core need no external linkage */
        /* except for Cygwin to handle embedding */
#                       if defined(__CYGWIN__)
#                               define PyMODINIT_FUNC Py_EXPORTED_SYMBOL PyObject*
#                       else /* __CYGWIN__ */
#                               define PyMODINIT_FUNC PyObject*
#                       endif /* __CYGWIN__ */
#               else /* Py_BUILD_CORE */
        /* Building an extension module, or an embedded situation */
        /* public Python functions and data are imported */
        /* Under Cygwin, auto-import functions to prevent compilation */
        /* failures similar to those described at the bottom of 4.1: */
        /* http://docs.python.org/extending/windows.html#a-cookbook-approach */
#                       if !defined(__CYGWIN__)
#                               define PyAPI_FUNC(RTYPE) Py_IMPORTED_SYMBOL RTYPE
#                       endif /* !__CYGWIN__ */
#                       define PyAPI_DATA(RTYPE) extern Py_IMPORTED_SYMBOL RTYPE
        /* module init functions outside the core must be exported */
#                       if defined(__cplusplus)
#                               define PyMODINIT_FUNC extern "C" Py_EXPORTED_SYMBOL PyObject*
#                       else /* __cplusplus */
#                               define PyMODINIT_FUNC Py_EXPORTED_SYMBOL PyObject*
#                       endif /* __cplusplus */
#               endif /* Py_BUILD_CORE */
#       endif /* HAVE_DECLSPEC_DLL */
#endif /* Py_ENABLE_SHARED */

/* If no external linkage macros defined by now, create defaults */
#ifndef PyAPI_FUNC
#       define PyAPI_FUNC(RTYPE) Py_EXPORTED_SYMBOL RTYPE
#endif
#ifndef PyAPI_DATA
#       define PyAPI_DATA(RTYPE) extern Py_EXPORTED_SYMBOL RTYPE
#endif
#ifndef PyMODINIT_FUNC
#       if defined(__cplusplus)
#               define PyMODINIT_FUNC extern "C" Py_EXPORTED_SYMBOL PyObject*
#       else /* __cplusplus */
#               define PyMODINIT_FUNC Py_EXPORTED_SYMBOL PyObject*
#       endif /* __cplusplus */
#endif

PyAPI_DATA(PyObject) _Py_NoneStruct; /* Don't use this directly */
#define Py_None (&_Py_NoneStruct)
#define _PyObject_CAST(op) ((PyObject*)(op))
PyAPI_FUNC(int) PyArg_ParseTuple(PyObject *, const char *, ...);
PyAPI_FUNC(void) _Py_Dealloc(PyObject *);
PyAPI_FUNC(int) PyObject_SetAttr(PyObject *, PyObject *, PyObject *);
PyAPI_FUNC(void) PyErr_SetString(
    PyObject *exception,
    const char *string   /* decoded from utf-8 */
    );
PyAPI_FUNC(int) PyObject_SetAttrString(PyObject *, const char *, PyObject *);
static inline void _Py_DECREF(
#if defined(Py_REF_DEBUG) && !(defined(Py_LIMITED_API) && Py_LIMITED_API+0 >= 0x030A0000)
    const char *filename, int lineno,
#endif
    PyObject *op)
{
#if defined(Py_REF_DEBUG) && defined(Py_LIMITED_API) && Py_LIMITED_API+0 >= 0x030A0000
    // Stable ABI for Python 3.10 built in debug mode.
    _Py_DecRef(op);
#else
    // Non-limited C API and limited C API for Python 3.9 and older access
    // directly PyObject.ob_refcnt.
#ifdef Py_REF_DEBUG
    _Py_RefTotal--;
#endif
    if (--op->ob_refcnt != 0) {
#ifdef Py_REF_DEBUG
        if (op->ob_refcnt < 0) {
            _Py_NegativeRefcount(filename, lineno, op);
        }
#endif
    }
    else {
        _Py_Dealloc(op);
    }
#endif
}
#if defined(Py_REF_DEBUG) && !(defined(Py_LIMITED_API) && Py_LIMITED_API+0 >= 0x030A0000)
#  define Py_DECREF(op) _Py_DECREF(__FILE__, __LINE__, _PyObject_CAST(op))
#else
#  define Py_DECREF(op) _Py_DECREF(_PyObject_CAST(op))
#endif

static inline void _Py_XDECREF(PyObject *op)
{
    if (op != NULL) {
        Py_DECREF(op);
    }
}
PyAPI_FUNC(int) PyType_IsSubtype(PyTypeObject *, PyTypeObject *);
#define Py_XDECREF(op) _Py_XDECREF(_PyObject_CAST(op))
#define Py_TYPE(ob)             (_PyObject_CAST(ob)->ob_type)
static inline int _Py_IS_TYPE(const PyObject *ob, const PyTypeObject *type) {
    return ob->ob_type == type;
}
#define _PyObject_CAST_CONST(op) ((const PyObject*)(op))
#define Py_IS_TYPE(ob, type) _Py_IS_TYPE(_PyObject_CAST_CONST(ob), type)
static inline int _PyObject_TypeCheck(PyObject *ob, PyTypeObject *type) {
    return Py_IS_TYPE(ob, type) || PyType_IsSubtype(Py_TYPE(ob), type);
}
#define PyObject_TypeCheck(ob, type) _PyObject_TypeCheck(_PyObject_CAST(ob), type)
#define PyByteArray_Check(self) PyObject_TypeCheck(self, &PyByteArray_Type)
PyAPI_DATA(PyTypeObject) PyByteArray_Type;
PyAPI_FUNC(Py_ssize_t) PyByteArray_Size(PyObject *);
PyAPI_FUNC(char *) PyByteArray_AsString(PyObject *);
PyAPI_FUNC(int) PyByteArray_Resize(PyObject *, Py_ssize_t);
PyAPI_FUNC(PyObject *) PyList_New(Py_ssize_t size);
PyAPI_FUNC(int) PyList_Append(PyObject *, PyObject *);
static inline void _Py_INCREF(PyObject *op)
{
#if defined(Py_REF_DEBUG) && defined(Py_LIMITED_API) && Py_LIMITED_API+0 >= 0x030A0000
    _Py_IncRef(op);
#else
#ifdef Py_REF_DEBUG
    _Py_RefTotal++;
#endif
    op->ob_refcnt++;
#endif
}
#define Py_INCREF(op) _Py_INCREF(_PyObject_CAST(op))

/*
    Native mavlink glue for python.
    Author: kevinh@geeksville.com
*/

#undef NDEBUG

//#include <Python.h>
//#include <structmember.h>



#if PY_MAJOR_VERSION >= 3
// In python3 it only has longs, not 32 bit ints
#define PyInt_AsLong PyLong_AsLong
#define PyInt_FromLong PyLong_FromLong

// We returns strings for byte arreays in python2, but bytes objects in python3
#define PyByteString_FromString PyBytes_FromString
#define PyByteString_FromStringAndSize PyBytes_FromStringAndSize
#define PyByteString_ConcatAndDel PyBytes_ConcatAndDel
#else
#define PyByteString_FromString PyString_FromString
#define PyByteString_FromStringAndSize PyString_FromStringAndSize
#define PyByteString_ConcatAndDel PyString_ConcatAndDel
#endif

#include "mavlink_defaults.h"

#define MAVLINK_USE_CONVENIENCE_FUNCTIONS

#include <mavlink_types.h>

// Mavlink send support
// Not currently used, but keeps mavlink_helpers send code happy
static mavlink_system_t mavlink_system = {42,11,};
static void comm_send_ch(mavlink_channel_t chan, uint8_t c) {
    // Sending not supported yet in native code
    assert(0);
}

#define MAVLINK_ASSERT(x) assert(x)

// static mavlink_message_t last_msg;

/*
  default message crc function. You can override this per-system to
  put this data in a different memory segment
*/
#define MAVLINK_MESSAGE_CRC(msgid) py_message_info[msgid].crc_extra

/* Enable this option to check the length of each message.
 This allows invalid messages to be caught much sooner. Use if the transmission
 medium is prone to missing (or extra) characters (e.g. a radio that fades in
 and out). Only use if the channel will only contain messages types listed in
 the headers.
*/

#define MAVLINK_MESSAGE_LENGTH(msgid) py_message_info[msgid].len

// #include <mavlink.h>

#define TRUE 1
#define FALSE 0

typedef struct {
        PyObject                *name;               // name of this field
        mavlink_message_type_t  type;                // type of this field
        unsigned int            array_length;        // if non-zero, field is an array
        unsigned int            wire_offset;         // offset of each field in the payload
} py_field_info_t;

// note that in this structure the order of fields is the order
// in the XML file, not necessary the wire order
typedef struct {
    PyObject            *id;                                          // The int id for this msg
    PyObject            *name;                                        // name of the message
    unsigned            len;                                          // the raw message length of this message - not including headers & CRC
    uint8_t             crc_extra;                                    // the CRC extra for this message
    unsigned            num_fields;                                   // how many fields in this message
    PyObject            *fieldnames;                                  // fieldnames in the correct order expected by user (not wire order)
    py_field_info_t     fields[MAVLINK_MAX_FIELDS];                   // field information
} py_message_info_t;

static py_message_info_t py_message_info[256];
static uint8_t           info_inited = FALSE; // We only do the init once (assuming only one dialect in use)

#include <protocol.h>

/**
 * Contains a structure mavlink_message but also the raw bytes that make up that message
 */
typedef struct {
    mavlink_message_t   msg;
    int                 numBytes;
    uint8_t             bytes[MAVLINK_MAX_PACKET_LEN];
} py_message_t;

typedef struct {
    PyObject_HEAD

    PyObject            *MAVLinkMessage;
    mavlink_status_t    mav_status;
    py_message_t        msg;
} NativeConnection;

// #define MAVNATIVE_DEBUG
#ifdef MAVNATIVE_DEBUG
#  define mavdebug    printf
#else
#  define mavdebug(x...)
#endif


// My exception type
static PyObject *MAVNativeError;

static jmp_buf python_entry;

#define PYTHON_ENTRY if(!setjmp(python_entry)) {
#define PYTHON_EXIT  } else { return NULL; }   // Used for routines thar return ptrs
#define PYTHON_EXIT_INT  } else { return -1; } // Used for routines that return ints


/** (originally from mavlink_helpers.h - but now customized to not be channel based)
 * This is a convenience function which handles the complete MAVLink parsing.
 * the function will parse one byte at a time and return the complete packet once
 * it could be successfully decoded. Checksum and other failures will be silently
 * ignored.
 *
 * Messages are parsed into an internal buffer (one for each channel). When a complete
 * message is received it is copies into *returnMsg and the channel's status is
 * copied into *returnStats.
 *
 * @param c        The char to parse
 *
 * @param returnMsg NULL if no message could be decoded, the message data else
 * @param returnStats if a message was decoded, this is filled with the channel's stats
 * @return 0 if no message could be decoded, 1 else
 *
 */
MAVLINK_HELPER uint8_t py_mavlink_parse_char(uint8_t c, py_message_t* pymsg, mavlink_status_t* status)
{
    mavlink_message_t *rxmsg = &pymsg->msg;

    int bufferIndex = 0;

    status->msg_received = 0;

    switch (status->parse_state)
    {
    case MAVLINK_PARSE_STATE_UNINIT:
    case MAVLINK_PARSE_STATE_IDLE:
        if (c == MAVLINK_STX)
        {
            status->parse_state = MAVLINK_PARSE_STATE_GOT_STX;
            rxmsg->len = 0;
            pymsg->numBytes = 0;
            rxmsg->magic = c;
            mavlink_start_checksum(rxmsg);
            pymsg->bytes[pymsg->numBytes++] = c;
        }
        break;

    case MAVLINK_PARSE_STATE_GOT_STX:
            if (status->msg_received 
/* Support shorter buffers than the
   default maximum packet size */
#if (MAVLINK_MAX_PAYLOAD_LEN < 255)
                || c > MAVLINK_MAX_PAYLOAD_LEN
#endif
                )
        {
            status->buffer_overrun++;
            status->parse_error++;
            status->msg_received = 0;
            status->parse_state = MAVLINK_PARSE_STATE_IDLE;
        }
        else
        {
            // NOT counting STX, LENGTH, SEQ, SYSID, COMPID, MSGID, CRC1 and CRC2
            rxmsg->len = c;
            status->packet_idx = 0;
            mavlink_update_checksum(rxmsg, c);
            pymsg->bytes[pymsg->numBytes++] = c;
            status->parse_state = MAVLINK_PARSE_STATE_GOT_LENGTH;
        }
        break;

    case MAVLINK_PARSE_STATE_GOT_LENGTH:
        rxmsg->seq = c;
        mavlink_update_checksum(rxmsg, c);
        pymsg->bytes[pymsg->numBytes++] = c;
        status->parse_state = MAVLINK_PARSE_STATE_GOT_SEQ;
        break;

    case MAVLINK_PARSE_STATE_GOT_SEQ:
        rxmsg->sysid = c;
        mavlink_update_checksum(rxmsg, c);
        pymsg->bytes[pymsg->numBytes++] = c;
        status->parse_state = MAVLINK_PARSE_STATE_GOT_SYSID;
        break;

    case MAVLINK_PARSE_STATE_GOT_SYSID:
        rxmsg->compid = c;
        mavlink_update_checksum(rxmsg, c);
        pymsg->bytes[pymsg->numBytes++] = c;
        status->parse_state = MAVLINK_PARSE_STATE_GOT_COMPID;
        break;

    case MAVLINK_PARSE_STATE_GOT_COMPID:
#ifdef MAVLINK_CHECK_MESSAGE_LENGTH
            if (rxmsg->len != MAVLINK_MESSAGE_LENGTH(c))
        {
            status->parse_error++;
            status->parse_state = MAVLINK_PARSE_STATE_IDLE;
            break;
        }
#endif
        rxmsg->msgid = c;
        mavlink_update_checksum(rxmsg, c);
        pymsg->bytes[pymsg->numBytes++] = c;
        if (rxmsg->len == 0)
        {
            status->parse_state = MAVLINK_PARSE_STATE_GOT_PAYLOAD;
        }
        else
        {
            status->parse_state = MAVLINK_PARSE_STATE_GOT_MSGID;
        }
        break;

    case MAVLINK_PARSE_STATE_GOT_MSGID:
        _MAV_PAYLOAD_NON_CONST(rxmsg)[status->packet_idx++] = (char)c;
        mavlink_update_checksum(rxmsg, c);
        pymsg->bytes[pymsg->numBytes++] = c;
        if (status->packet_idx == rxmsg->len)
        {
            status->parse_state = MAVLINK_PARSE_STATE_GOT_PAYLOAD;
        }
        break;

    case MAVLINK_PARSE_STATE_GOT_PAYLOAD:
#if MAVLINK_CRC_EXTRA
        mavlink_update_checksum(rxmsg, MAVLINK_MESSAGE_CRC(rxmsg->msgid));
#endif
        pymsg->bytes[pymsg->numBytes++] = c;
        if (c != (rxmsg->checksum & 0xFF)) {
            // Check first checksum byte
            status->parse_error++;
            status->msg_received = 0;
            status->parse_state = MAVLINK_PARSE_STATE_IDLE;
            if (c == MAVLINK_STX)
            {
                status->parse_state = MAVLINK_PARSE_STATE_GOT_STX;
                rxmsg->len = 0;
                mavlink_start_checksum(rxmsg);
            }
        }
        else
        {
            status->parse_state = MAVLINK_PARSE_STATE_GOT_CRC1;
            _MAV_PAYLOAD_NON_CONST(rxmsg)[status->packet_idx] = (char)c;
        }
        break;

    case MAVLINK_PARSE_STATE_GOT_CRC1:
        pymsg->bytes[pymsg->numBytes++] = c;
        if (c != (rxmsg->checksum >> 8)) {
            // Check second checksum byte
            status->parse_error++;
            status->msg_received = 0;
            status->parse_state = MAVLINK_PARSE_STATE_IDLE;
            if (c == MAVLINK_STX)
            {
                status->parse_state = MAVLINK_PARSE_STATE_GOT_STX;
                rxmsg->len = 0;
                mavlink_start_checksum(rxmsg);
            }
        }
        else
        {
            // Successfully got message
            status->msg_received = 1;
            status->parse_state = MAVLINK_PARSE_STATE_IDLE;
            _MAV_PAYLOAD_NON_CONST(rxmsg)[status->packet_idx+1] = (char)c;
        }
        break;

    case MAVLINK_PARSE_STATE_GOT_BAD_CRC1:  // not used, just to fix compiler warning
        break;
    }

    bufferIndex++;
    // If a message has been successfully decoded, check index
    if (status->msg_received == 1)
    {
        //while(status->current_seq != rxmsg->seq)
        //{
        //  status->packet_rx_drop_count++;
        //               status->current_seq++;
        //}
        status->current_rx_seq = rxmsg->seq;
        // Initial condition: If no packet has been received so far, drop count is undefined
        if (status->packet_rx_success_count == 0) status->packet_rx_drop_count = 0;
        // Count this packet as received
        status->packet_rx_success_count++;
    }

    return status->msg_received;
}


// Raise a python exception
static void set_pyerror(const char *msg) {
    PyErr_SetString(MAVNativeError,  msg);
}

// Pass assertion failures back to python (if we can)
extern void __assert_fail(const char *__assertion, const char *__file, unsigned int __line, const char *__function)
{
    char msg[256];

    sprintf(msg, "Assertion failed: %s, %s:%d", __assertion, __file, __line);

    set_pyerror(msg);
    longjmp(python_entry, 1);
}


static unsigned get_field_size(int field_type) {
    unsigned fieldSize;

    switch(field_type) 
    {
    case MAVLINK_TYPE_CHAR: 
        fieldSize = 1;
        break;
    case MAVLINK_TYPE_UINT8_T:
        fieldSize = 1;
        break;
    case MAVLINK_TYPE_INT8_T:
        fieldSize = 1;
        break;
    case MAVLINK_TYPE_UINT16_T:
        fieldSize = 2;
        break;
    case MAVLINK_TYPE_INT16_T:
        fieldSize = 2;
        break;
    case MAVLINK_TYPE_UINT32_T:
        fieldSize = 4;
        break;
    case MAVLINK_TYPE_INT32_T:
        fieldSize = 4;
        break;
    case MAVLINK_TYPE_UINT64_T:
        fieldSize = 8;
        break;
    case MAVLINK_TYPE_INT64_T:                 
        fieldSize = 8;
        break;
    case MAVLINK_TYPE_FLOAT:              
        fieldSize = 4;
        break;
    case MAVLINK_TYPE_DOUBLE:      
        fieldSize = 8;
        break;            
    default:
        mavdebug("BAD MAV TYPE %d\n", field_type);
        set_pyerror("Unexpected mavlink type");
        fieldSize = 1;
    }

    return fieldSize;
}


/**
 * Given a python type character & array_size advance the wire_offset to the correct next value.

 * @return the equivalent C++ type code.
 */
static int get_py_typeinfo(char type_char, int array_size, unsigned *wire_offset)
{
    int type_code;

    switch(type_char) 
    {
    case 'f': type_code = MAVLINK_TYPE_FLOAT; break;
    case 'd': type_code = MAVLINK_TYPE_DOUBLE; break;
    case 'c': type_code = MAVLINK_TYPE_CHAR; break;
    case 'v': type_code = MAVLINK_TYPE_UINT8_T; break;
    case 'b': type_code = MAVLINK_TYPE_INT8_T; break;
    case 'B': type_code = MAVLINK_TYPE_UINT8_T; break;
    case 'h': type_code = MAVLINK_TYPE_INT16_T; break;
    case 'H': type_code = MAVLINK_TYPE_UINT16_T; break;
    case 'i': type_code = MAVLINK_TYPE_INT32_T; break;
    case 'I': type_code = MAVLINK_TYPE_UINT32_T; break;
    case 'q': type_code = MAVLINK_TYPE_INT64_T; break;
    case 'Q': type_code = MAVLINK_TYPE_UINT64_T; break;
    default:
        assert(0);
    }

    int total_len = get_field_size(type_code) * (array_size == 0 ? 1 : array_size);

    *wire_offset += total_len;

    return type_code;
}

/**
    We preconvert message info from the C style representation to python objects (to minimize # of object allocs).

    FIXME - we really should free these PyObjects if our module gets unloaded.

    @param mavlink_map - the mavlink_map object from python a dict from an int msgid -> tuple(fmt, type_class, order_list, len_list, crc_extra)
*/
static void init_message_info(PyObject *mavlink_map) {
    // static const mavlink_message_info_t src[256] = MAVLINK_MESSAGE_INFO;
    
    PyObject *items_list = PyDict_Values(mavlink_map);
    assert(items_list); // A list of the tuples in mavlink_map

    Py_ssize_t numMsgs = PyList_Size(items_list);

    int i;
    for(i = 0; i < numMsgs; i++) {
        PyObject *type_class = PyList_GetItem(items_list, i); // returns a _borrowed_ reference
        assert(type_class);

        PyObject *id_obj = PyObject_GetAttrString(type_class, "id"); // A _new_ reference
        assert(id_obj);
        PyObject *name_obj = PyObject_GetAttrString(type_class, "name"); // A new reference
        assert(name_obj);
        PyObject *crc_extra_obj = PyObject_GetAttrString(type_class, "crc_extra"); // A new reference
        assert(crc_extra_obj);
        PyObject *fieldname_list = PyObject_GetAttrString(type_class, "ordered_fieldnames"); // A new reference
        assert(fieldname_list);
        //PyObject *order_list = PyObject_GetAttrString(type_class, "orders"); // A new reference
        //assert(order_list);
        PyObject *arrlen_list = PyObject_GetAttrString(type_class, "array_lengths"); // A new reference
        assert(arrlen_list);
        PyObject *type_format = PyObject_GetAttrString(type_class, "native_format"); // A new reference
        assert(type_format);
        char *type_str = PyByteArray_AsString(type_format);
        assert(type_str);
               
        Py_ssize_t num_fields = PyList_Size(fieldname_list);

        uint8_t id = (uint8_t) PyInt_AsLong(id_obj);
        py_message_info_t *d = &py_message_info[id];

        d->id = id_obj;
        d->name = name_obj;
        d->num_fields = num_fields;
        d->crc_extra = PyInt_AsLong(crc_extra_obj);
        d->fieldnames = PyObject_GetAttrString(type_class, "fieldnames"); // A new reference
        assert(d->fieldnames);

        int fnum;
        unsigned wire_offset = 0;
        for(fnum = 0; fnum < num_fields; fnum++) {
            PyObject *field_name_obj = PyList_GetItem(fieldname_list, fnum); // returns a _borrowed_ reference
            assert(field_name_obj);
            Py_INCREF(field_name_obj);

            PyObject *len_obj = PyList_GetItem(arrlen_list, fnum); // returns a _borrowed_ reference
            assert(len_obj);                        

            d->fields[fnum].name = field_name_obj;
            d->fields[fnum].array_length = PyInt_AsLong(len_obj);
            char type_char = type_str[1 + fnum];
            d->fields[fnum].wire_offset = wire_offset; // Store the current offset before advancing
            d->fields[fnum].type = get_py_typeinfo(type_char, d->fields[fnum].array_length, &wire_offset);            
        }
        d->len = wire_offset;

        Py_DECREF(crc_extra_obj);
        Py_DECREF(arrlen_list);
        Py_DECREF(type_format);
        //Py_DECREF(order_list);
    }

    Py_DECREF(items_list);
}

static PyObject *createPyNone(void)
{
    Py_INCREF(Py_None);
    return Py_None;
}



/**
    Set an attribute, but handing over ownership on the value
*/
static void set_attribute(PyObject *obj, const char *attrName, PyObject *val) {
    assert(val);
    PyObject_SetAttrString(obj, attrName, val);
    Py_DECREF(val);
}



/**
    Extract a field value from a mavlink msg

    @return possibly null if mavlink stream is corrupted (FIXME, caller should check)
*/
static PyObject *pyextract_mavlink(const mavlink_message_t *msg, const py_field_info_t *field) {
    unsigned offset = field->wire_offset;
    int index;

    // For arrays of chars we build the result in a string instead of an array
    PyObject *arrayResult =  (field->array_length != 0 && field->type != MAVLINK_TYPE_CHAR) ? PyList_New(field->array_length) : NULL;
    PyObject *stringResult = (field->array_length != 0 && field->type == MAVLINK_TYPE_CHAR) ? PyByteString_FromString("") : NULL;
    PyObject *result = NULL;

    int numValues = (field->array_length == 0) ? 1 : field->array_length;
    unsigned fieldSize = get_field_size(field->type);

    uint8_t string_ended = FALSE;

    if(arrayResult != NULL)
        result = arrayResult;

    // Either build a full array of results, or return a single value 
    for(index = 0; index < numValues; index++) {
        PyObject *val = NULL;

        switch(field->type) {
            case MAVLINK_TYPE_CHAR: {
                // If we are building a string, stop writing when we see a null char
                char c = _MAV_RETURN_char(msg, offset);

                if(stringResult && c == 0) 
                    string_ended = TRUE;

                val = PyByteString_FromStringAndSize(&c, 1);
                break;
                }
            case MAVLINK_TYPE_UINT8_T:
                val = PyInt_FromLong(_MAV_RETURN_uint8_t(msg, offset));
                break;
            case MAVLINK_TYPE_INT8_T:
                val = PyInt_FromLong(_MAV_RETURN_int8_t(msg, offset));
                break;
            case MAVLINK_TYPE_UINT16_T:
                val = PyInt_FromLong(_MAV_RETURN_uint16_t(msg, offset));
                break;
            case MAVLINK_TYPE_INT16_T:
                val = PyInt_FromLong(_MAV_RETURN_int16_t(msg, offset));
                break;
            case MAVLINK_TYPE_UINT32_T:
                val = PyLong_FromLong(_MAV_RETURN_uint32_t(msg, offset));
                break;
            case MAVLINK_TYPE_INT32_T:
                val = PyInt_FromLong(_MAV_RETURN_int32_t(msg, offset));   
                break;
            case MAVLINK_TYPE_UINT64_T:
                val = PyLong_FromLongLong(_MAV_RETURN_uint64_t(msg, offset));
                break;
            case MAVLINK_TYPE_INT64_T:
                val = PyLong_FromLongLong(_MAV_RETURN_int64_t(msg, offset));
                break;
            case MAVLINK_TYPE_FLOAT:
                val = PyFloat_FromDouble(_MAV_RETURN_float(msg, offset));
                break;
            case MAVLINK_TYPE_DOUBLE:
                val = PyFloat_FromDouble(_MAV_RETURN_double(msg, offset));
                break;            
            default:
                mavdebug("BAD MAV TYPE %d\n", field->type);
                set_pyerror("Unexpected mavlink type");
                return NULL;
        }
        offset += fieldSize;

        assert(val);
        if(arrayResult != NULL)  
            PyList_SetItem(arrayResult, index, val);
        else if(stringResult != NULL) {
            if(!string_ended)
                PyByteString_ConcatAndDel(&stringResult, val);
            else
                Py_DECREF(val); // We didn't use this char

            result = stringResult;
        }
        else // Not building an array
            result = val;
    }

    assert(result);
    return result;
}


/**
    Convert a message to a valid python representation.

    @return new message, or null if a valid encoding could not be made

    FIXME - move msgclass, the mavstatus and channel context into an instance, created once with the mavfile object
    */
static PyObject *msg_to_py(PyObject* msgclass, const py_message_t *pymsg) {
    const mavlink_message_t *msg = &pymsg->msg;
    const py_message_info_t *info = &py_message_info[msg->msgid];

    mavdebug("Found a msg: %s\n", PyString_AS_STRING(info->name));

    /* Call the class object to create our instance */
    // PyObject *obj = PyObject_CallObject((PyObject *) &NativeConnectionType, null);
    PyObject *argList = PyTuple_Pack(2, info->id, info->name);
    PyObject *obj = PyObject_CallObject(msgclass, argList);
    uint8_t objValid = TRUE;
    assert(obj);
    Py_DECREF(argList);

    // Find the header subobject
    PyObject *header = PyObject_GetAttrString(obj, "_header");
    assert(header);

    // msgid already set in the constructor call
    set_attribute(header, "mlen", PyInt_FromLong(msg->len));
    set_attribute(header, "seq", PyInt_FromLong(msg->seq));
    set_attribute(header, "srcSystem", PyInt_FromLong(msg->sysid));
    set_attribute(header, "srcComponent", PyInt_FromLong(msg->compid));
    Py_DECREF(header);
    header = NULL;

    // FIXME - we should generate this expensive field only as needed (via a getattr override)
    set_attribute(obj, "_msgbuf", PyByteArray_FromStringAndSize((const char *) pymsg->bytes, pymsg->numBytes));

    // Now add all the fields - FIXME - do this lazily using getattr overrides
    PyObject_SetAttrString(obj, "_fieldnames", info->fieldnames); // Will increment the reference count

    // FIXME - reuse the fieldnames list from python - so it is in the right order

    int fnum;
    for(fnum = 0; fnum < info->num_fields && objValid; fnum++) {
        const py_field_info_t *f = &info->fields[fnum];
        PyObject *val = pyextract_mavlink(msg, f);

        if(val != NULL) {
            PyObject_SetAttr(obj, f->name, val);
            Py_DECREF(val); // We no longer need val, the attribute will keep a ref
        }
        else 
            objValid = FALSE;
    }

    if(objValid)
        return obj;
    else {
        Py_DECREF(obj);
        return NULL;
    }
}


/**
  * How many bytes would we like to read to complete current packet
  */
static int get_expectedlength(NativeConnection *self)
{
    int desired;

    mavlink_message_t *msg = &self->msg.msg;

    switch(self->mav_status.parse_state) {
        case MAVLINK_PARSE_STATE_UNINIT: 
        case MAVLINK_PARSE_STATE_IDLE: 
            desired = 8;
            break;
        case MAVLINK_PARSE_STATE_GOT_STX: 
            desired = 7;
            break;
        case MAVLINK_PARSE_STATE_GOT_LENGTH:
            desired = msg->len + 6;
            break;
        case MAVLINK_PARSE_STATE_GOT_SEQ: 
            desired = msg->len + 5;
            break;
        case MAVLINK_PARSE_STATE_GOT_SYSID: 
            desired = msg->len + 4;
            break;        
        case MAVLINK_PARSE_STATE_GOT_COMPID:
            desired = msg->len + 3;
            break;         
        case MAVLINK_PARSE_STATE_GOT_MSGID: 
            desired = msg->len - self->mav_status.packet_idx + 2;
            break;          
        case MAVLINK_PARSE_STATE_GOT_PAYLOAD:
            desired = 2;
            break;          
        case MAVLINK_PARSE_STATE_GOT_CRC1: 
            desired = 1;
            break;  
        default:
            // Huh?  Just claim 1
            desired = 1;
            break;
    } 
    
    mavdebug("in state %d, expected_length=%d\n", (int) self->mav_status.parse_state, desired);
    return desired;
}
PyAPI_FUNC(int) PyByteArray_Resize(PyObject *, Py_ssize_t);

static PyObject *NativeConnection_getexpectedlength(NativeConnection *self, void *closure)
{
    return PyInt_FromLong(get_expectedlength(self));
}

/**
  Given a byte array of bytes
  @return a list of MAVProxy_message objects
*/
static PyObject *
py_parse_chars(NativeConnection *self, PyObject *args)
{
    PYTHON_ENTRY

    PyObject* byteObj;
    if (!PyArg_ParseTuple(args, "O", &byteObj)) {
        set_pyerror("Invalid arguments");
        return NULL;
    }
    
    assert(PyByteArray_Check(byteObj));
    Py_ssize_t numBytes = PyByteArray_Size(byteObj);    
    mavdebug("numbytes %u\n", (unsigned) numBytes);

    char *start = PyByteArray_AsString(byteObj);
    assert(start);
    char *bytes = start;
    PyObject *result = NULL;

    // Generate a list of messages found 
    while(numBytes) {
        char c = *bytes++;
        numBytes--;
        get_expectedlength(self); mavdebug("parse 0x%x\n", (unsigned char) c);

        if (py_mavlink_parse_char(c, &self->msg, &self->mav_status)) {
            mavdebug("got packet\n");
            result = msg_to_py(self->MAVLinkMessage, &self->msg);
            if(result != NULL)
                break;
        }
    }

    // We didn't process all bytes provided by the caller, so fixup their array
    memmove(start, bytes, numBytes);
    PyByteArray_Resize(byteObj, numBytes);

    if(result != NULL) 
        return result;
    else
        return createPyNone();

    PYTHON_EXIT
}

/**
  Given an string of bytes.

  This routine is more efficient than parse_chars, because it doesn't need to buffer characters.

  @return a list of MAVProxy_message objects
*/
static PyObject *
py_parse_buffer(NativeConnection *self, PyObject *args)
{
    PYTHON_ENTRY

    mavdebug("Enter py_parse_buffer\n");

    const char *bytes;
    Py_ssize_t numBytes = 0;

    if (!PyArg_ParseTuple(args, "s#", &bytes, &numBytes)) {
        set_pyerror("Invalid arguments");
        return NULL;
    }
    
    // mavdebug("numbytes %u\n", (unsigned) numBytes);

    PyObject* list = PyList_New(0);

    // Generate a list of messages found 
    while(numBytes--) {
        char c = *bytes++;
        // mavdebug("parse %c\n", c);

        if (py_mavlink_parse_char(c, &self->msg, &self->mav_status)) {
            PyObject *obj = msg_to_py(self->MAVLinkMessage, &self->msg);
            if(obj != NULL) {
                PyList_Append(list, obj);
            
                // Append will have bummped up the ref count on obj, so we need to release our count
                Py_DECREF(obj);
            }
        }
    }

    return list;

    PYTHON_EXIT
}

static PyObject *
NativeConnection_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    NativeConnection *self = (NativeConnection *)type->tp_alloc(type, 0);

    mavdebug("new connection\n");
    return (PyObject *)self;
}


static int
NativeConnection_init(NativeConnection *self, PyObject *args, PyObject *kwds)
{
    PYTHON_ENTRY

    mavdebug("Enter init\n");

    memset(&self->mav_status, 0, sizeof(self->mav_status));

    PyObject* msgclass, *mavlink_map;
    if (!PyArg_ParseTuple(args, "OO", &msgclass, &mavlink_map)) {
        set_pyerror("Invalid arguments");
        return -1;
    }

    // keep a ref to our mavlink instance constructor
    assert(msgclass);
    self->MAVLinkMessage = msgclass;
    Py_INCREF(msgclass);

    assert(mavlink_map);
    if(!info_inited) {
        init_message_info(mavlink_map);
        info_inited = TRUE;
    }

    mavdebug("inited connection\n");
    return 0;

    PYTHON_EXIT_INT
}

static void NativeConnection_dealloc(NativeConnection* self)
{
    Py_XDECREF(self->MAVLinkMessage);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyMemberDef NativeConnection_members[] = {
    {NULL}  /* Sentinel */
};

static PyGetSetDef NativeConnection_getseters[] = {
    {"expected_length", 
     (getter)NativeConnection_getexpectedlength, NULL,
     "How many characters would the state-machine like to read now",
     NULL},
    {NULL}  /* Sentinel */
};

static PyMethodDef NativeConnection_methods[] = {
    {"parse_chars",  (PyCFunction) py_parse_chars, METH_VARARGS,
     "Given a msg class and an array of bytes, Parse chars, returning a message or None"},    
    {"parse_buffer",  (PyCFunction) py_parse_buffer, METH_VARARGS,
     "Given a msg class and a string like object, Parse chars, returning a (possibly empty) list of messages"},
    {NULL,  NULL},
};

// FIXME - delete this?
static PyTypeObject NativeConnectionType = {
#if PY_MAJOR_VERSION >= 3
    PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(NULL)
    0,                       /* ob_size */
#endif
    "mavnative.NativeConnection",      /* tp_name */
    sizeof(NativeConnection),          /* tp_basicsize */
    0,                       /* tp_itemsize */
    (destructor)NativeConnection_dealloc,  /* tp_dealloc */
    0,                       /* tp_print */
    0,                       /* tp_getattr */
    0,                       /* tp_setattr */
    0,                       /* tp_compare */
    0,                       /* tp_repr */
    0,                       /* tp_as_number */
    0,                       /* tp_as_sequence */
    0,                       /* tp_as_mapping */
    0,                       /* tp_hash */
    0,                       /* tp_call */
    0,                       /* tp_str */
    0,                       /* tp_getattro */
    0,                       /* tp_setattro */
    0,                       /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
      Py_TPFLAGS_BASETYPE,   /* tp_flags */
    "NativeConnection objects",  /* tp_doc */
    0,                       /* tp_traverse */
    0,                       /* tp_clear */
    0,                       /* tp_richcompare */
    0,                       /* tp_weaklistoffset */
    0,                       /* tp_iter */
    0,                       /* tp_iternext */
    NativeConnection_methods,  /* tp_methods */
    NativeConnection_members,  /* tp_members */
    NativeConnection_getseters,  /* tp_getset */
    0,                       /* tp_base */
    0,                       /* tp_dict */
    0,                       /* tp_descr_get */
    0,                       /* tp_descr_set */
    0,                       /* tp_dictoffset */
    (initproc)NativeConnection_init,   /* tp_init */
    0,                       /* tp_alloc */
    NativeConnection_new,    /* tp_new */
};

#if PY_MAJOR_VERSION >= 3
#define MOD_RETURN(m) return m
#else
#define MOD_RETURN(m) return m
#endif

PyMODINIT_FUNC
#if PY_MAJOR_VERSION >= 3
    PyInit_mavnative(void)
#else
    initmavnative(void)
#endif
{
    if (PyType_Ready(&NativeConnectionType) < 0)
        MOD_RETURN(NULL);

#if PY_MAJOR_VERSION < 3
    static PyMethodDef ModuleMethods[] = {
        {NULL, NULL, 0, NULL}        /* Sentinel */
    };

    PyObject *m = Py_InitModule3("mavnative", ModuleMethods, "Mavnative module");
    if (m == NULL)
        MOD_RETURN(m);
#else
    static PyModuleDef mod_def = {
        PyModuleDef_HEAD_INIT,
        "mavnative",
        "EMavnative module",
        -1,
        NULL, NULL, NULL, NULL, NULL
    };

    PyObject *m = PyModule_Create(&mod_def);
#endif

    MAVNativeError = PyErr_NewException("mavnative.error", NULL, NULL);
    Py_INCREF(MAVNativeError);
    PyModule_AddObject(m, "error", MAVNativeError);

    Py_INCREF(&NativeConnectionType);
    PyModule_AddObject(m, "NativeConnection", (PyObject *) &NativeConnectionType);
    MOD_RETURN(m);
}
