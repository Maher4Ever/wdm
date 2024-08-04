#!/usr/bin/env rake
require 'bundler/gem_tasks'
require 'rake/extensiontask'
require 'rspec/core/rake_task'
require 'devkit' if RUBY_PLATFORM=~/mingw/i

RSpec::Core::RakeTask.new(:spec)

# Compile the extension
Rake::ExtensionTask.new('wdm_ext') do |ext|
  ext.ext_dir = 'ext/wdm'
end

desc "Open an irb session preloaded with WDM"
task :console do
  sh "irb -I lib -r wdm"
end
