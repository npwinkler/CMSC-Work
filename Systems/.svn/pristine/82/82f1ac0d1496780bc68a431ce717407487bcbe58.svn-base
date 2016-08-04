/*
** cs154 Project 5 (p5ims) IM protocol message library
** Copyright (C)  2014 University of Chicago.
**
** Because you are a student in cs154, you received this file through
** the repositories that are maintained for cs154.  This file is not
** licensed for any wider distribution. Specifically, you may not
** allow this file to be copied or downloaded by anyone who is not a
** current cs154 student. To do so would be a violation of copyright.
**
** Header file for the protocol message types and their helper functions
**
** Note: "Protocol string" refers to the char* ASCII text that is transmitted
** over the wire.  "Protocol message" refers to the in-memory representation
** of the message (impMsg).  However, "Instant message" is the ASCII text
** message sent from one friend to another.
**
**
** by Gordon Kindlmann, new for cs154-2014
*/

#ifndef IMP_HAS_BEEN_INCLUDED
#define IMP_HAS_BEEN_INCLUDED

#include <stdarg.h>   /* for var args */
#include <stdio.h>    /* for FILE */

/*
** IMP_NAME_MAXLEN and IMP_IM_MAXLEN: limits on lengths of things
**
** These limits are not actually relevant for the imp library itself, and the
** imp library does NOT enforce them!  Its up to your server to check for
** violations of these limits.
**
** NOTE: the graders are free to change these values, recompile imp, and
** recompile your server prior to grading.
*/
#define IMP_NAME_MAXLEN 20     /* maximum valid strlen of username */
#define IMP_IM_MAXLEN  200     /* maximum valid strlen of instant message */

/*
** IMP_PROTSTRING_MAXLEN: *approximate* maximum valid length of
** protocol string, set in light of IMP_NAME_MAXLEN and IMP_IM_MAXLEN
** above, for when you just have to allocate a buffer for network
** communications at compile time.  The problem is that broken servers
** may exceed these (if they don't enforce IMP_NAME_MAXLEN and
** IMP_IM_MAXLEN), and we allow clients to ignore those limits because
** its the server's job to enforce them. So this is necessarily
** conservative.
*/
#define IMP_PROTSTRING_MAXLEN 2000

/*
** impSaveInterval: positive non-zero interval (in seconds) with which
** IM server should save its database.  This global doesn't really
** belong in this library (which is for handling protocol messages),
** but this is a way of getting even the provided skeleton "ims" to
** require linking against the imp library.
*/
extern int impSaveInterval;

/*
** Note that with all of the enums below, the symbol used in the project
** write-up appears in quotes in the comment line following the introduction
** of each corresponding enum value.  But you should NEVER include the
** numeric values of the enum in your code; use only the named values that
** constitute the enum.  The numeric values are noted in case they help in
** debugging protocol strings.
*/

/* The four types of protocol messages in the IM system */
typedef enum {
  IMP_MSG_TYPE_UNKNOWN, /* 0 (not a valid value) */
  IMP_MSG_TYPE_OP,      /* 1: "OP" */
  IMP_MSG_TYPE_ACK,     /* 2: "ACK" */
  IMP_MSG_TYPE_ERROR,   /* 3: "ERROR" */
  IMP_MSG_TYPE_STATUS,  /* 4: "STATUS" */
  IMP_MSG_TYPE_INVALID  /* 5: (only for range checking) */
} impMsgType_t;
#define IMP_MSG_TYPE_VALID(x) (IMP_MSG_TYPE_UNKNOWN < (x) && (x) < IMP_MSG_TYPE_INVALID)

/* impOp_t: The different possible operations that can come from the
   client in an OP protocol message, or that can come from the server
   in an ACK protocol message, along with the corresponding arguments
   that are expected. Differences in the validity or arguments for an
   OP message versus for an ACK message are noted. */
typedef enum {
  IMP_OP_UNKNOWN,         /* 0 (not a valid value) */
  IMP_OP_REGISTER,        /* 1: "REGISTER" args = name */
  IMP_OP_LOGIN,           /* 2: "LOGIN" args = name */
  IMP_OP_LOGOUT,          /* 3: "LOGOUT" args = (none) */
  IMP_OP_FRIEND_REQUEST,  /* 4: "FRIEND_REQUEST" args = name
                                 (this operation is not ACK'd) */
  IMP_OP_FRIEND_REMOVE,   /* 5: "FRIEND_REMOVE" args = name
                                 (this operation is not ACK'd) */
  IMP_OP_FRIEND_LIST,     /* 6: "FRIEND_LIST" args = (none)
                                 (this operation is not ACK'd) */
  IMP_OP_IM,              /* 7: "IM": args for OP = name, msg
                                      args for ACK = name */
  IMP_OP_INVALID          /* 8: (only for range checking) */
} impOp_t;
#define IMP_OP_VALID(x) (IMP_OP_UNKNOWN < (x) && (x) < IMP_OP_INVALID)

