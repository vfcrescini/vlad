#!/bin/sh
# Vino Crescini
# 21 October 2001

echo "Content-type: text/html"
echo ""
echo "<html>"
echo "  <body"
echo "    bgcolor=\"black\""
echo "    text=\"white\">"
echo "    <h1>Uptime @talaxia.badlands</h1>"
echo "    <pre>"
uptime
echo "    </pre>"
echo "  </body>"
echo "</html>"
