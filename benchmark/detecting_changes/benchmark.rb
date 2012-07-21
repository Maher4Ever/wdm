require 'thread'
require 'benchmark'

require 'rb-fchange'
require 'wdm'
require 'em-dir-watcher'
require 'listen'

require_relative './../config.rb'

Thread.abort_on_exception = true

def run_test(options = {})
  results = []
  changes = 0
  callback = Proc.new { changes += 1 }

  puts "Running tests on #{options[:lib_name]}\n"

  options[:instance].send(options[:watch_method] || :watch, TestConfig::DIR, &callback)

  TestConfig::REPEAT_COUNT.times do |i|
    t = Thread.new { options[:instance].send(options[:run_method] || :run) }

    sleep(TestConfig::TEST_DURATION)

    options[:instance].send(options[:stop_method] || :stop)
    t.join

    puts "=> Recorded changes in run ##{i+1}: #{changes}"

    results << changes
    changes = 0
  end

  puts "\n=> Average recorded changes: #{results.inject(&:+)/TestConfig::REPEAT_COUNT}\n\n"
end

def run_em_test(options = {})
  results = []
  changes = 0
  callback = Proc.new { changes += 1 }

  puts "Running tests on #{options[:lib_name]}\n"

  TestConfig::REPEAT_COUNT.times do |i|
    t = Thread.new { EM.run {
        watcher = options[:module].send(options[:watch_method] || :watch, TestConfig::DIR, &callback)
    } }

    sleep(TestConfig::TEST_DURATION)

    watcher.send(options[:stop_method] || :stop)
    sleep(0.2)
    Thread.kill(t) if t

    puts "=> Recorded changes in run ##{i+1}: #{changes}"

    results << changes
    changes = 0
  end

  puts "\n=> Average recorded changes: #{results.reject(&:zero?).inject(&:+)/(TestConfig::REPEAT_COUNT/2)}\n\n"
end

def run_polling_test
  results = []
  changes = 0
  callback = Proc.new { changes += 1 }

  puts "Running tests on Listen (polling)\n"

  listener = Listen.to(TestConfig::DIR)
                   .force_polling(true)
                   .change(&callback)

  TestConfig::REPEAT_COUNT.times do |i|
    t = Thread.new { listener.start }

    sleep(TestConfig::TEST_DURATION)

    listener.stop
    t.join

    puts "=> Recorded changes in run ##{i+1}: #{changes}"

    results << changes
    changes = 0
  end

  puts "\n=> Average recorded changes: #{results.inject(&:+)/TestConfig::REPEAT_COUNT}\n\n"
end

# Setup
wdm = WDM::Monitor.new
fchange = FChange::Notifier.new

# Run the test
Benchmark.bmbm do |x|
  x.report("FChange") { run_test(lib_name: "FChange", instance: fchange) }
  x.report("WDM") { run_test(lib_name: "WDM", instance: wdm, run_method: :run!) }
  x.report("EM-Dir-Watcher") { run_em_test(lib_name: "EM-Dir-Watcher", module: EMDirWatcher) }
  x.report("Listen (polling)") { run_polling_test }
end