import yfinance as yf
import datetime as dt
import numpy as np

output_file = open("input/output.cnf", "w")

input_file = open("input/input.cnf", "r")
input_data = input_file.readlines()

date_start, date_end = input_data[0].strip(), input_data[1].strip()
date_start = dt.datetime.strptime(date_start, "%Y-%m-%d").date()
date_end = dt.datetime.strptime(date_end, "%Y-%m-%d").date()

output_file.write(str(date_start) + "\n")
output_file.write(str(date_end) + "\n")

tickers = []
for curr_row in input_data[2:]:
    ticker = curr_row.strip()
    if ticker != "":
        tickers.append(ticker)

tickers.sort()
output_file.write(",".join(tickers) + "\n")

for ticker in tickers:
    stock_data = yf.download(ticker, start=date_start, end=date_end, interval='1h')
    if not stock_data.empty:
        totalProfit = (stock_data.iloc[-1]['Close'] - stock_data.iloc[0]['Open']) / stock_data.iloc[0]['Open'] * 100
        output_file.write(str(totalProfit) + ",")
        output_file.write(str(np.std(stock_data['Close'] / 100)) + "\n")

output_file.write("=====\n")

stock_data = yf.download(tickers, start=date_start, end=date_end, interval='1h')
returns = stock_data['Close'].pct_change().dropna()
correlation_matrix = returns.corr()

for index, row in correlation_matrix.iterrows():
    output_file.write(",".join([str(i) for i in row.tolist()]) + "\n")

input_file.close()
output_file.close()
