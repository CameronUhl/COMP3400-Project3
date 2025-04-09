#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main ()
{
  char buffer[1024];// = strdup("GET /cgi-bin/show.cgi?db=foo.txt&record=2 HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n");
  FILE *fd = fopen("tests/forms/form_full.txt", "r");
  size_t bytes = fread(buffer, 1, 1024, fd);
  char *form = strndup (buffer, bytes);
  printf ("%s\n", form);
  /*char *form = strdup("POST /cgi-bin/show.cgi HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\nContent-Type: multipart/form-data; boundary=------WebKitFormBoundary4XdOKY1sHBOLMWEE\r\n[other HTTP headers we are ignoring...]\r\n\r\n--------WebKitFormBoundary4XdOKY1sHBOLMWEE\r\nContent-Disposition: form-data; name=\"db\"\r\n\r\nfoo.txt\r\n--------WebKitFormBoundary4XdOKY1sHBOLMWEE\r\nContent-Disposition: form-data; name=\"hash\"\r\n\r\nf1f4b8705111a70c176a942d26f765fb548e2be9\r\n--------WebKitFormBoundary4XdOKY1sHBOLMWEE\r\nContent-Disposition: form-data; name=\"record\"\r\n\r\n2\r\n--------WebKitFormBoundary4XdOKY1sHBOLMWEE--\r\n");*/
  char *method = strdup("POST");
  char *bound = strdup("------WebKitFormBoundary4XdOKY1sHBOLMWEE");
  //printf ("FORM:%s\n", form);
  //printf ("BOUNDARY:%s\n", bound);
  if (strcmp (method, "GET")==0) //GET request
    {
      //printf ("Get method");
      
    }
  else //POST request
    {
      char *line = strstr (form, bound);
      //line += strlen(bound) +1;
      while (strlen (line) > 4)
        { 
          char* keyStart = strstr (line, "name=");
          keyStart += 6;
          char* keyEnd = strstr (keyStart, "\"");
          size_t keySize = keyEnd - keyStart;
          char *key = malloc (keySize +1);
          strncpy (key, keyStart, keySize);
          key[keySize] = '\0';
          
          char* valueStart = keyEnd + 5;
          char* valueEnd = strstr (valueStart, "\n");
          size_t valueSize = valueEnd - valueStart;
          char* value = malloc (valueSize + 1);
          strncpy (value, valueStart, valueSize);
          value [valueSize] = '\0';
          
          printf ("%s=%s\n", key, value);
          
          line = strstr (keyEnd, bound);
          line += strlen (bound); //move past boundary plus the \n and the additional "--" that are added to the read in boundary.
          //printf ("Line length: %ld\n", strlen (line));
        }
    }
    return 0;
}
