#pragma once
#ifdef __cplusplus
extern "C" {
#endif 
#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>

#include<stdio.h>
#include<stdlib.h>

#define	T(s) 				s

#define PUBLIC
#define PUBLIC_DATA extern
#define PRIVATE     static

#ifndef ssize_t
    #define ssize_t int
#endif

#ifndef ssize
    #define ssize int
#endif

/**
    Constant char data type.
*/
typedef const char cchar;

/**
    Hash table ID returned by hashCreate
 */
typedef int WebsHash;                       /* Returned by symCreate */

/**
    Callback for write I/O events
 */
typedef void (*WebsWriteProc)(struct Webs *wp);

/************************************ Overrides *******************************/
/*
    Override osdep defaults
 */
#define ME_MAX_IP 64                /**< Maximum IP address size */

/* Settings */
#ifndef ME_AUTHOR
    #define ME_AUTHOR "Embedthis Software"
#endif
#ifndef ME_CERTS_BITS
    #define ME_CERTS_BITS 2048
#endif
#ifndef ME_CERTS_DAYS
    #define ME_CERTS_DAYS 3650
#endif
#ifndef ME_CERTS_GENDH
    #define ME_CERTS_GENDH 1
#endif
#ifndef ME_COMPANY
    #define ME_COMPANY "embedthis"
#endif
#ifndef ME_COMPATIBLE
    #define ME_COMPATIBLE "3.6"
#endif
#ifndef ME_COMPILER_FORTIFY
    #define ME_COMPILER_FORTIFY 1
#endif
#ifndef ME_COMPILER_HAS_ATOMIC
    #define ME_COMPILER_HAS_ATOMIC 0
#endif
#ifndef ME_COMPILER_HAS_ATOMIC64
    #define ME_COMPILER_HAS_ATOMIC64 0
#endif
#ifndef ME_COMPILER_HAS_DOUBLE_BRACES
    #define ME_COMPILER_HAS_DOUBLE_BRACES 0
#endif
#ifndef ME_COMPILER_HAS_DYN_LOAD
    #define ME_COMPILER_HAS_DYN_LOAD 1
#endif
#ifndef ME_COMPILER_HAS_LIB_EDIT
    #define ME_COMPILER_HAS_LIB_EDIT 0
#endif
#ifndef ME_COMPILER_HAS_LIB_RT
    #define ME_COMPILER_HAS_LIB_RT 1
#endif
#ifndef ME_COMPILER_HAS_MMU
    #define ME_COMPILER_HAS_MMU 1
#endif
#ifndef ME_COMPILER_HAS_MTUNE
    #define ME_COMPILER_HAS_MTUNE 1
#endif
#ifndef ME_COMPILER_HAS_PAM
    #define ME_COMPILER_HAS_PAM 0
#endif
#ifndef ME_COMPILER_HAS_STACK_PROTECTOR
    #define ME_COMPILER_HAS_STACK_PROTECTOR 1
#endif
#ifndef ME_COMPILER_HAS_SYNC
    #define ME_COMPILER_HAS_SYNC 1
#endif
#ifndef ME_COMPILER_HAS_SYNC64
    #define ME_COMPILER_HAS_SYNC64 1
#endif
#ifndef ME_COMPILER_HAS_SYNC_CAS
    #define ME_COMPILER_HAS_SYNC_CAS 0
#endif
#ifndef ME_COMPILER_HAS_UNNAMED_UNIONS
    #define ME_COMPILER_HAS_UNNAMED_UNIONS 1
#endif
#ifndef ME_COMPILER_WARN64TO32
    #define ME_COMPILER_WARN64TO32 0
#endif
#ifndef ME_COMPILER_WARN_UNUSED
    #define ME_COMPILER_WARN_UNUSED 1
#endif
#ifndef ME_DEBUG
    #define ME_DEBUG 1
#endif
#ifndef ME_DEPTH
    #define ME_DEPTH 1
#endif
#ifndef ME_DESCRIPTION
    #define ME_DESCRIPTION "Embedthis GoAhead Embedded Web Server"
#endif
#ifndef ME_GOAHEAD_ACCESS_LOG
    #define ME_GOAHEAD_ACCESS_LOG 0
#endif
#ifndef ME_GOAHEAD_AUTH
    #define ME_GOAHEAD_AUTH 1
#endif
#ifndef ME_GOAHEAD_AUTH_STORE
    #define ME_GOAHEAD_AUTH_STORE "file"
#endif
#ifndef ME_GOAHEAD_AUTO_LOGIN
    #define ME_GOAHEAD_AUTO_LOGIN 0
#endif
#ifndef ME_GOAHEAD_CGI
    #define ME_GOAHEAD_CGI 1
#endif
#ifndef ME_GOAHEAD_CLIENT_CACHE
    #define ME_GOAHEAD_CLIENT_CACHE "css,gif,ico,jpg,js,png"
#endif
#ifndef ME_GOAHEAD_CLIENT_CACHE_LIFESPAN
    #define ME_GOAHEAD_CLIENT_CACHE_LIFESPAN 86400
#endif
#ifndef ME_GOAHEAD_DIGEST
    #define ME_GOAHEAD_DIGEST 1
#endif
#ifndef ME_GOAHEAD_DOCUMENTS
    #define ME_GOAHEAD_DOCUMENTS "web"
#endif
#ifndef ME_GOAHEAD_JAVASCRIPT
    #define ME_GOAHEAD_JAVASCRIPT 1
#endif
#ifndef ME_GOAHEAD_LEGACY
    #define ME_GOAHEAD_LEGACY 0
#endif
#ifndef ME_GOAHEAD_LIMIT_BUFFER
    #define ME_GOAHEAD_LIMIT_BUFFER 1024
#endif
#ifndef ME_GOAHEAD_LIMIT_FILENAME
    #define ME_GOAHEAD_LIMIT_FILENAME 256
#endif
#ifndef ME_GOAHEAD_LIMIT_FILES
    #define ME_GOAHEAD_LIMIT_FILES 0
#endif
#ifndef ME_GOAHEAD_LIMIT_HEADER
    #define ME_GOAHEAD_LIMIT_HEADER 2048
#endif
#ifndef ME_GOAHEAD_LIMIT_HEADERS
    #define ME_GOAHEAD_LIMIT_HEADERS 4096
#endif
#ifndef ME_GOAHEAD_LIMIT_NUM_HEADERS
    #define ME_GOAHEAD_LIMIT_NUM_HEADERS 64
#endif
#ifndef ME_GOAHEAD_LIMIT_PARSE_TIMEOUT
    #define ME_GOAHEAD_LIMIT_PARSE_TIMEOUT 5
#endif
#ifndef ME_GOAHEAD_LIMIT_PASSWORD
    #define ME_GOAHEAD_LIMIT_PASSWORD 32
#endif
#ifndef ME_GOAHEAD_LIMIT_POST
    #define ME_GOAHEAD_LIMIT_POST 16384
#endif
#ifndef ME_GOAHEAD_LIMIT_PUT
    #define ME_GOAHEAD_LIMIT_PUT 204800000
#endif
#ifndef ME_GOAHEAD_LIMIT_SESSION_COUNT
    #define ME_GOAHEAD_LIMIT_SESSION_COUNT 512
#endif
#ifndef ME_GOAHEAD_LIMIT_SESSION_LIFE
    #define ME_GOAHEAD_LIMIT_SESSION_LIFE 1800
#endif
#ifndef ME_GOAHEAD_LIMIT_STRING
    #define ME_GOAHEAD_LIMIT_STRING 256
#endif
#ifndef ME_GOAHEAD_LIMIT_TIMEOUT
    #define ME_GOAHEAD_LIMIT_TIMEOUT 60
#endif
#ifndef ME_GOAHEAD_LIMIT_UPLOAD
    #define ME_GOAHEAD_LIMIT_UPLOAD 204800000
#endif
#ifndef ME_GOAHEAD_LIMIT_URI
    #define ME_GOAHEAD_LIMIT_URI 2048
#endif
#ifndef ME_GOAHEAD_LISTEN
    #define ME_GOAHEAD_LISTEN "http://*:80,https://*:443"
#endif
#ifndef ME_GOAHEAD_LOGFILE
    #define ME_GOAHEAD_LOGFILE "stderr:0"
#endif
#ifndef ME_GOAHEAD_LOGGING
    #define ME_GOAHEAD_LOGGING 1
#endif
#ifndef ME_GOAHEAD_PUT_DIR
    #define ME_GOAHEAD_PUT_DIR "."
#endif
#ifndef ME_GOAHEAD_REALM
    #define ME_GOAHEAD_REALM "example.com"
#endif
#ifndef ME_GOAHEAD_REPLACE_MALLOC
    #define ME_GOAHEAD_REPLACE_MALLOC 0
#endif
#ifndef ME_GOAHEAD_SSL_AUTHORITY
    #define ME_GOAHEAD_SSL_AUTHORITY ""
#endif
#ifndef ME_GOAHEAD_SSL_CACHE
    #define ME_GOAHEAD_SSL_CACHE 512
#endif
#ifndef ME_GOAHEAD_SSL_CERTIFICATE
    #define ME_GOAHEAD_SSL_CERTIFICATE "self.crt"
#endif
#ifndef ME_GOAHEAD_SSL_CIPHERS
    #define ME_GOAHEAD_SSL_CIPHERS ""
#endif
#ifndef ME_GOAHEAD_SSL_HANDSHAKES
    #define ME_GOAHEAD_SSL_HANDSHAKES 1
#endif
#ifndef ME_GOAHEAD_SSL_KEY
    #define ME_GOAHEAD_SSL_KEY "self.key"
#endif
#ifndef ME_GOAHEAD_SSL_LOG_LEVEL
    #define ME_GOAHEAD_SSL_LOG_LEVEL 5
#endif
#ifndef ME_GOAHEAD_SSL_REVOKE
    #define ME_GOAHEAD_SSL_REVOKE ""
#endif
#ifndef ME_GOAHEAD_SSL_TICKET
    #define ME_GOAHEAD_SSL_TICKET 1
#endif
#ifndef ME_GOAHEAD_SSL_TIMEOUT
    #define ME_GOAHEAD_SSL_TIMEOUT 86400
#endif
#ifndef ME_GOAHEAD_SSL_VERIFY_ISSUER
    #define ME_GOAHEAD_SSL_VERIFY_ISSUER 0
#endif
#ifndef ME_GOAHEAD_SSL_VERIFY_PEER
    #define ME_GOAHEAD_SSL_VERIFY_PEER 0
#endif
#ifndef ME_GOAHEAD_STEALTH
    #define ME_GOAHEAD_STEALTH 1
#endif
#ifndef ME_GOAHEAD_TRACING
    #define ME_GOAHEAD_TRACING 1
#endif
#ifndef ME_GOAHEAD_UPLOAD
    #define ME_GOAHEAD_UPLOAD 1
#endif
#ifndef ME_GOAHEAD_UPLOAD_DIR
    #define ME_GOAHEAD_UPLOAD_DIR "tmp"
#endif
#ifndef ME_GOAHEAD_XFRAME_HEADER
    #define ME_GOAHEAD_XFRAME_HEADER "SAMEORIGIN"
#endif
#ifndef ME_INTEGRATE
    #define ME_INTEGRATE 1
#endif
#ifndef ME_MANIFEST
    #define ME_MANIFEST "installs/manifest.me"
#endif
#ifndef ME_MBEDTLS_COMPACT
    #define ME_MBEDTLS_COMPACT 1
#endif
#ifndef ME_NAME
    #define ME_NAME "goahead"
#endif
#ifndef ME_PREFIXES
    #define ME_PREFIXES "install-prefixes"
#endif
#ifndef ME_ROM
    #define ME_ROM 0
#endif
#ifndef ME_TITLE
    #define ME_TITLE "Embedthis GoAhead"
#endif
#ifndef ME_VERSION
    #define ME_VERSION "3.6.3"
#endif

/* Prefixes */
#ifndef ME_ROOT_PREFIX
    #define ME_ROOT_PREFIX "/"
#endif
#ifndef ME_BASE_PREFIX
    #define ME_BASE_PREFIX "/usr/local"
#endif
#ifndef ME_DATA_PREFIX
    #define ME_DATA_PREFIX "/"
#endif
#ifndef ME_STATE_PREFIX
    #define ME_STATE_PREFIX "/var"
#endif
#ifndef ME_APP_PREFIX
    #define ME_APP_PREFIX "/usr/local/lib/goahead"
#endif
#ifndef ME_VAPP_PREFIX
    #define ME_VAPP_PREFIX "/usr/local/lib/goahead/3.6.3"
#endif
#ifndef ME_BIN_PREFIX
    #define ME_BIN_PREFIX "/usr/local/bin"
#endif
#ifndef ME_INC_PREFIX
    #define ME_INC_PREFIX "/usr/local/include"
#endif
#ifndef ME_LIB_PREFIX
    #define ME_LIB_PREFIX "/usr/local/lib"
#endif
#ifndef ME_MAN_PREFIX
    #define ME_MAN_PREFIX "/usr/local/share/man"
#endif
#ifndef ME_SBIN_PREFIX
    #define ME_SBIN_PREFIX "/usr/local/sbin"
#endif
#ifndef ME_ETC_PREFIX
    #define ME_ETC_PREFIX "/etc/goahead"
#endif
#ifndef ME_WEB_PREFIX
    #define ME_WEB_PREFIX "/var/www/goahead"
#endif
#ifndef ME_LOG_PREFIX
    #define ME_LOG_PREFIX "/var/log/goahead"
#endif
#ifndef ME_SPOOL_PREFIX
    #define ME_SPOOL_PREFIX "/var/spool/goahead"
#endif
#ifndef ME_CACHE_PREFIX
    #define ME_CACHE_PREFIX "/var/spool/goahead/cache"
#endif
#ifndef ME_SRC_PREFIX
    #define ME_SRC_PREFIX "goahead-3.6.3"
#endif

/* Suffixes */
#ifndef ME_EXE
    #define ME_EXE ""
#endif
#ifndef ME_SHLIB
    #define ME_SHLIB ".so"
#endif
#ifndef ME_SHOBJ
    #define ME_SHOBJ ".so"
#endif
#ifndef ME_LIB
    #define ME_LIB ".a"
#endif
#ifndef ME_OBJ
    #define ME_OBJ ".o"
#endif

/* Profile */
#ifndef ME_CONFIG_CMD
    #define ME_CONFIG_CMD "me -d -q -platform linux-x86-default -configure . -gen make"
#endif
#ifndef ME_GOAHEAD_PRODUCT
    #define ME_GOAHEAD_PRODUCT 1
#endif
#ifndef ME_PROFILE
    #define ME_PROFILE "default"
#endif
#ifndef ME_TUNE_SIZE
    #define ME_TUNE_SIZE 1
#endif

/* Miscellaneous */
#ifndef ME_MAJOR_VERSION
    #define ME_MAJOR_VERSION 3
#endif
#ifndef ME_MINOR_VERSION
    #define ME_MINOR_VERSION 6
#endif
#ifndef ME_PATCH_VERSION
    #define ME_PATCH_VERSION 3
#endif
#ifndef ME_VNUM
    #define ME_VNUM 300060003
#endif

/* Components */
#ifndef ME_COM_CC
    #define ME_COM_CC 1
#endif
#ifndef ME_COM_LIB
    #define ME_COM_LIB 1
#endif
#ifndef ME_COM_MATRIXSSL
    #define ME_COM_MATRIXSSL 0
#endif
#ifndef ME_COM_MBEDTLS
    #define ME_COM_MBEDTLS 1
#endif
#ifndef ME_COM_NANOSSL
    #define ME_COM_NANOSSL 0
#endif
#ifndef ME_COM_OPENSSL
    #define ME_COM_OPENSSL 0
#endif
#ifndef ME_COM_OSDEP
    #define ME_COM_OSDEP 1
#endif
#ifndef ME_COM_SSL
    #define ME_COM_SSL 1
#endif
#ifndef ME_COM_VXWORKS
    #define ME_COM_VXWORKS 0
#endif
/* Settings End*/


typedef char char_t;
typedef int sym_fd_t;						/* Returned by symOpen */



/**
    System native time type. This is the time in seconds.
    This may be 32 or 64 bits and may be signed or unsigned on some systems.
 */
typedef time_t WebsTime;

/**
    A WebsBuf (ring queue) allows maximum utilization of memory for data storage and is
    ideal for input/output buffering. This module provides a highly effecient
    implementation and a vehicle for dynamic strings.
    \n\n
    WARNING:  This is a public implementation and callers have full access to
    the queue structure and pointers.  Change this module very carefully.
    \n\n
    This module follows the open/close model.
    \n\n
    Operation of a WebsBuf where bp is a pointer to a WebsBuf :

        bp->buflen contains the size of the buffer.
        bp->buf will point to the start of the buffer.
        bp->servp will point to the first (un-consumed) data byte.
        bp->endp will point to the next free location to which new data is added
        bp->endbuf will point to one past the end of the buffer.
    \n\n
    Eg. If the WebsBuf contains the data "abcdef", it might look like :
    \n\n
    +-------------------------------------------------------------------+
    |   |   |   |   |   |   |   | a | b | c | d | e | f |   |   |   |   |
    +-------------------------------------------------------------------+
      ^                           ^                       ^               ^
      |                           |                       |               |
    bp->buf                    bp->servp               bp->endp      bp->enduf
    \n\n
    The queue is empty when servp == endp.  This means that the queue will hold
    at most bp->buflen -1 bytes.  It is the fillers responsibility to ensure
    the WebsBuf is never filled such that servp == endp.
    \n\n
    It is the fillers responsibility to "wrap" the endp back to point to
    bp->buf when the pointer steps past the end. Correspondingly it is the
    consumers responsibility to "wrap" the servp when it steps to bp->endbuf.
    The bufPutc and bufGetc routines will do this automatically.
    @defgroup WebsBuf WebsBuf
    @stability Stable
 */
typedef struct WebsBuf {
    char    *buf;               /**< Holding buffer for data */
    char    *servp;             /**< Pointer to start of data */
    char    *endp;              /**< Pointer to end of data */
    char    *endbuf;            /**< Pointer to end of buffer */
    ssize   buflen;             /**< Length of ring queue */
    ssize   maxsize;            /**< Maximum size */
    int     increment;          /**< Growth increment */
} WebsBuf;

/**
    Session state storage
    @defgroup WebsSession WebsSession
 */
typedef struct WebsSession {
    char            *id;                    /**< Session ID key */
    int             lifespan;               /**< Session inactivity timeout (msecs) */
    WebsTime        expires;                /**< When the session expires */
    WebsHash        cache;                  /**< Cache of session variables */
} WebsSession;



/**
    File upload structure
    @see websUploadOpen websLookupUpload websGetUpload
    @defgroup WebsUpload WebsUpload
 */
typedef struct WebsUpload {
    char    *filename;              /**< Local (temp) name of the file */
    char    *clientFilename;        /**< Client side name of the file */
    char    *contentType;           /**< Content type */
    ssize   size;                   /**< Uploaded file size */
} WebsUpload;

/**
    GoAhead request structure. This is a per-socket connection structure.
    @defgroup Webs Webs
 */
typedef struct Webs {
    WebsBuf         rxbuf;              /**< Raw receive buffer */
    WebsBuf         input;              /**< Receive buffer after de-chunking */
    WebsBuf         output;             /**< Transmit buffer after chunking */
    WebsBuf         chunkbuf;           /**< Pre-chunking data buffer */
    WebsBuf         *txbuf;
    WebsTime        since;              /**< Parsed if-modified-since time */
    WebsTime        timestamp;          /**< Last transaction with browser */
    WebsHash        vars;               /**< CGI standard variables */
    int             timeout;            /**< Timeout handle */
    char            ipaddr[ME_MAX_IP];  /**< Connecting ipaddress */
    char            ifaddr[ME_MAX_IP];  /**< Local interface ipaddress */

    int             rxChunkState;       /**< Rx chunk encoding state */
    ssize           rxChunkSize;        /**< Rx chunk size */
    char            *rxEndp;            /**< Pointer to end of raw data in input beyond endp */
    ssize           lastRead;           /**< Number of bytes last read from the socket */
    bool            eof;                /**< If at the end of the request content */

    char            txChunkPrefix[16];  /**< Transmit chunk prefix */
    char            *txChunkPrefixNext; /**< Current I/O pos in txChunkPrefix */
    ssize           txChunkPrefixLen;   /**< Length of prefix */
    ssize           txChunkLen;         /**< Length of the chunk */
    int             txChunkState;       /**< Transmit chunk state */

    char            *authDetails;       /**< Http header auth details */
    char            *authResponse;      /**< Outgoing auth header */
    char            *authType;          /**< Authorization type (Basic/DAA) */
    char            *contentType;       /**< Body content type */
    char            *cookie;            /**< Request cookie string */
    char            *decodedQuery;      /**< Decoded request query */
    char            *digest;            /**< Password digest */
    char            *ext;               /**< Path extension */
    char            *filename;          /**< Document path name */
    char            *host;              /**< Requested host */
    char            *method;            /**< HTTP request method */
    char            *password;          /**< Authorization password */
    char            *path;              /**< Path name without query. This is decoded. */
    char            *protoVersion;      /**< Protocol version (HTTP/1.1)*/
    char            *protocol;          /**< Protocol scheme (normally http|https) */
    char            *putname;           /**< PUT temporary filename */
    char            *query;             /**< Request query. This is decoded. */
    char            *realm;             /**< Realm field supplied in auth header */
    char            *referrer;          /**< The referring page */
    char            *responseCookie;    /**< Outgoing cookie */
    char            *url;               /**< Full request url. This is not decoded. */
    char            *userAgent;         /**< User agent (browser) */
    char            *username;          /**< Authorization username */
    int             sid;                /**< Socket id (handler) */
    int             listenSid;          /**< Listen Socket id */
    int             port;               /**< Request port number */
    int             state;              /**< Current state */
    int             flags;              /**< Current flags -- see above */
    int             code;               /**< Response status code */
    int             routeCount;         /**< Route count limiter */
    ssize           rxLen;              /**< Rx content length */
    ssize           rxRemaining;        /**< Remaining content to read from client */
    ssize           txLen;              /**< Tx content length header value */
    int             wid;                /**< Index into webs */
#if ME_GOAHEAD_CGI
    char            *cgiStdin;          /**< Filename for CGI program input */
    int             cgifd;              /**< File handle for CGI program input */
#endif
#if !ME_ROM
    int             putfd;              /**< File handle to write PUT data */
#endif
    int             docfd;              /**< File descriptor for document being served */
    ssize           written;            /**< Bytes actually transferred */
    ssize           putLen;             /**< Bytes read by a PUT request */

    int             finalized: 1;          /**< Request has been completed */
    int             error: 1;              /**< Request has an error */
    int             connError: 1;          /**< Request has a connection error */

    struct WebsSession *session;        /**< Session record */
    struct WebsRoute *route;            /**< Request route */
    struct WebsUser *user;              /**< User auth record */
    WebsWriteProc   writeData;          /**< Handler write I/O event callback. Used by fileHandler */
    int             encoded;            /**< True if the password is MD5(username:realm:password) */
#if ME_GOAHEAD_DIGEST
    char            *cnonce;            /**< check nonce */
    char            *digestUri;         /**< URI found in digest header */
    char            *nonce;             /**< opaque-to-client string sent by server */
    char            *nc;                /**< nonce count */
    char            *opaque;            /**< opaque value passed from server */
    char            *qop;               /**< quality operator */
#endif
#if ME_GOAHEAD_UPLOAD
    int             upfd;               /**< Upload file handle */
    WebsHash        files;              /**< Uploaded files */
    char            *boundary;          /**< Mime boundary (static) */
    ssize           boundaryLen;        /**< Boundary length */
    int             uploadState;        /**< Current file upload state */
    WebsUpload      *currentFile;       /**< Current file context */
    char            *clientFilename;    /**< Current file filename */
    char            *uploadTmp;         /**< Current temp filename for upload data */
    char            *uploadVar;         /**< Current upload form variable name */
#endif
    void            *ssl;               /**< SSL context */
} Webs;


#define websFormDefine websDefineAction
#define websAspDefine websDefineJst
/*
    Define a function in the "action" map space
 */
PUBLIC int websDefineAction(cchar *name, void *fn);

typedef int (*WebsJstProc)(int jid, Webs *wp, int argc, char **argv);
/*
    Define a Javascript function. Bind an Javascript name to a C procedure.
 */
PUBLIC int websDefineJst(const char *name, WebsJstProc fn);

ssize websWrite(Webs *wp, const char *fmt, ...);


PUBLIC int fmtValloc(char **s, int n, char *fmt, va_list arg);
PUBLIC int fmtAlloc(char **s, int n, char *fmt, ...);
PUBLIC void websFooter(Webs *wp);
PUBLIC void websHeader(Webs *wp);
PUBLIC int websPublish(char *prefix, char *path);
PUBLIC void websSetRequestFilename(Webs *wp, char *filename);

typedef int (*WebsLegacyHandlerProc)(Webs *wp, char *prefix, char *dir, int flags);
PUBLIC int websUrlHandlerDefine(char *prefix, char *dir, int arg, WebsLegacyHandlerProc handler, int flags);
PUBLIC void websDone(Webs *wp);
PUBLIC char *websGetVar(Webs *wp, const char *var, const char *defaultGetValue);


void setWebServerConfig(const char *RootWeb, const char *DefaultPage);
void setFirstPage(const char* page);
void setWebFilesPath(const char* path);
void work(void *Pfunc);


//
void *HttpServerMain(void *argv);


#ifdef __cplusplus
}
#endif 


