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
    
    void logError(char *, ...);
    void logInfo(char *, ...);
    void logTrace(char *, ...);
    void logWarn(char *, ...);

  private:
    int mLogLevel;
    FILE *mLogFileStream;
} ;

#endif
