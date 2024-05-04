# Cryptocurrency Strategy Backtester

[[_TOC_]]

## 1. Abstract

Our proposed semester-long project aims to create specialized backtesting software for cryptocurrencies. The existing software, RCM-X Strategy Studio, lacks support for cryptocurrency assets due to their sub-penny pricing, therefore, the users had to manually adjust prices by multiplying them to maintain a minimum tick size of one cent. Additionally, Strategy Studio lacks features related to latency differences and exchange-specific latency configuration. Hence, this semester, our group is dedicated to develop backtesting software specifically for cryptocurrencies, incorporating more user-configurable variables.

## 2. Introduction

### 2.1 Team Profile

**Min Hyuk (Michael) Bhang**

I am Min Hyuk (Michael) Bhang, an undergraduate majoring in Industiral Engineering (Track option: Economics and Finance) and minoring in Computer Science at the University of Illinois at Urbana-Champaign. My professional industry experience includes investigating Fintech utilizing blockchain technology to prevent fraud and anonymize personal data, and connecting crypto consultee with venture capitals and private equities to raise funds for seed and early stages in East Asia. Alongside the industry experience, I have strong interest and background in day-trading cash equities, futures, options, and cryptocurrencies with 5 years of experience. I currently scalp/daytrade NASDAQ futures (NQ) only on a daily basis as a contracted daytrader at a proprietary firm with AUM of $500k.
    
