# Cryptocurrency Strategy Backtester

## 1. Abstract

Our proposed semester-long project aims to create specialized backtesting software for cryptocurrencies. The existing software, RCM-X Strategy Studio, lacks support for cryptocurrency assets due to their sub-penny pricing, therefore, the users had to manually adjust prices by multiplying them to maintain a minimum tick size of one cent. Additionally, Strategy Studio lacks features related to latency differences and exchange-specific latency configuration. Hence, this semester, our group is dedicated to developing backtesting software specifically for cryptocurrencies, incorporating more user-configurable variables.

## 2. Introduction

### 2.1 Team Profile

**Min Hyuk (Michael) Bhang**

I am Min Hyuk (Michael) Bhang, an undergraduate majoring in Industrial Engineering (Track option: Economics and Finance) and minoring in Computer Science at the University of Illinois at Urbana-Champaign. My professional industry experience includes investigating Fintech utilizing blockchain technology to prevent fraud and anonymize personal data, and connecting crypto consultees with venture capitals and private equities to raise funds for seed and early stages in East Asia. Alongside my industry experience, I have a strong interest and background in day-trading cash equities, futures, options, and cryptocurrencies with 5 years of experience. I currently scalp/daytrade NASDAQ futures (NQ) only on a daily basis as a contracted day trader at a proprietary firm with an AUM of $500k.
    
