# A sample Guardfile
# More info at https://github.com/guard/guard#readme

group :compile do
  guard :shell do
    watch(%r{ext/.+\.(?:h|c)}) { `rake compile` }
  end
end

group :test do
  guard :rspec, :all_on_start => false, :all_after_pass => false, :cli => '--fail-fast --format doc' do
    watch(%r{^spec/.+_spec\.rb$})
    watch(%r{^lib/(.+)\.rb$})     { |m| "spec/lib/#{m[1]}_spec.rb" }
    watch('spec/spec_helper.rb')  { "spec" }
  end
end