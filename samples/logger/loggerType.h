/*
 * loggerType.h
 * Vino Crescini
 * 18 July 2001
 */

#ifndef __LOGGERTYPE_H
#define __LOGGERTYPE_H

#include <stdarg.h>

#define LOGGERTYPE_LOGNULL  0
#define LOGGERTYPE_LOGERROR 1
#define LOGGERTYPE_LOGWARN  2
#define LOGGERTYPE_LOGTRACE 3
#define LOGGERTYPE_LOGINFO  4

class loggerType
{
  public:
    loggerType();
    loggerType(const char *);
    loggerType(FILE *);
    ~loggerType();

    bool setLevel(int);
    
    void logError(const char *, ...);
    void logInfo(const char *, ...);
    void logTrace(const char *, ...);
    void logWarn(const char *, ...);

  private:
    int mLogLevel;
    FILE *mLogFileStream;
    
    void printLog(const char *, const char *, ...);
} ;

#endif
