describe 'require' do
  it "fails on non-Windows platforms" do
    skip("can not test this on #{RUBY_PLATFORM}") if RUBY_PLATFORM =~ /mingw|mswin/

    expect { require "wdm" }.to raise_error(LoadError, /not supported on your system/i)
  end

  it "succeeds on Windows platforms" do
    skip("can not test this on #{RUBY_PLATFORM}") if RUBY_PLATFORM !~ /mingw|mswin/

    require "wdm"
  end
end
