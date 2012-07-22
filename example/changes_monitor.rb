require_relative '../lib/wdm'

monitor = WDM::Monitor.new
monitor.watch_recursively('C:\Users\Maher\Desktop\test') do |change|
  puts "#{change.type.to_s.upcase}: '#{change.path}'"
end

puts "Running the monitor..."

thread = Thread.new {
	monitor.run!
}

sleep(10)

puts "Stopping the monitor..."

monitor.stop
thread.join