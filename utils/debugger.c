// pthread_t listener_thread;

// void* terminal_listener() {
//     char input[100];
    
//     while (1) {
//         if (fgets(input, sizeof(input), stdin) != NULL) {
//             input[strcspn(input, "\n")] = 0;
            
//             if (strcmp(input, "server_fd") == 0) {
//                 printf("%d\n", server_fd);
//             } else if (strcmp(input, "client_sockets") == 0) {
//                 printf("client_sockets[]: ");
//                 for (int i=0; i < MAX_CLIENTS; i++) {
//                     printf("%d ", client_sockets[i]);
//                 }
//                 printf("\n");
//             } else if (strcmp(input, "exit") == 0) {
//                 printf("Exiting terminal listener thread...\n");
//                 break;
//             } else {
//                 printf("Unknown variable: %s\n", input);
//             }
//         }
//     }
//     return NULL;
// }



// if (pthread_create(&listener_thread, NULL, terminal_listener, NULL) != 0) {
//     perror("Failed to create terminal listener thread");
// }


// pthread_join(listener_thread, NULL);