- Email: [mbhang2@illinois.edu](/fin556_algo_market_micro_fall_2023/fin556_algo_fall_2023_group_02/group_02_project/-/blob/main/mbhang2@illinois.edu) 
- LinkedIn: [Min Hyuk Bhang](https://www.linkedin.com/in/michael-bhang/)

**Jiayuan (Jerry) Hong**

I am a proficient software engineer transitioning from full-stack development to system engineering, with a newfound passion for C/C++ and CUDA programming. My expertise extends to modern CPU architecture, and I will have valuable experience working on the trade execution system of CME Globex at CME Group, focusing on high-frequency trading performance.

As a quick learner, I excel in collaborating with clients to develop efficient, scalable, and user-friendly solutions that address real-world challenges. Goal-oriented and resourceful, I am committed to moving projects forward, taking responsibility for any challenges and obstacles that arise. I'm so exited to explore the wonderful world of trading systems and I'm ready for the challenges ahead!
- Email: jh79@illinois.edu
- LinkedIn: [Jiayuan Hong](https://www.linkedin.com/in/jiayuan-hong-a624121a0/)

**Zexuan Liu**

"TBA"

### 2.2 Terminology

#### 2.2.1 Cryptocurrency

Cryptocurrency refers to a digital or virtual form of currency that utilizes cryptographic techniques for secure financial transactions. Unlike traditional fiat currencies issued by central banks, cryptocurrencies operate on decentralized networks, typically based on blockchain technology. 

Cryptocurrencies are characterized by the following key features:

1. **Decentralization**: Cryptocurrencies are not controlled by any central authority (such as a government or financial institution). Instead, they rely on distributed ledger technology (blockchain) to maintain transaction records across a network of nodes.

2. **Security**: Cryptography ensures the security and integrity of transactions. Public and private keys are used to verify ownership and authorize transfers.

3. **Anonymity**: While transactions are transparent on the blockchain, users’ identities remain pseudonymous. Participants are identified by their wallet addresses rather than personal information.

4. **Limited Supply**: Many cryptocurrencies have a fixed supply cap, ensuring scarcity similar to precious metals. For example, Bitcoin has a maximum supply of 21 million coins.

5. **Volatility**: Cryptocurrency prices can be highly volatile due to factors like market speculation, regulatory changes, and technological developments.

6. **Use Cases**: Cryptocurrencies serve various purposes, including peer-to-peer payments, store of value, smart contracts (via platforms like Ethereum), and decentralized finance (DeFi) applications.

#### 2.2.2 Strategy

A trading strategy is a systematic methodology used for buying and selling in the securities markets. A trading strategy is based on predefined rules and criteria used when making trading decisions ([Hayes, 2022](https://www.investopedia.com/terms/t/trading-strategy.asp)). 

#### 2.2.3 Backtesting

Backtesting refers to the process of evaluating a trading strategy or investment approach using historical data. Traders and investors use backtesting to assess how a particular strategy would have performed in the past, assuming it was applied consistently.

### 2.3 Market Microstructure

Market microstructure refers to the intricate organizational and operational elements of financial markets that influence the trading of assets. It encompasses the processes related to order placement, price determination, trade execution, and the flow of information within a market.

#### 2.3.1 Exchange

cryptocurrency exchange serves as a digital marketplace where participants can buy, sell, and trade various digital assets. Similar to traditional stock exchanges, these platforms facilitate transactions involving cryptocurrencies. Participants interact through mobile apps or desktop interfaces, executing orders based on market conditions. Cryptocurrency exchanges play a crucial role in providing liquidity, price discovery, and access to a wide range of digital currencies. They act as intermediaries, matching buyers with sellers and ensuring efficient trading.

#### 2.3.2 Security

![Currency notation](../images/currency_notation.jpeg)

A security refers to a fungible, negotiable financial instrument that holds some type of monetary value. Within the backtesting software, we use a base-quote notation as shown in the figure above.

#### 2.3.3 Bid and Ask

**Bid Price** 
* The maximum price as which a buyer is willing to pay for a security.
* The best bid price is the highest bid price.
* Represents the demand side of the market.

**Ask (Offer) Price**
* The minimum price at which a seller is willing to sell a security.
* The best ask price is the lowest ask price.
* Represents the supply side of the market.

**Bid-Ask Spread**
* The difference between the best bid and offer (BBO).
* Reflects the cost of trading and represents the profit potential for market makers.
* Narrower spread generally indicates higher liquidity.

#### 2.3.4 Order Book

![Order book image](../images/crypto-order-book-example.png)

The order book is a dynamic, real-time electronic record of buy and sell orders organized by price level for a specific security. It plays a central role in market microstructure, providing transparency into the current supply and demand by informing on price, availability, depth of trade, and who initiates transactions. 

Three major components of an order book are: 

* Buy Orders (Bids):
    * Listed in descending order, with the best bid at the top.
    * Accumulation of buy orders at a specific price point can indicate a support level.

* Sell Orders (Asks or Offers):
    * Listed in ascending order, with the best ask presented first.
    * Likewise, accumulation of sell orders at a specific price point can indicate a resistance level.

* Order History:
    * Refers to the historical data of executed trades that have occurred.
    * Provides insights into past transactions and helps traders analyze market behavior.

#### 2.3.5 Latency

Latency refers to the time delay between the initiation (sending out) of a trade and its receival. In our backtester, order submission-reception latency is considered.

#### 2.3.6 Updates

**Trade Update**: Called when trade occurred.

**Quote Update**: Called when top quote (best bid or ask) price and/or size is updated.

**Depth Update**: Called when non-best bid or ask price and/or size is updated.

#### 2.3.7 Market "Maker" vs Market "Taker"

In financial markets, the interplay between liquidity providers and liquidity consumers is fundamental to the efficient functioning of trading systems. Two key roles emerge from this dynamic: market makers and market takers. Each plays a distinct part in shaping market behavior and ensuring the smooth execution of trades.

**Market Making**

Market makers play a crucial role in financial markets. They are like the architects who construct the framework for trading. A market maker’s primary goal is to provide liquidity by continuously offering to buy and sell a particular security by maintaining a tight bid-ask spread. Market makers profit from the difference between these two prices (the spread). They constantly adjust their quotes based on market conditions, ensuring that buyers and sellers can execute trades efficiently. In return for providing liquidity, market makers receive a premium from market takers.

**Market Taking**

Conversely, market takers are the swift actors. Market takers prioritize immediacy over getting the best price, and take the existing bid or ask price set by market makers. By executing trades promptly, market takers ensure that liquidity remains available. 

#### 2.3.8 Fees

![Trading fee structure example](../images/exchange_fee_structure.png)
*Market makers pay lesser fee (could even get paid) than market takers.*

Cryptocurrency exchanges charge fees for facilitating trades on their platforms. The specific fee structure varies across exchanges and can depend on factors like trading volume, type of order, and membership status. **Makers pay less fee than the takers**, which incentivizes the traders to become market makers and add liquidity to the market.

## 3. Tools and Technology

### 3.1 Programming Languages

- **Bash**: A command-line language commonly used in Unix and Linux environments for scripting and automation. It excels at handling system tasks, file operations, and executing commands in a shell environment. It was used primarily for file operations.

- **C++**: A high-performance object-oriented programming language widely used in finance for its efficiency and speed. It was employed to ensure rapid code execution.

- **Python**: A versatile and widely adopted programming language known for its readability and ease of use. Python is often chosen for rapid development (prototyping), data manipulation, and as a general-purpose language.

### 3.2 External Libraries

- **[BOOST](https://www.boost.org/)**: A comprehensive collection of C++ libraries designed to enhance and extend the capabilities of the C++ programming language. These libraries cover a wide range of functionalities, including linear algebra, pseudorandom number generation, multithreading, image processing, regular expressions, and unit testing. 

- **[DOXYGEN](https://www.doxygen.nl/)**: A widely-used documentation generator tool in software development. It automates the generation of documentation from source code comments, parsing information about classes, functions, and variables to produce output in formats like HTML and PDF.

- **[Matplotlib](https://matplotlib.org):** A comprehensive 2D plotting library for Python. It produces publication-quality figures in a variety of formats and interactive environments across platforms. Matplotlib can be used to generate plots, histograms, power spectra, bar charts, error charts, scatterplots, and more. It serves as the foundation for many other plotting libraries and is highly customizable.

- **[Pandas](https://pandas.pydata.org/):** A powerful data manipulation library for Python, providing data structures like DataFrames that are essential for handling and analyzing structured data efficiently. It was used for data manipulation. 

- **[RapidJSON](https://rapidjson.org/)**: A powerful JSON parser and generator designed specifically for C++. We utilized it to read exchange configuration JSON file quickly.

## 4. Backtesting Algorithm

![Backtesting algorithm](../images/algorithm.png)

1. Market data parser reads data line by line.

2. Update order book data accordingly, and check if any of the user's order resting on the orderbook has been filled either partially or fully.

3. User's strategy's function is called based on the update type.

4. If user submitted new order(s) add them to the current orders.

5. Check if order arrived at the exchange (based on sending latency).

6. Check if order could be filled immediately.

7. Record user's trades that took place.

Once the backtesting is over, list of trades is outputted to the CSV file to aid further data manipulation/analysis for the user.

## 5. Latency Analysis

Current version of latency analysis performs backteset given hypothetical sending latency of 0, 10, 25, 50, 100, 200, 500, 1000 nanoseconds. Then the software outputs the latency vs P&L CSV file as well as the log-latency vs P&L plot to visualize the effect of latency to the user's strategy.

## 6. Future Work

## 7. Project Reflections
