/* This is a small demo of the high-performance ThreadX kernel running as a module. It includes 
   examples of eight threads of different priorities, using a message queue, semaphore, mutex, 
   event flags group, byte pool, and block pool.  */

/* Specify that this is a module!  */

#define TXM_MODULE


/* Include the ThreadX module definitions.  */

#include "txm_module.h"

#include "stdio.h"
#include "common_hardware_code.h"

/* Define constants.  */

#define DEMO_STACK_SIZE         512
#define DEMO_BYTE_POOL_SIZE     6000
#define DEMO_BLOCK_POOL_SIZE    100
#define DEMO_QUEUE_SIZE         100


/* Define the pool space in the bss section of the module. ULONG is used to 
   get the word alignment.  */

ULONG                   demo_module_pool_space[DEMO_BYTE_POOL_SIZE / 4];


/* Define the ThreadX object control blocks...  */

TX_THREAD               *thread_0;
TX_THREAD               *thread_1;
TX_THREAD               *thread_2;
TX_THREAD               *thread_3;
TX_THREAD               *thread_4;
TX_THREAD               *thread_5;
TX_THREAD               *thread_6;
TX_THREAD               *thread_7;
TX_QUEUE                *queue_0;
TX_SEMAPHORE            *semaphore_0;
TX_MUTEX                *mutex_0;
TX_EVENT_FLAGS_GROUP    *event_flags_0;
TX_BYTE_POOL            *byte_pool_0;
TX_BLOCK_POOL           *block_pool_0;


/* Define the counters used in the demo application...  */

ULONG           thread_0_counter;
ULONG           thread_1_counter;
ULONG           thread_1_messages_sent;
ULONG           thread_2_counter;
ULONG           thread_2_messages_received;
ULONG           thread_3_counter;
ULONG           thread_4_counter;
ULONG           thread_5_counter;
ULONG           thread_6_counter;
ULONG           thread_7_counter;
ULONG           semaphore_0_puts;
ULONG           event_0_sets;
ULONG           queue_0_sends;

/* Define thread prototypes.  */

void    thread_0_entry(ULONG thread_input);
void    thread_1_entry(ULONG thread_input);
void    thread_2_entry(ULONG thread_input);
void    thread_3_and_4_entry(ULONG thread_input);
void    thread_5_entry(ULONG thread_input);
void    thread_6_and_7_entry(ULONG thread_input);

void  semaphore_0_notify(TX_SEMAPHORE *semaphore_ptr)
{

    if (semaphore_ptr == semaphore_0)
        semaphore_0_puts++;
}


void  event_0_notify(TX_EVENT_FLAGS_GROUP *event_flag_group_ptr)
{

    if (event_flag_group_ptr == event_flags_0)
        event_0_sets++;
}


void  queue_0_notify(TX_QUEUE *queue_ptr)
{

    if (queue_ptr == queue_0)
        queue_0_sends++;
}

void module_board_setup();
/* Define the module start function.  */

