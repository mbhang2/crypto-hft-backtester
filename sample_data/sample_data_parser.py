import numpy as numpy
import pandas as pd

trade_df = pd.read_csv("./sample_data/20230103_trades.csv")
trade_df["PRICE"] *= 180
trade_df["PRICE"].round(2)
trade_df["SIZE"] /= 100

del trade_df["TRADE_ID"]
del trade_df["TRADE_FLAGS"]

quote_df = pd.read_csv("./sample_data/20230103_book_updates.csv")
quote_df["BID_PRICE_1"] *= 180
quote_df["BID_PRICE_2"] *= 180
quote_df["BID_PRICE_3"] *= 180
quote_df["ASK_PRICE_1"] *= 180
quote_df["ASK_PRICE_2"] *= 180
quote_df["ASK_PRICE_3"] *= 180
quote_df["BID_SIZE_1"] /= 100
quote_df["BID_SIZE_2"] /= 100
quote_df["BID_SIZE_3"] /= 100
quote_df["ASK_SIZE_1"] /= 100
quote_df["ASK_SIZE_2"] /= 100
quote_df["ASK_SIZE_3"] /= 100

result_df = pd.concat([trade_df, quote_df])
result_df["SYMBOL"] = "BTC/USDT"
result_df["MARKET_CENTER"] = "Binance"
result_df["MARKET_TYPE"] = "S"

result_df["PRICE"] = result_df["PRICE"].round(2)
result_df["BID_PRICE_1"] = result_df["BID_PRICE_1"].round(2)
result_df["BID_PRICE_2"] = result_df["BID_PRICE_2"].round(2)
result_df["BID_PRICE_3"] = result_df["BID_PRICE_3"].round(2)
result_df["ASK_PRICE_1"] = result_df["ASK_PRICE_1"].round(2)
result_df["ASK_PRICE_2"] = result_df["ASK_PRICE_2"].round(2)
result_df["ASK_PRICE_3"] = result_df["ASK_PRICE_3"].round(2)

result_df.sort_values(by=["COLLECTION_TIME"], inplace=True)
result_df.reset_index(drop=True, inplace=True)

result_df.to_csv("./sample_data/sample_input.csv", index=False)
