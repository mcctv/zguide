//
//  Weather update client in C
//  Connects SUB socket to tcp://localhost:5556
//  Collects weather updates and finds avg temp in zipcode
//
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int main (int argc, char *argv[])
{
    void *context;          //  ØMQ context for our process
    void *socket;           //  Socket to talk to server
    int update_nbr;
    long total_temp = 0;
    //  Grab zipcode, default is NYC, 10001
    char *filter = (argc > 1)? argv [1]: "10001 ";

    //  Prepare our context and socket
    context = zmq_init (1);
    socket = zmq_socket (context, ZMQ_SUB);

    printf ("Collecting updates from weather server...\n");
    zmq_connect (socket, "tcp://localhost:5556");

    //  Subscribe to updates for our zipcode
    zmq_setsockopt (socket, ZMQ_SUBSCRIBE, filter, strlen (filter));

    //  Process 100 updates
    for (update_nbr = 0; update_nbr < 100; update_nbr++) {
        zmq_msg_t update;
        int zipcode, temperature, relhumidity;

        zmq_msg_init (&update);
        zmq_recv (socket, &update, 0);
        sscanf ((char *) zmq_msg_data (&update), "%d %d %d",
            &zipcode, &temperature, &relhumidity);
        total_temp += temperature;
        zmq_msg_close (&update);
    }
    printf ("Average temperature for zipcode '%s' was %dF\n",
        filter, (int) (total_temp / update_nbr));
    return 0;
}