#!/bin/sh

if type mgcc > /dev/null; then
	cc=mgcc
else
	cc=gcc
fi
$cc -mwindows windowsswitcher.c switch.c winapiutil.c && ./a.exe 
