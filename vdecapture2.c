/* vdecapture2
 * Capture vde traffic in pcap format
 *
 * (c) C.Lohr 2025
 *
 * Acknoledments: Renzo Davoli & Virtualsquare for vdecapture
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <stdbool.h>
#include <libvdeplug.h>
#include <libgen.h>
#include <unistd.h>
#include <signal.h>
#include <sys/uio.h>
#include <errno.h>

#define SNAPLEN 65535

void usage(char *prog, int e) {
  fprintf(stderr, "Usage: %s [-o <output>] [-h] [<VNL>]\n"
          "Capture vde traffic in pcap format\n"
          "Options:\n"
          " -o output Pcap file (default '-', meaning stdout) \n"
          " -h        Display this help\n"
          " VNL       Virtual Network Locator (e.g. vde:///tmp/vde.ctl) see vde_plug(1)\n",
           basename(prog));
  exit(e);
}


void parse_opts(int argc, char *argv[], char **output, char **vnl) {
  int opt;
  
  while ((opt = getopt(argc, argv, "o:h")) != -1) {
    switch (opt) {
    case 'o':
      *output = optarg;
      break;
    case 'h':
      usage(argv[0], EXIT_SUCCESS);
    default:
      fprintf(stderr, "\n");
      usage(argv[0], EXIT_FAILURE);
    }
  }
  if (argc == (optind+1) )
    *vnl = argv[optind];
}


int get_pcap(char *output) {
  int fd;
  
  if ( strncmp(output, "-", 1) == 0 )
    fd = STDOUT_FILENO;
  else {
   fd = open(output, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH );
    if (fd == -1) {
      perror(output);
      exit(EXIT_FAILURE);
    }
  }

  const struct {
    uint32_t magic_number;
    uint16_t version_major;
    uint16_t version_minor;
     int32_t thiszone;
    uint32_t sigfigs;
    uint32_t snaplen;
    uint32_t network;
  } hdr = { 0xa1b2c3d4, 2, 4, 0, 0, SNAPLEN, 1 };

  if ( write(fd, &hdr, sizeof(hdr)) == -1 ) {
    perror("write hdr");
    exit(EXIT_FAILURE);
  }

  return fd;
}


// return false in case of error (broken pipe), to retry smoothly
bool write_pkt(int fd, const void *buf, size_t len) {
  struct timeval now;
  gettimeofday(&now, NULL);

  struct {
    uint32_t ts_sec;
    uint32_t ts_usec;
    uint32_t incl_len;
    uint32_t orig_len;
  } hdr = { now.tv_sec, now.tv_usec, len, len };

  struct iovec iov[2];
  iov[0].iov_base = &hdr;
  iov[0].iov_len = sizeof(hdr);
  iov[1].iov_base = (void *)buf;
  iov[1].iov_len = len;
  
  if ( writev(fd, iov, 2) == -1 ) {
    if ( errno == EPIPE ) // Write on a fifo, reader left
      return true;        // retry smoothly
    perror("write pkt");  // Else, abord.
    exit(EXIT_FAILURE);
  }
  return false;
}


VDECONN *conn = NULL;
int fd = -1;

void bye() {
  close(fd);
  vde_close(conn);
}

void sighandler(int) {
 exit(EXIT_SUCCESS);
}


int main(int argc, char **argv) {
  char *output = "-";
  char *vnl = "";

  parse_opts(argc, argv, &output, &vnl);
  
  conn = vde_open(vnl, "vdereplay", NULL);
  if (conn == NULL) {
    fprintf(stderr, "%s: vde_open error '%s'\n\n", argv[0], vnl);
    usage(argv[0], EXIT_FAILURE);
  }
  
  fd = get_pcap(output);

  atexit(bye);

  signal(SIGHUP, sighandler);
  signal(SIGINT, sighandler);
  signal(SIGQUIT, sighandler);
  signal(SIGTERM, sighandler);
  signal(SIGUSR1, sighandler);
  signal(SIGUSR2, sighandler);
  signal(SIGPIPE, SIG_IGN);

  char buf[SNAPLEN];
  while (true) {

    ssize_t n = vde_recv(conn, buf, SNAPLEN, 0);
    if (n <= 0)
      exit(EXIT_SUCCESS);
    
    if ( write_pkt(fd, buf, n) ) {
      close(fd);
      fd = get_pcap(output);
    }
  }
}
