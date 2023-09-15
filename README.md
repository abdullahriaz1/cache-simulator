First, I had to parse the cmd line args. I decided to use getopt and used the following source: https://www.gnu.org/software/libc/manual/html_node/Using-Getopt.html
I could get the cache size using the colon after the 'N' and set it to optarg.
Next, I had to create a cache structure consisting of two data structures a circular buff, called entries, and a linkedlist, called head.
the circular buff is for holding elements in the cache itself, and the linkedlist is for holding all elements that ever entered the program.
Next, I implemented input functionality to retrieve every key using fgets from the following source: https://stackoverflow.com/questions/3919009/how-to-read-from-stdin-with-fgets
and I found a way to parse that input using '\0': https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
Then I implemented the cache policy based on this. In any policy, if the buff contained the key, it would be a hit. If the buff did not contain the key, but the key was in the linkedlist, then it would be a capacity miss. Otherwise, it would be a compulsory miss.
For FIFO eviction, I only had to worry about replacing the first and last entries to the cache.
For LRU eviction, I had to shift every element down 1 index if the key was being accessed again and placed back at the rear.
For Clock eviction, I used this source: https://www.youtube.com/watch?v=b-dRK8B8dQk&ab_channel=MargoSeltzer
which made me decide to do a while loop through the buff and change the clock pointer each time there was an element that had reference bit 1 (that would change to 0 then). Once an element with reference bit 0 was found, it would be replaced.