require 'mkmf'

if have_library("kernel32") and
   have_header("windows.h") and
   have_header("ruby.h")    and
   have_const('HAVE_RUBY_ENCODING_H')
then
    create_makefile("wdm")
end