- Email: [mbhang2@illinois.edu](/fin556_algo_market_micro_fall_2023/fin556_algo_fall_2023_group_02/group_02_project/-/blob/main/mbhang2@illinois.edu) 
- LinkedIn: [Min Hyuk Bhang](https://www.linkedin.com/in/michael-bhang/)

**Jiayuan (Jerry) Hong**

I am a proficient software engineer transitioning from full-stack development to system engineering, with a neI am a proficient software engineer transitioning from full-stack development to system engineering, with a newfound passion for C/C++ and CUDA programming. My expertise extends to modern CPU architecture, and I will have valuable experience working on the trade execution system of CME Globex at CME Group, focusing on high-frequency trading performance. Goal-oriented and resourceful, I am committed to moving projects forward, and taking responsibility for any challenges and obstacles that arise. I'm so excited to explore the wonderful world of trading systems and I'm ready for the challenges ahead!
- Email: jh79@illinois.edu
- LinkedIn: [Jiayuan Hong](https://www.linkedin.com/in/jiayuan-hong-a624121a0/)

**Zexuan Liu**

This is Ze-Xuan Liu, a third-year PhD student interested in operational management. I am currently leading research 
projects in online decision-making and supply chain management. In my spare time, I am a Solana chain enthusiast and 
developing decentralized arbitrage and market-making strategies. 

- Email: [zexuanl5@illinois.edu](mailto:zexuanl5@illinois.edu)
- LinkedIn: [Ze-Xuan Liu](https://www.linkedin.com/in/zexuan-liu-b1966119b/)

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

#### 2.2.4 Order Match Engine
An order matching engine (OME) is a software system that matches buy and sell orders from market participants to facilitate the execution of trades.

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
* A narrower spread generally indicates higher liquidity.

#### 2.3.4 Order Book

![Order book image](../images/crypto-order-book-example.png)

The order book is a dynamic, real-time electronic record of buy and sell orders organized by price level for a specific security. It plays a central role in market microstructure, providing transparency into the current supply and demand by informing on price, availability, depth of trade, and who initiates transactions. 

Three major components of an order book are: 

* Buy Orders (Bids):
    * Listed in descending order, with the best bid at the top.
    * Accumulation of buy orders at a specific price point can indicate a support level.

* Sell Orders (Asks or Offers):
    * Listed in ascending order, with the best ask presented first.
    * Likewise, the accumulation of sell orders at a specific price point can indicate a resistance level.

* Order History:
    * Refers to the historical data of executed trades that have occurred.
    * Provides insights into past transactions and helps traders analyze market behavior.

#### 2.3.5 Latency

Latency refers to the time delay between the initiation (sending out) of a trade and its receival. In our backtester, order submission-reception latency is considered.

#### 2.3.6 Updates

**Trade Update**: Called when the trade occurred.

**Quote Update**: Called when top quote (best bid or ask) price and/or size is updated.

**Depth Update**: Called when a non-best bid or ask price and/or size is updated.

#### 2.3.7 Market "Maker" vs Market "Taker"

In financial markets, the interplay between liquidity providers and liquidity consumers is fundamental to the efficient functioning of trading systems. Two key roles emerge from this dynamic: market makers and market takers. Each plays a distinct part in shaping market behavior and ensuring the smooth execution of trades.

**Market Making**

Market makers play a crucial role in financial markets. They are like the architects who construct the framework for trading. A market maker’s primary goal is to provide liquidity by continuously offering to buy and sell a particular security by maintaining a tight bid-ask spread. Market makers profit from the difference between these two prices (the spread). They constantly adjust their quotes based on market conditions, ensuring that buyers and sellers can execute trades efficiently. In return for providing liquidity, market makers receive a premium from market takers.

**Market Taking**

Conversely, market takers are the swift actors. Market takers prioritize immediacy over getting the best price, and take the existing bid or ask price set by market makers. By executing trades promptly, market takers ensure that liquidity remains available. 

#### 2.3.8 Fees

![Trading fee structure example](../images/exchange_fee_structure.png)

*Market makers pay lesser fees (could even get paid) than market takers.*

Cryptocurrency exchanges charge fees for facilitating trades on their platforms. The specific fee structure varies across exchanges and can depend on factors like trading volume, type of order, and membership status. **Makers pay less fee than the takers**, which incentivizes the traders to become market makers and add liquidity to the market.

#### 2.3.9 Trading Rules

![trade_rules](../images/trade_rules.png)

Exchanges have trading rules that each order needs to meet to be accepted.

* **Minimum Tick Size**:
    * Refers to the minimum price movement for a given cryptocurrency or stock.
    * Determines how much the price can change in a single step.
    * Smaller tick sizes allow for finer price granularity.

* **Minimum Order Quantity/Value**:
    * Smallest amount of cryptocurrency that can be bought or sold in a single trade.
    * Ensures that orders contribute meaningfully to market liquidity.

* **Maximum Order Quantity/Value**:
    * Exchanges often impose maximum order quantities to prevent market manipulation and maintain liquidity.
    * These limits vary based on the specific cryptocurrency being traded.

#### 2.3.10 Market Type

In the crypto market, two major market types are spot/margin and perpetual futures.

| **Aspect** | **Spot/Margin** | **Perpetual Futures** |
|--------|-------------|-------------------|
| **Ownership** | Direct ownership | No ownership |
| **Settlement** | Immediate | No contract expiration date; indefinite holding |
| **Leverage** | Partially available for margin | Available (sometimes up to 125x) | 
| **Risk Level** | Low - Mid | High |
| **Liquidation Risk** | Margin call possible if trading on margin | Liquidation possible |

**Spot/Margin**:

* Refers to the direct exchange of cryptocurrencies for immediate delivery and settlement.
* Could use leverage to trade the spot market on a margin.
* When a trader buys a cryptocurrency in the spot market, the trader actually owns the asset and can use it (transfer, hold, or use it for transactions).
* Transactions are settled “on the spot,” meaning ownership of the asset is transferred immediately.

**Perpetual Futures**:

* Derivative contracts that allow traders to speculate on the price movement of a cryptocurrency without owning the underlying asset.
* Traders don’t own the actual cryptocurrency; they trade contracts representing its value.
* Perpetual futures do not have an expiry date; traders can hold positions indefinitely.
* Perpetual futures allow for leverage, potentially leading to higher profits or losses.

#### 2.3.11 Leverage

Leverage refers to using borrowed capital to open a trading position, allowing traders to control a larger position size than their available capital. In the crypto market, leverage enables traders to amplify both potential gains and potential losses. In the crypto market, there are two types of leverages: cross and isolated.

**Cross Margin**:

* In cross-margin mode, your entire account balance is used to margin all open positions.
* P&L from one position can support another position that is close to liquidation.
* Cross margin carries the risk of losing the entire account if liquidation occurs.
* Traders using cross-margin must add more funds to prevent liquidation.

**Isolated Margin**:

* In isolated margin mode, you allocate margin specifically to a single position or trading pair.
* Trader transfers funds into the isolated margin for a specific trade, which allows traders to manage risk for a specific position by choosing how much margin is allocated to it.
* Only the funds allocated to a position can be liquidated, limiting potential losses.

#### 2.3.12 Order Type

Four major type of orders are: Limit, Market, Stop, and Stop Limit.

**Limit**: 
* Allows investors to buy or sell securities at a specific price in the future.
* The trader sets the maximum or minimum price at which the trader is willing to buy or sell.
* The order will only be filled if the market reaches the pre-defined level.
* Limit orders provide **control over execution price** but **no guarantee of execution**.

![limit order](../images/limit.png)

**Market**:
* Instructs the exchange to buy or sell immediately at the current market price.
* When placed:
    * If buying, the trader will pay a price at or near the posted ask (selling) price.
    * If selling, the trader will receive a price at or near the posted bid (buying) price.
* Market orders **guarantee immediate execution** but **not a specific price**.

![market order](../images/market.png)

**Stop**:
* Becomes a market order when the stock reaches a specified price.
* Often used to limit losses or lock in profits.

![stop order](../images/stop.png)

**Stop Limit**:
* Combines features of stop and limit orders; becomes a limit order when the stock reaches a specified price.
* Trader sets both a stop price (when the order becomes active) and a limit price (the maximum or minimum price you’re willing to accept).

![stop limit order](../images/stoplimit.png)

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

- **[RapidJSON](https://rapidjson.org/)**: A powerful JSON parser and generator designed specifically for C++. We utilized it to read the exchange configuration JSON file quickly.

## 4. Backtesting Algorithm

![Backtesting algorithm](../images/algorithm.png)

1. Market data parser reads data line by line.

2. Update order book data accordingly, and check if any of the user's order resting on the orderbook has been filled either partially or fully.

3. The user's strategy's function is called based on the update type.

4. If the user submitted new order(s) add them to the current orders.

5. Check if the order arrived at the exchange (based on sending latency).

6. Check if the order can be filled immediately.

7. Record the user's trades that took place.

Once the backtesting is over, a list of trades is outputted to the CSV file to aid further data manipulation/analysis for the user.

## 5. Latency Analysis

The current version of latency analysis performs a backtest given hypothetical sending latency of 0, 10, 25, 50, 100, 200, 500, and 1000 nanoseconds. Then the software outputs the latency vs P&L CSV file as well as the log-latency vs P&L plot to visualize the effect of latency on the user's strategy.

## 6. Future Work

1. Class Exchange: More Freedom in Latency Configuration

    * Latency, in fact, is not set throughout the backtesting period but could vary based on the time of day, and day of the week.
    * We plan to have a method for the user to be able to configure latency more freely and realistically.

2. Class Order: Ensure Exchange's Market Type Supports Certain Order Type

    * There could be a case where some exchange does not support a certain order type in a certain market type.
    * Further research needs to be done to identify and include these restrictions.

3. Class Order: Advanced Order Types

    * There are more advanced order types that have not been yet implemented such as trailing stop, or one-cancels-other (OCO).

4. Class Order: Rate Limit

    * When trading through an API, exchanges have a rate limit, which limits the number/weight of API calls in a given rolling time window.
    * Further research needs to be done to implement the rate limit feature, which has different allowances per user level and different rates per different API calls.

5. Class User: Add Restriction per Subaccount

    * User can have multiple subaccounts, and there can be restrictions on those subaccounts, for example, this account can only trade spot market without leverage.

6. Class User: Risk/Position Management per Firm/User/Subaccount

    * Suppose a user has two subaccounts that are each long in Spot BTC/USDT and long 10x in BTC/USDT perpetual futures. The user has great exposure to risk regarding the price movement of BTC/USDT.
    * Implement a method to return or visualize the exposure to certain security.

7. General: Exception Logger

    * Currently, a fatal error (such as not meeting trade rules) throws an exception.
    * We would like to have an error logger that could record all these errors.

## 7. Project Reflections

**Min Hyuk (Michael) Bhang**

1. **What did you specifically do individually for this project?**
    * Developing backtesting main framework and latency analysis
    * Setting up to have configuration customizable
    * Writing sample strategy to test backtesting
    * Order, Trade, Message, Security, TimeType, Exchange, User, Record classes
    * Improving Orderbook class
    * Implementing external libraries (Boost, RapidJson)
    * Documentation

2. **What did you learn as a result of doing your project?**

    From this project, I was not only able to further understand the market microstructure, but, more importantly, was able to train what core developers do at HFT firms: developing backtesting engines for the firm, and handling market data and order entries.

3. **If you had a time machine and could go back to the beginning, what would you have done differently?**

    If I could go back to the beginning of the project, I would like to make use of smart pointers from the start of the project. While developing the main backtesting framework, we got stuck in various memory and lifetime issues for a while since we were quite skeptical about the use of pointers. However, smart pointers have resolved all of those immediately; if we have employed them since the beginning, we could have had more time to develop more advanced features.

4. **If you were to continue working on this project, what would you continue to do to improve it, how, and why?**

    If I were to continue working on this project, I would like to support more advanced order types, improve the latency analysis tool as well as also supporting real-time paper and actual trading through API access, as mentioned in the future work section above.

5. **What advice do you offer to future students taking this course and working on their semester long project?**

    First and foremost, I would like to recommend future students to make great use of Large Language Models (LLM) such as ChatGPT or Copilot to aid the coding process. LLMs have been extremely helpful, acting as the fourth member of the team.

    Also, drawing out the entire architecture and sub-algorithms during the planning process and in-line commenting as coding is crucial. Drawing out the architecture and sub-algorithms enables the group to understand the development steps by heart. In addition, by commenting and having legible codes, your teammates could easily go over the code and provide any necessary help without any struggle.


**Jiayuan (Jerry) Hong**
1. **What did you specifically do individually for this project?**
  In this project, I mainly work on the order book and order match engine. Choosing the right data structure (map, priority queue) and building the OME. We also add Self-match prevention (SME) into the match engine to prevent self-match orders. In the design process, I firstly had a meeting with Michael and developed a simplified order book that can support basic `addLevel` `addOrder`, `tradeHappened`. Then, when we were given real-world market data, we developed more practical APIs to help us manage the order book. And OME will help us match the resting orders with an aggressive order.
   
1. **What did you learn as a result of doing your project?**
   In this project, I learned tons of things not only the knowledge about the financial market and backtesting, but also how we could utilize different tools to help us write clean and efficient code.
    
    Most importantly, learning from other people
    - Zexuan is very knowledgeable and patient. He loves to share with me about his ideas and what projects he is working on. He has an enthusiasm for quantitive trading, crypto, and currencies, always inspiring me in the quant field.
    - Michael has strong C++ programming skills and tons of knowledge about finance. We worked on the order book and match engine together. Michael is a great leader, and teacher and always inspired me to learn more about the finance world.
2. **If you had a time machine and could go back to the beginning, what would you have done differently?**
   I will first learn more basic financial knowledge and how the HFT works as well as some core ideas and design patterns of the order matching engine. Therefore I will quickly finish developing the Orderbook class and implementing all necessary functions. And I could have more time for some advanced functions and latency-analysis staff.
3. **If you were to continue working on this project, what would you continue to do to improve it, how, and why?**
   If I were to continue working on the project, I would design a more advanced API and think in a more system-design level, and borrow some ideas from the industry, for example, maybe we could use a distributed/multi-threaded system to run our backtesting software.
4. **What advice do you offer to future students taking this course and working on their semester long project?**
   Before designing, it is good to have a basic foundation knowledge of financial/trading systems. Before writing any code, you are supposed to have an overall design or a blueprint in your brain. Then divide that into different stages and small pieces and appropriately schedule your design timeline. Additionally, taking advantage of using tools like ChatGPT and Copilot can be very helpful and beneficial.


**Zexuan Liu**
Throughout this project, we explored the complexities of developing cryptocurrency backtesting software that can simulate real-world trading scenarios. 
We encountered various challenges, such as handling latency differences, implementing order-matching logic, and ensuring compatibility with different exchange configurations. 
It is a prototype at the moment and can be further developed to include more advanced features and functionalities, such as additional order types, randomized spread with Monte Carlo simulation, and more realistic latency configuration.

We first adopted CMAKE for the project but later switched to a simpler Makefile due to the project's size and complexity. 
Also, we prepared a complicated CI/CD infrastructure, but later which was used little. We learned that it is important to keep the project simple and focused on the main goal. And we need better pre-project brainstorming to avoid unnecessary work -- pay attention before starting the project.

We also learned the importance of effective communication and collaboration among team members. For example, we sometimes had different interpretations of one design idea because of the languages and terms we used. We think it would be better if we could have a more detailed document to review after each meeting and that can align our understanding.

We also enjoyed in the process of learning new technologies and tools, such as Doxygen, RapidJSON, and most importantly, the crypto market. 

**Key Points**:
- I learned a lot from Jiayuan and Michael:
    - Jiayuan is always curious and eager to learn. We had several discussions about the project and also extensions to the finance, crypto markets, and job market. I enjoyed a lot from our conversations.
    - Michael is a very responsible programmer and trader. He always finishes his part on time and tries to solve the problem by himself. His attitude towards the project inspired me a lot. Also, he is a good team leader who can always keep the project on track.
- I think I should have put more effort into the project. I basically designed and deployed the infrastructure of CI/CD, the building process, and the project design. However, I made it too complicated and do not shift my focus to the main goal of the project. After that, I switched to finish the data processing part but after a while michael found he has an exising code base can be used. I literally feel I worked as a consultant for this project like debug and design,  but has few code input. 
- I also learned that there are few document and too many different designs in crypto world. Almost all CEXes have their own API and fee structures. It need large effort to design a trading bot and backtesting environment to fit in the production environment.


