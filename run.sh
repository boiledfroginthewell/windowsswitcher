#!/bin/sh

if type mgcc > /dev/null 2>&1; then
	cc=mgcc
else
	cc=gcc
fi
$cc -mwindows windowsswitcher.c switch.c winapiutil.c && ./a.exe 