/* The different possible friend status values. It would be more consistent
   to prefix with "IMP_FRIEND_STATUS", but its too long/annoying to type.
   Note: the "status" field in impMsgStatus_t takes on values from here */
typedef enum {
  IMP_FRIEND_UNKNOWN,   /* 0 (not a valid value) */
  IMP_FRIEND_NOT,       /* 1: "FRIEND_NOT": not friends, and not in
                           process of becoming friends */
  IMP_FRIEND_REQUESTED, /* 2: "FRIEND_REQUESTED": I sent a friend request
                           to Y, but Y hasn't answered me yet */
  IMP_FRIEND_PENDING,   /* 3: "FRIEND_PENDING": Y sent a friend request
                           to me, but I haven't answered yet */
  IMP_FRIEND_YES,       /* 4: "FRIEND_YES": both sides have agreed to
                           be friends, and can now IM each other */
  IMP_FRIEND_INVALID    /* 5: (only for range checking) */
} impFriend_t;
#define IMP_FRIEND_VALID(x) (IMP_FRIEND_UNKNOWN < (x) && (x) < IMP_FRIEND_INVALID)

/* impError_t: The different possible errors reported by server */
typedef enum {
  IMP_ERROR_UNKNOWN,              /*  0 (not a valid value) */
  IMP_ERROR_USER_EXISTS,          /*  1: "USER_EXISTS" args = name */
  IMP_ERROR_CLIENT_BOUND,         /*  2: "CLIENT_BOUND" args = name */
  IMP_ERROR_BAD_COMMAND,          /*  3: "BAD_COMMAND" args = (none) */
  IMP_ERROR_USER_DOES_NOT_EXIST,  /*  4: "USER_DOES_NOT_EXIST" args = name */
  IMP_ERROR_USER_ALREADY_ACTIVE,  /*  5: "USER_ALREADY_ACTIVE" args = name */
  IMP_ERROR_CLIENT_NOT_BOUND,     /*  6: "CLIENT_NOT_BOUND" args = (none) */
  IMP_ERROR_NOT_FRIEND,           /*  7: "NOT_FRIEND" args = name */
  IMP_ERROR_REQUESTED_ALREADY,    /*  8: "REQUESTED_ALREADY" args = name */
  IMP_ERROR_FRIEND_ALREADY,       /*  9: "FRIEND_ALREADY" args = name */
  IMP_ERROR_USER_NOT_ACTIVE,      /* 10: "USER_NOT_ACTIVE" args = name */
  IMP_ERROR_IM_FAILED,            /* 11: "IM_FAILED" args = name */
  IMP_ERROR_INVALID,              /* 12: (only for range checking) */
} impError_t;
#define IMP_ERROR_VALID(x) (IMP_ERROR_UNKNOWN < (x) && (x) < IMP_ERROR_INVALID)

/* The possible active values (all two of them). Its probably overkill to have
   a whole enum for this, but having the same UNKNOWN and INVALID bounds
   facilitates welcome error checking in impMsgNew */
typedef enum {
  IMP_ACTIVE_UNKNOWN=-1, /* -1 (not a valid value) */
  IMP_ACTIVE_NOT,        /* 0: "ACTIVE_NOT" */
  IMP_ACTIVE_YES,        /* 1: "ACTIVE_YES" */
  IMP_ACTIVE_INVALID     /* 2: (only for range checking) */
} impActive_t;
#define IMP_ACTIVE_VALID(x) (IMP_ACTIVE_UNKNOWN < (x) && (x) < IMP_ACTIVE_INVALID)

/*
** impMsg: "generic type" for all IM protocol messages.
**
** Note that the message "subtypes" structs start with the same two fields:
** 1) "mt" indicates the message type; it is always set.
** 2) "userName" is a user name.  If this is used for a given message,
** this points to a (heap-allocated) string, otherwise this is NULL
**
** The "IM" field in impMsgOp is similar: is it NULL if not needed
** otherwise it points to a heap-allocated string.
*/
typedef struct {
  impMsgType_t mt;   /* message type */
  char *userName;    /* used for every type of protocal message, so it
                        belongs in generic impMsg (if not always used
                        in every instance of a given message type) */
} impMsg;

