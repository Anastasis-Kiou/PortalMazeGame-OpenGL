#!/bin/sh
bindir=$(pwd)
cd /Users/anastasiskiout/Documents/PortalMazeCmake/PortalMazeCmake/
export 

if test "x$1" = "x--debugger"; then
	shift
	if test "x" = "xYES"; then
		echo "r  " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		GDB_COMMAND-NOTFOUND -batch -command=$bindir/gdbscript  /Users/anastasiskiout/Documents/PortalMazeCmake/build/PortalMazeCmake 
	else
		"/Users/anastasiskiout/Documents/PortalMazeCmake/build/PortalMazeCmake"  
	fi
else
	"/Users/anastasiskiout/Documents/PortalMazeCmake/build/PortalMazeCmake"  
fi
