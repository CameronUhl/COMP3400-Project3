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
	    
	    if (strcmp (method, "GET") == 0)
	      {
	        if (query != NULL)
	          {
	            char buffer[1024];
	            size_t n = snprintf (buffer, 0, "QUERY_STRING=%s", query);
	            snprintf (buffer, n, "QUERY_STRING=%s", query);
	            buffer[n+1] = '\0';
	            char *qs = strdup (buffer);
	            char *env[] = { qs, NULL };
	            execve (uri, args, env);
                  }
                else 
                  {
                    char *env[] = { NULL };
	            execve (uri, args, env);
                  }
              }
            else
              {
	        char *line = strstr (body, boundary);
	        int count = 0;
	        char *env[] = { NULL, NULL, NULL, NULL };
                while (strlen (line) > 5)
                  { 
                    char* keyStart = strstr (line, "name=");
                    keyStart += 6;
                    char* keyEnd = strstr (keyStart, "\"");
                    size_t keySize = keyEnd - keyStart;
                    char *key = malloc (keySize +1);
                    strncpy (key, keyStart, keySize);
                    key[keySize] = '\0';
                    
                    char* valueStart = keyEnd + 5;
                    char* valueEnd = strstr (valueStart, "\r");
                    size_t valueSize = valueEnd - valueStart;
                    char* value = malloc (valueSize + 1);
                    strncpy (value, valueStart, valueSize);
                    value [valueSize] = '\0';
                    
                    if (key != NULL && value!= NULL)
                      {
                        sprintf (env[count], "%s=%s", key, value);
                        ++count;
                      }
                    line = valueEnd + strlen (boundary) + 3; //move past boundary plus the \n and the additional "--" that are added to the read in boundary.
                    //printf ("Line length: %ld\n", strlen (line));
                  }
                  execve (uri, args, env);
              }
            
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
