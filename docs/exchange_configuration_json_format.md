## Exchange Configuration Json Format

``` cpp
{
    "Exchange 1": {
        /**
         * Latency to exchange in nanoseconds
         * @type int
         */
        "nanosecondLatencyTo": 800,
        
        /**
         * Latency from exchange in nanoseconds
         * @type int
         */
        "nanosecondLatencyFrom": 1000,
        
        /**
         * Trading Rules
         */
         "tradingRules": {
            "Spot/Margin": {
                /**
                 * Indices:
                 * 0: minimum tick size in quote currency
                 * 1: minimum order quantity in base currency
                 * 2: minimum order value in quote currency
                 * 3: maximum limit order quantity in base currency
                 * 4: maximum limit order value in quote currency
                 * 5: maximum market order quantity in base currency
                 * 6: maximum market order value in quote currency
                 * 7: maximum number of open limit order
                 * 8: limit order price limit (%)
                 * 9: market order price limit (%)
                 * 10: maximum isolated leverage (margin)
                 * 11: maximum cross leverage (margin)
                 *
                 * Any empty values should be -1
                 * @type double
                 */
                "Security 1": [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11],
                "Security 2": [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11],
                ...
            },
            "Futures": {
                ...
            }
         },
         "feeStructure": {
             "Spot/Margin": {
                /**
                 * Fee schedule from lowest status to highest in %
                 */
                 "Maker": [0.3, 0.2, 0.1, 0.05],
                 "Taker": [0.5, 0.4, 0.3, 0.2]
             },
             "Futures": {
                 ...
             }
         }
    },
    "Exchange 2": {
        ...
    }
}
```