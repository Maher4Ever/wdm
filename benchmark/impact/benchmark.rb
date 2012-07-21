require 'ruby-prof'
require 'fileutils'

require 'rb-fchange'
require 'wdm'

require_relative './../config.rb'

def write_results(result, type)
  printer = RubyProf::GraphHtmlPrinter.new(result)
  printer.print(File.new("#{TestConfig::DIR}/result-#{type}.html", "w"))
end

def run_test
  10.times do
    FileUtils.touch "#{TestConfig::DIR}/file.txt"
    FileUtils.rm "#{TestConfig::DIR}/file.txt"
  end
end

2.times do
  GC.enable
  GC.start
  GC.disable

  #####################
  # Without FChange
  #####################

  puts "Profiling ruby"

  # Profile the code

  RubyProf.start

  run_test

  result = RubyProf.stop
  write_results(result, "ruby")

  #####################
  # With WDM
  #####################

  puts "Profiling with wdm"

  monitor = WDM::Monitor.new
  monitor.watch(TestConfig::DIR, &Proc.new {})

  t = Thread.new { monitor.run! }
  sleep(0.5)

  # Profile the code

  RubyProf.start

  run_test

  result = RubyProf.stop

  Thread.kill(t)

  # Print results

  write_results(result, "wdm")

  #####################
  # With FChange
  #####################

  puts "Profiling with fchange"

  notifier = FChange::Notifier.new
  notifier.watch(TestConfig::DIR, :all_events, :recursive, &Proc.new {})

  t = Thread.new { notifier.run }
  sleep(0.5)

  # Profile the code

  RubyProf.start

  run_test

  result = RubyProf.stop

  Thread.kill(t)

  # Print results

  write_results(result, "fchange")
end

puts "Finished profiling"