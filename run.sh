#!/bin/sh

if type gcc; then
	cc=gcc
else
	cc=mgcc
fi
$cc -mwindows windowsswitcher.c switch.c winapiutil.c && ./a.exe 
