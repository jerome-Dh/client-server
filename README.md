# Simple chat Client-server application

## About

<p>This app is a simple client-server chat implementation running in Ubuntu.</p>

## How to use & tests

To compile and run this project, you should compile and run the server side first:

```$ gcc main_server.c server.c -o server```

```$ ./server```

And then compile and run client:

```$ gcc main_client.c -o client```

So you can start multiple clients in different consoles to chat with each other:

```$ ./client localhost 1977```

## Author

[Jerome Dh](https://github.com/jerome-Dh/)
 
## License

This project is open-source software licensed under the [MIT license](https://opensource.org/licenses/MIT).