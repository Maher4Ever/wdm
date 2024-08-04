describe 'wdm on non-Windows' do
  describe 'require' do
    it "fails to require wdm on non-windows platforms" do
      expect { require "wdm" }.to raise_error(LoadError, /not supported on your system/i)
    end
  end
end
