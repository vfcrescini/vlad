#!/bin/sh
# Vino Crescini
# 21 October 2001

echo "Content-type: text/html"
echo ""
echo "<html>"
echo "  <body"
echo "    bgcolor=\"black\""
echo "    text=\"white\">"
echo "    <h1>Logged on Users @talaxia.badlands</h1>"
echo "    <table border=\"1\" cols=\"1\" width=\"100%\">"
echo "      <tr bgcolor=\"#3300ff\">"
echo "        <th>USER</th>"
echo "        <th>TTY</th>"
echo "        <th>FROM</th>"
echo "        <th>LOGIN</th>"
echo "        <th>IDLE</th>"
echo "        <th>JCPU</th>"
echo "        <th>PCPU</th>"
echo "        <th>WHAT</th>"
echo "      </tr>"

count=0
w -h | while read i; do
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
  echo "${i}" | cut -c10-17
  echo "      </td>"
  echo "      <td>"
  echo "${i}" | cut -c19-35
  echo "      </td>"
  echo "      <td>"
  echo "${i}" | cut -c36-42
  echo "      </td>"
  echo "      <td>"
  echo "${i}" | cut -c44-49
  echo "      </td>"
  echo "      <td>"
  echo "${i}" | cut -c51-57
  echo "      </td>"
  echo "      <td>"
  echo "${i}" | cut -c58-63
  echo "      </td>"
  echo "      <td>"
  echo "${i}" | cut -c64-80
  echo "      </td>"
  echo "    </tr>"
done

echo "    </table>"
echo "  </body>"
echo "</html>"
