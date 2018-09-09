#include <iostream>
#include <sys / types.h>
#include <unistd.h>
#include <sys / wait.h>
using namespace std;
const int NUMBER_OF_CHILDREN = 6;
const int PARENT_CONTROL = (2 - NUMBER_OF_CHILDREN);
const int SIZE_OF_INT = sizeof(int);
const int INITIAL_NUMBER_OF_SEATS = 100;



void travel_agent(int *child_to_parent, int *parent_to_child, int id);

int main()
{

    int child_to_parent[2];
    int parent_to_child[2];
    pipe(child_to_parent);
    pipe(parent_to_child);
    pid_t shut_down[NUMBER_OF_CHILDREN];
    int seats_left = INITIAL_NUMBER_OF_SEATS;
    int pid;
    for (int i = 0; i < NUMBER_OF_CHILDREN; i++)
    {
        pid = fork();
        if (pid < 0)
        {

            cout << "OH SNAP! Child " << i << " failed";

            return -1;
        }
        if (pid == 0)
        {
            cout << "CHILD: " << i << " CREATED " << getpid() << endl;
            travel_agent(child_to_parent, parent_to_child, i);

            break;
        }
        else
        {
            shut_down[i] = pid;
        }

    } 

    if (pid > 0)
    { 
        bool loop = true;
        while (loop)
        {
            write(parent_to_child[1], &seats_left, SIZE_OF_INT);
            read(child_to_parent[0], &seats_left, SIZE_OF_INT);
            if (seats_left > 0)
                cout << "the main just read seats_left: " << seats_left << endl;

            if (seats_left < PARENT_CONTROL)
            {
                loop = false;
                cout << "Parent exit loop. PID: " << getpid() << endl;
            }
        }

        for (int i = 0; i < NUMBER_OF_CHILDREN; i++)
        {

            cout << "Waiting for PID: " << shut_down[i] << " to finish" << endl;

            waitpid(shut_down[i], NULL, 0);

            cout << "PID: " << shut_down[i] << " has shut down" << endl;
        }
    }

    cout << "Did we all Join?  There will be 7 of us if we did.  PID: " << getpid() << endl;

    if (pid > 0)

        cout << "There are no ZOMBIES!" << endl;

    return 0;
}

void travel_agent(int *child_to_parent, int *parent_to_child, int id)
{

    bool loop = true;

    while (loop)
    {

        int seats_left = 0;

        read(parent_to_child[0], &seats_left, SIZE_OF_INT);

        if (seats_left > 0)
        {

            cout << "I am Child: " << id << " There are: " << seats_left << " seats, booking one!";

            cout << " My PID is : " << getpid() << endl;

            seats_left--;

            if (seats_left == 0)

                loop = false;

            write(child_to_parent[1], &seats_left, SIZE_OF_INT);

            usleep(100);
        }

        else
        {

            loop = false;

            seats_left--;

            write(child_to_parent[1], &seats_left, SIZE_OF_INT);
        }
    }
}