/****************************************************************************
(c) 2004-2019 by GenICam GenTL Subcommittee

License: This file is published under the license of the EMVA GenICam Standard Group.
A text file describing the legal terms is included in your installation as 'license.txt'.
If for some reason you are missing this file please contact the EMVA or visit the website
(http://www.genicam.org) for a full copy.

THIS SOFTWARE IS PROVIDED BY THE EMVA GENICAM STANDARD GROUP "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE EMVA GENICAM STANDARD  GROUP
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT  LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY  THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

******************************************************************************/

/**
 *  \file     GenTL.h
 *  \brief    GenICam Transport Layer Client Interface
 *  \version  1.6
 *  \author   GenTL Subcommittee
 *  \date     2019
 *
 *  \par Version history
 *  \li Version 0.1.0   First official version from the meeting in Pilsen
 *  \li Version 0.1.1   rst (SI) 0.4.160
 *                      - added _E_INTERFACE_INFO_CMD_LIST_T_ enum name
 *                      - added _E_DEVICE_ACCCESS_STATUS_TYPE_T_ to be used
 *                      with the /a IFGetDeviceInfo and DevGetInfo
 *                      - rename of the enum value DEVICE_INFO_ACCESSMODE to
 *                      DEVICE_INFO_ACCESS_STATUS which now refers to
 *                      _E_DEVICE_ACCCESS_STATUS_TYPE_T_
 *                      - added Timeout parameter to update interface list and
 *                      device list.
 *  \li Version 0.1.2   - change datatype of timeout parameter of
 *                      TLUpdateInterfaceList and IFUpdateDeviceList to
 *                      uint64_t to match with the timeout in the event object.
 *                      - changed all enums to have a typedef to uint32_t
 *                      with them to allow custom ids
 *                      - changed type of string constants to be char * instead
 *                      of gcstring
 *  \li Version 0.1.3 rst (SI), cbi (IDS) 0.4.163
 *                      - adjusted parameter names to be closer aligned with
 *                      the standard text
 *                      - changed typedefs for enums from uint32_t to int32_t
 *                      - removed default parameter
 *                      - added parameter name to DevGetPort function
 *  \li Version 0.1.4 jb (LV)
 *                      - fixes to align the file with standard text
 *                      - make the file self-contained, independent on GenApi
 *  \li Version 0.1.5 rst (SI) cbi (IDS) jb (LV) tho (MVTec)
 *                      - Adjust it for Linux
 *                      - Cosmetics
 *  \li Version 1.0   rst (SI) cbi (IDS) jb (LV) tho (MVTec)
 *                      - Adjust for Standard 1.0
 *                      - Make it plain C compliant
 *                      - Cosmetics
 *  \li Version 1.2   rst (SI) jb (LV) tho (MVTec)
 *                      - Adjust for Standard 1.2
 *                      - adjust packing
 *                      - Cosmetics
 *  \li Version 1.3   (Stemmer, Leutron, Matrix Vision, MVTec, MathWorks)
 *                      - Adjust for Standard 1.3
 *                      - added chunk handling
 *                      - added Mac OS X
 *                      - Cosmetics
 *  \li Version 1.3.1 (MathWorks)
 *                      - Spelling corrections in comments
 *  \li Version 1.4    GenTL Subcommittee
 *  \li Version 1.5    GenTL Subcommittee
 *                      - Changed namespace to GenTL
 *                      - Changes for GenTL 1.5, Please refer to the GenTL spec
 *                        for a list of changes.
 *  \li Version 1.6    GenTL Subcommittee
 *                      - Changes for GenTL 1.6, Please refer to the GenTL spec
 *                        for a list of changes.
 */

#ifndef GC_TLI_CLIENT_H_
#define GC_TLI_CLIENT_H_ 1

#ifndef GC_USER_DEFINED_TYPES
/* The types should be the same as defined in GCTypes.h from GenApi. But in
 * case you do not have this header the necessary types are defined here. */
#if defined(_WIN32)
#if defined(_MSC_VER) && _MSC_VER >= 1600 /* VS2010 provides stdint.h */
#include <stdint.h>
#elif !defined _STDINT_H && !defined _STDINT
/* stdint.h is usually not available under Windows */
typedef unsigned char uint8_t;
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#endif
#else
#include <stdint.h>
#endif

#ifdef __cplusplus
typedef bool bool8_t;
#else
typedef uint8_t bool8_t;
#endif
#endif /* GC_DEFINE_TYPES */

#include <stddef.h>

/* Function declaration modifiers */
#if defined(_WIN32)
#define GCTLIDLL
#ifndef GCTLI_NO_DECLSPEC_STATEMENTS
#ifdef GCTLIDLL
#define GC_IMPORT_EXPORT __declspec(dllexport)
#else
#define GC_IMPORT_EXPORT __declspec(dllimport)
#endif
#else
#define GC_IMPORT_EXPORT
#endif /* #  ifndef GCTLI_NO_DECLSPEC_STATEMENTS */
#if defined(_M_IX86) || defined(__i386__)
#define GC_CALLTYPE __stdcall
#else
#define GC_CALLTYPE /* default */
#endif
#ifndef EXTERN_C
#define EXTERN_C extern "C"
#endif

#elif defined(__GNUC__) && (__GNUC__ >= 4) && (defined(__linux__) || defined(__APPLE__))
#define GC_IMPORT_EXPORT __attribute__((visibility("default")))
#if defined(__i386__)
#define GC_CALLTYPE __attribute__((stdcall))
#else
#define GC_CALLTYPE /* default */
#endif
#ifndef EXTERN_C
#define EXTERN_C extern "C"
#endif

#else
#error Unknown platform, file needs adaption
#endif

