#!/bin/sh
# Vino Crescini
# 10 December 2001

echo "Content-type: text/html"
echo ""
echo "<html>"
echo "  <body"
echo "    bgcolor=\"black\""
echo "    text=\"white\">"
echo "    <h1>Line Printer Queue @talaxia.badlands</h1>"
echo "    <table border=\"1\" cols=\"1\" width=\"100%\">"
echo "      <tr bgcolor=\"#3300ff\">"
echo "        <th>RANK</th>"
echo "        <th>OWNER</th>"
echo "        <th>JOB</th>"
echo "        <th>FILES</th>"
echo "        <th>TOTAL SIZE</th>"
echo "      </tr>"

count=0
lpq | while read i; do
  if [ ${count} = "0" ] || [ ${count} = "1" ]; then
    count=$[${count} + 1]
  else
    if [ $[${count} % 2] -eq 0 ]; then
      echo "    <tr bgcolor=\"#330055\">"
    else
      echo "    <tr bgcolor=\"#330088\">"
    fi

    count=$[${count} + 1]

    echo "      <td>"
    echo "${i}" | cut -c-7
    echo "      </td>"
    echo "      <td>"
    echo "${i}" | cut -c8-17
    echo "      </td>"
    echo "      <td>"
    echo "${i}" | cut -c18-23
    echo "      </td>"
    echo "      <td>"
    echo "${i}" | cut -c24-61
    echo "      </td>"
    echo "      <td>"
    echo "${i}" | cut -c62-80
    echo "      </td>"
    echo "    </tr>"
  fi
done

echo "    </table>"

echo "    <br>"
echo "    `lpq | head -n1`"
echo "    <br>"

echo "  </body>"
echo "</html>"
