require 'mkmf'

if have_library("kernel32") and
   have_header("windows.h")
then
	create_makefile("wdm")
end