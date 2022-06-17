gcc -o server ex4_srv.c
gcc -o client ex4_client.c

./server &
sleep 3

process_id=`ps -s $COMMAND| grep "./[s]erver" | awk '{print $2}'`

./client $process_id 1 1 1 &

./client $process_id 2 2 1 &

./client $process_id 1 1 4 &

./client $process_id 2 3 2 &