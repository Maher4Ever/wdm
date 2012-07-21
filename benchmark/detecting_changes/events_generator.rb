require 'fileutils'
require 'securerandom'

require_relative './../config.rb'

include FileUtils

files = ['file.txt', 'something.txt', 'foo.txt', 'blah.txt']

loop do
  begin
    touch "#{TestConfig::DIR}/#{files[ARGV.first.to_i]}"
  rescue Errno::EACCES # Happens when the polling test is run!
    sleep 0.1
    retry
  end

  sleep(TestConfig::EVENTS_INTERVAL)
end