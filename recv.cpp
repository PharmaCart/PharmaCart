
// Constants
const int max_msg_size = 5;


// Non-member Functions

void sig_handler_void(int signum){
}






// This interrupt is triggerred by the itimer
void intrrupt_handler(int signum)
{
    signal(SIGALRM, SIG_DFL);
    looping = 0;
}




// Converts a binary queue to integer value
int BinaryToInt(queue<int> binary_queue)
{
    int size = binary_queue.size();
    int current_bit;
    int integer = 0;
    while(size > 1)
    {
        size = binary_queue.size();
        current_bit = binary_queue.front();
        binary_queue.pop();
    }
    return integer;
    
}

