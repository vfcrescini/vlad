#!/bin/sh

# Vino Crescini
# 23 October 2001

COUNTFILE="/var/www/cgi-bin/data/count"
COUNT="0"
DATE="24 October 2001"

# if the file doesn't exist, generate one
if ! [ -f ${COUNTFILE} ]; then
  echo "0" > ${COUNTFILE}
fi

# if file is readable, get the current count
if [ -r ${COUNTFILE} ]; then
  COUNT=$[`cat ${COUNTFILE}` + 1]
fi

# if file is writable, save the current count
if [ -w ${COUNTFILE} ]; then
  echo "${COUNT}" > ${COUNTFILE}
fi

# generate content
echo "Content-type: text/html"
echo ""
echo "<html>"
echo "  <body"
echo "    bgcolor=\"black\""
echo "    text=\"white\">"
echo "    <center>"
echo "      <font size=\"1\">"

# we have to be grammatically correct.
if [ ${COUNT} = "1" ]; then
  echo "        This page has been accessed ${COUNT} time since ${DATE}."
else
  echo "        This page has been accessed ${COUNT} times since ${DATE}."
fi

echo "      </font>"
echo "    </center>"
echo "  </body>"
echo "</html>"
