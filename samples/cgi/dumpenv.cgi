#!/bin/sh
# Vino Crescini
# 21 October 2001

echo "Content-type: text/html"
echo ""
echo "<html>"
echo "  <body"
echo "    bgcolor=\"black\""
echo "    text=\"white\">"
echo "    <h1>Environment Variables @talaxia.badlands</h1>"
echo "    <table border=\"1\" cols=\"1\" width=\"100%\">"
echo "      <tr bgcolor=\"#3300ff\">"
echo "        <th>VARIABLE</th>"
echo "        <th>VALUE</th>"
echo "      </tr>"

count=0
printenv | while read i; do

  if [ $[$count % 2] -eq 0 ]; then
    echo "    <tr bgcolor=\"#330055\">"
  else
    echo "    <tr bgcolor=\"#330088\">"
  fi

  count=$[$count + 1]

  echo "      <td>"
  echo "        ${i%%=*}" 
  echo "      </td>"
  echo "      <td>"

  if [ "${i#*=}" = "" ]; then
    echo "        &nbsp"
  else
    echo "        ${i#*=}" 
  fi

  echo "      </td>"
  echo "    </tr>"
done

echo "    </table>"
echo "  </body>"
echo "</html>"