/*
** impMsgOp, impMsgAck, impMsgError, impMsgStatus:
** These are the different structs specific to each of the four
** protocol message types
*/

/* impMsgOp: "subtype" of impMsg for operation protocol messages */
typedef struct {
  impMsgType_t mt; /* == IMP_MSG_TYPE_OP */
  char *userName;  /* some but not all operation messages include user name */
  impOp_t op;      /* which operation is being done */
  char *IM;        /* used only when op == IMP_OP_IM */
} impMsgOp;

/* impMsgAck: "subtype" of impMsg for acknowledgment protocol messages */
typedef struct {
  impMsgType_t mt; /* == IMP_MSG_TYPE_ACK */
  char *userName;  /* some but not all ack messages include a user name */
  impOp_t op;      /* which operation is being ack'd */
} impMsgAck;

/* impMsgError: "subtype" of impMsg for error protocol messages */
typedef struct {
  impMsgType_t mt; /* == IMP_MSG_TYPE_ERROR */
  char *userName;  /* some but not all error messages include a user name */
  impError_t et;   /* which kind of error is this */
} impMsgError;

/* impMsgStatus: "subtype" of impMsg for status protocol messages */
typedef struct _impMsgStatus_t {
  impMsgType_t mt;     /* == IMP_MSG_TYPE_STATUS */
  char *userName;      /* the user being described by this status message */
  impFriend_t status;  /* the current friend status */
  impActive_t active;  /* active or not */
} impMsgStatus;

/*
** impEm: a little struct for holding error messages generated by imp
** functions in a thread-safe way, and in a way that helps avoids
** printing to stderr.
**
** NOTE: In the later functions that take "impEm *iem" as a first argument,
** you can pass a non-NULL "iem" as the place for error for messages to be
** stored. You create an impEm with:
** "impEm *iem; ... iem = impEmNew()".
** and free it with "impEmFree(iem)"
** If you have no interest in the error messages, pass NULL.
**
** Even though this was created for use in imp, there's no reason why
** you can't also use it as an accumulator for error messages in your
** own code. Look at the source of the imp library for examples of using
** impEmAdd, as well as imClient/txtimc.c
**
** (Aside: We're using just "Em" to stand for "error message", since
** adding another kind of "message" to the already confusing
** terminology of "protocol message" and "instant message" would be
** too much.)
*/
typedef struct {
  char **msg; /* msg[0] through msg[msgLen-1] are strings describing
                 something that went wrong during the execution of an imp
                 function; msg[0] records where things first went south */
  unsigned int msgLen;
} impEm;

/*
** You use the following function to create and manage an impEm.
*/
/* impEmNew: the "constructor" */
extern impEm *impEmNew(void);
/* impEmFree: the "destructor", when you're done with it */
extern void impEmFree(impEm *iem);
/* impEmFprint: print the accumulated error messages to a FILE* */
extern void impEmFprint(FILE *file, const impEm *iem);
/* impEmSprint: *allocate* a string, sprint the errors into it
   (putting a newline at the end of each error) and return it; you
   have to free() this string. If there are no error messages stored
   in the impEm (or this is passed a NULL iem) this returns NULL. */
extern char *impEmSprint(const impEm *iem);
/* impEmAdd: add a new error message (either the first one, or an
   additional one), using the exact same format string and conversion
   specifications as you'd use for fprintf, but with the impEm* as the
   first arg instead of FILE*. */
extern void impEmAdd(impEm *iem, const char *fmt, ...)
#ifdef __GNUC__
__attribute__ ((format(printf,2,3)))
#endif
;
/* impEmReset: free the messages inside, but reuse the impEm struct,
   so its the same as after impEmNew */
extern void impEmReset(impEm *iem);


/*
** NOTE also that the functions below which return impMsg* and char* (but not
** const char*) are *allocating* memory for the returned value.  It is up to
** you to free that memory, using impMsgFree() and free(), respectively
*/

