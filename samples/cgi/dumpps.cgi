#!/bin/sh
# Vino Crescini
# 21 October 2001

echo "Content-type: text/html"
echo ""
echo "<html>"
echo "  <body"
echo "    bgcolor=\"black\""
echo "    text=\"white\">"
echo "    <h1>Process Listing @talaxia.badlands</h1>"
echo "    <table border=\"1\" cols=\"1\" width=\"100%\">"
echo "      <tr bgcolor=\"#3300ff\">"
echo "        <th>USER</th>"
echo "        <th>PID</th>"
echo "        <th>%CPU</th>"
echo "        <th>%MEM</th>"
echo "        <th>VSZ</th>"
echo "        <th>RSS</th>"
echo "        <th>TTY</th>"
echo "        <th>STAT</th>"
echo "        <th>START</th>"
echo "        <th>TIME</th>"
echo "        <th>COMMAND</th>"
echo "      </tr>"

count=0
ps auxhcw | while read i; do
  if [ $[$count % 2] -eq 0 ]; then
    echo "    <tr bgcolor=\"#330055\">"
  else
    echo "    <tr bgcolor=\"#330088\">"
  fi

  count=$[$count + 1]

  echo "      <td>"
  echo "${i}" | cut -c-8
  echo "      </td>"
  echo "      <td>"
  echo "${i}" | cut -c10-14
  echo "      </td>"
  echo "      <td>"
  echo "${i}" | cut -c16-19
  echo "      </td>"
  echo "      <td>"
  echo "${i}" | cut -c21-25
  echo "      </td>"
  echo "      <td>"
  echo "${i}" | cut -c26-30
  echo "      </td>"
  echo "      <td>"
  echo "${i}" | cut -c32-35
  echo "      </td>"
  echo "      <td>"
  echo "${i}" | cut -c37-44
  echo "      </td>"
  echo "      <td>"
  echo "${i}" | cut -c46-48
  echo "      </td>"
  echo "      <td>"
  echo "${i}" | cut -c50-56
  echo "      </td>"
  echo "      <td>"
  echo "${i}" | cut -c58-62
  echo "      </td>"
  echo "      <td>"
  echo "${i}" | cut -c64-80
  echo "      </td>"
  echo "    </tr>"
done

echo "    </table>"

echo "  </body>"
echo "</html>"
