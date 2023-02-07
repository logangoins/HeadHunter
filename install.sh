#!/bin/bash
# 
# Compile the binary with "make" then use chmod +x install.sh.
# Run sudo ./install.sh to install HeadHunter.
# written by Logan Goins
#

if [[ -f /usr/bin/headhunter ]]; then
	echo "HeadHunter binary exists. Replacing current binary in PATH."
	rm /usr/bin/headhunter
	cp headhunter /usr/bin/headhunter
else 
	echo "Installing HeadHunter binary to PATH."
	cp headhunter /usr/bin/headhunter
fi

