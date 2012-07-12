require_relative '../lib/wdm'

monitor = WDM::Monitor.new
monitor.watch('C:\Users\Maher\Desktop\test') { puts "change 1!" }
monitor.watch('C:\Users\Maher\Desktop\psds') { puts "change 2!" }

thread = Thread.new {
	monitor.run!
}

sleep(10)

monitor.stop