echo "read test"


for logN in {10..34}
do
for unused in {1,..10}
do
perf stat -d  ./prefetch_test $logN 1 >/dev/null 2>/dev/null
done 
perf stat -d  ./prefetch_test $logN 1 2>&1 | grep L1-dcache-load-misses | sed 's/   */:/g' | cut -d : -f 2
done 

echo "write read test"


for logN in {10..34}
do
for unused in {1,..10}
do
perf stat -d  ./prefetch_test $logN 2 >/dev/null 2>/dev/null
done 
perf stat -d  ./prefetch_test $logN 2 2>&1 | grep L1-dcache-load-misses | sed 's/   */:/g' | cut -d : -f 2
done 