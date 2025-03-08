
# Google Hash Code 2021 Practice Problem

## Representing the data

In order to determine how to achieve the best deliveries, a query that would be executed a lot is **how many unique ingredients would result from the reunion of the
sets** which contain the ingredients of some specific pizzas. Since there can be many pizzas in a dataset, and many unique ingredients(10000), it is important for this
operation to be fast and memory efficient. For this reason, a unique number is associated to each ingredient. Each Pizza object will contain a **bitset** of length equal
to the number of unique ingredients in all the data set. For each index i, if bitset[i] equals 1, then that ingredient is present in that specific pizza. This means that
the number of unique ingredients in a delivery can be computed in **O(n)**, with **n / 8 bytes** of memory used per pizza, where **n** is the number of unique ingredients
in the dataset.

## Solution 1

This solution is a combination of greedy and backtracking. The pizza vector is sorted descendingly based on the number of ingredients they contain. Then, a list is created
from this vector, since the simulation will imply a lot of deletions, which list is capable of executing in **O(1)**. At each step, the first element of the list is selected.
Then, the program gets the best deliveries of 2, 3, and 4 pizzas which contain the first element. Those are selected by applying backtracking on the next **b** elements from
the list. This **b** is defined as a constant, and it can be increased depending on how beefy your computer is. Obviously, the more you can increase it, the better results
you will get. For this attempt, I set it to **10**. The score for each delivery is calculated as **(nr_unique_ingredients - (nr_ingredients_wasted *0,67))**. 

#### Score

| File                                   | Score         |
|----------------------------------------|--------------|
| *a_example.in*                         | 74           |
| *b_little_bit_of_everything.in*        | 8193         |
| *c_many_ingredients.in*                | 686298420    |
| *d_many_pizzas.in*                     | 5915208      |
| *e_many_teams.in*                      | 8358631      |
| **Total**                              | **700580526**|

## Solution 2: Parallel post-processing

This solution is similar to solution 1, but after the result is obtained, we go over the deliveries, and try to obtain a better score by swapping pizza i from one delivery
with pizza j from another. For the bigger datasets this could take days, so this operation is done on **two threads** to achieve more in the same time. Also, this operation
is set to stop after a certain time limit was exceeded(this time, I set it to 30 minutes).

#### Score

| File                                   | Score         |
|----------------------------------------|--------------|
| *a_example.in*                         | 74           |
| *b_little_bit_of_everything.in*        | 9097         |
| *c_many_ingredients.in*                | 693570291    |
| *d_many_pizzas.in*                     | 6469059      |
| *e_many_teams.in*                      | 9168995      |
| **Total**                              | **709217516**|