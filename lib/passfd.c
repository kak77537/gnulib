/* Copyright (C) 2011 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "passfd.h"

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <sys/socket.h>
#if HAVE_SYS_UN_H
# include <sys/un.h>
#endif

/* sendfd sends the file descriptor fd along the socket
   to a process calling recvfd on the other end.

   Return 0 on success, or -1 with errno set in case of error.
*/
int
sendfd (int sock, int fd)
{
  char send = 0;
  struct iovec iov[1];
  struct msghdr msg;

  /* send at least one char */
  iov[0].iov_base = &send;
  iov[0].iov_len = 1;
  msg.msg_iov = iov;
  msg.msg_iovlen = 1;
  msg.msg_name = 0;
  msg.msg_namelen = 0;

  {
#if HAVE_UNIXSOCKET_SCM_RIGHTS_BSD44_WAY
    struct cmsghdr *cmsg;
    char buf[CMSG_SPACE (sizeof (fd))];

    msg.msg_control = buf;
    msg.msg_controllen = sizeof (buf);
    cmsg = CMSG_FIRSTHDR (&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN (sizeof (int));
    /* Initialize the payload: */
    memcpy (CMSG_DATA (cmsg), &fd, sizeof (fd));
    msg.msg_controllen = cmsg->cmsg_len;
#elif HAVE_UNIXSOCKET_SCM_RIGHTS_BSD43_WAY
    msg.msg_accrights = &fd;
    msg.msg_accrightslen = sizeof (fd);
#else
    errno = ENOSYS;
    return -1;
#endif
  }

  if (sendmsg (sock, &msg, 0) != iov[0].iov_len)
    return -1;
  return 0;
}

/* recvfd receives a file descriptor through the socket.

   Return 0 on success, or -1 with errno set in case of error.
*/
int
recvfd (int sock)
{
  char recv = 0;
  struct iovec iov[1];
  struct msghdr msg;

  /* send at least one char */
  iov[0].iov_base = &recv;
  iov[0].iov_len = 1;
  msg.msg_iov = iov;
  msg.msg_iovlen = 1;
  msg.msg_name = 0;
  msg.msg_namelen = 0;

  {
#if HAVE_UNIXSOCKET_SCM_RIGHTS_BSD44_WAY
    int fd;
    struct cmsghdr *cmsg;
    char buf[CMSG_SPACE (sizeof (fd))];
    const int mone = -1;

    msg.msg_control = buf;
    msg.msg_controllen = sizeof (buf);
    cmsg = CMSG_FIRSTHDR (&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN (sizeof (int));
    /* Initialize the payload: */
    memcpy (CMSG_DATA (cmsg), &mone, sizeof (mone));
    msg.msg_controllen = cmsg->cmsg_len;

    if (recvmsg (sock, &msg, 0) < 0)
      return -1;

    cmsg = CMSG_FIRSTHDR (&msg);
    /* be paranoiac */
    if (cmsg == NULL || cmsg->cmsg_len != CMSG_LEN (sizeof (int))
        || cmsg->cmsg_level != SOL_SOCKET || cmsg->cmsg_type != SCM_RIGHTS)
      {
        /* fake errno: at end the file is not available */
        errno = EACCES;
        return -1;
      }

    memcpy (&fd, CMSG_DATA (cmsg), sizeof (fd));
    return fd;
#elif HAVE_UNIXSOCKET_SCM_RIGHTS_BSD43_WAY
    int fd;

    msg.msg_accrights = &fd;
    msg.msg_accrightslen = sizeof (fd);
    if (recvmsg (sock, &msg, 0) < 0)
      return -1;
    return fd;
#else
    errno = ENOSYS;
    return -1;
#endif
  }
}