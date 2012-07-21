This directory has some benchmark scripts and their results.

## Benchmarked libraries

I tried to include all ruby directories monitors which use Win32 API that I know of. If there are more, please let me know about them.

- WDM
- [FChange](https://github.com/stereobooster/rb-fchange)
- [em-dir-watcher](https://github.com/mockko/em-dir-watcher/)


## Usage

1. Run `bundle install` to install all the prerequisites.
2. Each type of benchmark is stored inside a separate directory. To run a benchmark, go into the directory of the benchmark and run the `benchmark.rb` file.

## Results

Here are the results of the benchmarks which were gathered on an Intel Core 2 Duo machine running Windows 7 (64-bit):

### Detecting changes

![Detecting changes results](https://github.com/Maher4Ever/wdm/raw/0a00e997b7f2b066bd6841e84018abbb7d8283fa/benchmark/detecting_changes/results.png)

### Impact on performance

From the previous results you can conclude that FChange and WDM are the obvious winners. Thus, this test didn't include em-dir-watcher.

- FChange: 65216,47%.
- WDM: 41,13%.