# Crypto HFT Backtester

An event-driven C++20 backtester for cryptocurrency strategies at order-book resolution. It was built for IE 421 (High-Frequency Trading Technology) at UIUC in Spring 2024.

Commercial tools such as RCM-X Strategy Studio assume a one-cent minimum tick, so crypto's sub-penny prices have to be rescaled by hand. They also can't model exchange-specific latency. This backtester takes each venue's real tick sizes, lot sizes, trading rules, fee tiers and latencies from one JSON file.

## Features

- **Order-book replay.** It replays L2 book updates and trades event by event and fills resting orders partially or fully as the book moves.
- **Latency-aware execution.** Orders reach the exchange only after a configurable send latency and are matched on arrival. A latency sweep (0 ns → 1 µs) reports how P&L degrades with latency.
- **Venue configuration.** Binance, Bybit, Coinbase and OKX are configured with per-symbol trading rules, such as price and lot increments and min/max notional, plus maker/taker fee tiers. Spot and futures run with separate balances.
- **Order types.** Market, limit, stop and stop-limit orders are supported, with leverage, margin type and pre-trade rule checks.
- **Strategy API.** You subclass one strategy class and implement event callbacks for trades and book updates. The run outputs a trade log and a balance time series as CSV.

![Backtesting loop](images/algorithm.png)

## Quick start

```bash
# Boost is used header-only
brew install boost          # macOS (or: sudo apt install libboost-dev)

# Usage: <spot balance> <futures balance> <exchange config> <market data>
sh run_backtest.sh 1000000 1000000 ./configuration/exchange.json ./sample_data/sample_market_data.csv
sh run_latency_analysis.sh 1000000 1000000 ./configuration/exchange.json ./sample_data/sample_market_data.csv
```

The bundled sample is about 326k BTC/USDT book events from Binance, recorded Jan 3, 2023. Results are written to `sample_data/`. To test your own strategy, edit [`src/sample_strategy.h`](src/sample_strategy.h).

## Layout

```
include/backtesting/  backtester, OrderBook, order, trade, strategy, user
include/data/         exchange config, security, event message, time types
include/record/       trade and order logs
src/                  backtest and latency-analysis drivers, sample strategy, plotting
configuration/        exchange.json (rules, fees, latency per venue)
tests/                GoogleTest unit tests (written against an earlier Order API)
docs/                 proposal, API docs, final report
```

## Team

- **Michael Bhang** ([@mbhang2](https://github.com/mbhang2)) built the backtesting core and the latency analysis. He wrote the order, trade, message, security, time, exchange, user and record classes, the JSON-driven configuration and the sample strategy, and extended the order book.
- **Jiayuan (Jerry) Hong** built the original order book and matching engine, including self-match prevention.
- **Ze-Xuan Liu** designed the project structure and CI/CD and handled data processing and unit tests.

Our thanks go to Prof. David Lariviere, the course instructor.

See [`docs/final_report.md`](docs/final_report.md) for the full write-up.
