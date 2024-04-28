import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("./sample_data/sample_result.csv")
df["TIMESTAMP"] = pd.to_datetime(df["TIMESTAMP"])

plt.figure(figsize=(10, 6))
plt.plot(df["TIMESTAMP"], df["SPOT_BALANCE"], marker="o")
plt.xlabel("Timestamp")
plt.ylabel("Spot Balance ($)")
plt.title("Spot Balance vs. Timestamp")
plt.grid(True)
plt.show()