#!/bin/bash


	URL=file://$(realpath $1)
	FILENAME=$(basename "$1")
	SIZE=$(stat -f%z $1)
	CHKSUM=$(shasum -a 256 $1|cut "-d " -f1)
cat << EOF
                    "url": "$URL",
                    "archiveFileName": "$FILENAME",
                    "checksum": "SHA-256:$CHKSUM",
                    "size": "$SIZE"
EOF