void    demo_module_start(ULONG id)
{

CHAR    *pointer;
	/* STM32F7xx HAL library initialization */
	module_board_setup();

    /* Allocate all the objects. In MPU mode, modules cannot allocate control blocks within
       their own memory area so they cannot corrupt the resident portion of ThreadX by overwriting 
       the control block(s).  */
    txm_module_object_allocate((void*)&thread_0, sizeof(TX_THREAD));
    txm_module_object_allocate((void*)&thread_1, sizeof(TX_THREAD));
    txm_module_object_allocate((void*)&thread_2, sizeof(TX_THREAD));
    txm_module_object_allocate((void*)&thread_3, sizeof(TX_THREAD));
    txm_module_object_allocate((void*)&thread_4, sizeof(TX_THREAD));
    txm_module_object_allocate((void*)&thread_5, sizeof(TX_THREAD));
    txm_module_object_allocate((void*)&thread_6, sizeof(TX_THREAD));
    txm_module_object_allocate((void*)&thread_7, sizeof(TX_THREAD));
    txm_module_object_allocate((void*)&queue_0, sizeof(TX_QUEUE));
    txm_module_object_allocate((void*)&semaphore_0, sizeof(TX_SEMAPHORE));
    txm_module_object_allocate((void*)&mutex_0, sizeof(TX_MUTEX));
    txm_module_object_allocate((void*)&event_flags_0, sizeof(TX_EVENT_FLAGS_GROUP));
    txm_module_object_allocate((void*)&byte_pool_0, sizeof(TX_BYTE_POOL));
    txm_module_object_allocate((void*)&block_pool_0, sizeof(TX_BLOCK_POOL));
                               

    /* Create a byte memory pool from which to allocate the thread stacks.  */
    tx_byte_pool_create(byte_pool_0, "module byte pool 0", (UCHAR*)demo_module_pool_space, DEMO_BYTE_POOL_SIZE);

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    /* Allocate the stack for thread 0.  */
    tx_byte_allocate(byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    /* Create the main thread.  */
    tx_thread_create(thread_0, "module thread 0", thread_0_entry, 0,  
            pointer, DEMO_STACK_SIZE, 
            1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);


    /* Allocate the stack for thread 1.  */
    tx_byte_allocate(byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    /* Create threads 1 and 2. These threads pass information through a ThreadX 
       message queue.  It is also interesting to note that these threads have a time
       slice.  */
    tx_thread_create(thread_1, "module thread 1", thread_1_entry, 1,  
            pointer, DEMO_STACK_SIZE, 
            16, 16, 4, TX_AUTO_START);

    /* Allocate the stack for thread 2.  */
    tx_byte_allocate(byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    tx_thread_create(thread_2, "module thread 2", thread_2_entry, 2,  
            pointer, DEMO_STACK_SIZE, 
            16, 16, 4, TX_AUTO_START);

    /* Allocate the stack for thread 3.  */
    tx_byte_allocate(byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    /* Create threads 3 and 4.  These threads compete for a ThreadX counting semaphore.  
       An interesting thing here is that both threads share the same instruction area.  */
    tx_thread_create(thread_3, "module thread 3", thread_3_and_4_entry, 3,  
            pointer, DEMO_STACK_SIZE, 
            8, 8, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Allocate the stack for thread 4.  */
    tx_byte_allocate(byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    tx_thread_create(thread_4, "module thread 4", thread_3_and_4_entry, 4,  
            pointer, DEMO_STACK_SIZE, 
            8, 8, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Allocate the stack for thread 5.  */
    tx_byte_allocate(byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    /* Create thread 5.  This thread simply pends on an event flag which will be set
       by thread_0.  */
    tx_thread_create(thread_5, "module thread 5", thread_5_entry, 5,  
            pointer, DEMO_STACK_SIZE, 
            4, 4, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Allocate the stack for thread 6.  */
    tx_byte_allocate(byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    /* Create threads 6 and 7.  These threads compete for a ThreadX mutex.  */
    tx_thread_create(thread_6, "module thread 6", thread_6_and_7_entry, 6,  
            pointer, DEMO_STACK_SIZE, 
            8, 8, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Allocate the stack for thread 7.  */
    tx_byte_allocate(byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    tx_thread_create(thread_7, "module thread 7", thread_6_and_7_entry, 7,  
            pointer, DEMO_STACK_SIZE, 
            8, 8, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Allocate the message queue.  */
    tx_byte_allocate(byte_pool_0, (VOID **) &pointer, DEMO_QUEUE_SIZE*sizeof(ULONG), TX_NO_WAIT);

    /* Create the message queue shared by threads 1 and 2.  */
    tx_queue_create(queue_0, "module queue 0", TX_1_ULONG, pointer, DEMO_QUEUE_SIZE*sizeof(ULONG));

    tx_queue_send_notify(queue_0, queue_0_notify);

    /* Create the semaphore used by threads 3 and 4.  */
    tx_semaphore_create(semaphore_0, "module semaphore 0", 1);

    tx_semaphore_put_notify(semaphore_0, semaphore_0_notify);

    /* Create the event flags group used by threads 1 and 5.  */
    tx_event_flags_create(event_flags_0, "module event flags 0");

    tx_event_flags_set_notify(event_flags_0, event_0_notify);

    /* Create the mutex used by thread 6 and 7 without priority inheritance.  */
    tx_mutex_create(mutex_0, "module mutex 0", TX_NO_INHERIT);

    /* Allocate the memory for a small block pool.  */
    tx_byte_allocate(byte_pool_0, (VOID **) &pointer, DEMO_BLOCK_POOL_SIZE, TX_NO_WAIT);

    /* Create a block memory pool to allocate a message buffer from.  */
    tx_block_pool_create(block_pool_0, "module block pool 0", sizeof(ULONG), pointer, DEMO_BLOCK_POOL_SIZE);

    /* Allocate a block and release the block memory.  */
    tx_block_allocate(block_pool_0, (VOID **) &pointer, TX_NO_WAIT);

    /* Release the block back to the pool.  */
    tx_block_release(pointer);
}


/* Define the test threads.  */
UCHAR tmp = 0xaf;
volatile UCHAR tmp1 = 0xa5;
volatile UCHAR tmp2 = 0xaf;
void hex2char(UINT a, UCHAR* buf)
{
int i;

	for(i = 0;i < 8;i++)
	{

		tmp = (a >> (i*4)) &0x0f;

		if((tmp >= 0) && (tmp<= 9))
		{
		buf[7-i] = tmp-0 +'0';
		}else
		{
		buf[7-i] = tmp-0x0a +'a';
		}

	}
}

/* this two values are only to test the global pointer and global buffer variety address*/
#if 1
char buffer[100];
volatile char* pBuffer = buffer;
volatile char* pBuffer11 = &buffer[1];
volatile char* pBuffer22 = (char*)thread_0_entry;
volatile char* pBuffer33 = (char*)thread_1_entry;
volatile char* pBuffer44 = (char*)thread_2_entry;
#endif

#if 1

/*
Structure and enumeration definitons
*/

typedef enum
{
Field_1 = 1,
Field_2,
Field_3,
Field_4,
}ENUM_TEST_t;

typedef struct
{
char appName;
int appId;
char appDescription;
}App_Test_t;

typedef struct
{
int test_A;
unsigned char testChArray[100];
const char *text;
int test_B;
ENUM_TEST_t *T1;
App_Test_t *app;
}GCC_TEST_t;

/* Statically initializing struct globally */

GCC_TEST_t user_test = {10,"struct member testChArray","const char * text",27,NULL,NULL};

#endif

extern UART_HandleTypeDef huart1;
void    thread_0_entry(ULONG thread_input)
{

UINT    status;
uint8_t test[10]="hello";


	/*printf not supported, because the standard library by default are not PIC*/
	//printf("buffer address: %p, buffer address via pBuffer: %p \n", buffer, pBuffer);

#if 1
	//assert(buffer == pBuffer );

	//assert(&buffer[1] == pBuffer11 );

	//assert((char*)thread_0_entry == pBuffer22 );
	HAL_UART_Transmit(&huart1, "***********test1. pointer***********",strlen("***********test1. pointer***********"), 0xFFFF);
	HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 0xFFFF);

	/*pubffer address*/
	hex2char((UINT)&pBuffer, test);
	HAL_UART_Transmit(&huart1, &test[0], 8, 0xFFFF);
	HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 0xFFFF);

	hex2char((UINT)buffer, test);
	HAL_UART_Transmit(&huart1, &test[0], 8, 0xFFFF);
	HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 0xFFFF);

	/*pubffer value*/
	hex2char((UINT)pBuffer, test);
	HAL_UART_Transmit(&huart1, &test[0], 8, 0xFFFF);
	HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 0xFFFF);

	hex2char((UINT)thread_0_entry, test);
	HAL_UART_Transmit(&huart1, &test[0], 8, 0xFFFF);
	HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 0xFFFF);

	hex2char((UINT)pBuffer22, test);
	HAL_UART_Transmit(&huart1, &test[0], 8, 0xFFFF);
	HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 0xFFFF);

	HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 0xFFFF);

#endif


#if 1

	HAL_UART_Transmit(&huart1, "***********test2. print struct member****",
								strlen("***********test2. print struct member****"), 0xFFFF);
	HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 0xFFFF);

	hex2char((UINT)user_test.test_A, test);
	HAL_UART_Transmit(&huart1, &test[0], 8, 0xFFFF);
	HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 0xFFFF);


	//hex2char((UINT)user_test.testChArray, test);
	//HAL_UART_Transmit(&huart1, &test[0], 8, 0xFFFF);
	//HAL_UART_Transmit(&huart1, (uint8_t*)"\n", 1, 0xFFFF);


	HAL_UART_Transmit(&huart1, user_test.testChArray, strlen("struct member testChArray"), 0xFFFF);
	HAL_UART_Transmit(&huart1, (uint8_t*)"\n\r", 2, 0xFFFF);


	HAL_UART_Transmit(&huart1, user_test.text, strlen("const char * text"), 0xFFFF);
	HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 0xFFFF);

	hex2char((UINT)(user_test.test_B), test);
	HAL_UART_Transmit(&huart1, &test[0], 8, 0xFFFF);
	HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 0xFFFF);

	hex2char((UINT)(user_test.T1), test);
	HAL_UART_Transmit(&huart1, &test[0], 8, 0xFFFF);
	HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 0xFFFF);

	hex2char((UINT)(user_test.app), test);
	HAL_UART_Transmit(&huart1, &test[0], 8, 0xFFFF);
	HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 0xFFFF);

#endif

    /* This thread simply sits in while-forever-sleep loop.  */
    while(1)
    {

        /* Increment the thread counter.  */
        thread_0_counter++;

        /* Sleep for 10 ticks.  */
        tx_thread_sleep(10);
       
        /* Set event flag 0 to wakeup thread 5.  */
        status =  tx_event_flags_set(event_flags_0, 0x1, TX_OR);

        HAL_UART_Transmit(&huart1, &test[0], 1, 0xFFFF);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;
    }
}