/*
** impMsgNew: allocate and set up a new protocol message (the in-memory
**   representation, not the protocol string). You must pass an impEm*
**   (return from impEmNew) as first argument, for gathering error messages
**   in case of problems.
**
** Return value:
** - if no error: non-NULL pointer to newly allocatd impMsg, constructed
**   according to the passed arguments
** - if error: NULL.  Errors described in impEm
**
** When char* are arguments to impMsgNew(), they are treated as "const
** char*": the string is strdup() rather than using the pointer as is; the
** intent being that impMsg alone (or the imp library) should be in control
** of when its dynamically allocated fields are free()d.
**
** For completeness, below are examples of all possible calls to impMsgNew.
** You will most likely be using variables instead of constants for the
** arguments, but this makes it explicit which (non-string) values are
** possible.

impMsgNew(iem, IMP_MSG_TYPE_OP, IMP_OP_REGISTER, "TheUserName", IMP_END);
impMsgNew(iem, IMP_MSG_TYPE_OP, IMP_OP_LOGIN, "TheUserName", IMP_END);
impMsgNew(iem, IMP_MSG_TYPE_OP, IMP_OP_LOGOUT, IMP_END);
impMsgNew(iem, IMP_MSG_TYPE_OP, IMP_OP_FRIEND_REQUEST, "TheUserName", IMP_END);
impMsgNew(iem, IMP_MSG_TYPE_OP, IMP_OP_FRIEND_REMOVE, "TheUserName", IMP_END);
impMsgNew(iem, IMP_MSG_TYPE_OP, IMP_OP_FRIEND_LIST, IMP_END);
impMsgNew(iem, IMP_MSG_TYPE_OP, IMP_OP_IM, "TheUserName", "msg-for-TheUserName", IMP_END);

impMsgNew(iem, IMP_MSG_TYPE_ACK, IMP_OP_REGISTER, "TheUserName", IMP_END);
impMsgNew(iem, IMP_MSG_TYPE_ACK, IMP_OP_LOGIN, "TheUserName", IMP_END);
impMsgNew(iem, IMP_MSG_TYPE_ACK, IMP_OP_LOGOUT, IMP_END);
(the FRIEND OPs are answered with STATUS messages, not ACKs)
impMsgNew(iem, IMP_MSG_TYPE_ACK, IMP_OP_IM, "TheUserName", IMP_END);

impMsgNew(iem, IMP_MSG_TYPE_STATUS, "TheUserName", IMP_FRIEND_NOT, IMP_ACTIVE_YES, IMP_END);
impMsgNew(iem, IMP_MSG_TYPE_STATUS, "TheUserName", IMP_FRIEND_NOT, IMP_ACTIVE_NOT, IMP_END);
( ... plus 2x3 other examples w/ different IMP_FRIEND_* values)

impMsgNew(iem, IMP_MSG_TYPE_ERROR, IMP_ERROR_USER_EXISTS, "TheUserName", IMP_END);
impMsgNew(iem, IMP_MSG_TYPE_ERROR, IMP_ERROR_CLIENT_BOUND, "TheUserName", IMP_END);
impMsgNew(iem, IMP_MSG_TYPE_ERROR, IMP_ERROR_BAD_COMMAND, IMP_END);
impMsgNew(iem, IMP_MSG_TYPE_ERROR, IMP_ERROR_USER_DOES_NOT_EXIST, "TheUserName", IMP_END);
impMsgNew(iem, IMP_MSG_TYPE_ERROR, IMP_ERROR_USER_ALREADY_ACTIVE, "TheUserName", IMP_END);
impMsgNew(iem, IMP_MSG_TYPE_ERROR, IMP_ERROR_CLIENT_NOT_BOUND, IMP_END);
impMsgNew(iem, IMP_MSG_TYPE_ERROR, IMP_ERROR_NOT_FRIEND, "TheUserName", IMP_END);
impMsgNew(iem, IMP_MSG_TYPE_ERROR, IMP_ERROR_REQUESTED_ALREADY, "TheUserName", IMP_END);
impMsgNew(iem, IMP_MSG_TYPE_ERROR, IMP_ERROR_FRIEND_ALREADY, "TheUserName", IMP_END);
impMsgNew(iem, IMP_MSG_TYPE_ERROR, IMP_ERROR_USER_NOT_ACTIVE, "TheUserName", IMP_END);
impMsgNew(iem, IMP_MSG_TYPE_ERROR, IMP_ERROR_IM_FAILED, "TheUserName", IMP_END);

** (Aside: impMsgNew uses var-args http://en.wikipedia.org/wiki/Stdarg.h
** because of the benefits of having a single fucntion implement what is
** logically a single task (create a new impMsg).  The drawback is common to
** any function that uses var-args: the compiler is completely helpless to
** offer type-based error checking. The aggressive error checking on values
** (with IMP_OP_VALID and the like) in impMsgNew may alleviate this.  Another
** var-arg drawback is that the compiler can't know if you've passed the
** right number of arguments to the function. For this we have a modest
** remedy: you must pass IMP_END as the last argument to impMsgNew.)
**
** (Extra aside/trivia: When you see gcc offering you warnings about what it
** sees as incorrect use of printf (or fprintf, etc), it is going way over
** and above the type checking possible in C per se: the compiler has in fact
** generously learned the specific semantics of the printf format string, so
** that it can do error checking on the (var-args) arguments to printf. In
** fact, the gcc-specific "__attribute__ format" keywords allow us invoke
** that same error checking machinery for other functions that work like
** printf, such as impEmAdd above.  But for impMsgNew and var-args functions
** in general, no type checking is available.)
*/
#define IMP_END  154154154  /* totally arbitrary value */
extern impMsg *impMsgNew(impEm *iem, impMsgType_t mt, ...);

