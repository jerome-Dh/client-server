/**
 * Server side implementation
 * 
 * @author Jerome Dh
 * @date 05/09/2020
 */

#include "server.h"

/**
 * Main app
 */ 
int main(int argc, char** argv)
{
    printf("Welcome to server side !\n");
    
    init();

    app();

    end();

    return EXIT_SUCCESS;
}