void    thread_1_entry(ULONG thread_input)
{

UINT    status;


    /* This thread simply sends messages to a queue shared by thread 2.  */
    while(1)
    {

        /* Increment the thread counter.  */
        thread_1_counter++;

        /* Send message to queue 0.  */
        status =  tx_queue_send(queue_0, &thread_1_messages_sent, TX_WAIT_FOREVER);

        /* Check completion status.  */
        if (status != TX_SUCCESS)
            break;

        /* Increment the message sent.  */
        thread_1_messages_sent++;
    }
}


void    thread_2_entry(ULONG thread_input)
{

ULONG   received_message;
UINT    status;

    /* This thread retrieves messages placed on the queue by thread 1.  */
    while(1)
    {
        /* Test memory handler.  */
      //  *(ULONG *)0x20008000 = 0xCDCDCDCD;
        
        
        /* Increment the thread counter.  */
        thread_2_counter++;

        /* Retrieve a message from the queue.  */
        status = tx_queue_receive(queue_0, &received_message, TX_WAIT_FOREVER);

        /* Check completion status and make sure the message is what we 
           expected.  */
        if ((status != TX_SUCCESS) || (received_message != thread_2_messages_received))
            break;
        
        /* Otherwise, all is okay.  Increment the received message count.  */
        thread_2_messages_received++;
    }
}


