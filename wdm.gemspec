# -*- encoding: utf-8 -*-

Gem::Specification.new do |gem|
  gem.authors       = ['Maher Sallam']
  gem.email         = ['maher@sallam.me']
  gem.description   = %q{Windows Directory Monitor (WDM) is a library which can be used to monitor directories for changes. It's mostly implemented in C and uses the Win32 API for a better performance.}
  gem.summary       = %q{Windows Directory Monitor (WDM) is a threaded directories monitor for Windows.}
  gem.license      = 'MIT'
  gem.homepage      = "https://github.com/Maher4Ever/wdm"

  gem.files         = `git ls-files -- ext/* lib/*`.split($\) + %w[LICENSE README.md]
  gem.extensions    = ['ext/wdm/extconf.rb']
  gem.executables   = gem.files.grep(%r{^bin/}).map{ |f| File.basename(f) }
  gem.test_files    = gem.files.grep(%r{^(test|spec|features)/})
  gem.name          = 'wdm'
  gem.require_paths = ['lib']
  gem.version       = '0.1.1'

  gem.required_ruby_version = '>= 2.5'
end
