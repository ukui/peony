#!/bin/sh
# This script generates the po/peony-admin.pot file
FILEPATH="$(readlink -f "$0")"
DIR="$(dirname "$FILEPATH")"
cd "$DIR"
xgettext --package-name=peony-admin \
         --package-version=0.0.1 \
         --copyright-holder='Bruno Nova <brunomb.nova@gmail.com>' \
         --msgid-bugs-address='https://github.com/infirit/peony-admin/issues' \
         -cTRANSLATORS \
         -s -o "po/peony-admin.pot" \
         "extension/peony-admin.py"