/*
** impMsgUserArg: indicates (with a non-zero return) whether a userName
**   argument should be passed to impMsgNew (or impStrNew, or impDisplayNew).
**
** Return value:
** 1: if the impMsgType_t "mt" and subtype (meaning one of impOp_t or
**    impError_t) correspond to a message that needs a username
** 0: if the message doens't need a username, or its an invalid combination
**    of mt and subtype
**
** The tricky part of using the var-args functions correctly is that you need
** to supply exactly the right arguments, and only those arguments, but how
** many arguments you should pass depends on the values of earlier arguments:
** the first impMsgType_t argument (one of the IMP_MSG_TYPE_* enum values)
** determines how the next argument is interpreted (it is either one of the
** IMP_OP_* values, or IMP_ERROR_*).  As is evident above, the userName
** argument sometimes appears in the message, and sometimes not.  To save you
** the annoyance of building your own case table, this function
** (impMsgUserArg) tells you whether you need to pass a userName argument.
** Note that only one specific message (IMP_MSG_TYPE_OP IMP_OP_IM) takes both
** userName and IM, so that is an easy case to handle.
*/
extern int impMsgUserArg(impMsgType_t mt, int subtype);

/*
** impMsgFree: free a protocol message
*/
extern void impMsgFree(impMsg *pm);

/*
** impMsgToStr: given a protocol message, construct a protocol string
** impStrNew: like impMsgNew and then impMsgToStr: you use this
**   to get to a newly allocated protocol string, starting with the same
**   var-args description of the protocol message as for impMsgNew
**   (see above; also has to end with IMP_END)
** impDisplayNew: like impMsgNew and then impMsgToDisplay,
**   very analogous to impStrNew
**
** Return value:
** - if no error: non-NULL pointer to (newly allocated) protocol string
** - if error: NULL.  Errors described in impEm
*/
extern char *impMsgToStr(impEm *iem, const impMsg *pm);
extern char *impStrNew(impEm *iem, impMsgType_t mt, ...);
extern char *impDisplayNew(impEm *iem, impMsgType_t mt, ...);

/*
** impStrToMsg: parse the given a protocol string, and build (allocate)
**   the corresponding protocol message
**
** Return value:
** - if no error: non-NULL pointer to a newly allocated impMsg
** - if error: NULL.  Errors described in impEm
*/
extern impMsg *impStrToMsg(impEm *iem, const char *str);

/*
** impMsgToDisplay: given a protocol message, construct a human readable
**   version of the corresponding protocol string. The resulting string
**   CANNOT be used as a protocol string.  It is just for debugging
**   purposes free() it when you are done with it.
** impStrToDisplay: like impStrToMsg then impMsgToDisplay: starting
**   from a protocol string, parse the protocol message, and then
**   generate the display string (w/out having to allocate and manage
**   the intermediate impMsg)
**
** Return value:
** - if no error: non-NULL pointer to (newly allocated) display string
** - if error: NULL.  Errors described in impEm
*/
extern char *impMsgToDisplay(impEm *iem, const impMsg *pm);
extern char *impStrToDisplay(impEm *iem, const char *str);

/*
** impMsgTypeStr, impOpStr, impErrorStr, impFriendStr, impActiveStr:
** Utilities for mapping from enum values to a string, with bounds checking.
** The "const char*" returns should not be free()d
*/
extern const char *impMsgTypeStr(impMsgType_t mt);
extern const char *impOpStr(impOp_t op);
extern const char *impErrorStr(impError_t err);
extern const char *impFriendStr(impFriend_t status);
extern const char *impActiveStr(impActive_t active);

#endif  /* IMP_HAS_BEEN_INCLUDED */
