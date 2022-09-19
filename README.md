# Test snake

Originally this program was just a test to try to **write a snake without
storing snake positions in a table** as we usually do. Here I use only one table
that contains the map and the snake. After each moves I use a recursive function
to move all the body of the snake.

## Not effective

This is obviously not an efficient way to do the things but it is fun. When the
snake moves I have to search for each part of the body, the one which comes next
(I use numbers in the grid for this, the head is 2, the first body bloc is 3,
...) which means looking at all the surrounding cases in the map. As I am doing
this for each part of the snake, this multiply the time complexity (just for
moving the snake) by 8. This is a lot more than moving values in a table.

In terms of time complexity, the best for the snake is to use a double-liked
list but it can be considered as a waste of space because those data structures
are quite heavy.

## Not completely bad

Something we have to check each time the snake moves is that we have to make
sure that the snake doesn't hit itself. To do this we normally have to check if
the new head of the snake is in the list of the snake's body. Here we just have
to check if the new position in the map is 0. Of course this is not a huge time
saving comparing to what we are loosing when the snake moves but it still
interesting to notice.

## Conclusion

This had to be a simple and quick test, it was just to try even if I already
knew that the algorithm was terrible, but it happened that I spent way to much
time on this. A least, even if the time complexity is terrible, the grid is not
to big so it works and it's fun to play (like a normal snake actually) so that's
why I spent a little more time to change it and making it more usable.
