/*
 * loggerType.h
 * Vino Crescini
 * 18 July 2001
 */

#ifndef __LOGGERTYPE_H
#define __LOGGERTYPE_H

#include <stdarg.h>

#define LOGGERTYPE_LOGNULL  0
#define LOGGERTYPE_LOGINFO  1
#define LOGGERTYPE_LOGTRACE 2
#define LOGGERTYPE_LOGWARN  3
#define LOGGERTYPE_LOGERROR 4

class loggerType
{
  public:
    loggerType();
    ~loggerType();

    bool setLevel(int);
    bool setOutput(char *);
    
    void logInfo(char *, ...);
    void logTrace(char *, ...);
    void logWarn(char *, ...);
    void logError(char *, ...);

  private:
    int mLogLevel;
    FILE *mLogFileStream;
} ;

#endif
