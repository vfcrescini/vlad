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

loggerType::loggerType()
{
  mLogLevel = 0;
  mLogFileStream = NULL;
}

loggerType::~loggerType()
{
  if (mLogFileStream)
    fclose(mLogFileStream);
}

bool loggerType::setLevel(int aLevel)
{
  if (aLevel >= LOGGERTYPE_LOGNULL && aLevel <= LOGGERTYPE_LOGERROR) {
    mLogLevel = aLevel;
    return true;
  }
  return false;
}

bool loggerType::setOutput(char *aFilename)
{
  if (!aFilename)
    mLogFileStream = stderr;
  else
    mLogFileStream = fopen(aFilename, "w");

  return (mLogFileStream != NULL);
}

void loggerType::logInfo(char *aFormat, ...)
{
  va_list argPointer;
  
  if (aFormat &&
      mLogLevel >= LOGGERTYPE_LOGINFO &&
      mLogFileStream) {
      
    va_start(argPointer, aFormat);
    fprintf(mLogFileStream, "%s\n", LOGGERTYPE_STRINGHEADER);
    fprintf(mLogFileStream, "%s\n", LOGGERTYPE_STRINGINFO);
    fprintf(mLogFileStream, "%s\n", LOGGERTYPE_STRINGTRAILER);
    vfprintf(mLogFileStream, aFormat, argPointer);
    va_end(argPointer);    
  }
}

void loggerType::logTrace(char *aFormat, ...)
{
  va_list argPointer;
  
  if (aFormat &&
      mLogLevel >= LOGGERTYPE_LOGTRACE &&
      mLogFileStream) {
      
    va_start(argPointer, aFormat);
    fprintf(mLogFileStream, "%s\n", LOGGERTYPE_STRINGHEADER);
    fprintf(mLogFileStream, "%s\n", LOGGERTYPE_STRINGTRACE);
    fprintf(mLogFileStream, "%s\n", LOGGERTYPE_STRINGTRAILER);
    vfprintf(mLogFileStream, aFormat, argPointer);
    va_end(argPointer);    
  }
}

void loggerType::logWarn(char *aFormat, ...)
{
  va_list argPointer;
  
  if (aFormat &&
      mLogLevel >= LOGGERTYPE_LOGWARN &&
      mLogFileStream) {
      
    va_start(argPointer, aFormat);
    fprintf(mLogFileStream, "%s\n", LOGGERTYPE_STRINGHEADER);
    fprintf(mLogFileStream, "%s\n", LOGGERTYPE_STRINGWARN);
    fprintf(mLogFileStream, "%s\n", LOGGERTYPE_STRINGTRAILER);
    vfprintf(mLogFileStream, aFormat, argPointer);
    va_end(argPointer);    
  }
}

void loggerType::logError(char *aFormat, ...)
{
  va_list argPointer;
  
  if (aFormat &&
      mLogLevel >= LOGGERTYPE_LOGERROR &&
      mLogFileStream) {
      
    va_start(argPointer, aFormat);
    fprintf(mLogFileStream, "%s\n", LOGGERTYPE_STRINGHEADER);
    fprintf(mLogFileStream, "%s\n", LOGGERTYPE_STRINGERROR);
    fprintf(mLogFileStream, "%s\n", LOGGERTYPE_STRINGTRAILER);
    vfprintf(mLogFileStream, aFormat, argPointer);
    va_end(argPointer);    
  }
}
