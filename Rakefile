#!/usr/bin/env rake
require 'bundler/gem_tasks'
require 'rake/extensiontask'

ENV['PATH'] = 'c:\\Ruby193\\bin;c:\\Ruby193\\mingw\\bin;' + ENV['PATH']
 
# Compile the extension
Rake::ExtensionTask.new('wdm')

desc "Open an irb session preloaded with WDM"
task :console do
  sh "irb -rubygems -I lib -r wdm"
end