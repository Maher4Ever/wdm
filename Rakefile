#!/usr/bin/env rake
require 'bundler/gem_tasks'
require 'rake/extensiontask'

# Compile the extension
Rake::ExtensionTask.new('wdm')

desc "Open an irb session preloaded with WDM"
task :console do
  sh "irb -rubygems -I lib -r wdm"
end