// /* For sockaddr_in */
// #include <netinet/in.h>
// /* For socket functions */
// #include <sys/socket.h>
// /* For gethostbyname */
// #include <netdb.h>

// #include <stdio.h>
// #include <string.h>
// #include <unistd.h>

// int getSocket(int family, int type, int protocol) {
//   int fd;
//   if ((fd = socket(family, type, protocol)) < 0) {
//     perror("socket");
//   }
//   return fd;
// }

// int main(int c, char **v) {
//   const char query[] =
//       "GET / HTTP/1.0\r\n"
//       "Host: www.google.com\r\n"
//       "\r\n";
//   const char hostname[] = "www.google.com";
//   struct sockaddr_in sin;
//   struct hostent *h;
//   const char *cp;
//   int fd;
//   ssize_t n_written, remaining;
//   char buf[1024];

//   /* Look up the IP address for the hostname.   Watch out; this isn't
//      threadsafe on most platforms. */
//   h = gethostbyname(hostname);
//   if (!h) {
//     fprintf(stderr, "Couldn't lookup %s: %s", hostname, hstrerror(h_errno));
//     return 1;
//   }
//   if (h->h_addrtype != AF_INET) {
//     fprintf(stderr, "No ipv6 support, sorry.");
//     return 1;
//   }

//   /* Allocate a new socket */
//   fd = getSocket(AF_INET, SOCK_STREAM, 0);

//   /* Connect to the remote host. */
//   sin.sin_family = AF_INET;
//   sin.sin_port = htons(80);
//   sin.sin_addr = *(struct in_addr *)h->h_addr;
//   if (connect(fd, (struct sockaddr *)&sin, sizeof(sin))) {
//     perror("connect");
//     close(fd);
//     return 1;
//   }

//   /* Write the query. */
//   /* XXX Can send succeed partially? */
//   cp = query;
//   remaining = strlen(query);
//   while (remaining) {
//     n_written = send(fd, cp, remaining, 0);
//     if (n_written <= 0) {
//       perror("send");
//       return 1;
//     }
//     remaining -= n_written;
//     cp += n_written;
//   }

//   /* Get an answer back. */
//   while (1) {
//     ssize_t result = recv(fd, buf, sizeof(buf), 0);
//     if (result == 0) {
//       break;
//     } else if (result < 0) {
//       perror("recv");
//       close(fd);
//       return 1;
//     }
//     fwrite(buf, 1, result, stdout);
//   }

//   close(fd);
//   return 0;
// }

#include "tests/TestCar.h"
#include "tests/TestTruck.h"
#include "transactional/TransactionExecutor.h"
#include "transactional/TransactionalCommand.h"

#include <folly/ScopeGuard.h>
#include <glog/logging.h>

using std::shared_ptr;
using std::unique_ptr;
using std::vector;

int main() {
  unique_ptr<State> oldState1 = std::make_unique<State>(0, 0);
  unique_ptr<State> oldState2 = std::make_unique<State>(10, 9.2);
  unique_ptr<State> oldState3 = std::make_unique<State>(20, 18.9);

  TestCar car1(std::move(oldState1->clone()));
  TestCar car2(std::move(oldState2->clone()));
  TestCar car3(std::move(oldState3->clone()));

  SCOPE_EXIT {
    LOG(INFO) << "car1's speed is " << car1.getSpeed();
    LOG(INFO) << "car2's speed is " << car2.getSpeed();
    LOG(INFO) << "car3's speed is " << car3.getSpeed();
  };

  unique_ptr<State> newState1 = std::make_unique<State>(40, 27);
  unique_ptr<State> newState2 = std::make_unique<State>(80, 9.2);
  unique_ptr<State> newState3 = std::make_unique<State>(70, 18.9);

  auto cmd1 = std::make_shared<TransactionalCommand>(
      "UpdateCar1State", car1.getUpdateStateFunc(&car1, newState1->clone()),
      car1.getUpdateStateFunc(&car1, oldState1->clone()));

  auto cmd2 = std::make_shared<TransactionalCommand>(
      "UpdateCar2State", car2.getUpdateStateFunc(&car2, newState2->clone()),
      []() -> bool {
        LOG(INFO) << "Rollback do nothing";
        return true;
      });

  auto cmd3 = std::make_shared<TransactionalCommand>(
      "UpdateCar3StateThrowException",
      car3.getUpdateStateThrowsExceptionFunc(&car3, newState3->clone()),
      car3.getUpdateStateFunc(&car3, oldState3->clone()));

  vector<shared_ptr<TransactionalCommand>> commands{cmd1, cmd2, cmd3};
  TransactionExecutor exe{commands};

  folly::CPUThreadPoolExecutor exec(1);
  bool result = exe.executeCommandsAsync(&exec).get();
  return result;
  // exe.executeCommandsInParellel();
}