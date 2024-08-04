require 'pathname'
require 'wdm'

Dir["#{File.dirname(__FILE__)}/support/**/*.rb"].each { |f| require f }

RSpec.configure do |config|
  config.run_all_when_everything_filtered = true
  config.color = true
  config.filter_run :focus

  # check if the filesystem stores 8.3 short file name aliases
  long_file_name  = 'longer_than_12_chars.txt'
  short_file_name = 'LONGER~1.TXT'
  File.binwrite(long_file_name, "long")
  unless (File.binread(short_file_name) rescue nil) == "long"
    # ... and disable the spec if not
    config.filter_run_excluding( :file_8_3_alias )
  end
  File.unlink(long_file_name)

  config.before(:all) { `rake compile` }

  config.include WDM::SpecSupport
end

# Be verbose about errors in threads
Thread.abort_on_exception = true
