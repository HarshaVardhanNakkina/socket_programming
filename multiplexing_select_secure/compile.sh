gcc udp_client.c -o client -lssl -lcrypto
gcc tcp_client.c -o tcp_client -lssl -lcrypto
gcc server.c -o server -lssl -lcrypto
