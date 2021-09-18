# minimum loss 

[Problem](https://www.hackerrank.com/challenges/minimum-loss/problem)

## Solution 

Create a dictionary like structure, where the inserted keys are stored sorted

The key is the price, the value is the time(index of the original array).

To find the answer, we need to check the differences between the prices, but since it's sorted, and we need to find the minimum value, only the neighbouring elements should be checked. 

It's is important to check whether the buy time was earlier.

```
  for every (price_t, t), price_t+1, t+1) 
  do
    if t - t+1 negative:
      if smallest_diff > price_t+1 - price_t:
        set smallest_diff to the difference
  end 
```