void    thread_3_and_4_entry(ULONG thread_input)
{

UINT    status;


    /* This function is executed from thread 3 and thread 4.  As the loop
       below shows, these function compete for ownership of semaphore_0.  */
    while(1)
    {

        /* Increment the thread counter.  */
        if (thread_input == 3)
            thread_3_counter++;
        else
            thread_4_counter++;

        /* Get the semaphore with suspension.  */
        status =  tx_semaphore_get(semaphore_0, TX_WAIT_FOREVER);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;

        /* Sleep for 2 ticks to hold the semaphore.  */
        tx_thread_sleep(2);

        /* Release the semaphore.  */
        status =  tx_semaphore_put(semaphore_0);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;
    }
}


void    thread_5_entry(ULONG thread_input)
{

UINT    status;
ULONG   actual_flags;


    /* This thread simply waits for an event in a forever loop.  */
    while(1)
    {

        /* Increment the thread counter.  */
        thread_5_counter++;

        /* Wait for event flag 0.  */
        status =  tx_event_flags_get(event_flags_0, 0x1, TX_OR_CLEAR, 
                                                &actual_flags, TX_WAIT_FOREVER);

        /* Check status.  */
        if ((status != TX_SUCCESS) || (actual_flags != 0x1))
            break;
    }
}


void    thread_6_and_7_entry(ULONG thread_input)
{

UINT    status;


    /* This function is executed from thread 6 and thread 7.  As the loop
       below shows, these function compete for ownership of mutex_0.  */
    while(1)
    {

        /* Increment the thread counter.  */
        if (thread_input == 6)
            thread_6_counter++;
        else
            thread_7_counter++;

        /* Get the mutex with suspension.  */
        status =  tx_mutex_get(mutex_0, TX_WAIT_FOREVER);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;

        /* Get the mutex again with suspension.  This shows
           that an owning thread may retrieve the mutex it
           owns multiple times.  */
        status =  tx_mutex_get(mutex_0, TX_WAIT_FOREVER);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;

        /* Sleep for 2 ticks to hold the mutex.  */
        tx_thread_sleep(2);

        /* Release the mutex.  */
        status =  tx_mutex_put(mutex_0);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;

        /* Release the mutex again.  This will actually 
           release ownership since it was obtained twice.  */
        status =  tx_mutex_put(mutex_0);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;
    }
}


