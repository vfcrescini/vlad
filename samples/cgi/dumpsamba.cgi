#!/bin/sh
# Vino Crescini
# 1 November 2001

echo "Content-type: text/html"
echo ""
echo "<html>"
echo "  <body"
echo "    bgcolor=\"black\""
echo "    text=\"white\">"
echo "    <h1>Samba Server Status @talaxia.badlands</h1>"
echo "    <pre>"
smbstatus
echo "    </pre>"
echo "  </body>"
echo "</html>"
