/* 
 * loggerType.cpp
 * Vino Crescini
 * 18 June 2001
 */

#include <stdio.h>
#include <stdlib.h>
#include "loggerType.h"

#define LOGGERTYPE_STRINGINFO    "Logger Info  :"
#define LOGGERTYPE_STRINGTRACE   "Logger Trace :"
#define LOGGERTYPE_STRINGWARN    "Logger Warn  :"
#define LOGGERTYPE_STRINGERROR   "Logger Error :"
#define LOGGERTYPE_STRINGHEADER  "---------------------------------------"
#define LOGGERTYPE_STRINGTRAILER "---------------------------------------"

// default constructor
loggerType::loggerType()
{
  mLogLevel = LOGGERTYPE_LOGNULL;
  mLogFileStream = stderr;
}

// constructor that takes a filename as an argument
loggerType::loggerType(const char *aFileName)
{
  mLogLevel = LOGGERTYPE_LOGNULL;
  
  if (!(aFileName && (mLogFileStream = fopen(aFileName, "w"))))
    mLogFileStream = stderr;
}

// constructor that takes a filestream as an argument
loggerType::loggerType(FILE *aOutputFile)
{
  mLogLevel = LOGGERTYPE_LOGNULL;
  mLogFileStream = aOutputFile ? aOutputFile : stderr;
}

loggerType::~loggerType()
{
  if (mLogFileStream)
    fclose(mLogFileStream);
}

bool loggerType::setLevel(int aLevel)
{
  if (aLevel >= LOGGERTYPE_LOGNULL && aLevel <= LOGGERTYPE_LOGINFO) {
    mLogLevel = aLevel;
    return true;
  }
  return false;
}

void loggerType::logInfo(const char *aFormat, ...)
{
  va_list argPointer;
  if (mLogLevel >= LOGGERTYPE_LOGINFO) {
    va_start(argPointer, aFormat);
    printLog(LOGGERTYPE_STRINGINFO, aFormat, argPointer); 
    va_end(argPointer);
  }
}

void loggerType::logTrace(const char *aFormat, ...)
{
  va_list argPointer;
  if (mLogLevel >= LOGGERTYPE_LOGTRACE) {
    va_start(argPointer, aFormat);
    printLog(LOGGERTYPE_STRINGTRACE, aFormat, argPointer); 
    va_end(argPointer);
  }
}

void loggerType::logWarn(const char *aFormat, ...)
{
  va_list argPointer;
  
  if (mLogLevel >= LOGGERTYPE_LOGWARN) {
    va_start(argPointer, aFormat);
    printLog(LOGGERTYPE_STRINGWARN, aFormat, argPointer); 
    va_end(argPointer);
  }
}

void loggerType::logError(const char *aFormat, ...)
{
  va_list argPointer;
  
  if (mLogLevel >= LOGGERTYPE_LOGERROR) {
    va_start(argPointer, aFormat);
    printLog(LOGGERTYPE_STRINGERROR, aFormat, argPointer); 
    va_end(argPointer);
  }
}

void loggerType::printLog(const char *aMessageLog, const char *aFormat, ...)
{
  va_list argPointer;
  
  if (aFormat && aMessageLog && mLogFileStream) {
    va_start(argPointer, aFormat);
    fprintf(mLogFileStream, "\n%s\n", LOGGERTYPE_STRINGHEADER);
    fprintf(mLogFileStream, "%s\n", aMessageLog);
    vfprintf(mLogFileStream, aFormat, argPointer);
    fprintf(mLogFileStream, "\n%s\n", LOGGERTYPE_STRINGTRAILER);
    va_end(argPointer);    
  }
}
