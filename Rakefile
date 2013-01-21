#!/usr/bin/env rake
require 'bundler/gem_tasks'
require 'rake/extensiontask'

# Compile the extension
Rake::ExtensionTask.new('wdm')

require 'rspec/core/rake_task'
RSpec::Core::RakeTask.new(:spec)

desc "Open an irb session preloaded with WDM"
task :console do
  sh "irb -rubygems -I lib -r wdm"
end
