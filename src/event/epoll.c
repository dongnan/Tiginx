#include "../core/main.h"
#include "epoll.h"
#define MAX_EPOLL_EVENT_SIZE 10
void epoll_loop( int );

void epoll_loop( int listen_socket_fd ) {
  // 指向epoll内核事件表的fd
  int epoll_fd;
  int _temp_fd;
  int connect_socket_fd;
  int affected_epoll_fd;
  int struct_length;
  struct epoll_event ev_struct;  
  struct epoll_event ev_array[ MAX_EPOLL_EVENT_SIZE ];
  struct sockaddr_in connect_sockaddr_struct;
  char buffer[ 1024 ];
  // 开始创建epoll并将listen-socket-fd添加到epoll内核事件表
  ev_struct.data.fd = listen_socket_fd;
  ev_struct.events  = EPOLLIN | EPOLLET;
  epoll_fd = epoll_create1( 0 ); 
  epoll_ctl( epoll_fd, EPOLL_CTL_ADD, listen_socket_fd, &ev_struct ); 
  // 进入无限循环
  for ( ; ; ) {
    affected_epoll_fd = epoll_wait( epoll_fd, ev_array, MAX_EPOLL_EVENT_SIZE, -1 ); 
    //printf( "%d\n", affected_epoll_fd ); 
    if ( 0 > affected_epoll_fd ) {
      printf( "epoll wait error.\n" );
      exit( -1 );
    }
    if ( 0 == affected_epoll_fd ) {
      continue;
    }
    sleep( 1 );
    for ( int i = 0; i < MAX_EPOLL_EVENT_SIZE; i++ ) {
      _temp_fd = ev_array[ i ].data.fd; 
      if ( listen_socket_fd == _temp_fd ) {
        //printf( "accept\n" );
        struct_length = sizeof( connect_sockaddr_struct );
        connect_socket_fd = accept( listen_socket_fd, ( struct sockaddr * )&connect_sockaddr_struct, &struct_length );  
        ev_struct.data.fd = connect_socket_fd;
        ev_struct.events  = EPOLLIN | EPOLLET;
        epoll_ctl( epoll_fd, EPOLL_CTL_ADD, connect_socket_fd, &ev_struct );  
      }
      else {
        //printf( "recv\n" );
        memset( buffer, '\0', 1024 );
        recv( _temp_fd, buffer, 1024, 0 );  
        printf( "%s", buffer );
      }
    }
  } 
}
