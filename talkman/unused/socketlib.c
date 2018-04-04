// Socketlib.c

int server_tcp_socket(char *hostname, int port) {

  struct sockaddr_in sn;
  int s;
  struct hostent *he;

  if (!(he = gethostbyname(hostname))) {
    fprintf(stderr, "gethostname()");
    exit(1);
  }

  bzero((char *)&sn, sizeof(sn));

  sn.sin_family = AF_INET;
  sn.sin_port   = htons((short)port);
  sn.sin_addr   = *(struct in_addr *)(he->h_addr_list[0]);

  if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket()");
    exit(1);
  }

  if (bind(s, (struct sockaddr*)&sn, sizeof(sn)) == -1) {
    perror("bind()");
    exit(1);
  }

  return s;
}

int accept_tcp_connection(int s) {

  struct sockaddr_in sn;
  socklen_t l = sizeof(sn);
  int x;

  sn.sin_family = AF_INET;

  if (listen(s, 1) == -1) {
    perror("listen()");
    exit(1);
  }

  x = accept(s, (struct sockaddr *)&sn, (socklen_t *)&l);

  if (x == -1) {
    perror("accept()");
    exit(1);
  }
  return x;
}

int request_tcp_connection (char *hn, int port) {

  struct sockaddr_in sn;
  int s, ok;
  struct hostent *he;

  if (!(he = gethostbyname(hn))) {
    fprintf(stderr, "gethostname()");
    exit(1);
  }

  ok = 0;

  while (!ok) {
    sn.sin_family = AF_INER;
    sn.sin_port = htons((unsigned short)port);
    sn.sin_addr.s_addr = *(uint32_t *)(he -> h_addr_list[0]);

    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      perror("socket()");
      exit(1);
    }
    ok = connect(s, (struct sockaddr *)&sn, sizeof(sn) != -1);

    if (!ok)
      sleep(1);
  }
  return s;
}