#ifdef __cplusplus
extern "C"
{
    namespace GenTL
    {
#endif

    /*! Errors List: Return codes*/
    enum GC_ERROR_LIST
    {
        GC_ERR_SUCCESS = 0,                                           /*!< everything is ok */
        GC_ERR_ERROR = -1001,                                         /*!< unknown /unspecified error */
        GC_ERR_NOT_INITIALIZED = -1002,                               /*!< function arg/param is not initialized */
        GC_ERR_NOT_IMPLEMENTED = -1003,                               /*!< function has not been implemented */
        GC_ERR_RESOURCE_IN_USE = -1004,                               /*!< resource in use */
        GC_ERR_ACCESS_DENIED = -1005,                                 /*!< access denied */
        GC_ERR_INVALID_HANDLE = -1006,                                /*!< invalid handle */
        GC_ERR_INVALID_ID = -1007,                                    /*!< invalid id */
        GC_ERR_NO_DATA = -1008,                                       /*!< no data */
        GC_ERR_INVALID_PARAMETER = -1009,                             /*!< invalid parameter */
        GC_ERR_IO = -1010,                                            /*!< error IO */
        GC_ERR_TIMEOUT = -1011,                                       /*!< timeout */
        GC_ERR_ABORT = -1012, /*!< abort */                           /* GenTL v1.1 */
        GC_ERR_INVALID_BUFFER = -1013, /*!< invalid buffer */         /* GenTL v1.1 */
        GC_ERR_NOT_AVAILABLE = -1014, /*!< not avaible */             /* GenTL v1.2 */
        GC_ERR_INVALID_ADDRESS = -1015, /*!< invalid address */       /* GenTL v1.3 */
        GC_ERR_BUFFER_TOO_SMALL = -1016, /*!< buffer is too small */  /* GenTL v1.4 */
        GC_ERR_INVALID_INDEX = -1017, /*!< invalid index */           /* GenTL v1.4 */
        GC_ERR_PARSING_CHUNK_DATA = -1018, /*!< parse chunk data */   /* GenTL v1.4 */
        GC_ERR_INVALID_VALUE = -1019, /*!< invalid value */           /* GenTL v1.4 */
        GC_ERR_RESOURCE_EXHAUSTED = -1020, /*!< resource exhausted */ /* GenTL v1.4 */
        GC_ERR_OUT_OF_MEMORY = -1021, /*!< out of memory */           /* GenTL v1.4 */
        GC_ERR_BUSY = -1022, /*!< busy */                             /* GenTL v1.5 */
        GC_ERR_AMBIGUOUS = -1023, /*!< ambiguous */                   /* GenTL v1.6 */

        GC_ERR_CUSTOM_ID = -10000,
        GC_ERR_FT_OPEN_SDEV_FAILED = -10001, /*!< can not open p1 camera by ftdi */
        GC_ERR_HID_SDEV_EMPTY = -10002,      /*!< can not open c1 camera, has not imu stream */
        GC_ERR_COMMUNICATE_ERROR = -10003,   /*!< can not open c1 camera, get communicate version failed */
        GC_ERR_MF_ACTIVATE_FAILED = -10004,  /*!< can not open c1 camera, can not get source by mf */
        GC_ERR_FT_CONFIGURE_FAILED = -10005, /*!< can not use ftdi */

        // The starting position of the camera error code
        GC_ERR_CAMERA_ALGO_LIST_CHANGED =
            -20126 /*!< the algorithm list has been updated and needs to be retrieved again */
    };
    typedef int32_t GC_ERROR;

#ifndef GC_GENTL_HEADER_VERSION

#define GenTLMajorVersion                                                                                              \
    1 /* defines the major version of the GenICam GenTL standard version this header is based on */
#define GenTLMinorVersion                                                                                              \
    6 /* defines the minor version of the GenICam GenTL standard version this header is based on */
#define GenTLSubMinorVersion                                                                                           \
    0 /* defines the sub minor version of the GenICam GenTL standard version this header is based on */

#define GC_GENTL_HEADER_VERSION_CODE(major, minor, subminor) (((major) << 24) + ((minor) << 16) + (subminor))
#define GC_GENTL_HEADER_VERSION                              GC_GENTL_HEADER_VERSION_CODE(GenTLMajorVersion, GenTLMinorVersion, GenTLSubMinorVersion)

#endif /* GC_GENTL_HEADER_VERSION */

#ifndef GC_GENTL_DONT_USE_TYPE_DEFINITIONS
#define TLTypeMixedName    "Mixed"    /* Type to use for several supported technologies */
#define TLTypeCustomName   "Custom"   /* Type to use for custom technologies */
#define TLTypeGEVName      "GEV"      /* Type to use for GigE Vision technology */
#define TLTypeCLName       "CL"       /* Type to use for Camera Link technology */
#define TLTypeIIDCName     "IIDC"     /* Type to use for IIDC 1394 technology */
#define TLTypeUVCName      "UVC"      /* Type to use for USB video class devices */
#define TLTypeCXPName      "CXP"      /* Type to use for CoaXPress, V1.3 */
#define TLTypeCLHSName     "CLHS"     /* Type to use for Camera Link HS, V1.3 */
#define TLTypeU3VName      "U3V"      /* Type to use for USB3 Vision Standard, V1.4 */
#define TLTypeETHERNETName "Ethernet" /* Type to use for Ethernet devices, V1.3 */
#define TLTypePCIName      "PCI"      /* Type to use for PCI/PCIe devices, V1.3 */
#endif                                /* GC_GENTL_DONT_USE_TYPE_DEFINITIONS */

#ifndef GC_GENTL_DONT_USE_MODULE_NAMES
#define TLSystemModuleName       "TLSystem"     /* Name to identify a system module */
#define TLInterfaceModuleName    "TLInterface"  /* Name to identify a interface module */
#define TLDeviceModuleName       "TLDevice"     /* Name to identify a device module */
#define TLDataStreamModuleName   "TLDataStream" /* Name to identify a data stream module */
#define TLBufferModuleName       "TLBuffer"     /* Name to identify a buffer module */
#define TLRemoteDeviceModuleName "Device"       /* Name to identify a remote device module */
#endif                                          /* GC_GENTL_DONT_USE_MODULE_NAMES */

    /* Handles */
    typedef void *TL_HANDLE;   /*!< Transport Layer handle, obtained through the TLOpen */
    typedef void *IF_HANDLE;   /*!< Interface handle, obtained through ::TLOpenInterface */
    typedef void *DEV_HANDLE;  /*!< Device handle, obtained through the ::IFOpenDevice */
    typedef void *DS_HANDLE;   /*!< Handle to an data stream object, obtained through DevOpenDataStream */
    typedef void *PORT_HANDLE; /*!< A Port handle is used to access the register space of a port
                                 a PORT_HANDLE can be one of the following TL_HANDLE, IF_HANDLE,
                                 DEV_HANDLE, handle to a device port, obtained through ::DevGetPort,
                                  DS_HANDLE, BUFFER_HANDLE */

    typedef void *BUFFER_HANDLE; /*>! Buffer handle, obtained through the ::DSAnnounceBuffer or related function */
    typedef void *
        EVENTSRC_HANDLE; /* A Event source handle is used to register a OS Event and to retrieve a GenTL event handle */
                         /* a EVENTSRC_HANDLE can be on of the following TL_HANDLE, */
                         /* IF_HANDLE, DEV_HANDLE, DS_HANDLE, BUFFER_HANDLE */
    typedef void *EVENT_HANDLE; /* Event handle, obtained through the ::GCRegisterEvent */

#define GENTL_INVALID_HANDLE NULL                  /* Invalid handle value, V1.4 */
#define GENTL_INFINITE       0xFFFFFFFFFFFFFFFFULL /* Infinite value to be used in various function calls, V1.4 */

    /*! Defines the data type possible for the various Info functions. */
    enum INFO_DATATYPE_LIST
    {
        INFO_DATATYPE_UNKNOWN = 0, /*!< Unknown data type */
        INFO_DATATYPE_STRING = 1,  /*!< NULL-terminated C string (ASCII encoded). */
        INFO_DATATYPE_STRINGLIST =
            2, /*!< Concatenated INFO_DATATYPE_STRING list. End of list is signaled with an additional NULL. */
        INFO_DATATYPE_INT16 = 3,   /*!< Signed 16 bit integer. */
        INFO_DATATYPE_UINT16 = 4,  /*!< Unsigned 16 bit integer */
        INFO_DATATYPE_INT32 = 5,   /*!< Signed 32 bit integer */
        INFO_DATATYPE_UINT32 = 6,  /*!< Unsigned 32 bit integer */
        INFO_DATATYPE_INT64 = 7,   /*!< Signed 64 bit integer */
        INFO_DATATYPE_UINT64 = 8,  /*!< Unsigned 64 bit integer */
        INFO_DATATYPE_FLOAT64 = 9, /*!< Signed 64 bit floating point number. */
        INFO_DATATYPE_PTR = 10,   /*!< Pointer type (void*). Size is platform dependent (32 bit on 32 bit platforms). */
        INFO_DATATYPE_BOOL8 = 11, /*!< Boolean value occupying 8 bit. 0 for false and anything for true. */
        INFO_DATATYPE_SIZET = 12, /*!< Platform dependent unsigned integer (32 bit on 32 bit platforms). */
        INFO_DATATYPE_BUFFER = 13,  /*!< Like a INFO_DATATYPE_STRING but with arbitrary data and no NULL termination. */
        INFO_DATATYPE_PTRDIFF = 14, /*!< Platform dependent signed integer (32 bit on 32 bit platforms). GenTL v1.3 */

        INFO_DATATYPE_CUSTOM_ID = 1000, /*!< Starting value for custom IDs. */
        INFO_DATATYPE_FLOAT32 = 1001,   /*!< Signed 32 bit floating point number. */
        INFO_DATATYPE_UINT8 = 1002      /*!< Unsigned 8 bit integer */
    };
    typedef int32_t INFO_DATATYPE;

    /*! Defines char encoding schemes used by the producer, GenTL v1.4 */
    enum TL_CHAR_ENCODING_LIST
    {
        TL_CHAR_ENCODING_ASCII = 0, /*!< char encoding type is ASCII. */
        TL_CHAR_ENCODING_UTF8 = 1   /*!< char encoding type is UTF8. */
    };
    typedef int32_t TL_CHAR_ENCODING; /* GenTL v1.4 */

    /*! System module information commands for the GenTL::TLGetInfo and GenTL::GCGetInfo functions. */
    enum TL_INFO_CMD_LIST
    {
        TL_INFO_ID = 0,            /*!< STRING    Transport layer ID. */
        TL_INFO_VENDOR = 1,        /*!< STRING    Transport layer vendor name. */
        TL_INFO_MODEL = 2,         /*!< STRING    Transport layer model name. */
        TL_INFO_VERSION = 3,       /*!< STRING    Transport layer version. */
        TL_INFO_TLTYPE = 4,        /*!< STRING    Transport layer technology that is supported. */
        TL_INFO_NAME = 5,          /*!< STRING    File name including extension of the library. */
        TL_INFO_PATHNAME = 6,      /*!< STRING    Full path including file name and extension of the library. */
        TL_INFO_DISPLAYNAME = 7,   /*!< STRING    User readable name of the device. If this is not defined in the device
                                      this should be VENDOR MODEL (ID). */
        TL_INFO_CHAR_ENCODING = 8, /*!< INT32     Reporting the char encoding used by this Producer, GenTL v1.4 */
        TL_INFO_GENTL_VER_MAJOR =
            9, /*!< UINT32    Major number of the GenTL spec this producer complies with, GenTL v1.5 */
        TL_INFO_GENTL_VER_MINOR =
            10, /*!< UINT32    Minor number of the GenTL spec this producer complies with, GenTL v1.5 */
        TL_INFO_CUSTOM_ID = 1000, /*!< Starting value for custom IDs. */
        TL_INFO_COMMIT_ID = 1001, /*!< STRING    commiy ID. */
        TL_INFO_COMPILE = 1002,   /*!< STRING    compile INFO dd/mm/yy compiler. */
    };
    typedef int32_t TL_INFO_CMD;

    /*! This enumeration defines commands to retrieve information with the GenTL::IFGetInfo function from the Interface
     * module. */
    enum INTERFACE_INFO_CMD_LIST
    {
        INTERFACE_INFO_ID = 0,            /*!< STRING     Unique ID of the interface. */
        INTERFACE_INFO_DISPLAYNAME = 1,   /*!< STRING     User readable name of the interface. */
        INTERFACE_INFO_TLTYPE = 2,        /*!< STRING     Transport layer technology that is supported. */
        INTERFACE_INFO_CUSTOM_ID = 1000,  /*!< Starting value for GenTL Producer custom IDs. */
        INTERFACE_INFO_CUSTOM_TYPE = 1001 /*!< INT32      Interface type. */
    };
    typedef int32_t INTERFACE_INFO_CMD;

    /*! This enumeration defines flags of how a device is to be opened with the GenTL::IFOpenDevice function. */
    enum DEVICE_ACCESS_FLAGS_LIST
    {
        DEVICE_ACCESS_UNKNOWN =
            0, /*!< Not used in a command. Can be used to initialize a variable to query that information. */
        DEVICE_ACCESS_NONE = 1, /*!< This either means that the device is not open because it was not opened before or
                                   the access to it was denied. */
        DEVICE_ACCESS_READONLY = 2, /*!< Open the device read only. All Port functions can only read from the device. */
        DEVICE_ACCESS_CONTROL = 3, /*!< Open the device in a way that other hosts/processes can have read only access to
                                      the device. Device access level is read/write for this process. */
        DEVICE_ACCESS_EXCLUSIVE = 4, /*!< Open the device in a way that only this host/process can have access to the
                                        device. Device access level is read/write for this process. */

        DEVICE_ACCESS_CUSTOM_ID = 1000 /*!<  Starting value for GenTL Producer custom IDs. */
    };
    typedef int32_t DEVICE_ACCESS_FLAGS;

    /*! This enumeration defines values for the accessibility of the device to be returned in the GenTL::DevGetInfo
      function on a device handle. */
    enum DEVICE_ACCESS_STATUS_LIST
    {
        DEVICE_ACCESS_STATUS_UNKNOWN = 0,   /*!< The device accessibility is not known. */
        DEVICE_ACCESS_STATUS_READWRITE = 1, /*!< The device is available for read/write access. */
        DEVICE_ACCESS_STATUS_READONLY = 2,  /*!< The device is available for read only access. */
        DEVICE_ACCESS_STATUS_NOACCESS = 3,  /*!< The device is not accessible. */
        DEVICE_ACCESS_STATUS_BUSY = 4, /*!< The device has already been opened by another process/host. GenTL v1.5 */
        DEVICE_ACCESS_STATUS_OPEN_READWRITE = 5, /*!< The device has already been opened by this process. GenTL v1.5 */
        DEVICE_ACCESS_STATUS_OPEN_READONLY = 6,  /*!< The device has already been opened by this process. GenTL v1.5 */

        DEVICE_ACCESS_STATUS_CUSTOM_ID = 1000 /*!< Starting value for custom IDs. */
    };
    typedef int32_t DEVICE_ACCESS_STATUS;

    /*! This enumeration defines commands to retrieve information with the GenTL::DevGetInfo function on a device
     * handle.
     */
    enum DEVICE_INFO_CMD_LIST
    {
        DEVICE_INFO_ID = 0,     /*!< STRING     Unique ID of the device. */
        DEVICE_INFO_VENDOR = 1, /*!< STRING     Device vendor name. */
        DEVICE_INFO_MODEL = 2,  /*!< STRING     Device model name. */
        DEVICE_INFO_TLTYPE = 3, /*!< STRING     Transport layer technology that is supported. */
        DEVICE_INFO_DISPLAYNAME =
            4, /*!< STRING     String containing a display name for the device ( including a unique id ) */
        DEVICE_INFO_ACCESS_STATUS = 5, /*!< INT32      Gets the access mode the GenTL Producer has on the opened device.
                                (DEVICE_ACCESS_STATUS enumeration value). */
        DEVICE_INFO_USER_DEFINED_NAME = 6, /*!< STRING     String containing the user defined name, GenTL v1.4  */
        DEVICE_INFO_SERIAL_NUMBER = 7,     /*!< STRING     String containing the device's serial number, GenTL v1.4  */
        DEVICE_INFO_VERSION = 8,           /*!< STRING     String containing the device version, GenTL v1.4  */
        DEVICE_INFO_TIMESTAMP_FREQUENCY = 9, /*!< UINT64     Tick-frequency of the time stamp clock, GenTL v1.4 */

        DEVICE_INFO_CUSTOM_ID = 1000 /*!< Starting value for GenTL Producer custom IDs. */
    };
    typedef int32_t DEVICE_INFO_CMD; /* DEVICE_INFO_CMD_LIST */

    /*! This enumeration defines special stop flags for the acquisition engine. The function used is
     GenTL::DSStopAcquisition. */
    enum ACQ_STOP_FLAGS_LIST
    {
        ACQ_STOP_FLAGS_DEFAULT = 0, /*!< Stop the acquisition engine when the currently running tasks like filling a
                                   buffer are completed (default behavior). */
        ACQ_STOP_FLAGS_KILL = 1, /*!< Stop the acquisition engine immediately and leave buffers currently being filled
                                    in the Input Buffer Pool. */

        ACQ_STOP_FLAGS_CUSTOM_ID = 1000 /*!< Starting value for GenTL Producer custom IDs. */
    };
    typedef int32_t ACQ_STOP_FLAGS;

    /*! This enumeration defines special start flags for the acquisition engine. The function used is
      GenTL::DSStartAcquisition. */
    enum ACQ_START_FLAGS_LIST
    {
        ACQ_START_FLAGS_DEFAULT = 0, /*!< Default behavior. */

        ACQ_START_FLAGS_CUSTOM_ID = 1000 /*!< Starting value for GenTL Producer custom IDs. */
    };
    typedef int32_t ACQ_START_FLAGS;

    /*! This enumeration commands from which to which queue/pool buffers are flushed with the GenTL::DSFlushQueue
      function. */
    enum ACQ_QUEUE_TYPE_LIST
    {
        ACQ_QUEUE_INPUT_TO_OUTPUT = 0, /*!< Flushes the input pool to the output queue and if necessary adds entries in
                                      the New Buffer event data queue. */
        ACQ_QUEUE_OUTPUT_DISCARD = 1,  /*!< Discards all buffers in the output queue and if necessary remove the entries
                                      from the event data queue. */
        ACQ_QUEUE_ALL_TO_INPUT = 2, /*!< Puts all buffers in the input pool. Even those in the output queue and discard
                                   entries in the event data queue. */
        ACQ_QUEUE_UNQUEUED_TO_INPUT =
            3, /*!< Puts all buffers that are not in the input pool or the output queue in the input pool. */
        ACQ_QUEUE_ALL_DISCARD = 4, /*!< Discards all buffers in the input pool and output queue. */

        ACQ_QUEUE_CUSTOM_ID = 1000 /*!< Starting value for GenTL Producer custom IDs. */
    };
    typedef int32_t ACQ_QUEUE_TYPE;

    /*! This enumeration defines commands to retrieve information with the GenTL::DSGetInfo function on a data stream
      handle */
    enum STREAM_INFO_CMD_LIST
    {
        STREAM_INFO_ID = 0,                 /*!< STRING     Unique ID of the data stream. */
        STREAM_INFO_NUM_DELIVERED = 1,      /*!< UINT64     Number of delivered buffers since last acquisition start. */
        STREAM_INFO_NUM_UNDERRUN = 2,       /*!< UINT64     Number of lost buffers due to queue underrun. */
        STREAM_INFO_NUM_ANNOUNCED = 3,      /*!< SIZET      Number of announced buffers. */
        STREAM_INFO_NUM_QUEUED = 4,         /*!< SIZET      Number of buffers in the input pool. */
        STREAM_INFO_NUM_AWAIT_DELIVERY = 5, /*!< SIZET      Number of buffers in the output queue. */
        STREAM_INFO_NUM_STARTED = 6,        /*!< UINT64     Number of buffers started in the acquisition engine. */
        STREAM_INFO_PAYLOAD_SIZE = 7,       /*!< SIZET      Size of the expected data in bytes. */
        STREAM_INFO_IS_GRABBING =
            8, /*!< BOOL8      Flag indicating whether the acquisition engine is started or not. */
        STREAM_INFO_DEFINES_PAYLOADSIZE = 9, /*!< BOOL8      Flag that indicated that this data stream defines a payload
                                            size independent from the remote device. */
        STREAM_INFO_TLTYPE = 10,             /*!< STRING     Transport layer technology that is supported. */
        STREAM_INFO_NUM_CHUNKS_MAX = 11,     /*!< SIZET      Max number of chunks in a buffer, if known. GenTL v1.3 */
        STREAM_INFO_BUF_ANNOUNCE_MIN =
            12, /*!< SIZET      Min number of buffers to announce before acq can start, if known. GenTL v1.3 */
        STREAM_INFO_BUF_ALIGNMENT = 13,             /*!< SIZET      Buffer alignment in bytes. GenTL v1.3 */
        STREAM_INFO_FLOW_TABLE = 14,                /*!< BUFFER     Flow mapping table in GenDC format. GenTL v1.6 */
        STREAM_INFO_GENDC_PREFETCH_DESCRIPTOR = 15, /*!< BUFFER     Prefetch version of GenDC descriptor. GenTL v1.6 */

        STREAM_INFO_CUSTOM_ID = 1000,         /*!< Starting value for GenTL Producer custom IDs. */
        STREAM_CAM_INT_PARA = 1001,           /*!< BUFFER     Intrinsic camera paras of current stream. */
        STREAM_CAM_EXT_PARA = 1002,           /*!< BUFFER     Extrinsic camera paras of current stream. */
        STREAM_INFO_FOV_Y = 1003,             /*!< FLOAT      The Vertical Field of View of PCL stream. */
        STREAM_CAM_UNDISTORT_INT_PARA = 1004, /*!< BUFFER     Undistrot intrinsic camera paras of current stream. */
        STREAM_FEATURE_LIST =
            1005, /*!< BUFFER     List of supported feature names for the stream, split by char '|'. */
        STREAM_UNDISTORT_POINT =
            1006, /*!< BUFFER     Point data that needs to be undistorted, float data arranged in order of x and y,
                    distortion coefficient and intrinsics usage correspond to the data stream. */
        STREAM_PCL_FRUSTUM_POINT =
            1007 /*!< BUFFER     Input depth and output the corresponding four vertices of the
                     frustum, float point type. When inputting, the first float of buffer is depth. */
    };
    typedef int32_t STREAM_INFO_CMD;

    enum BUFFER_INFO_CMD_LIST
    {
        BUFFER_INFO_BASE = 0,      /*!< PTR        Base address of the buffer memory. */
        BUFFER_INFO_SIZE = 1,      /*!< SIZET      Size of the buffer in bytes. */
        BUFFER_INFO_USER_PTR = 2,  /*!< PTR        Private data pointer of the GenTL Consumer. */
        BUFFER_INFO_TIMESTAMP = 3, /*!< UINT64     Timestamp the buffer was acquired. */
        BUFFER_INFO_NEW_DATA =
            4, /*!< BOOL8      Flag to indicate that the buffer contains new data since the last call. */
        BUFFER_INFO_IS_QUEUED =
            5, /*!< BOOL8      Flag to indicate if the buffer is in the input pool or output queue. */
        BUFFER_INFO_IS_ACQUIRING =
            6, /*!< BOOL8      Flag to indicate that the buffer is currently being filled with data. */
        BUFFER_INFO_IS_INCOMPLETE =
            7, /*!< BOOL8      Flag to indicate that a buffer was filled but an error occurred during that process. */
        BUFFER_INFO_TLTYPE = 8,        /*!< STRING     Transport layer technology that is supported. */
        BUFFER_INFO_SIZE_FILLED = 9,   /*!< SIZET      Number of bytes written into the buffer last time it has been
  filled.   This value is reset to 0 when the buffer is placed into the Input Buffer Pool. */
        BUFFER_INFO_WIDTH = 10,        /*!< SIZET      Width GenTL v1.2 */
        BUFFER_INFO_HEIGHT = 11,       /*!< SIZET      Height GenTL v1.2 */
        BUFFER_INFO_XOFFSET = 12,      /*!< SIZET      Xoffset GenTL v1.2 */
        BUFFER_INFO_YOFFSET = 13,      /*!< SIZET      Yoffset GenTL v1.2 */
        BUFFER_INFO_XPADDING = 14,     /*!< SIZET      Xpadding GenTL v1.2 */
        BUFFER_INFO_YPADDING = 15,     /*!< SIZET      Ypadding GenTL v1.2 */
        BUFFER_INFO_FRAMEID = 16,      /*!< UINT64     Frame ID GenTL v1.2 */
        BUFFER_INFO_IMAGEPRESENT = 17, /*!< BOOL8  Image present     GenTL v1.2 */
        BUFFER_INFO_IMAGEOFFSET = 18,  /*!< SIZET  Image offset     GenTL v1.2 */
        BUFFER_INFO_PAYLOADTYPE = 19,  /*!< SIZET  Payload type    GenTL v1.2 */
        BUFFER_INFO_PIXELFORMAT = 20,  /*!< UINT64 Pixel format     GenTL v1.2 */
        BUFFER_INFO_PIXELFORMAT_NAMESPACE = 21,      /*!< UINT64 Pixel format namespace    GenTL v1.2 */
        BUFFER_INFO_DELIVERED_IMAGEHEIGHT = 22,      /*!< SIZET  Delivered image height    GenTL v1.2 */
        BUFFER_INFO_DELIVERED_CHUNKPAYLOADSIZE = 23, /*!< SIZET  Delivered chunk payload size    GenTL v1.2 */
        BUFFER_INFO_CHUNKLAYOUTID = 24,              /*!< UINT64 Chunk layout ID    GenTL v1.2 */
        BUFFER_INFO_FILENAME = 25,                   /*!< STRING File name    GenTL v1.2 */
        BUFFER_INFO_PIXEL_ENDIANNESS = 26,           /*!< INT32  Pixel endianness    GenTL v1.4 */
        BUFFER_INFO_DATA_SIZE = 27,                  /*!< SIZET  Data size    GenTL v1.4 */
        BUFFER_INFO_TIMESTAMP_NS = 28,               /*!< UINT64 Time stamp size    GenTL v1.4 */
        BUFFER_INFO_DATA_LARGER_THAN_BUFFER = 29,    /*!< BOOL8  Data larger than buffer    GenTL v1.4 */
        BUFFER_INFO_CONTAINS_CHUNKDATA = 30,         /*!< BOOL8  contains chunk data    GenTL v1.4 */
        BUFFER_INFO_IS_COMPOSITE = 31,               /*!< BOOL8  Is composite    GenTL v1.6 */

        BUFFER_INFO_CUSTOM_ID = 1000,   /*!< Starting value for GenTL Producer custom IDs. */
        BUFFER_INFO_ALGO_RESULTS = 1001 /*!< STRING Getting the algorithmic processing results of an image */
    };
    typedef int32_t BUFFER_INFO_CMD;

    /*! This enumeration defines commands to retrieve information about individual data parts in a multi-part buffer
 using the GenTL::DSGetBufferPartInfo function. Introduced in GenTL v1.5. */
    enum BUFFER_PART_INFO_CMD_LIST
    {
        BUFFER_PART_INFO_BASE = 0,      /*!< PTR        Base address of the buffer part memory. */
        BUFFER_PART_INFO_DATA_SIZE = 1, /*!< SIZET      Size of the buffer part in bytes. */
        BUFFER_PART_INFO_DATA_TYPE =
            2, /*!< SIZET      Type of the data in given part (PARTDATATYPE_ID enumeration value). */
        BUFFER_PART_INFO_DATA_FORMAT =
            3, /*!< UINT64     Format of individual items (such as pixels) in the buffer part. */
        BUFFER_PART_INFO_DATA_FORMAT_NAMESPACE =
            4,                         /*!< UINT64     Allows interpretation of BUFFER_PART_INFO_DATA_FORMAT
                                      (PIXELFORMAT_NAMESPACE_ID enumeration value). */
        BUFFER_PART_INFO_WIDTH = 5,    /*!< SIZET      Width of data in the buffer part in pixels. */
        BUFFER_PART_INFO_HEIGHT = 6,   /*!< SIZET      Expected height of data in the buffer part in pixels . */
        BUFFER_PART_INFO_XOFFSET = 7,  /*!< SIZET      Horizontal offset of data in the buffer part in pixels. */
        BUFFER_PART_INFO_YOFFSET = 8,  /*!< SIZET      Vertical offset of data in the buffer part in pixels. */
        BUFFER_PART_INFO_XPADDING = 9, /*!< SIZET      Horizontal padding of data in the buffer part in pixels. */
        BUFFER_PART_INFO_SOURCE_ID =
            10, /*!< UINT64     Identifier allowing to group data parts belonging to the same source. */
        BUFFER_PART_INFO_DELIVERED_IMAGEHEIGHT =
            11, /*!< SIZET      Height of the data currently in the buffer part in pixels*/
        BUFFER_PART_INFO_REGION_ID =
            12, /*!< UINT64     Identifier allowing to group data parts belonging to the same region. GenTL v1.6 */
        BUFFER_PART_INFO_DATA_PURPOSE_ID =
            13, /*!< UINT64     Identifier allowing to group data parts having the same purpose. GenTL v1.6 */
        BUFFER_PART_INFO_CUSTOM_ID = 1000 /*!< Starting value for GenTL Producer custom IDs. */
    };
    typedef int32_t BUFFER_PART_INFO_CMD; /* GenTL v1.5 */

    /*! Enumeration of TLType dependent payload types. Introduced in GenTL v1.2 */
    enum PAYLOADTYPE_INFO_IDS
    {
        PAYLOAD_TYPE_UNKNOWN = 0,         /*!< GenTL v1.2 */
        PAYLOAD_TYPE_IMAGE = 1,           /*!< GenTL v1.2 */
        PAYLOAD_TYPE_RAW_DATA = 2,        /*!< GenTL v1.2 */
        PAYLOAD_TYPE_FILE = 3,            /*!< GenTL v1.2 */
        PAYLOAD_TYPE_CHUNK_DATA = 4,      /*!< GenTL v1.2, Deprecated in GenTL 1.5*/
        PAYLOAD_TYPE_JPEG = 5,            /*!< GenTL v1.4 */
        PAYLOAD_TYPE_JPEG2000 = 6,        /*!< GenTL v1.4 */
        PAYLOAD_TYPE_H264 = 7,            /*!< GenTL v1.4 */
        PAYLOAD_TYPE_CHUNK_ONLY = 8,      /*!< GenTL v1.4 */
        PAYLOAD_TYPE_DEVICE_SPECIFIC = 9, /*!< GenTL v1.4 */
        PAYLOAD_TYPE_MULTI_PART = 10,     /*!< GenTL v1.5 */
        PAYLOAD_TYPE_GENDC = 11,          /*!< GenTL v1.6 */

        PAYLOAD_TYPE_CUSTOM_ID = 1000 /*!< Starting value for GenTL Producer custom IDs. */
    };
    typedef int32_t PAYLOADTYPE_INFO_ID;

    /*! Enumeration of TLType dependent pixel format namespaces introduced GenTL v1.2 */
    enum PIXELFORMAT_NAMESPACE_IDS
    {
        PIXELFORMAT_NAMESPACE_UNKNOWN = 0,    /*!< Pixel format namespace unknown, GenTL v1.2 */
        PIXELFORMAT_NAMESPACE_GEV = 1,        /*!< Pixel format namespace GEV, GenTL v1.2 */
        PIXELFORMAT_NAMESPACE_IIDC = 2,       /*!< Pixel format namespace IIDC, GenTL v1.2 */
        PIXELFORMAT_NAMESPACE_PFNC_16BIT = 3, /*!< Pixel format namespace PFNC_16BIT, GenTL v1.4 */
        PIXELFORMAT_NAMESPACE_PFNC_32BIT = 4, /*!< Pixel format namespace PFNC_32BIT, GenTL v1.4 */

        PIXELFORMAT_NAMESPACE_CUSTOM_ID = 1000 /*!< Starting value for GenTL Producer custom IDs. */
    };
    typedef int32_t PIXELFORMAT_NAMESPACE_ID; /* GenTL v1.2 */

    /*! Enumeration of pixel endianness values. Introduced in GenTL v1.4 */
    enum PIXELENDIANNESS_IDS
    {
        PIXELENDIANNESS_UNKNOWN = 0, /*!< Unknown pixel endianness. GenTL v1.4 */
        PIXELENDIANNESS_LITTLE = 1,  /*!< Little endian pixel data. GenTL v1.4 */
        PIXELENDIANNESS_BIG = 2      /*!< Big endian pixel data. GenTL v1.4 */
    };
    typedef int32_t PIXELENDIANNESS_ID; /* GenTL v1.4*/

    /*! Enumeration describing which data type is present in given buffer part. Introduced in GenTL v1.5 */
    enum PARTDATATYPE_IDS
    {
        PART_DATATYPE_UNKNOWN = 0,             /*!< Unknown data type */
        PART_DATATYPE_2D_IMAGE = 1,            /*!< Color or monochrome 2D image. */
        PART_DATATYPE_2D_PLANE_BIPLANAR = 2,   /*!< Single color plane of a planar 2D image consisting of 2 planes. */
        PART_DATATYPE_2D_PLANE_TRIPLANAR = 3,  /*!< Single color plane of a planar 2D image consisting of 3 planes. */
        PART_DATATYPE_2D_PLANE_QUADPLANAR = 4, /*!< Single color plane of a planar 2D image consisting of 4 planes. */
        PART_DATATYPE_3D_IMAGE = 5,            /*!< 3D image (pixel coordinates). */
        PART_DATATYPE_3D_PLANE_BIPLANAR = 6,   /*!< Single plane of a planar 3D image consisting of 2 planes. */
        PART_DATATYPE_3D_PLANE_TRIPLANAR = 7,  /*!< Single plane of a planar 3D image consisting of 3 planes. */
        PART_DATATYPE_3D_PLANE_QUADPLANAR = 8, /*!< Single plane of a planar 3D image consisting of 4 planes. */
        PART_DATATYPE_CONFIDENCE_MAP = 9,      /*!< Confidence of the individual pixel values. */
        PART_DATATYPE_JPEG = 10,               /*!< JPEG compressed data. GenTL v1.6 */
        PART_DATATYPE_JPEG2000 = 11,           /*!< JPEG 2000 compressed data. GenTL v1.6 */

        PART_DATATYPE_CUSTOM_ID = 1000 /*!< Starting value for GenTL Producer custom IDs. */
    };
    typedef int32_t PARTDATATYPE_ID; /* GenTL v1.5*/

    /*! This enumeration defines commands to retrieve information with the GenTL::GCGetPortInfo function on a module or
      remote device handle. */
    enum PORT_INFO_CMD_LIST
    {
        PORT_INFO_ID = 0,            /*!< STRING     Unique ID of the port. */
        PORT_INFO_VENDOR = 1,        /*!< STRING     Port vendor name. */
        PORT_INFO_MODEL = 2,         /*!< STRING     Port model name. */
        PORT_INFO_TLTYPE = 3,        /*!< STRING     Transport layer technology that is supported. */
        PORT_INFO_MODULE = 4,        /*!< STRING     GenTL Module the port refers to. */
        PORT_INFO_LITTLE_ENDIAN = 5, /*!< BOOL8      Flag indicating that the port data is little endian. */
        PORT_INFO_BIG_ENDIAN = 6,    /*!< BOOL8      Flag indicating that the port data is big endian. */
        PORT_INFO_ACCESS_READ = 7,   /*!< BOOL8      Port has read access. */
        PORT_INFO_ACCESS_WRITE = 8,  /*!< BOOL8      Port has write access. */
        PORT_INFO_ACCESS_NA = 9,     /*!< BOOL8      Port is not accessible. */
        PORT_INFO_ACCESS_NI = 10,    /*!< BOOL8      Port is not implemented. */
        PORT_INFO_VERSION = 11,      /*!< STRING     Version of the port. */
        PORT_INFO_PORTNAME = 12,     /*!< STRING     Name of the port as referenced in the XML description. */

        PORT_INFO_CUSTOM_ID = 1000 /*!< Starting value for GenTL Producer custom IDs. */
    };
    typedef int32_t PORT_INFO_CMD;

    /*! This enumeration defines enum values returned by the URL_INFO_SCHEME command.
 introduced in GenTL v1.5 */
    enum URL_SCHEME_IDS
    {
        URL_SCHEME_LOCAL = 0, /*!< The XML can be read from the local register map */
        URL_SCHEME_HTTP = 1,  /*!< The XML can be downloaded from a http server */
        URL_SCHEME_FILE = 2,  /*!< The XML can be read from the local hard drive */

        URL_SCHEME_CUSTOM_ID = 1000 /*!< Starting value for custom scheme locations */
    };
    typedef int32_t URL_SCHEME_ID;

    /*! This enumeration defines commands to retrieve information with the GenTL::GCGetPortURLInfo
 function on a module or remote device handle. Introduced in GenTL v1.1 */
    enum URL_INFO_CMD_LIST
    {
        URL_INFO_URL = 0,              /*!< STRING     URL as defined in chapter 4.1.2 GenTL v1.1  */
        URL_INFO_SCHEMA_VER_MAJOR = 1, /*!< INT32      Major version of the schema this URL refers to. GenTL v1.1 */
        URL_INFO_SCHEMA_VER_MINOR = 2, /*!< INT32      Minor version of the schema this URL refers to. GenTL v1.1 */
        URL_INFO_FILE_VER_MAJOR = 3,   /*!< INT32      Major version of the XML-file this URL refers to. GenTL v1.1 */
        URL_INFO_FILE_VER_MINOR = 4,   /*!< INT32      Minor version of the XML-file this URL refers to. GenTL v1.1 */
        URL_INFO_FILE_VER_SUBMINOR =
            5,                       /*!< INT32      Subminor version of the XML-file this URL refers to. GenTL v1.1 */
        URL_INFO_FILE_SHA1_HASH = 6, /*!< BUFFER     160-bit SHA1 Hash code of XML-file. GenTL v1.4 */
        URL_INFO_FILE_REGISTER_ADDRESS = 7, /*!< UINT64     Register address in the device's register map. GenTL v1.5 */
        URL_INFO_FILE_SIZE = 8,             /*!< UINT64     File size in bytes. GenTL v1.5 */
        URL_INFO_SCHEME = 9,    /*!< INT32      Scheme of the URL as defined in URL_SCHEME_IDS. GenTL v1.5 */
        URL_INFO_FILENAME = 10, /*!< STRING     File name if the scheme of the URL is file. GenTL v1.5 */

        URL_INFO_CUSTOM_ID = 1000 /*!< Starting value for GenTL Producer custom IDs. */
    };
    typedef int32_t URL_INFO_CMD; /* GenTL v1.1 */

    /*! Known event types that can be registered on certain modules with the GenTL::GCRegisterEvent function. */
    enum EVENT_TYPE_LIST
    {
        EVENT_ERROR = 0,              /*!< Notification on module errors. */
        EVENT_NEW_BUFFER = 1,         /*!< Notification on newly filled buffers. */
        EVENT_FEATURE_INVALIDATE = 2, /*!< Notification if a feature was changed by the GenTL Producer library and thus
                                     needs to be invalidated in the GenICam GenApi instance using the module. */
        EVENT_FEATURE_CHANGE = 3, /*!< Notification if the GenTL Producer library wants to manually set a feature in the
                             GenICam GenApi instance using the module. */
        EVENT_REMOTE_DEVICE = 4,  /*!< Notification if the GenTL Producer wants to inform the GenICam GenApi instance of
                             the remote device that a GenApi compatible event was fired. */
        EVENT_MODULE = 5, /*!< Notification if the GenTL Producer wants to inform the GenICam GenApi instance of the
             module that a GenApi compatible event was fired.  GenTL v1.4 */

        EVENT_CUSTOM_ID = 1000,       /*!< Starting value for GenTL Producer custom IDs. */
        EVENT_DETECTION_REMOTE_DEVICE /*!< Notification if the GenTL producer wants to inform the GenICam GenApi
                                         instance of the remote device that insertion or removal event was fired. */
    };
    typedef int32_t EVENT_TYPE;

    /*! Event info command */
    enum EVENT_INFO_CMD_LIST
    {
        EVENT_EVENT_TYPE = 0,   /*!< INT32      The event type of the event handle (EVENT_TYPE enum value). */
        EVENT_NUM_IN_QUEUE = 1, /*!< SIZET      Number of events in the event data queue. */
        EVENT_NUM_FIRED = 2,    /*!< UINT64     Number of events that were fired since the creation of the module. */
        EVENT_SIZE_MAX = 3,     /*!< SIZET      Max size of data carried with an event in bytes. GenTL v1.2 */
        EVENT_INFO_DATA_SIZE_MAX =
            4, /*!< SIZET      Max size of data provided through EventGetDataInfo in bytes. GenTL v1.2 */

        EVENT_INFO_CUSTOM_ID = 1000 /*!< Starting value for GenTL Producer custom IDs. */
    };
    typedef int32_t EVENT_INFO_CMD;

    /*! Event data info command */
    enum EVENT_DATA_INFO_CMD_LIST
    {
        EVENT_DATA_ID = 0,    /*!< Event specific    Unique Event ID (String or Number)*/
        EVENT_DATA_VALUE = 1, /*!< Event specific Data */
        EVENT_DATA_NUMID = 2, /*!< UINT64   Numeric representation of the unique Event ID, GenTL v1.3. */

        EVENT_DATA_CUSTOM_ID = 1000 /*!< Starting value for GenTL Producer custom IDs. */
    };
    typedef int32_t EVENT_DATA_INFO_CMD;

    /*! This enumeration defines commands to retrieve information about individual data stream flows
 using the GenTL::DSGetFlowInfo function. Introduced in GenTL v1.6. */
    enum FLOW_INFO_CMD_LIST
    {
        FLOW_INFO_SIZE = 0,        /*!< SIZET      Size of the flow in bytes. */
        FLOW_INFO_CUSTOM_ID = 1000 /*!< Starting value for GenTL Producer custom IDs. */
    };
    typedef int32_t FLOW_INFO_CMD; /* GenTL v1.6 */

    /*! This enumeration defines commands to retrieve information about composite buffer segments
 using the GenTL::DSGetBufferSegmentInfo function. Introduced in GenTL v1.6. */
    enum SEGMENT_INFO_CMD_LIST
    {
        SEGMENT_INFO_BASE = 0, /*!< PTR        Based address of the buffer segment memory. */
        SEGMENT_INFO_SIZE = 1, /*!< SIZET      Size of the buffer segment in bytes. */
        SEGMENT_INFO_IS_INCOMPLETE =
            2, /*!< BOOL8      Flag to indicate that an error occured while filling the segment. */
        SEGMENT_INFO_SIZE_FILLED =
            3, /*!< SIZET      Number of bytes written into the buffer last time it has been filled. */
        SEGMENT_INFO_DATA_SIZE =
            4, /*!< SIZET      Size of the data intended to be written to the buffer last time it has been filled. */
        SEGMENT_INFO_CUSTOM_ID = 1000 /*!< Starting value for GenTL Producer custom IDs. */
    };
    typedef int32_t SEGMENT_INFO_CMD; /*!< GenTL v1.6 */

    /*! Structure of the data returned from a signaled "New Buffer" event. */
#pragma pack(push, 1)
    typedef struct S_EVENT_NEW_BUFFER
    {
        BUFFER_HANDLE BufferHandle; /*!< Buffer handle which contains new data. */
        void *pUserPointer;         /*!< User pointer provided at announcement of the buffer. */
    } EVENT_NEW_BUFFER_DATA;
#pragma pack(pop)

    /*! Structure to be use with GCWritePortStacked and GCReadPortStacked. */
#pragma pack(push, 1)
    typedef struct S_PORT_REGISTER_STACK_ENTRY
    {
        uint64_t Address; /*!< Address of the register. */
        void *pBuffer;    /*!< Pointer to the buffer containing the data. */
        size_t Size;      /*!< Number of bytes to read write. */
    } PORT_REGISTER_STACK_ENTRY;
#pragma pack(pop)

#pragma pack(push, 1)
    /*! Structure carrying information about a single chunk in the buffer. Introduced in GenTL v1.3. */
    typedef struct S_SINGLE_CHUNK_DATA
    {
        uint64_t ChunkID;      /*!< Numeric representation of ChunkID */
        ptrdiff_t ChunkOffset; /*!< Chunk offset in the buffer */
        size_t ChunkLength;    /*!< Size of the chunk data */
    } SINGLE_CHUNK_DATA;
#pragma pack(pop)

#pragma pack(push, 1)
    /*! Structure carrying information about a buffer info within a stacked request. Introduced in GenTL v1.6. */
    typedef struct S_DS_BUFFER_INFO_STACKED
    {
        BUFFER_INFO_CMD iInfoCmd; /*!< Queried buffer info */
        INFO_DATATYPE iType;      /*!< The info's data type */
        void *pBuffer;            /*!< Pointer to buffer to hold the info data */
        size_t iSize;             /*!< Size of the info buffer (in) and the actual info data (out) */
        GC_ERROR iResult;         /*!< Result of the buffer info queury */
    } DS_BUFFER_INFO_STACKED;
#pragma pack(pop)

#pragma pack(push, 1)
    /*! Structure carrying information about a buffer part info within a stacked request. Introduced in GenTL v1.6. */
    typedef struct S_DS_BUFFER_PART_INFO_STACKED
    {
        uint32_t iPartIndex;           /*!< Index of buffer part to query */
        BUFFER_PART_INFO_CMD iInfoCmd; /*!< Queried buffer part info */
        GC_ERROR iResult;              /*!< Result of the buffer info queury */
        INFO_DATATYPE iType;           /*!< The info's data type */
        void *pBuffer;                 /*!< Pointer to buffer to hold the info data */
        size_t iSize;                  /*!< Size of the info buffer (in) and the actual info data (out) */
    } DS_BUFFER_PART_INFO_STACKED;
#pragma pack(pop)

#pragma pack(push, 1)
    typedef struct S_DS_BUFFER_IMU_DATA
    {
        uint64_t AcceTime;
        float AcceData[3];
        uint64_t GyroTime;
        float GyroData[3];
        uint8_t Reserve[24];
    } DS_BUFFER_IMU_DATA;
#pragma pack(pop)

    /* C API Interface Functions */
#define GC_API GC_IMPORT_EXPORT GC_ERROR GC_CALLTYPE
    /**
     * @brief Enable Debug logger level of DYV SDK
     */
    GC_API GCEnableDebugLoggerLevel(void);

    /**
     * @brief Get an infomation of the GenTL system object
     *
     * @param iInfoCmd TL_INFO_CMD enum
     * @param piType  type of the value the buffer filled
     * @param pBuffer data storaged
     * @param piSize size of the data filled, if pBuffer eauqals null, that means the samllet buffer size should be
     * provided
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API GCGetInfo(TL_INFO_CMD iInfoCmd, INFO_DATATYPE *piType, void *pBuffer, size_t *piSize);

    /**
     * @brief Get the last errcode and error message
     *
     * @param piErrorCode last errcode
     * @param sErrText error message
     * @param piSize size of the error message
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API GCGetLastError(GC_ERROR *piErrorCode, char *sErrText, size_t *piSize);

    /**
     * @brief Init the dynamic lib
     *
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API GCInitLib(void);

    /**
     * @brief Close the dynamic lib, tear down the intern object
     *
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API GCCloseLib(void);

    /**
     * @brief Read the register of the GenTL object. Notice that address of one GenTL object is independent coded.
     *
     * @param hPort handle of the GenTL object,such as system/interface/device/stream,
                    notice that buffer is not implemented as common GenTL module
     * @param iAddress address of the rigester to be read
     * @param pBuffer data to be storaged
     * @param piSize size of the data to be read
     * @return GC_ERR_SUCCESS or other errcode
     */

    GC_API GCReadPort(PORT_HANDLE hPort, uint64_t iAddress, void *pBuffer, size_t *piSize);

    /**
     * @brief Write the register of the GenTL object. Notice that address of one GenTL object is independent coded.
     *
     * @param hPort handle of the GenTL object,such as system/interface/device/stream,
                    notice that buffer is not implemented as common GenTL module
     * @param iAddress  address of the rigester to be written
     * @param pBuffer the data ready to be written
     * @param piSize size of the data to be written
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API GCWritePort(PORT_HANDLE hPort, uint64_t iAddress, const void *pBuffer, size_t *piSize);

    /**
     * @brief Get the url of description file which describe the GenTL object
     *
     * @param hPort handle of the GenTL object,such as system/interface/device/stream,
                    notice that buffer is not implemented as common GenTL module
     * @param sURL the buffer resverd to be filled with the url string, 128 bytes is recommended.
     * @param piSize the url length
     * @return GC_ERR_SUCCESS or other errcode
     */

    GC_API GCGetPortURL(PORT_HANDLE hPort, char *sURL, size_t *piSize);

    /**
     * @brief Get the information of GenTL object
     *
     * @param hPort handle of the GenTL object,such as system/interface/device/stream,
                    notice that buffer is not implemented as common GenTL module
     * @param iInfoCmd PORT_INFO_CMD enum
     * @param piType type of the value the buffer filled
     * @param pBuffer data storaged
     * @param piSize size of the data filled, if pBuffer eauqals null, that means the samllet buffer size should be
     provided
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API GCGetPortInfo(PORT_HANDLE hPort, PORT_INFO_CMD iInfoCmd, INFO_DATATYPE *piType, void *pBuffer,
                         size_t *piSize);

    /**
     * @brief Enable event tigger func of a GenTL object
     *
     * @param hEventSrc a GenTL object which can trigger the event
     * @param iEventID specific envent type the GenTL object should trigger, like EVENT_NEW_BUFFER
     * @param phEvent handle to the event, which can be use to check the event was triggered.
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API GCRegisterEvent(EVENTSRC_HANDLE hEventSrc, EVENT_TYPE iEventID, EVENT_HANDLE *phEvent);

    /**
     * @brief Disable event tigger func of a GenTL object, cooperate with the GCRegisterEvent function
     *
     * @param hEventSrc a GenTL object which can trigger the event
     * @param iEventID specific envent type the GenTL object should disable, like EVENT_NEW_BUFFER
     * @return GC_ERR_SUCCESS or other errcode
     */

    GC_API GCUnregisterEvent(EVENTSRC_HANDLE hEventSrc, EVENT_TYPE iEventID);

    /**
     * @brief Get a triggered event and then save data
     *
     * @param hEvent the handle of the event
     * @param pBuffer data to be storaged
     * @param piSize size of the valid data
     * @param iTimeout check the event if trigger untill timeout or get a trigger signal
     * @return GC_ERR_SUCCESS, GC_ERR_TIMEOUT or other errcode
     */

    GC_API EventGetData(EVENT_HANDLE hEvent, void *pBuffer, size_t *piSize, uint64_t iTimeout);

    /**
     * @brief Acquire specific information from the event data
     *
     * @param hEvent the handle of the event
     * @param pInBuffer  data to be extrated
     * @param iInSize size of the data
     * @param iInfoCmd specific part of the data want to be extracted
     * @param piType type of the data
     * @param pOutBuffer to storage the result
     * @param piOutSize size of the result
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API EventGetDataInfo(EVENT_HANDLE hEvent, const void *pInBuffer, size_t iInSize, EVENT_DATA_INFO_CMD iInfoCmd,
                            INFO_DATATYPE *piType, void *pOutBuffer, size_t *piOutSize);

    /**
     * @brief Get the infomation from the event handle
     *
     * @param hEvent the handle of the event
     * @param iInfoCmd EVENT_INFO_CMD enum
     * @param piType type of the data
     * @param pBuffer to storage the result
     * @param piSize size of the result
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API EventGetInfo(EVENT_HANDLE hEvent, EVENT_INFO_CMD iInfoCmd, INFO_DATATYPE *piType, void *pBuffer,
                        size_t *piSize);

    /**
     * @brief Clear the event triggered stete if triggered
     *
     * @param hEvent the handle of the event
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API EventFlush(EVENT_HANDLE hEvent);

    /**
     * @brief Kill the event state polling process
     *
     * @param hEvent the handle of the event
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API EventKill(EVENT_HANDLE hEvent);

    /**
     * @brief Open the GenTL system module
     *
     * @param phTL handle of the GenTL system module
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API TLOpen(TL_HANDLE *phTL);

    /**
     * @brief Close the GenTL system module, which may invalid all interface handle
     *
     * @param hTL  handle of the GenTL system module
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API TLClose(TL_HANDLE hTL);

    /**
     * @brief Get some infomation of the system module
     *
     * @param hTL handle of the GenTL system module
     * @param iInfoCmd TL_INFO_CMD enum
     * @param piType type of the data
     * @param pBuffer data storaged
     * @param piSize size of the data filled, if pBuffer eauqals null, that means the samllet buffer size should be
     * provided
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API TLGetInfo(TL_HANDLE hTL, TL_INFO_CMD iInfoCmd, INFO_DATATYPE *piType, void *pBuffer, size_t *piSize);

    /**
     * @brief Get number of interface module
     *
     * @param hTL handle of the GenTL system module
     * @param piNumIfaces number of the GenTL interface module based on this system module
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API TLGetNumInterfaces(TL_HANDLE hTL, uint32_t *piNumIfaces);

    /**
     * @brief Get interface module ID by index
     *
     * @param hTL handle of the GenTL system module
     * @param iIndex index of the interface module
     * @param sID ID string to be storaged
     * @param piSize size of sID buffer, and retuen as size of the ID string. If size is insufficient,
                     it will be size of the ID string, and the function return GC_ERR_BUFFER_TOO_SMALL.
     * @return GC_ERR_SUCCESS, GC_ERR_BUFFER_TOO_SMALL or other errcode
     */
    GC_API TLGetInterfaceID(TL_HANDLE hTL, uint32_t iIndex, char *sID, size_t *piSize);

    /**
     * @brief Get some information of the interface module
     *
     * @param hTL handle of the GenTL system module
     * @param sIfaceID  ID string of the interface module
     * @param iInfoCmd INTERFACE_INFO_CMD enum
     * @param piType type of the data
     * @param pBuffer data storaged
     * @param piSize size of the data filled, if pBuffer eauqals null, that means the samllet buffer size should be
     * provided
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API TLGetInterfaceInfo(TL_HANDLE hTL, const char *sIfaceID, INTERFACE_INFO_CMD iInfoCmd, INFO_DATATYPE *piType,
                              void *pBuffer, size_t *piSize);
    /**
     * @brief Open the interface module
     *
     * @param hTL handle of the GenTL system module
     * @param sIfaceID ID string of the interface module
     * @param phIface handle of the interface module
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API TLOpenInterface(TL_HANDLE hTL, const char *sIfaceID, IF_HANDLE *phIface);
    /**
     * @brief re-Enumerate the Interface module
     *
     * @param hTL handle of the GenTL system module
     * @param pbChanged if interface module list is changed
     * @param iTimeout re-Enumerate untill timeout or get a valid result
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API TLUpdateInterfaceList(TL_HANDLE hTL, bool8_t *pbChanged, uint64_t iTimeout);

    /**
     * @brief Close the interface, all device based on this interface module should be inaccessible.
     *
     * @param hIface handle of the GenTL interface module
     * @return  GC_ERR_SUCCESS or other errcode
     */
    GC_API IFClose(IF_HANDLE hIface);

    /**
     * @brief Get some information of the Interface
     *
     * @param hIface handle of the GenTL interface module
     * @param iInfoCmd INTERFACE_INFO_CMD enum
     * @param piType type of the data
     * @param pBuffer data storaged
     * @param piSize size of the data filled, if pBuffer eauqals null, that means the samllet buffer size should be
     * provided
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API IFGetInfo(IF_HANDLE hIface, INTERFACE_INFO_CMD iInfoCmd, INFO_DATATYPE *piType, void *pBuffer,
                     size_t *piSize);

    /**
     * @brief Get number of device module
     *
     * @param hIface handle of the GenTL interface module
     * @param piNumDevices  number of the GenTL device module based on this system module
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API IFGetNumDevices(IF_HANDLE hIface, uint32_t *piNumDevices);

    /**
     * @brief get ID string of the device module
     *
     * @param hIface handle of the GenTL interface module
     * @param iIndex index of the device module
     * @param sIDeviceID ID string to be storaged
     * @param piSize size of sID buffer, and retuen as size of the ID string. If size is insufficient,
                 it will be size of the ID string, and the function return GC_ERR_BUFFER_TOO_SMALL.
     * @return GC_ERR_SUCCESS, GC_ERR_BUFFER_TOO_SMALL or other errcode
     * @return GC_API
     */
    GC_API IFGetDeviceID(IF_HANDLE hIface, uint32_t iIndex, char *sIDeviceID, size_t *piSize);

    /**
     * @brief re-Enumerate the device module
     *
     * @param hIface handle of the GenTL interface module
     * @param pbChanged if device module list is changed
     * @param iTimeout re-Enumerate untill timeout or get a valid result
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API IFUpdateDeviceList(IF_HANDLE hIface, bool8_t *pbChanged, uint64_t iTimeout);

    /**
     * @brief Get some information of the device module
     *
     * @param hIface handle of the GenTL interface module
     * @param sDeviceID ID string of the device module
     * @param iInfoCmd DEVICE_INFO_CMD enum
     * @param piType type of the data
     * @param pBuffer data storaged
     * @param piSize size of the data filled, if pBuffer eauqals null, that means the samllet buffer size should be
     * provided
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API IFGetDeviceInfo(IF_HANDLE hIface, const char *sDeviceID, DEVICE_INFO_CMD iInfoCmd, INFO_DATATYPE *piType,
                           void *pBuffer, size_t *piSize);
    /**
     * @brief Open the device module
     *
     * @param hIface handle of the GenTL interface module
     * @param sDeviceID ID string of the device module
     * @param iOpenFlag DEVICE_ACCESS_FLAGS enum
     * @param phDevice handle of the device module
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API IFOpenDevice(IF_HANDLE hIface, const char *sDeviceID, DEVICE_ACCESS_FLAGS iOpenFlag, DEV_HANDLE *phDevice);

    /**
     * @brief Get the remote device write/read node
     *
     * @param hDevice handle of the device module
     * @param phRemoteDevice handle of the remote device module
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API DevGetPort(DEV_HANDLE hDevice, PORT_HANDLE *phRemoteDevice);

    /**
     * @brief Get number of device module
     *
     * @param hDevice  handle of the GenTL device module
     * @param piNumDataStreams number of the GenTL stream module based on this device module
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API DevGetNumDataStreams(DEV_HANDLE hDevice, uint32_t *piNumDataStreams);

    /**
     * @brief Get stream module ID by index
     *
     * @param hDevice handle of the GenTL device module
     * @param iIndex index of the stream module
     * @param sDataStreamID ID string to be storaged
     * @param piSize size of sID buffer, and retuen as size of the ID string. If size is insufficient,
                 it will be size of the ID string, and the function return GC_ERR_BUFFER_TOO_SMALL.
     * @return GC_ERR_SUCCESS, GC_ERR_BUFFER_TOO_SMALL or other errcode
     */
    GC_API DevGetDataStreamID(DEV_HANDLE hDevice, uint32_t iIndex, char *sDataStreamID, size_t *piSize);
    /**
     * @brief Open the stream module
     *
     * @param hDevice  handle of the GenTL device module
     * @param sDataStreamID ID string of the stream module
     * @param phDataStream handle of the stream module
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API DevOpenDataStream(DEV_HANDLE hDevice, const char *sDataStreamID, DS_HANDLE *phDataStream);

    /**
     * @brief Get some information of the device
     *
     * @param hDevice handle of the GenTL device module
     * @param iInfoCmd DEVICE_INFO_CMD enum
     * @param piType type of the data
     * @param pBuffer data storaged
     * @param piSize size of the data filled, if pBuffer eauqals null, that means the samllet buffer size should be
     * provided
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API DevGetInfo(DEV_HANDLE hDevice, DEVICE_INFO_CMD iInfoCmd, INFO_DATATYPE *piType, void *pBuffer,
                      size_t *piSize);
    /**
     * @brief Close the device, all stream based on this device module should be inaccessible.
     *
     * @param hDevice handle of the GenTL device module
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API DevClose(DEV_HANDLE hDevice);

    /**
     * @brief Notify the GenTL stream module to use the allocated buffer for grabing stream
     *
     * @param hDataStream handle of the GenTL stream module
     * @param pBuffer point the allocated memory
     * @param iSize size of the buffer
     * @param pPrivate user-defined data attached to the buffer
     * @param phBuffer handle of the buffer, when the function invoked, user can not directly access the buffer but use
     * GenTL api with this handle.
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API DSAnnounceBuffer(DS_HANDLE hDataStream, void *pBuffer, size_t iSize, void *pPrivate,
                            BUFFER_HANDLE *phBuffer);

    /**
     * @brief Notift the GenTL stream module to allocate buffer for grabing stream
     *
     * @param hDataStream handle of the GenTL stream module
     * @param iSize size of the buffer should be allocated bt GenTL stream module
     * @param pPrivate ser-defined data attached to the buffer
     * @param phBuffer handle of the internal buffer
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API DSAllocAndAnnounceBuffer(DS_HANDLE hDataStream, size_t iSize, void *pPrivate, BUFFER_HANDLE *phBuffer);

    /**
     * @brief Clear the buffer queue which wait to be filled stream (image) data
     *
     * @param hDataStream handle of the GenTL stream module
     * @param iOperation ACQ_QUEUE_TYPE enum
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API DSFlushQueue(DS_HANDLE hDataStream, ACQ_QUEUE_TYPE iOperation);

    /**
     * @brief Start stream acquistion
     *
     * @param hDataStream handle of the GenTL stream module
     * @param iStartFlags ACQ_START_FLAGS enum
     * @param iNumToAcquire number of frame should be grabbed. Int64_t(-1) means infinite.
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API DSStartAcquisition(DS_HANDLE hDataStream, ACQ_START_FLAGS iStartFlags, uint64_t iNumToAcquire);

    /**
     * @brief Stop the stream acquisition
     *
     * @param hDataStream handle of the GenTL stream module
     * @param iStopFlags ACQ_STOP_FLAGS enum
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API DSStopAcquisition(DS_HANDLE hDataStream, ACQ_STOP_FLAGS iStopFlags);
    /**
     * @brief Get some information of the stream
     *
     * @param hDataStream handle of the GenTL stream module
     * @param iInfoCmd STREAM_INFO_CMD enum
     * @param piType type of the data
     * @param pBuffer data storaged
     * @param piSize size of the data filled, if pBuffer eauqals null, that means the samllet buffer size should be
     * provided
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API DSGetInfo(DS_HANDLE hDataStream, STREAM_INFO_CMD iInfoCmd, INFO_DATATYPE *piType, void *pBuffer,
                     size_t *piSize);

    /**
     * @brief Get buffer handle by Index
     *
     * @param hDataStream handle of the GenTL stream module
     * @param iIndex index of the buffer
     * @param phBuffer handle of the GenTL buffer
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API DSGetBufferID(DS_HANDLE hDataStream, uint32_t iIndex, BUFFER_HANDLE *phBuffer);

    /**
     * @brief Close the stream
     *
     * @param hDataStream handle of the GenTL stream module
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API DSClose(DS_HANDLE hDataStream);

    /**
     * @brief revoke the buffer attach to the GenTL stream module
     *
     * @param hDataStream handle of the GenTL stream module
     * @param hBuffer handle of the GenTL buffer
     * @param pBuffer C pointer can free (call free(pBuffer)) the buffer
     * @param pPrivate C pointer point the user defined data which attach the buffer before.
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API DSRevokeBuffer(DS_HANDLE hDataStream, BUFFER_HANDLE hBuffer, void **pBuffer, void **pPrivate);

    /**
     * @brief Let the stream-attached buffer ready for grabing stream data
     *
     * @param hDataStream handle of the GenTL stream module
     * @param hBuffer handle of the GenTL buffer
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API DSQueueBuffer(DS_HANDLE hDataStream, BUFFER_HANDLE hBuffer);

    /**
     * @brief Get some information of the buffer
     *
     * @param hDataStream handle of the GenTL stream module
     * @param hBuffer handle of the GenTL buffer
     * @param iInfoCmd BUFFER_INFO_CMD enum
     * @param piType type of the data
     * @param pBuffer data storaged
     * @param piSize size of the data filled, if pBuffer eauqals null, that means the samllet buffer size should be
     * provided
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API DSGetBufferInfo(DS_HANDLE hDataStream, BUFFER_HANDLE hBuffer, BUFFER_INFO_CMD iInfoCmd,
                           INFO_DATATYPE *piType, void *pBuffer, size_t *piSize);

    // start customize
    /**
     * @brief Read the var of the GenTL object with var name
     *
     * @param hPort handle of the GenTL object,such as system/interface/device/stream,
                    notice that buffer is not implemented as common GenTL module
     * @param varName name of the specific register
     * @param piType type of the data
     * @param pBuffer data storaged
     * @param piSize size of the data filled
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API GCReadPortByName(PORT_HANDLE hPort, const char *varName, INFO_DATATYPE *piType, void *pBuffer,
                            size_t *piSize);
    /**
     * @brief Write the var of the GenTL object with var name
     *
     * @param hPort handle of the GenTL object,such as system/interface/device/stream,
                    notice that buffer is not implemented as common GenTL module
     * @param varName name of the specific register
     * @param piType type of the data
     * @param pBuffer data storaged
     * @param piSize size of the data filled
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API GCWritePortByName(PORT_HANDLE hPort, const char *varName, INFO_DATATYPE *piType, const void *pBuffer,
                             size_t *piSize);

    /**
     * @brief Get some information of the buffer (Simplified version compared with the standard GenTL version:
     * DSGetBufferInfo)
     *
     * @param hBuffer handle of the GenTL buffer
     * @param iInfoCmd BUFFER_INFO_CMD enum
     * @param piType type of the data
     * @param pBuffer data storaged
     * @param piSize size of the data filled
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API PDBufferGetInfo(BUFFER_HANDLE hBuffer, BUFFER_INFO_CMD iInfoCmd, INFO_DATATYPE *piType, void *pBuffer,
                           size_t *piSize);
    /**
     * @brief Get stream metadata information of the buffer
     *
     * @param hBuffer handle of the GenTL buffer
     * @param index index of the metadata item
     * @param piType type of the data
     * @param pBuffer data storaged
     * @param piSize size of the data filled
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API PDBufferGetMeta(BUFFER_HANDLE hBuffer, uint32_t index, INFO_DATATYPE *piType, void *pBuffer, size_t *piSize);

    /**
     * @brief Get stream metadata information of the buffer
     *
     * @param hBuffer handle of the GenTL buffer
     * @param varName name of the specific metadata var
     * @param piType type of the data
     * @param pBuffer data storaged
     * @param piSize size of the data filled
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API PDBufferGetMetaByName(BUFFER_HANDLE hBuffer, const char *varName, INFO_DATATYPE *piType, void *pBuffer,
                                 size_t *piSize);

    /**
     * @brief Get name and the describe information of the metadata item
     *
     * @param hBuffer handle of the GenTL buffer
     * @param index index of the metadata item
     * @param ppVarName pointer will be assigned to point to the name of the metadata item
     * @param ppDescName pointer will be assigned to point to the description of the metadata item
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API PDBufferGetMetaDesc(BUFFER_HANDLE hBuffer, uint32_t index, const char **ppVarName, const char **ppDescName);

    /**
     * @brief Get number of the metadata items
     *
     * @param hBuffer handle of the GenTL buffer
     * @param pMetaItemNum number of the metadata items
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API PDBufferGetMetaNum(BUFFER_HANDLE hBuffer, size_t *pMetaItemNum);

    /**
     * @brief Write the buffer to disk
     *
     * @param hBuffer handle of the GenTL buffer
     * @param filesName save as filesName.suffix, suffix is determined by SDK. Nullptr is also accepted, and then
     *  name will be constructed by SDK itself.
     * @param opt 0: save part buffers; 1: save ply with color if color exist; 2: save ply patch with color if color
     * exist; others: reserved
     * @return GC_ERR_SUCCESS or other errcode
     */

    GC_API PDBufferSave(BUFFER_HANDLE hBuffer, const char *filesName, int opt);
    // end customize

    /* GenTL v1.1 */
    /**
     * @brief Get the number of usrls the GenTL object belongs to
     *
     * @param hPort handle of the GenTL object,such as system/interface/device/stream,
                notice that buffer is not implemented as common GenTL module
     * @param piNumURLs number of the URLS the GenTL object belongs to
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API GCGetNumPortURLs(PORT_HANDLE hPort, uint32_t *piNumURLs);

    /**
     * @brief Get some informations of the selected url the GenTL object belongs to
     *
     * @param hPort  handle of the GenTL object,such as system/interface/device/stream,
                notice that buffer is not implemented as common GenTL module
     * @param iURLIndex index of the URL
     * @param iInfoCmd URL_INFO_CMD enum
        * @param piType type of the value the buffer filled
        * @param pBuffer data storaged
        * @param piSize size of the data filled, if pBuffer eauqals null, that means the samllet buffer size should be
     provided
        * @return GC_ERR_SUCCESS or other errcode
 */
    GC_API GCGetPortURLInfo(PORT_HANDLE hPort, uint32_t iURLIndex, URL_INFO_CMD iInfoCmd, INFO_DATATYPE *piType,
                            void *pBuffer, size_t *piSize);

    /**
     * @brief A set of GCReadPort operations
     *
     * @param hPort handle of the GenTL object,such as system/interface/device/stream,
                    notice that buffer is not implemented as common GenTL module
     * @param pEntries entries of the GCReadPort arguments
     * @param piNumEntries number of operations
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API GCReadPortStacked(PORT_HANDLE hPort, PORT_REGISTER_STACK_ENTRY *pEntries, size_t *piNumEntries);

    /**
     * @brief A set of GCWritePort operations
     *
     * @param hPort handle of the GenTL object,such as system/interface/device/stream,
                notice that buffer is not implemented as common GenTL module
     * @param pEntries entries of the GCReadPort arguments
     * @param piNumEntries number of operations
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API GCWritePortStacked(PORT_HANDLE hPort, PORT_REGISTER_STACK_ENTRY *pEntries, size_t *piNumEntries);

    /* GenTL v1.3 */
    GC_API DSGetBufferChunkData(DS_HANDLE hDataStream, BUFFER_HANDLE hBuffer, SINGLE_CHUNK_DATA *pChunkData,
                                size_t *piNumChunks);

    /* GenTL v1.4 */
    /**
     * @brief Get the GenTL system module which current interface is based on
     *
     * @param hIface handle of the GenTL interface module
     * @param phSystem handle of the GenTL system module
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API IFGetParentTL(IF_HANDLE hIface, TL_HANDLE *phSystem);

    /**
     * @brief Get the GenTL interface module which current device is based on
     *
     * @param hDevice handle of the GenTL device module
     * @param phIface handle of the GenTL interface module
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API DevGetParentIF(DEV_HANDLE hDevice, IF_HANDLE *phIface);

    /**
     * @brief Get the GenTL device module which current stream is based on
     *
     * @param hDataStream  handle of the GenTL stream module
     * @param phDevice handle of the GenTL device module
     * @return GC_API
     */
    GC_API DSGetParentDev(DS_HANDLE hDataStream, DEV_HANDLE *phDevice);

    /* GenTL v1.5 */
    /**
     * @brief Get the number of parts of the buffer
     *
     * @param hDataStream  handle of the GenTL stream module
     * @param hBuffer handle of the internal buffer
     * @param piNumParts number of the buffer parts
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API DSGetNumBufferParts(DS_HANDLE hDataStream, BUFFER_HANDLE hBuffer, uint32_t *piNumParts);

    /**
     * @brief Get some information of the specific part of the buffer
     *
     * @param hDataStream handle of the GenTL stream module
     * @param hBuffer  handle of the internal buffer
     * @param iPartIndex index of the part
     * @param iInfoCmd BUFFER_PART_INFO_CMD enum
     * @param piType type of the data
     * @param pBuffer data storaged
     * @param piSize size of the data filled, if pBuffer eauqals null, that means the samllet buffer size should be
     * provided
     * @return GC_ERR_SUCCESS or other errcode
     */
    GC_API DSGetBufferPartInfo(DS_HANDLE hDataStream, BUFFER_HANDLE hBuffer, uint32_t iPartIndex,
                               BUFFER_PART_INFO_CMD iInfoCmd, INFO_DATATYPE *piType, void *pBuffer, size_t *piSize);

    /* GenTL v1.6 */
    GC_API DSAnnounceCompositeBuffer(DS_HANDLE hDataStream, size_t iNumSegments, void **ppSegments, size_t *piSizes,
                                     void *pPrivate, BUFFER_HANDLE *phBuffer);
    GC_API DSGetBufferInfoStacked(DS_HANDLE hDataStream, BUFFER_HANDLE hBuffer, DS_BUFFER_INFO_STACKED *pInfoStacked,
                                  size_t iNumInfos);
    GC_API DSGetBufferPartInfoStacked(DS_HANDLE hDataStream, BUFFER_HANDLE hBuffer,
                                      DS_BUFFER_PART_INFO_STACKED *pInfoStacked, size_t iNumInfos);
    GC_API DSGetNumFlows(DS_HANDLE hDataStream, uint32_t *piNumFlows);
    GC_API DSGetFlowInfo(DS_HANDLE hDataStream, uint32_t iFlowIndex, FLOW_INFO_CMD iInfoCmd, INFO_DATATYPE *piType,
                         void *pBuffer, size_t *piSize);
    GC_API DSGetNumBufferSegments(DS_HANDLE hDataStream, BUFFER_HANDLE hBuffer, uint32_t *piNumSegments);
    GC_API DSGetBufferSegmentInfo(DS_HANDLE hDataStream, BUFFER_HANDLE hBuffer, uint32_t iSegmentIndex,
                                  SEGMENT_INFO_CMD iInfoCmd, INFO_DATATYPE *piType, void *pBuffer, size_t *piSize);

    /* typedefs for dynamic loading */
#define GC_API_P(function) typedef GC_ERROR(GC_CALLTYPE *function)
    GC_API_P(PGCEnableDebugLoggerLevel)
    (void);
    GC_API_P(PGCGetInfo)
    (TL_INFO_CMD iInfoCmd, INFO_DATATYPE *piType, void *pBuffer, size_t *piSize);
    GC_API_P(PGCGetLastError)
    (GC_ERROR *piErrorCode, char *sErrText, size_t *piSize);
    GC_API_P(PGCInitLib)
    (void);
    GC_API_P(PGCCloseLib)
    (void);
    GC_API_P(PGCReadPort)
    (PORT_HANDLE hPort, uint64_t iAddress, void *pBuffer, size_t *piSize);
    GC_API_P(PGCWritePort)
    (PORT_HANDLE hPort, uint64_t iAddress, const void *pBuffer, size_t *piSize);
    GC_API_P(PGCGetPortURL)
    (PORT_HANDLE hPort, char *sURL, size_t *piSize);
    GC_API_P(PGCGetPortInfo)
    (PORT_HANDLE hPort, PORT_INFO_CMD iInfoCmd, INFO_DATATYPE *piType, void *pBuffer, size_t *piSize);

    GC_API_P(PGCRegisterEvent)
    (EVENTSRC_HANDLE hEventSrc, EVENT_TYPE iEventID, EVENT_HANDLE *phEvent);
    GC_API_P(PGCUnregisterEvent)
    (EVENTSRC_HANDLE hEventSrc, EVENT_TYPE iEventID);
    GC_API_P(PEventGetData)
    (EVENT_HANDLE hEvent, void *pBuffer, size_t *piSize, uint64_t iTimeout);
    GC_API_P(PEventGetDataInfo)
    (EVENT_HANDLE hEvent, const void *pInBuffer, size_t iInSize, EVENT_DATA_INFO_CMD iInfoCmd, INFO_DATATYPE *piType,
     void *pOutBuffer, size_t *piOutSize);
    GC_API_P(PEventGetInfo)
    (EVENT_HANDLE hEvent, EVENT_INFO_CMD iInfoCmd, INFO_DATATYPE *piType, void *pBuffer, size_t *piSize);
    GC_API_P(PEventFlush)
    (EVENT_HANDLE hEvent);
    GC_API_P(PEventKill)
    (EVENT_HANDLE hEvent);
    GC_API_P(PTLOpen)
    (TL_HANDLE *phTL);
    GC_API_P(PTLClose)
    (TL_HANDLE hTL);
    GC_API_P(PTLGetInfo)
    (TL_HANDLE hTL, TL_INFO_CMD iInfoCmd, INFO_DATATYPE *piType, void *pBuffer, size_t *piSize);
    GC_API_P(PTLGetNumInterfaces)
    (TL_HANDLE hTL, uint32_t *piNumIfaces);
    GC_API_P(PTLGetInterfaceID)
    (TL_HANDLE hTL, uint32_t iIndex, char *sID, size_t *piSize);
    GC_API_P(PTLGetInterfaceInfo)
    (TL_HANDLE hTL, const char *sIfaceID, INTERFACE_INFO_CMD iInfoCmd, INFO_DATATYPE *piType, void *pBuffer,
     size_t *piSize);
    GC_API_P(PTLOpenInterface)
    (TL_HANDLE hTL, const char *sIfaceID, IF_HANDLE *phIface);
    GC_API_P(PTLUpdateInterfaceList)
    (TL_HANDLE hTL, bool8_t *pbChanged, uint64_t iTimeout);
    GC_API_P(PIFClose)
    (IF_HANDLE hIface);
    GC_API_P(PIFGetInfo)
    (IF_HANDLE hIface, INTERFACE_INFO_CMD iInfoCmd, INFO_DATATYPE *piType, void *pBuffer, size_t *piSize);
    GC_API_P(PIFGetNumDevices)
    (IF_HANDLE hIface, uint32_t *piNumDevices);
    GC_API_P(PIFGetDeviceID)
    (IF_HANDLE hIface, uint32_t iIndex, char *sIDeviceID, size_t *piSize);
    GC_API_P(PIFUpdateDeviceList)
    (IF_HANDLE hIface, bool8_t *pbChanged, uint64_t iTimeout);
    GC_API_P(PIFGetDeviceInfo)
    (IF_HANDLE hIface, const char *sDeviceID, DEVICE_INFO_CMD iInfoCmd, INFO_DATATYPE *piType, void *pBuffer,
     size_t *piSize);
    GC_API_P(PIFOpenDevice)
    (IF_HANDLE hIface, const char *sDeviceID, DEVICE_ACCESS_FLAGS iOpenFlag, DEV_HANDLE *phDevice);

    GC_API_P(PDevGetPort)
    (DEV_HANDLE hDevice, PORT_HANDLE *phRemoteDevice);
    GC_API_P(PDevGetNumDataStreams)
    (DEV_HANDLE hDevice, uint32_t *piNumDataStreams);
    GC_API_P(PDevGetDataStreamID)
    (DEV_HANDLE hDevice, uint32_t iIndex, char *sDataStreamID, size_t *piSize);
    GC_API_P(PDevOpenDataStream)
    (DEV_HANDLE hDevice, const char *sDataStreamID, DS_HANDLE *phDataStream);
    GC_API_P(PDevGetInfo)
    (DEV_HANDLE hDevice, DEVICE_INFO_CMD iInfoCmd, INFO_DATATYPE *piType, void *pBuffer, size_t *piSize);
    GC_API_P(PDevClose)
    (DEV_HANDLE hDevice);

    GC_API_P(PDSAnnounceBuffer)
    (DS_HANDLE hDataStream, void *pBuffer, size_t iSize, void *pPrivate, BUFFER_HANDLE *phBuffer);
    GC_API_P(PDSAllocAndAnnounceBuffer)
    (DS_HANDLE hDataStream, size_t iSize, void *pPrivate, BUFFER_HANDLE *phBuffer);
    GC_API_P(PDSFlushQueue)
    (DS_HANDLE hDataStream, ACQ_QUEUE_TYPE iOperation);
    GC_API_P(PDSStartAcquisition)
    (DS_HANDLE hDataStream, ACQ_START_FLAGS iStartFlags, uint64_t iNumToAcquire);
    GC_API_P(PDSStopAcquisition)
    (DS_HANDLE hDataStream, ACQ_STOP_FLAGS iStopFlags);
    GC_API_P(PDSGetInfo)
    (DS_HANDLE hDataStream, STREAM_INFO_CMD iInfoCmd, INFO_DATATYPE *piType, void *pBuffer, size_t *piSize);
    GC_API_P(PDSGetBufferID)
    (DS_HANDLE hDataStream, uint32_t iIndex, BUFFER_HANDLE *phBuffer);
    GC_API_P(PDSClose)
    (DS_HANDLE hDataStream);
    GC_API_P(PDSRevokeBuffer)
    (DS_HANDLE hDataStream, BUFFER_HANDLE hBuffer, void **pBuffer, void **pPrivate);
    GC_API_P(PDSQueueBuffer)
    (DS_HANDLE hDataStream, BUFFER_HANDLE hBuffer);
    GC_API_P(PDSGetBufferInfo)
    (DS_HANDLE hDataStream, BUFFER_HANDLE hBuffer, BUFFER_INFO_CMD iInfoCmd, INFO_DATATYPE *piType, void *pBuffer,
     size_t *piSize);

    // start customize
    GC_API_P(PPDBufferGetInfo)
    (BUFFER_HANDLE hBuffer, BUFFER_INFO_CMD iInfoCmd, INFO_DATATYPE *piType, void *pBuffer, size_t *piSize);
    GC_API_P(PPDBufferGetMeta)
    (BUFFER_HANDLE hBuffer, uint32_t index, INFO_DATATYPE *piType, void *buffer, size_t *piSize);
    GC_API_P(PPDBufferGetMetaByName)
    (BUFFER_HANDLE hBuffer, const char *name, INFO_DATATYPE *piType, void *buffer, size_t *piSize);
    GC_API_P(PPDBufferGetMetaDesc)
    (BUFFER_HANDLE hBuffer, uint32_t index, const char **ppVarName, const char **ppDescName);
    GC_API_P(PPDBufferGetMetaNum)(BUFFER_HANDLE hBuffer, size_t *pMetaItemNum);
    GC_API_P(PPDBufferSave)(BUFFER_HANDLE hBuffer, const char *name, int opt);
    // end customize

    /* GenTL v1.1 */
    GC_API_P(PGCGetNumPortURLs)
    (PORT_HANDLE hPort, uint32_t *iNumURLs);
    GC_API_P(PGCGetPortURLInfo)
    (PORT_HANDLE hPort, uint32_t iURLIndex, URL_INFO_CMD iInfoCmd, INFO_DATATYPE *piType, void *pBuffer,
     size_t *piSize);
    GC_API_P(PGCReadPortStacked)
    (PORT_HANDLE hPort, PORT_REGISTER_STACK_ENTRY *pEntries, size_t *piNumEntries);
    GC_API_P(PGCWritePortStacked)
    (PORT_HANDLE hPort, PORT_REGISTER_STACK_ENTRY *pEntries, size_t *piNumEntries);

    /* GenTL v1.3 */
    GC_API_P(PDSGetBufferChunkData)
    (DS_HANDLE hDataStream, BUFFER_HANDLE hBuffer, SINGLE_CHUNK_DATA *pChunkData, size_t *piNumChunks);

    /* GenTL v1.4 */
    GC_API_P(PIFGetParentTL)
    (IF_HANDLE hIface, TL_HANDLE *phSystem);
    GC_API_P(PDevGetParentIF)
    (DEV_HANDLE hDevice, IF_HANDLE *phIface);
    GC_API_P(PDSGetParentDev)
    (DS_HANDLE hDataStream, DEV_HANDLE *phDevice);

    /* GenTL v1.5 */
    GC_API_P(PDSGetNumBufferParts)
    (DS_HANDLE hDataStream, BUFFER_HANDLE hBuffer, uint32_t *piNumParts);
    GC_API_P(PDSGetBufferPartInfo)
    (DS_HANDLE hDataStream, BUFFER_HANDLE hBuffer, uint32_t iPartIndex, BUFFER_PART_INFO_CMD iInfoCmd,
     INFO_DATATYPE *piType, void *pBuffer, size_t *piSize);

    /* GenTL v1.6 */
    GC_API_P(PDSAnnounceCompositeBuffer)
    (DS_HANDLE hDataStream, size_t iNumSegments, void **ppSegments, size_t *piSizes, void *pPrivate,
     BUFFER_HANDLE *phBuffer);
    GC_API_P(PDSGetBufferInfoStacked)
    (DS_HANDLE hDataStream, BUFFER_HANDLE hBuffer, DS_BUFFER_INFO_STACKED *pInfoStacked, size_t iNumInfos);
    GC_API_P(PDSGetBufferPartInfoStacked)
    (DS_HANDLE hDataStream, BUFFER_HANDLE hBuffer, DS_BUFFER_PART_INFO_STACKED *pInfoStacked, size_t iNumInfos);
    GC_API_P(PDSGetNumFlows)
    (DS_HANDLE hDataStream, uint32_t *piNumFlows);
    GC_API_P(PDSGetFlowInfo)
    (DS_HANDLE hDataStream, uint32_t iFlowIndex, FLOW_INFO_CMD iInfoCmd, INFO_DATATYPE *piType, void *pBuffer,
     size_t *piSize);
    GC_API_P(PDSGetNumBufferSegments)
    (DS_HANDLE hDataStream, BUFFER_HANDLE hBuffer, uint32_t *piNumSegments);
    GC_API_P(PDSGetBufferSegmentInfo)
    (DS_HANDLE hDataStream, BUFFER_HANDLE hBuffer, uint32_t iSegmentIndex, SEGMENT_INFO_CMD iInfoCmd,
     INFO_DATATYPE *piType, void *pBuffer, size_t *piSize);
#ifdef __cplusplus
    } /* end of namespace GenTL */
} /* end of extern "C" */
#endif
#endif /* GC_TLI_CLIENT_H_ */
