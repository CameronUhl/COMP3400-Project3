#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_LENGTH 8192

#define CRLF "\r\n"

/* Used to execute a given CGI program in a separate process. Uses the
   version parameter to build the HTTP header. Other parameters are used
   for the FULL implementation only as follows:

     if method is "GET":
       Use query as the value of the QUERY_STRING environment variable.

     if method is "POST":
       Use boundary to split the body of the request (see the project
       page for an example). The size parameter is only used if the POST
       request is for uploading a file, and is the size of the data to
       be uploaded.
 */
char *
cgi_response (char *uri, char *version, char *method, char *query,
              ssize_t size, char *boundary, char *body)
{
  // TODO [PART]: If the URI exists and is executable, run it as a separate
  // process, redirecting its STDOUT back to this process. You can then use
  // that resulting string to determine the Content-Length to send back. As
  // an example, here would be the full response to return for
  // cgi-bin/hello.cgi (assuming HTTP/1.0 and CRLF means "\r\n"). Don't print
  // the quotes:
  //   "HTTP/1.0 200 OK" CRLF
  //   "Content-Type: text/html; charset: UTF-8" CRLF
  //   "Content-Length: 95" CRLF
  //   CRLF
  //   "<html>\n"
  //   "<head>\n"
  //   "  <title>Hello world demo</title>\n"
  //   "</head>\n"
  //   "\n"
  //   "<h2>Hello world!</h2>\n"
  //   "</body>\n"
  //   "</html>\n"
  if (strstr (uri, ".cgi") != NULL && access (uri, F_OK) == 0)
    {
	int pipefd[2];
	pipe (pipefd);
	
	pid_t child = fork();
	char* response = malloc (2048);
	if (child == 0) //child code
	  {
	    close (pipefd[0]); // close read end
	    dup2 (pipefd[1], STDOUT_FILENO);
	    char *args[] = { NULL };
	    //char *qs = "QUERY_STRING=";
	    //strcat (qs, query);
	    //return query;
	    char *env[] = { NULL };
	    /*char *env[5];
	    if (strcmp (method, "GET") == 0)
	      {
	        char *qs = "QUERY_STRING=";
	        strcat (qs, query);
	        env[0] = qs;
	        env[1] = NULL;
              }
            else
              {
	        int count = 0;
	        char *line = strtok (query, "&");
	        while (line != NULL)
	          {
	            env[count++] = line;
	            line = strtok (NULL, "&");
	          }
                env[count] = NULL;
              }*/
	    execve (uri, args, env);
	  }
	else
	  {
            close (pipefd[1]); //close write end
            read (pipefd[0], response, 2048);
            close (pipefd[0]); 
	  }
	int content_length = strlen (response);
	char* header = NULL;
	
	char* connection;
	if (strcmp (version, "HTTP/1.1") == 0)
	  {
            connection = "Connection: close\r\n\r\n";
	  }
	else 
	  {
	    connection = "\r\n";
	  }
	int n = snprintf (header, 0, "%s 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: %d\r\n%s", version, content_length, connection);
	header = malloc (n + content_length + 1);
	snprintf (header, n+1, "%s 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: %d\r\n%s", version, content_length, connection);
	//return header;
	strcat (header, response);
	return header;
    }		
  return strdup ("HTTP/1.0 404 Not Found" CRLF CRLF);	

  

  // TODO [FULL]: Set the environment variables needed for the CGI programs
  // located in cgi-bin. To do this, you will need to use either execve()
  // or execle() when running the CGI program, using an array of string
  // pairs. For example, the following array would set the db and record
  // environment variables:
  //
  //   char *env[] = { "db=foo.txt", "record=2", NULL };
  //
  // If the request is a GET request, you should only set the QUERY_STRING
  // variable to be the query parameter. For POST requests, you will need
  // to look through the body of the HTTP request, splitting based on the
  // boundary values (see the project description for an example).
}
